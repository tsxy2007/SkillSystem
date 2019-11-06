// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SkillTreeGraphNode.h"
#include "SkillGraphNode_Composite.generated.h"

/**
 * 
 */
UCLASS()
class SKILLSYSTEMEDITOR_API USkillGraphNode_Composite : public USkillTreeGraphNode
{
	GENERATED_UCLASS_BODY()
	
public:

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType)const override;
	virtual FText GetDescription() const override;
	virtual FText GetTooltipText() const override;
	virtual bool RefreshNodeClass() override { return false; }

	virtual void GetContextMenuActions(const FGraphNodeContextMenuBuilder& Context) const override;

	virtual bool CanPlaceBreakpoints() const override { return true; }
};
