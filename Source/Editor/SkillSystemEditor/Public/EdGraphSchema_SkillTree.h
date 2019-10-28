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
	//virtual void CreateDefaultNodesForGraph(UEdGraph& Graph) const override;
	//virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;
	//virtual void GetContextMenuActions(const UEdGraph* CurrentGraph, const UEdGraphNode* InGraphNode, const UEdGraphPin* InGraphPin, class FMenuBuilder* MenuBuilder, bool bIsDebugging) const override;
	//virtual const FPinConnectionResponse CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const override;
	//virtual const FPinConnectionResponse CanMergeNodes(const UEdGraphNode* A, const UEdGraphNode* B) const override;
	//virtual FLinearColor GetPinTypeColor(const FEdGraphPinType& PinType) const override;
	//~ End EdGraphSchema Interface
};
