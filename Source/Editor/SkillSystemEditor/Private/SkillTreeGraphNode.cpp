// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillTreeGraphNode.h"
#include "SkillTreeEditorTypes.h"
#include "STNode.h"
#include "Skill.h"

USkillTreeGraphNode::USkillTreeGraphNode(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	bHighlightInAbortRange0 = false;
	bHighlightInAbortRange1 = false;
	bHighlightInSearchRange0 = false;
	bHighlightInSearchRange1 = false;
	bHighlightInSearchTree = false;
	bHighlightChildNodeIndices = false;
	bRootLevel = false;
	bInjectedNode = false;
	bHasObserverError = false;
	bHasBreakpoint = false;
	bIsBreakpointEnabled = false;
	bDebuggerMarkCurrentlyActive = false;
	bDebuggerMarkPreviouslyActive = false;
	bDebuggerMarkFlashActive = false;
	bDebuggerMarkSearchSucceeded = false;
	bDebuggerMarkSearchFailed = false;
	bDebuggerMarkSearchTrigger = false;
	bDebuggerMarkSearchFailedTrigger = false;
	DebuggerSearchPathIndex = -1;
	DebuggerSearchPathSize = 0;
	DebuggerUpdateCounter = -1;
}

USkillEdGraph * USkillTreeGraphNode::GetSkillTreeGraph()
{
	return CastChecked<USkillEdGraph>(GetGraph());
}

void USkillTreeGraphNode::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, USkillTreeEditorTypes::PinCategory_MultipleNodes, TEXT("In"));
	CreatePin(EGPD_Output, USkillTreeEditorTypes::PinCategory_MultipleNodes, TEXT("Out"));
}

FText USkillTreeGraphNode::GetTooltipText() const
{
	return FText();
}

bool USkillTreeGraphNode::CanCreateUnderSpecifiedSchema(const UEdGraphSchema * Schema) const
{
	return false;
}

void USkillTreeGraphNode::FindDiffs(UEdGraphNode * OtherNode, FDiffResults & Results)
{
}

void USkillTreeGraphNode::PostEditUndo()
{
}

FText USkillTreeGraphNode::GetDescription() const
{
	return FText();
}

bool USkillTreeGraphNode::HasErrors() const
{
	return false;
}

void USkillTreeGraphNode::InitializeInstance()
{
	USTNode* STNode = Cast<USTNode>(NodeInstance);
	USkill* STAsset = STNode ? Cast<USkill>(STNode->GetOuter()) : nullptr;
	if (STNode && STAsset)
	{
		
	}
}

void USkillTreeGraphNode::OnSubNodeAdded(USTGraphNode * SubNode)
{
}

void USkillTreeGraphNode::OnSubNodeRemoved(USTGraphNode * SubNode)
{
}

void USkillTreeGraphNode::RemoveAllSubNodes()
{
}

int32 USkillTreeGraphNode::FindSubNodeDropIndex(USTGraphNode * SubNode) const
{
	return int32();
}

void USkillTreeGraphNode::InsertSubNodeAt(USTGraphNode * SubNode, int32 DropIndex)
{
}

void USkillTreeGraphNode::ClearDebuggerState()
{
}

FName USkillTreeGraphNode::GetNameIcon() const
{
	return FName();
}

void USkillTreeGraphNode::CreateAddDecoratorSubMenu(FMenuBuilder & MenuBuilder, UEdGraph * Graph) const
{
}

void USkillTreeGraphNode::CreateAddServiceSubMenu(FMenuBuilder & MenuBuilder, UEdGraph * Graph) const
{
}

void USkillTreeGraphNode::AddContextMenuActionsDecorators(const FGraphNodeContextMenuBuilder & Context) const
{
}

void USkillTreeGraphNode::AddContextMenuActionsServices(const FGraphNodeContextMenuBuilder & Context) const
{
}
