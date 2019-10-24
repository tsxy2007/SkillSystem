// Fill out your copyright notice in the Description page of Project Settings.


#include "EdGraphSchema_SkillTree.h"
#include "ScopedTransaction.h"
#include "STGraphNode.h"
#include "EdGraph/EdGraph.h"
#include "SkillSystemEditor.h"
#include "SkillGraphTypes.h"
#include "STNode.h"
#include "SkillGraphNode_Composite.h"
#include "SkillGraphNode_Root.h"
#include "SkillTreeConnectionDrawingPolicy.h"

#define LOCTEXT_NAMESPACE "SkillGraph"
#define SNAP_GRID (16)
namespace
{
	const int32 NodeDistance = 60;
}

UEdGraphNode* FSkillSchemaAction_NewNode::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode /*= true*/)
{
	UEdGraphNode* ResultNode = NULL;
	if (NodeTemplate!=NULL)
	{
		const FScopedTransaction Transaction(LOCTEXT("AddNode", "Add Node"));
		ParentGraph->Modify();
		if (FromPin)
		{
			FromPin->Modify();
		}
		NodeTemplate->SetFlags(RF_Transactional);

		NodeTemplate->Rename(NULL, ParentGraph, REN_NonTransactional);
		ParentGraph->AddNode(NodeTemplate, true);
		NodeTemplate->CreateNewGuid();
		NodeTemplate->PostPlacedNewNode();

		int32 XLocation = Location.X;
		if (FromPin && FromPin->Direction == EGPD_Input)
		{
			UEdGraphNode* PinNode = FromPin->GetOwningNode();
			const float XDelta = FMath::Abs(PinNode->NodePosX - Location.X);
			if (XDelta < NodeDistance)
			{
				XLocation = PinNode->NodePosX - NodeDistance;
			}
		}
		NodeTemplate->NodePosX = XLocation;
		NodeTemplate->NodePosY = Location.Y;
		NodeTemplate->SnapToGrid(SNAP_GRID);

		NodeTemplate->AllocateDefaultPins();
		NodeTemplate->AutowireNewNode(FromPin);
		ResultNode = NodeTemplate;
	}
	return ResultNode;
}

UEdGraphNode* FSkillSchemaAction_NewNode::PerformAction(class UEdGraph* ParentGraph, TArray<UEdGraphPin*>& FromPins, const FVector2D Location, bool bSelectNewNode /*= true*/)
{
	UEdGraphNode* ResultNode = nullptr;
	if (FromPins.Num()>0)
	{
		ResultNode = PerformAction(ParentGraph, FromPins[0], Location);
		for (int32 Index = 1; Index < FromPins.Num(); Index++)
		{
			ResultNode->AutowireNewNode(FromPins[Index]);
		}
	}
	else
	{
		ResultNode = PerformAction(ParentGraph, NULL, Location,bSelectNewNode);
	}
	return ResultNode;
}

void FSkillSchemaAction_NewNode::AddReferencedObjects(FReferenceCollector& Collector)
{
	FEdGraphSchemaAction::AddReferencedObjects(Collector);
	Collector.AddReferencedObject(NodeTemplate);
}


UEdGraphNode* FSkillSchemaAction_NewSubNode::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode /*= true*/)
{
	ParentNode->AddSubNode(NodeTemplate, ParentGraph);
	return NULL;
}


UEdGraphNode* FSkillSchemaAction_NewSubNode::PerformAction(class UEdGraph* ParentGraph, TArray<UEdGraphPin *>& FromPins, const FVector2D Location, bool bSelectNewNode /*= true*/)
{
	return PerformAction(ParentGraph, NULL, Location, bSelectNewNode);
}

void FSkillSchemaAction_NewSubNode::AddReferencedObjects(FReferenceCollector& Collector)
{
	FEdGraphSchemaAction::AddReferencedObjects(Collector);
	Collector.AddReferencedObject(NodeTemplate);
	Collector.AddReferencedObject(ParentNode);
}

UEdGraphSchema_SkillTree::UEdGraphSchema_SkillTree(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{

}

void UEdGraphSchema_SkillTree::GetBreakLinkToSubMenuActions(class FMenuBuilder& MenuBuilder, class UEdGraphPin* InGraphPin)
{

}

void UEdGraphSchema_SkillTree::CreateDefaultNodesForGraph(UEdGraph& Graph) const
{
	FGraphNodeCreator<USkillGraphNode_Root> NodeCreator(Graph);
	USkillGraphNode_Root* MyRoot = NodeCreator.CreateNode();
	NodeCreator.Finalize();
	SetNodeMetaData(MyRoot, FNodeMetadata::DefaultGraphNode);
}

void UEdGraphSchema_SkillTree::GetContextMenuActions(const UEdGraph* CurrentGraph, const UEdGraphNode* InGraphNode, const UEdGraphPin* InGraphPin, class FMenuBuilder* MenuBuilder, bool bIsDebugging) const
{

}

const FPinConnectionResponse UEdGraphSchema_SkillTree::CanCreateConnection(const UEdGraphPin * A, const UEdGraphPin * B) const
{
	if (A->GetOwningNode() == B->GetOwningNode())
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("PinErrorSameNode", "Both are on the same node"));
	}
	
	return FPinConnectionResponse(CONNECT_RESPONSE_MAKE,LOCTEXT("PinConnect","Connect nodes"));
}

FLinearColor UEdGraphSchema_SkillTree::GetPinTypeColor(const FEdGraphPinType& PinType) const
{
	return FLinearColor::White;
}

bool UEdGraphSchema_SkillTree::ShouldHidePinDefaultValue(UEdGraphPin* Pin) const
{
	check(Pin != nullptr);
	return Pin->bDefaultValueIsIgnored;
}

class FConnectionDrawingPolicy* UEdGraphSchema_SkillTree::CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, class FSlateWindowElementList& InDrawElements, class UEdGraph* InGraphObj) const
{
	return new FSkillTreeConnectionDrawingPolicy(InBackLayerID,InFrontLayerID,InZoomFactor,InClippingRect,InDrawElements,InGraphObj);
}

void UEdGraphSchema_SkillTree::BreakNodeLinks(UEdGraphNode& TargetNode) const
{

}

void UEdGraphSchema_SkillTree::BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotification) const
{

}

void UEdGraphSchema_SkillTree::BreakSinglePinLink(UEdGraphPin* SourcePin, UEdGraphPin* TargetPin) const
{

}

void UEdGraphSchema_SkillTree::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	FSkillSystemEditorModule& SkillModule = FModuleManager::GetModuleChecked<FSkillSystemEditorModule>(TEXT("SkillSystemEditor"));
	FSkillGraphNodeClassHelper* ClassCache = SkillModule.GetClassCache().Get();

	FCategorizedGraphActionListBuilder NodeBuilder(TEXT("Nodes"));
	TArray<FSkillGraphNodeClassData> NodeClasses;
	ClassCache->GatherClasses(USTNode::StaticClass(), NodeClasses);

	for (const auto& NodeClass : NodeClasses)
	{
		const FText NodeTypeName = FText::FromString(FName::NameToDisplayString(NodeClass.ToString(), false));

		TSharedPtr<FSkillSchemaAction_NewNode> AddOpAction = UEdGraphSchema_SkillTree::AddNewNodeAction(NodeBuilder, NodeClass.GetCategory(), NodeTypeName, FText::GetEmpty());

		UClass* GraphNodeClass = USkillGraphNode_Composite::StaticClass();

		USTGraphNode* OpNode = NewObject<USTGraphNode>(ContextMenuBuilder.OwnerOfTemporaries, GraphNodeClass);
		OpNode->ClassData = NodeClass;
		AddOpAction->NodeTemplate = OpNode;
	}

	ContextMenuBuilder.Append(NodeBuilder);
}

TSharedPtr<FSkillSchemaAction_NewNode> UEdGraphSchema_SkillTree::AddNewNodeAction(FGraphActionListBuilderBase& ContextMenuBuilder, const FText& Category, const FText& MenuDesc, const FText& Tooltip)
{
	TSharedPtr<FSkillSchemaAction_NewNode> NewAction = TSharedPtr<FSkillSchemaAction_NewNode>(new FSkillSchemaAction_NewNode(Category, MenuDesc, Tooltip, 0));
	ContextMenuBuilder.AddAction(NewAction);
	return NewAction;
}

TSharedPtr<FSkillSchemaAction_NewSubNode> UEdGraphSchema_SkillTree::AddNewSubNodeAction(FGraphActionListBuilderBase& ContextMenuBuilder, const FText& Category, const FText& MenuDesc, const FText& Tooltip)
{
	TSharedPtr<FSkillSchemaAction_NewSubNode> NewAction = TSharedPtr<FSkillSchemaAction_NewSubNode>(new FSkillSchemaAction_NewSubNode(Category, MenuDesc, Tooltip, 0));
	ContextMenuBuilder.AddAction(NewAction);
	return NewAction;
}

#undef LOCTEXT_NAMESPACE
