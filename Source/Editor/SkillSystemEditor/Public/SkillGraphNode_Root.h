// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SkillTreeGraphNode.h"
#include "Textures/SlateIcon.h"
#include "SkillGraphNode_Root.generated.h"

/**
 * 
 */
UCLASS()
class SKILLSYSTEMEDITOR_API USkillGraphNode_Root : public USkillTreeGraphNode
{
	GENERATED_UCLASS_BODY()
public:
	//~ Begin UEdGraphNode Interface
	virtual void AllocateDefaultPins() override;
	virtual FText GetTooltipText() const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override;
	//~ End UEdGraphNode Interface
};
