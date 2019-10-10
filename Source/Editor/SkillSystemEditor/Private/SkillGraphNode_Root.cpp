// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillGraphNode_Root.h"
#include "EdGraphSchema_K2.h"
#include "EdGraph/EdGraphPin.h"
#include "SkillTreeEditorTypes.h"


#define LOCTEXT_NAMESPACE "SKillSystem"

USkillGraphNode_Root::USkillGraphNode_Root(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{

}

void USkillGraphNode_Root::AllocateDefaultPins()
{
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
	Super::AllocateDefaultPins();
}

FText USkillGraphNode_Root::GetTooltipText() const
{
	return NSLOCTEXT("STNode", "ExecutePinInOrder_Tooltip", "Executes a series of pins in order");
}

FLinearColor USkillGraphNode_Root::GetNodeTitleColor() const
{
	return FLinearColor::White;
}

FText USkillGraphNode_Root::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return NSLOCTEXT("STNode", "Root", "Root");
}

FSlateIcon USkillGraphNode_Root::GetIconAndTint(FLinearColor& OutColor) const
{
	static FSlateIcon Icon("EditorStyle", "GraphEditor.Sequence_16x");
	return Icon;
}

#undef LOCTEXT_NAMESPACE
