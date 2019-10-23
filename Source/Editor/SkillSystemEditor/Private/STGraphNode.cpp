// Fill out your copyright notice in the Description page of Project Settings.


#include "STGraphNode.h"
#include "AssetData.h"
#include "EdGraph/EdGraphSchema.h"
#include "SkillGraphTypes.h"
#include "SkillEdGraph.h"
#include "DiffResults.h"
#include "ScopedTransaction.h"
#include "BlueprintNodeHelpers.h"
#include "UObject/Class.h"
#include "UObject/UnrealType.h"
#include "Engine/Blueprint.h"
#include "STNode.h"

#define LOCTEXT_NAMESPACE "SKILLGraph"
USTGraphNode::USTGraphNode(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	NodeInstance = nullptr;
	CopySubNodeIndex = 0;
	bIsReadOnly = false;
	bIsSubNode = false;
}

void USTGraphNode::AddSubNode(USTGraphNode* SubNode, class UEdGraph* ParentGraph)
{
	const FScopedTransaction Transaction(LOCTEXT("AddNode", "Add Node"));
	ParentGraph->Modify();
	Modify();
	SubNode->SetFlags(RF_Transactional);
	SubNode->Rename(nullptr, ParentGraph, REN_NonTransactional);

	SubNode->ParentNode = this;
	SubNode->CreateNewGuid();
	SubNode->PostPlacedNewNode();
	SubNode->AllocateDefaultPins();
	SubNode->AutowireNewNode(nullptr);

	SubNode->NodePosX = 0;
	SubNode->NodePosY = 0;
	SubNodes.Add(SubNode);
	
	OnSubNodeAdded(SubNode);

	ParentGraph->NotifyGraphChanged();
	GetSTGraph()->UpdateAsset();
}

void USTGraphNode::RemoveSubNode(USTGraphNode* SubNode)
{
	SubNodes.RemoveSingle(SubNode);
	Modify();
	OnSubNodeRemoved(SubNode);
}

void USTGraphNode::RemoveAllSubNodes()
{
	SubNodes.Reset();
}

void USTGraphNode::OnSubNodeRemoved(USTGraphNode * SubNode)
{
}

void USTGraphNode::OnSubNodeAdded(USTGraphNode * SubNode)
{
}

class USkillEdGraph* USTGraphNode::GetSTGraph()
{
	return CastChecked<USkillEdGraph>(GetGraph());
}

FText USTGraphNode::GetDescription() const
{
	FString StoredClassName = ClassData.GetClassName();
	StoredClassName.RemoveFromEnd(TEXT("_C"));
	return FText::Format(LOCTEXT("NodeClassError", "Class {0} not found , make sure it's saved!"), FText::FromString(StoredClassName));
}

void USTGraphNode::PostCopyNode()
{
	ResetNodeOwner();
}

bool USTGraphNode::IsSubNode() const
{
	return bIsSubNode || (ParentNode != nullptr);
}

int32 USTGraphNode::FindSubNodeDropIndex(USTGraphNode * SubNode) const
{
	const int32 InsertIndex = SubNodes.IndexOfByKey(SubNode);
	return InsertIndex;
}

void USTGraphNode::InsertSubNodeAt(USTGraphNode * SubNode, int32 DropIndex)
{
	if (DropIndex > -1)
	{
		SubNodes.Insert(SubNode, DropIndex);
	}
	else
	{
		SubNodes.Add(SubNode);
	}
}

void USTGraphNode::InitializeInstance()
{
}

bool USTGraphNode::RefreshNodeClass()
{
	bool bUpdate = false;
	if (NodeInstance == nullptr)
	{
		if (FSkillGraphNodeClassHelper::IsClassKnown(ClassData))
		{
			PostPlacedNewNode();
			bUpdate = (NodeInstance != nullptr);
		}
		else
		{
			FSkillGraphNodeClassHelper::AddUnknownClass(ClassData);
		}
	}
	return bUpdate;
}

void USTGraphNode::UpdateNodeClassData()
{
	if (NodeInstance)
	{
		UpdateNodeClassDataFrom(NodeInstance->GetClass(), ClassData);
		ErrorMessage = ClassData.GetDeprecatedMessage();
	}
}

bool USTGraphNode::UsesBlueprint() const
{
	return NodeInstance && NodeInstance->GetClass()->HasAnyClassFlags(CLASS_CompiledFromBlueprint);
}

bool USTGraphNode::HasErrors() const
{
	return ErrorMessage.Len() > 0 || NodeInstance == nullptr;
}

void USTGraphNode::UpdateNodeClassDataFrom(UClass* InstanceClass, FSkillGraphNodeClassData& UpdatedData)
{
	if (InstanceClass)
	{
		UBlueprint* BPOwner = Cast<UBlueprint>(InstanceClass->ClassGeneratedBy);
		if (BPOwner)
		{
			UpdatedData = FSkillGraphNodeClassData(BPOwner->GetName(), BPOwner->GetOutermost()->GetName(), InstanceClass->GetName(), InstanceClass);
		}
		else
		{
			UpdatedData = FSkillGraphNodeClassData(InstanceClass,FSkillGraphNodeClassHelper::GetDeprecationMessage(InstanceClass));
		}
	}
}

void USTGraphNode::AutowireNewNode(UEdGraphPin * FromPin)
{
	Super::AutowireNewNode(FromPin);
	if (FromPin != nullptr)
	{
		UEdGraphPin* OutputPin = GetOutputPin();
		if (GetSchema()->TryCreateConnection(FromPin,GetInputPin()))
		{
			FromPin->GetOwningNode()->NodeConnectionListChanged();
		}
		else if (OutputPin != nullptr && GetSchema()->TryCreateConnection(OutputPin,FromPin))
		{
			NodeConnectionListChanged();
		}
	}
}

void USTGraphNode::PostPlacedNewNode()
{
	UClass* NodeClass = ClassData.GetClass(true);
	if (NodeClass && (NodeInstance == nullptr))
	{
		UEdGraph* MyGraph = GetGraph();
		UObject* GraphOwner = MyGraph ? MyGraph->GetOuter() : nullptr;
		if (GraphOwner)
		{
			NodeInstance = NewObject<UObject>(GraphOwner, NodeClass);
			NodeInstance->SetFlags(RF_Transactional);
			InitializeInstance();
		}
	}
}

void USTGraphNode::PrepareForCopying()
{
	if (NodeInstance)
	{
		NodeInstance->Rename(nullptr, this, REN_DontCreateRedirectors | REN_DoNotDirty);
	}
}

bool USTGraphNode::CanDuplicateNode() const
{
	return bIsReadOnly ? false : Super::CanDuplicateNode();
}

bool USTGraphNode::CanUserDeleteNode() const
{
	return bIsReadOnly ? false : Super::CanUserDeleteNode();
}

void USTGraphNode::DestroyNode()
{
	if (ParentNode)
	{
		ParentNode->RemoveSubNode(this);
	}
	UEdGraphNode::DestroyNode();
}

FText USTGraphNode::GetTooltipText() const
{
	FText TooltipDesc;
	if (!NodeInstance)
	{
		FString StoredClassName = ClassData.GetClassName();
		StoredClassName.RemoveFromEnd(TEXT("_C"));
		TooltipDesc = FText::Format(LOCTEXT("NodeClassError", "Class {0} not found , make sure it's saved!"), FText::FromString(StoredClassName));
	}
	else
	{
		if (ErrorMessage.Len() > 0)
		{
			TooltipDesc = FText::FromString(ErrorMessage);
		}
		else
		{
			if (NodeInstance->GetClass()->HasAnyClassFlags(CLASS_CompiledFromBlueprint))
			{
				FAssetData AssetData(NodeInstance->GetClass()->ClassGeneratedBy);
				FString Description = AssetData.GetTagValueRef<FString>(GET_MEMBER_NAME_CHECKED(UBlueprint, BlueprintDescription));
				if (!Description.IsEmpty())
				{
					Description.ReplaceInline(TEXT("\\n"), TEXT("\n"));
					TooltipDesc = FText::FromString(MoveTemp(Description));
				}
			}
			else
			{
				TooltipDesc = NodeInstance->GetClass()->GetToolTipText();
			}
		}
	}
	return TooltipDesc;
}

void USTGraphNode::NodeConnectionListChanged()
{
	Super::NodeConnectionListChanged();
	GetSTGraph()->UpdateAsset();
}

bool USTGraphNode::CanCreateUnderSpecifiedSchema(const UEdGraphSchema * Schema) const
{
	return false;
}

void USTGraphNode::FindDiffs(UEdGraphNode * OtherNode, FDiffResults & Results)
{
	Super::FindDiffs(OtherNode, Results);
	if (USTGraphNode* OtherGraphNode = Cast<USTGraphNode>(OtherNode))
	{
		if (NodeInstance && OtherGraphNode->NodeInstance)
		{
			FDiffSingleResult Diff;
			Diff.Diff = EDiffType::NODE_PROPERTY;
			Diff.Node1 = this;
			Diff.Node2 = OtherNode;
			Diff.ToolTip = LOCTEXT("DIF_NodeIntancePropertyToolTip", "A property of the node instance has changed");
			Diff.DisplayColor = FLinearColor(0.25f, 0.71f, 0.85f);
			DiffProperties(NodeInstance->GetClass(), OtherGraphNode->NodeInstance->GetClass(), NodeInstance, OtherGraphNode->NodeInstance, Results, Diff);
		}
	}
}

FString USTGraphNode::GetPropertyNameAndValueForDiff(const UProperty * Prop, const uint8 * PropertyAddr) const
{
	return BlueprintNodeHelpers::DescribeProperty(Prop,PropertyAddr);
}

#if WITH_EDITOR
void USTGraphNode::PostEditImport()
{
	ResetNodeOwner();
	if (NodeInstance)
	{
		InitializeInstance();
	}
}

void USTGraphNode::PostEditUndo()
{
	UEdGraphNode::PostEditUndo();
	ResetNodeOwner();
	if (ParentNode)
	{
		ParentNode->SubNodes.AddUnique(this);
	}
}
#endif

UEdGraphPin * USTGraphNode::GetInputPin(int32 InputIndex) const
{
	check(InputIndex >= 0);
	for (int32 PinIndex = 0, FoundInputs = 0; PinIndex < Pins.Num(); PinIndex++)
	{
		if (Pins[PinIndex]->Direction == EGPD_Input)
		{
			if (InputIndex == FoundInputs)
			{
				return Pins[PinIndex];
			}
			else
			{
				FoundInputs++;
			}
		}
	}
	return nullptr;
}

UEdGraphPin * USTGraphNode::GetOutputPin(int32 InputIndex) const
{
	check(InputIndex >= 0);
	for (int32 PinIndex = 0, FoundInputs = 0; PinIndex < Pins.Num(); PinIndex++)
	{
		if (Pins[PinIndex]->Direction == EGPD_Output)
		{
			if (InputIndex == FoundInputs)
			{
				return Pins[PinIndex];
			}
			else
			{
				FoundInputs++;
			}
		}
	}
	return nullptr;
}

void USTGraphNode::ResetNodeOwner()
{
	if (NodeInstance)
	{
		UEdGraph* MyGraph = GetGraph();
		UObject* GraphOwner = MyGraph ? MyGraph->GetOuter() : nullptr;
		NodeInstance->Rename(NULL, GraphOwner, REN_DontCreateRedirectors | REN_DoNotDirty);
		NodeInstance->ClearFlags(RF_Transactional);

		for (auto & SubNode : SubNodes)
		{
			SubNode->ResetNodeOwner();
		}
	}
}


#undef LOCTEXT_NAMESPACE