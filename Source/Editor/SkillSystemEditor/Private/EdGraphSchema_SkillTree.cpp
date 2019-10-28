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
#include "SkillEdGraph.h"

#define LOCTEXT_NAMESPACE "SkillTreeEditor"


UEdGraphNode* FSkillTreeSchemaAction_AutoArrange::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode /*= true*/)
{
	USkillEdGraph* Graph = Cast<USkillEdGraph>(ParentGraph);
	if (Graph)
	{
		Graph->AutoArrange();
	}
	return NULL;
}

UEdGraphNode* FSkillTreeSchemaAction_AddComment::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode /*= true*/)
{

	return nullptr;
}


UEdGraphSchema_SkillTree::UEdGraphSchema_SkillTree(const FObjectInitializer& Obj)
	:Super(Obj)
{

}