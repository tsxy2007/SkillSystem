// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "STGraphSchema.h"
#include "EdGraphSchema_SkillTree.generated.h"

class FSlateRect;
class UEdGraph;

USTRUCT()
struct FSkillTreeSchemaAction_AutoArrange : public FEdGraphSchemaAction
{
	GENERATED_USTRUCT_BODY();
public:
	FSkillTreeSchemaAction_AutoArrange()
		:FEdGraphSchemaAction()
	{

	}
	FSkillTreeSchemaAction_AutoArrange(FText InNodeCategory,FText InMenuDesc,FText InToolTip,const int32 InGrouping)
		:FEdGraphSchemaAction(MoveTemp(InNodeCategory),MoveTemp(InMenuDesc),MoveTemp(InToolTip),InGrouping)
	{}
	
	// ~ Begin FEdgraphSchemaAction Interface
	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
	// ~ End FEdGraphSchemaAction Interface
};


USTRUCT()
struct FSkillTreeSchemaAction_AddComment : public FEdGraphSchemaAction
{
	GENERATED_BODY()
public:
	FSkillTreeSchemaAction_AddComment():FEdGraphSchemaAction(){}
	FSkillTreeSchemaAction_AddComment(FText InDescription,FText InToolTip)
		: FEdGraphSchemaAction(FText(),MoveTemp(InDescription),MoveTemp(InToolTip),0)
	{}
	// ~ Begin FEdgraphSchemaAction Interface
	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
	// ~ End FEdGraphSchemaAction Interface
};

/**
 * 
 */
UCLASS()
class SKILLSYSTEMEDITOR_API UEdGraphSchema_SkillTree : public USTGraphSchema
{
	GENERATED_UCLASS_BODY()
public:
	//~ Begin EdgraphSchema Interface
	virtual void CreateDefaultNodesForGraph(UEdGraph& Graph) const override;
	virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;
	virtual void GetContextMenuActions(const UEdGraph* CurrentGraph, const UEdGraphNode* InGraphNode, const UEdGraphPin* InGraphPin, class FMenuBuilder* MenuBuilder, bool bIsDebugging) const override;
	virtual const FPinConnectionResponse CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const override;
	virtual const FPinConnectionResponse CanMergeNodes(const UEdGraphNode* A, const UEdGraphNode* B) const override;
	virtual FLinearColor GetPinTypeColor(const FEdGraphPinType& PinType) const override;
	virtual class FConnectionDrawingPolicy* CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, class FSlateWindowElementList& InDrawElements, class UEdGraph* InGraphObj) const override;
	virtual int32 GetNodeSelectionCount(const UEdGraph* Graph) const override;
	virtual bool IsCacheVisualizationOutOfDate(int32 InVisualizationCacheID) const override;
	virtual int32 GetCurrentVisualizationCacheID()const override;
	virtual void ForceVisualizationCacheClear()const override;
	virtual TSharedPtr<FEdGraphSchemaAction> GetCreateCommentAction() const override;
	//~ End EdGraphSchema Interface

	virtual void GetGraphNodeContextActions(FGraphContextMenuBuilder& ContextMenuBuilder, int32 SubNodeFlags) const override;
	virtual void GetSubNodeClasses(int32 SubNodeFlags, TArray<FSkillGraphNodeClassData>& ClassData, UClass*& GraphNodeClass) const override;
private:
	static int32 CurrentCacheRefreshID;
};
