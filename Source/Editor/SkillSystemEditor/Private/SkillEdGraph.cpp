// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillEdGraph.h"
#include "STGraphNode.h"
#include "SkillSystemEditor.h"
#include "SkillTreeEditorTypes.h"
#include "STNode.h"
#include "Skill.h"
#include "STCompositeNode.h"
#include "SkillTreeGraphNode.h"
#include "SkillGraphNode_Root.h"
#include "EdGraph/EdGraphSchema.h"


namespace STGraphHelpers
{
	void InitializeInjectedNodes(USkillTreeGraphNode* GraphNode, USTCompositeNode* RootNode, uint16 ExecutionIndex, uint8 TreeDepth, int32 Index)
	{

	}

	void VerifyDecorators(USkillTreeGraphNode* GraphNode)
	{

	}

	void CreateChildren(USkill* STAsset, USTCompositeNode* RootNode, const USkillTreeGraphNode* RootEdNode, uint16* ExcutionIndex, uint8 TreeDepth)
	{
		if (RootEdNode == nullptr)
		{
			return;
		}
		RootNode->Children.Reset();
		
		int32 ChildIdx = 0;
		for (int32 PinIdx = 0; PinIdx < RootEdNode->Pins.Num(); PinIdx++)
		{
			UEdGraphPin* Pin = RootEdNode->Pins[PinIdx];
			if (Pin->Direction != EGPD_Output)
			{
				continue;
			}
			Pin->LinkedTo.Sort(FCompareNodeXLocation());
			for (int32 Index = 0; Index < Pin->LinkedTo.Num(); ++Index)
			{
				USkillTreeGraphNode* GraphNode = Cast<USkillTreeGraphNode>(Pin->LinkedTo[Index]->GetOwningNode());
				if (GraphNode == nullptr)
				{
					continue;;
				}
				// TODO: TASK


				//comp
				USTCompositeNode* CompositeInstance = Cast<USTCompositeNode>(GraphNode->NodeInstance);
				if (CompositeInstance&&Cast<USkill>(CompositeInstance->GetOuter()) == nullptr)
				{
					CompositeInstance->Rename(nullptr, STAsset);
				}

				if (CompositeInstance == nullptr)
				{
					continue;
				}

				ChildIdx = RootNode->Children.AddDefaulted();
				FSTCompositeChild& ChildInfo = RootNode->Children[ChildIdx];
				ChildInfo.ChildComposite = CompositeInstance;

				USTNode* ChildNode = CompositeInstance ? (USTNode*)CompositeInstance : nullptr;
				if (ChildNode && Cast<USkill>(ChildNode->GetOuter()) == nullptr)
				{
					ChildNode->Rename(nullptr, STAsset);
				}

				InitializeInjectedNodes(GraphNode, RootNode, *ExcutionIndex, TreeDepth, ChildIdx);

				// 

				ChildNode->InitializeNode(RootNode, *ExcutionIndex, 0, TreeDepth);
				*ExcutionIndex += 1;

				VerifyDecorators(GraphNode);
				
				if (CompositeInstance)
				{
					CreateChildren(STAsset, CompositeInstance, GraphNode, ExcutionIndex, TreeDepth + 1);
					CompositeInstance->InitializeComposite((*ExcutionIndex) - 1);
				}
				
			}
		}

	}
}


USkillEdGraph::USkillEdGraph(const FObjectInitializer& Objectinitlializer)
	:Super(Objectinitlializer)
{
	bLockUpdates = false;
}

void USkillEdGraph::OnCreated()
{
	MarkVersion();
	
}

void USkillEdGraph::OnLoaded()
{
	UpdateDeprecatedClasses();
	UpdateUnknownNodeClasses();
}

void USkillEdGraph::Initialize()
{
	UpdateVersion();
}

void USkillEdGraph::UpdateAsset(int32 UpdateFlags /*= 0*/)
{
	if (bLockUpdates)
	{
		return;
	}
	USkillGraphNode_Root* RootNode = nullptr;
	for (int32 Index = 0; Index < Nodes.Num(); ++Index)
	{
		USkillTreeGraphNode* Node = Cast<USkillTreeGraphNode>(Nodes[Index]);
		if (Node == nullptr)
		{
			continue;
		}
		// TODO Debugger flag;
		//parent chain
		Node->ParentNode = nullptr;

		// prepare node instance 
		USTNode* NodeInstance = Cast<USTNode>(Node->NodeInstance);
		if (NodeInstance)
		{
			NodeInstance->InitializeNode(nullptr, MAX_uint16, 0, 0);
		}

		// cache root;
		if (RootNode == nullptr)
		{
			RootNode = Cast<USkillGraphNode_Root>(Nodes[Index]);
		}
	}

	// we 
	UEdGraphPin::ResolveAllPinReferences();
	if (RootNode && RootNode->Pins.Num() > 0 && RootNode->Pins[0]->LinkedTo.Num() > 0)
	{
		USkillTreeGraphNode* Node = Cast<USkillTreeGraphNode>(RootNode->Pins[0]->LinkedTo[0]->GetOwningNode());
		if (Node)
		{
			CreateSTFromGraph(Node);
			if ((UpdateFlags & KeepRebuildCounter) == 0)
			{
				ModCounter++;
			}
		}
	}
	// TODO Update blockboardchange();
}

void USkillEdGraph::UpdateVersion()
{
	if (GrapVersion == 1)
	{
		return;
	}
	MarkVersion();
	Modify();
}

void USkillEdGraph::MarkVersion()
{
	GrapVersion = 1;
}

void USkillEdGraph::CreateSTFromGraph(class USkillTreeGraphNode* RootEdNode)
{
	USkill* STAsset = Cast<USkill>(GetOuter());
	STAsset->RootNode = nullptr;

	uint16 ExecutionIndex = 0;
	uint8 TreeDepth = 0;
	STAsset->RootNode = Cast<USTCompositeNode>(RootEdNode->NodeInstance);
	if (STAsset->RootNode)
	{
		STAsset->RootNode->InitializeNode(nullptr, ExecutionIndex, 0, TreeDepth);
		ExecutionIndex++;
	}

	

	// connect tree nodes;
	STGraphHelpers::CreateChildren(STAsset, STAsset->RootNode, RootEdNode, &ExecutionIndex, TreeDepth + 1);

	RootEdNode->bRootLevel = true;
	
	if (STAsset->RootNode)
	{
		STAsset->RootNode->InitializeComposite(ExecutionIndex - 1);
	}
	RemoveOrphanedNodes();
}

void USkillEdGraph::OnSubNodeDropped()
{
	NotifyGraphChanged();
}

void USkillEdGraph::OnNodesPasted(const FString& ImportStr)
{

}

void USkillEdGraph::OnSave()
{
	UpdateAsset();
}

bool USkillEdGraph::UpdateUnknownNodeClasses()
{
	bool bUpdated = false;
	for (int32 NodeIdx = 0; NodeIdx < Nodes.Num(); NodeIdx++)
	{
		USTGraphNode* MyNode = Cast<USTGraphNode>(Nodes[NodeIdx]);
		if (MyNode)
		{
			const bool bUpdatedNode = MyNode->RefreshNodeClass();
			bUpdated = bUpdated || bUpdatedNode;
			for (int32 SubNodeIdx = 0; SubNodeIdx < MyNode->SubNodes.Num(); SubNodeIdx++)
			{
				if (MyNode->SubNodes[SubNodeIdx])
				{
					const bool bUpdatedSubNode = MyNode->SubNodes[SubNodeIdx]->RefreshNodeClass();
					bUpdated = bUpdated || bUpdatedSubNode;
				}
			}
		}
	}
	return bUpdated;
}

void UpdateSkillGraphNodeErrorMessage(USTGraphNode& Node)
{
	if (Node.NodeInstance)
	{
		Node.ErrorMessage = FSkillGraphNodeClassHelper::GetDeprecationMessage(Node.NodeInstance->GetClass());
	}
	else
	{
		FString StoredClassName = Node.ClassData.GetClassName();
		StoredClassName.RemoveFromEnd(TEXT("_C"));
		if (!StoredClassName.IsEmpty())
		{
			static const FString IsMissingClassMessage(" class missing. Referenced by ");
			Node.ErrorMessage = StoredClassName + IsMissingClassMessage + Node.GetFullName();
		}
	}
	if (Node.HasErrors())
	{
		UE_LOG(LogSkillEditor, Error, TEXT("%s"),*Node.ErrorMessage);
	}
}

bool USkillEdGraph::UpdateDeprecatedClasses()
{
	for (int32 Idx = 0, IdxNum = Nodes.Num(); Idx < IdxNum; ++Idx)
	{
		USTGraphNode* Node = Cast<USTGraphNode>(Nodes[Idx]);
		if (Node != nullptr)
		{
			UpdateSkillGraphNodeErrorMessage(*Node);
			for (int32 SubIdx = 0, SubIdxNum = Node->SubNodes.Num(); SubIdx < SubIdxNum; ++SubIdx)
			{
				if (Node->SubNodes[SubIdx] != nullptr)
				{
					UpdateSkillGraphNodeErrorMessage(*Node->SubNodes[SubIdx]);
				}
			}
		}
	}
	return true;
}

void USkillEdGraph::RemoveOrphanedNodes()
{
	TSet<UObject*> NodeInstances;
	CollectAllNodeInstance(NodeInstances);

	NodeInstances.Remove(nullptr);

	TArray<UObject*> AllInners;

	const bool bIncludeNestedObjects = false;
	GetObjectsWithOuter(GetOuter(), AllInners, bIncludeNestedObjects);

	for (auto InnerIt = AllInners.CreateConstIterator(); InnerIt; ++InnerIt)
	{
		UObject* TestObject = *InnerIt;
		if (!NodeInstances.Contains(TestObject) && CanRemoveNestdObject(TestObject))
		{
			OnNodeInstanceRemoved(TestObject);

			TestObject->SetFlags(RF_Transient);
			TestObject->Rename(NULL, GetTransientPackage(), REN_DontCreateRedirectors | REN_NonTransactional | REN_ForceNoResetLoaders);
		}
	}
}

void USkillEdGraph::UpdateClassData()
{
	for (int32 Idx = 0; Idx < Nodes.Num(); Idx++)
	{
		USTGraphNode* Node = Cast<USTGraphNode>(Nodes[Idx]);
		if (Node)
		{
			Node->UpdateNodeClassData();
			for (int32 SubIdx = 0; SubIdx < Node->SubNodes.Num(); SubIdx++)
			{
				if (Node->SubNodes[SubIdx])
				{
					Node->UpdateNodeClassData();
				}
			}
		}
	}
}

bool USkillEdGraph::IsLocked() const
{
	return bLockUpdates;
}

void USkillEdGraph::LockedUpdates()
{

}

void USkillEdGraph::UnlockUpdates()
{

}

void USkillEdGraph::RebuildChildOrder(UEdGraphNode* ParentNode)
{
	bool bUpdateExecutionOrder = false;
	if (ParentNode)
	{
		for (int32 i = 0; i < ParentNode->Pins.Num(); i++)
		{
			UEdGraphPin* Pin = ParentNode->Pins[i];
			if (Pin->Direction == EGPD_Output)
			{
				TArray<UEdGraphPin*> PrevOrder(Pin->LinkedTo);
				Pin->LinkedTo.Sort(FCompareNodeXLocation());
				bUpdateExecutionOrder = bUpdateExecutionOrder || (PrevOrder != Pin->LinkedTo);
			}
		}
	}
	if (bUpdateExecutionOrder)
	{
		UpdateAsset(KeepRebuildCounter);
		Modify();
	}
}

void USkillEdGraph::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
	if (Ar.IsSaving() || Ar.IsCooking())
	{
		UpdateDeprecatedClasses();
	}
}

void USkillEdGraph::AutoArrange()
{

}

void USkillEdGraph::CollectAllNodeInstance(TSet<UObject*>& NodeInstances)
{
	for (int32 Idx = 0; Idx < Nodes.Num(); Idx++)
	{
		USTGraphNode* MyNode = Cast<USTGraphNode>(Nodes[Idx]);
		if (MyNode)
		{
			NodeInstances.Add(MyNode->NodeInstance);
			for (int32 SubIdx = 0; SubIdx < MyNode->SubNodes.Num(); SubIdx++)
			{
				if (MyNode->SubNodes[SubIdx])
				{
					NodeInstances.Add(MyNode->SubNodes[SubIdx]->NodeInstance);
				}
			}
		}
	}
}

bool USkillEdGraph::CanRemoveNestdObject(UObject* TestObject) const
{
	return !TestObject->IsA(UEdGraphNode::StaticClass()) &&
		!TestObject->IsA(UEdGraph::StaticClass()) &&
		!TestObject->IsA(UEdGraphSchema::StaticClass());
}

void USkillEdGraph::OnNodeInstanceRemoved(UObject* NodeInstance)
{

}

UEdGraphPin* USkillEdGraph::FindGraphNodePin(UEdGraphNode* Node, EEdGraphPinDirection Dir)
{
	return nullptr;
}
