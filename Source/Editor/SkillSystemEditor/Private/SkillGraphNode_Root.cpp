// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillGraphNode_Root.h"
#include "EdGraphSchema_K2.h"
#include "EdGraph/EdGraphPin.h"
#include "SkillTreeEditorTypes.h"


#define LOCTEXT_NAMESPACE "SKillSystem"

USkillGraphNode_Root::USkillGraphNode_Root(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	bIsReadOnly = true;
}

void USkillGraphNode_Root::PostPlacedNewNode()
{
	Super::PostPlacedNewNode();
}

void USkillGraphNode_Root::AllocateDefaultPins()
{
	CreatePin(EGPD_Output, USkillTreeEditorTypes::PinCategory_SingleComposite, TEXT("In"));
}

FText USkillGraphNode_Root::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return NSLOCTEXT("SkillTreeEditor", "Root", "ROOT");
}

FName USkillGraphNode_Root::GetNameIcon() const
{
	return FName("BTEditor.Graph.BTNode.Root.Icon");
}

FText USkillGraphNode_Root::GetTooltipText() const
{
	return UEdGraphNode::GetTooltipText();
}

void USkillGraphNode_Root::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	//TODO:
	/*if (PropertyChangedEvent.Property &&
		PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(USkillGraphNode_Root, BlackboardAsset))
	{
	}*/
}

void USkillGraphNode_Root::PostEditUndo()
{
	Super::PostEditUndo();
	//TODO:
}

FText USkillGraphNode_Root::GetDescription() const
{
	return FText::FromString(GetNameSafe(this));
}

#undef LOCTEXT_NAMESPACE
