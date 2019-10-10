// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "EdGraph/EdGraphSchema.h"
#include "EdGraphSchema_SkillTree.generated.h"

class FSlateRect;
class UEdGraph;
class USkillGraphNode;

USTRUCT()
struct SKILLSYSTEMEDITOR_API FSkillSchemaAction_NewNode : public FEdGraphSchemaAction
{
	GENERATED_USTRUCT_BODY();
public:
	UPROPERTY()
		USkillGraphNode* NodeTemplate;

	FSkillSchemaAction_NewNode()
		:FEdGraphSchemaAction()
		, NodeTemplate(nullptr)
	{

	}

	FSkillSchemaAction_NewNode(FText InNodeCategory,FText InMenuDesc,FText InToolTip,const int32 InGrouping)
		:FEdGraphSchemaAction(MoveTemp(InNodeCategory),MoveTemp(InMenuDesc),MoveTemp(InToolTip),InGrouping)
		,NodeTemplate(nullptr)
	{}

	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, TArray<UEdGraphPin*>& FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

	template<typename NodeType>
	static NodeType* SpawnNodeFromTemplate(class UEdGraph* ParentGraph, NodeType* InTemplateNode, const FVector2D Location)
	{
		FSkillSchemaAction_NewNode Action;
		Action.NodeTemplate = InTemplateNode;
		return Cast<NodeType>(Action.PerformAction(ParentGraph, nullptr, Location));
	}
};

USTRUCT()
struct SKILLSYSTEMEDITOR_API FSkillSchemaAction_NewSubNode : public FEdGraphSchemaAction
{
	GENERATED_USTRUCT_BODY();
public:
	UPROPERTY()
		USkillGraphNode* NodeTemplate;
	UPROPERTY()
		USkillGraphNode* ParentNode;
	FSkillSchemaAction_NewSubNode()
		:FEdGraphSchemaAction()
		,NodeTemplate(nullptr)
		,ParentNode(nullptr)
	{}
	FSkillSchemaAction_NewSubNode(FText InNodeCategory, FText InMenuDesc, FText InToolTip, const int32 InGrouping)
		:FEdGraphSchemaAction(MoveTemp(InNodeCategory), MoveTemp(InMenuDesc), MoveTemp(InToolTip), InGrouping)
		, NodeTemplate(nullptr)
		, ParentNode(nullptr)
	{
	}
	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true);
	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, TArray<UEdGraphPin *>& FromPins, const FVector2D Location, bool bSelectNewNode = true);
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
};

/**
 * 
 */
UCLASS()
class SKILLSYSTEMEDITOR_API UEdGraphSchema_SkillTree : public UEdGraphSchema
{
	GENERATED_UCLASS_BODY()
public:
	void GetBreakLinkToSubMenuActions(class FMenuBuilder& MenuBuilder, class UEdGraphPin* InGraphPin);

	//~ Begin EdGraphSchema Interface
	virtual void CreateDefaultNodesForGraph(UEdGraph& Graph) const override;
	virtual void GetContextMenuActions(const UEdGraph* CurrentGraph, const UEdGraphNode* InGraphNode, const UEdGraphPin* InGraphPin, class FMenuBuilder* MenuBuilder, bool bIsDebugging) const override;
	virtual FLinearColor GetPinTypeColor(const FEdGraphPinType& PinType) const override;
	virtual bool ShouldHidePinDefaultValue(UEdGraphPin* Pin) const override;
	virtual class FConnectionDrawingPolicy* CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, class FSlateWindowElementList& InDrawElements, class UEdGraph* InGraphObj) const override;
	virtual void BreakNodeLinks(UEdGraphNode& TargetNode) const override;
	virtual void BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotification) const override;
	virtual void BreakSinglePinLink(UEdGraphPin* SourcePin, UEdGraphPin* TargetPin) const override;

	virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;
	//~ End EdGraphSchema Interface


	//virtual void GetGraphNodeContextActions(FGraphContextMenuBuilder& ContextMenuBuilder, int32 SubNodeFlags) const;
	//virtual void GetSubNodeClasses(int32 SubNodeFlags, TArray<FGraphNodeClassData>& ClassData, UClass*& GraphNodeClass) const;

protected:
	static TSharedPtr<FSkillSchemaAction_NewNode> AddNewNodeAction(FGraphActionListBuilderBase& ContextMenuBuilder, const FText& Category, const FText& MenuDesc, const FText& Tooltip);
	static TSharedPtr<FSkillSchemaAction_NewSubNode> AddNewSubNodeAction(FGraphActionListBuilderBase& ContextMenuBuilder, const FText& Category, const FText& MenuDesc, const FText& Tooltip);
};
