// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillGraphNode.h"
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

#define LOCTEXT_NAMESPACE "SKILLGraph"
USkillGraphNode::USkillGraphNode(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{

}

void USkillGraphNode::AddSubNode(USkillGraphNode* SubNode, class UEdGraph* ParentGraph)
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
	GetSkillGraph()->UpdateAsset();
}

void USkillGraphNode::RemoveSubNode(USkillGraphNode* SubNode)
{
	SubNodes.RemoveSingle(SubNode);
	Modify();
	OnSubNodeRemoved(SubNode);
}

void USkillGraphNode::RemoveAllSubNodes()
{
	SubNodes.Reset();
}

void USkillGraphNode::OnSubNodeRemoved(USkillGraphNode * SubNode)
{
}

void USkillGraphNode::OnSubNodeAdded(USkillGraphNode * SubNode)
{
}

class USkillEdGraph* USkillGraphNode::GetSkillGraph()
{
	return CastChecked<USkillEdGraph>(GetGraph());
}

FText USkillGraphNode::GetDescription() const
{
	FString StoredClassName = ClassData.GetClassName();
	StoredClassName.RemoveFromEnd(TEXT("_C"));
	return FText::Format(LOCTEXT("NodeClassError", "Class {0} not found , make sure it's saved!"), FText::FromString(StoredClassName));
}

void USkillGraphNode::PostCopyNode()
{
	ResetNodeOwner();
}

bool USkillGraphNode::IsSubNode() const
{
	return bIsSubNode || (ParentNode != nullptr);
}

int32 USkillGraphNode::FindSubNodeDropIndex(USkillGraphNode * SubNode) const
{
	const int32 InsertIndex = SubNodes.IndexOfByKey(SubNode);
	return InsertIndex;
}

void USkillGraphNode::InsertSubNodeAt(USkillGraphNode * SubNode, int32 DropIndex)
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

void USkillGraphNode::InitializeInstance()
{
}

bool USkillGraphNode::RefreshNodeClass()
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

void USkillGraphNode::UpdateNodeClassData()
{
	if (NodeInstance)
	{
		UpdateNodeClassDataFrom(NodeInstance->GetClass(), ClassData);
		ErrorMessage = ClassData.GetDeprecatedMessage();
	}
}

bool USkillGraphNode::UsesBlueprint() const
{
	return NodeInstance && NodeInstance->GetClass()->HasAnyClassFlags(CLASS_CompiledFromBlueprint);
}

bool USkillGraphNode::HasErrors() const
{
	return ErrorMessage.Len() > 0 || NodeInstance == nullptr;
}

void USkillGraphNode::UpdateNodeClassDataFrom(UClass* InstanceClass, FSkillGraphNodeClassData& UpdatedData)
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

void USkillGraphNode::AutowireNewNode(UEdGraphPin * FromPin)
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

void USkillGraphNode::PostPlacedNewNode()
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

void USkillGraphNode::PrepareForCopying()
{
	if (NodeInstance)
	{
		NodeInstance->Rename(nullptr, this, REN_DontCreateRedirectors | REN_DoNotDirty);
	}
}

bool USkillGraphNode::CanDuplicateNode() const
{
	return bIsReadOnly ? false : Super::CanDuplicateNode();
}

bool USkillGraphNode::CanUserDeleteNode() const
{
	return bIsReadOnly ? false : Super::CanUserDeleteNode();
}

void USkillGraphNode::DestroyNode()
{
	if (ParentNode)
	{
		ParentNode->RemoveSubNode(this);
	}
	UEdGraphNode::DestroyNode();
}

FText USkillGraphNode::GetTooltipText() const
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

void USkillGraphNode::NodeConnectionListChanged()
{
	Super::NodeConnectionListChanged();
	GetSkillGraph()->UpdateAsset();
}

bool USkillGraphNode::CanCreateUnderSpecifiedSchema(const UEdGraphSchema * Schema) const
{
	return false;
}

void USkillGraphNode::FindDiffs(UEdGraphNode * OtherNode, FDiffResults & Results)
{
	Super::FindDiffs(OtherNode, Results);
	if (USkillGraphNode* OtherGraphNode = Cast<USkillGraphNode>(OtherNode))
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

FString USkillGraphNode::GetPropertyNameAndValueForDiff(const UProperty * Prop, const uint8 * PropertyAddr) const
{
	return BlueprintNodeHelpers::DescribeProperty(Prop,PropertyAddr);
}

#if WITH_EDITOR
void USkillGraphNode::PostEditImport()
{
	ResetNodeOwner();
	if (NodeInstance)
	{
		InitializeInstance();
	}
}

void USkillGraphNode::PostEditUndo()
{
	UEdGraphNode::PostEditUndo();
	ResetNodeOwner();
	if (ParentNode)
	{
		ParentNode->SubNodes.AddUnique(this);
	}
}
#endif

UEdGraphPin * USkillGraphNode::GetInputPin(int32 InputIndex) const
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

UEdGraphPin * USkillGraphNode::GetOutputPin(int32 InputIndex) const
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

void USkillGraphNode::ResetNodeOwner()
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