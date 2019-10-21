// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillEdGraph.h"
#include "SkillGraphNode.h"
#include "SkillSystemEditor.h"
#include "SkillTreeEditorTypes.h"
#include "STNode.h"

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
		for (int32 iAux = 0; iAux < Node->Services.Num(); iAux++)
		{
			Node->Services[iAux]->ParentNode = Node;
		}

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

}

void USkillEdGraph::OnSubNodeDropped()
{

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
		USkillGraphNode* MyNode = Cast<USkillGraphNode>(Nodes[NodeIdx]);
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

void UpdateSkillGraphNodeErrorMessage(USkillGraphNode& Node)
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
		USkillGraphNode* Node = Cast<USkillGraphNode>(Nodes[Idx]);
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

}

void USkillEdGraph::UpdateClassData()
{
	for (int32 Idx = 0; Idx < Nodes.Num(); Idx++)
	{
		USkillGraphNode* Node = Cast<USkillGraphNode>(Nodes[Idx]);
		if (Node)
		{

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
			if (Pin->Direction == EGPD_Input)
			{
				TArray<UEdGraphPin*> PrevOrder(Pin->LinkedTo);
				Pin->LinkedTo.Sort(FCompareNodeXLocation());
				bUpdateExecutionOrder = bUpdateExecutionOrder || (PrevOrder != Pin->LinkedTo);
			}
		}
	}
	if (bUpdateExecutionOrder)
	{
		//UpdateAsset(keepre)
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

void USkillEdGraph::CollectAllNodeInstance(TSet<UObject*>& NodeInstances)
{

}

bool USkillEdGraph::CanRemoveNestdObject(UObject* TestObject) const
{

	return true;
}

void USkillEdGraph::OnNodeInstanceRemoved(UObject* NodeInstance)
{

}

UEdGraphPin* USkillEdGraph::FindGraphNodePin(UEdGraphNode* Node, EEdGraphPinDirection Dir)
{
	return nullptr;
}
