// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillGraphNode_Composite.h"
#include "STNode.h"
#include "STCompositeNode.h"

USkillGraphNode_Composite::USkillGraphNode_Composite(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{

}

FText USkillGraphNode_Composite::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	const USTNode* MyNode = Cast<USTNode>(NodeInstance);
	if (MyNode != nullptr)
	{
		return FText::FromString(MyNode->GetNodeName());
	}
	return Super::GetNodeTitle(TitleType);
}

FText USkillGraphNode_Composite::GetDescription() const
{
	const USTCompositeNode* CompositeNode = Cast<USTCompositeNode>(NodeInstance);
	if (CompositeNode&& CompositeNode->IsApplyingDecoratorScope())
	{
		return FText::Format(FText::FromString(TEXT("{0}\n{1}")),
			Super::GetDescription(),
			NSLOCTEXT("SkillTreeEditor", "CompositeNodeScopeDesc", "Local scope for observers"));
	}
	return Super::GetDescription();
}

FText USkillGraphNode_Composite::GetTooltipText() const
{
	const USTCompositeNode* CompositeNode = Cast<USTCompositeNode>(NodeInstance);
	if (CompositeNode&& CompositeNode->IsApplyingDecoratorScope())
	{
		return FText::Format(FText::FromString(TEXT("{0}\n\n{1}")),
			Super::GetDescription(),
			NSLOCTEXT("SkillTreeEditor", "CompositeNodeScopeTooltip", "Local scope for observers"));
	}
	return Super::GetDescription();
}

void USkillGraphNode_Composite::GetContextMenuActions(const FGraphNodeContextMenuBuilder& Context) const
{
	AddContextMenuActionsDecorators(Context);
	AddContextMenuActionsServices(Context);
}
