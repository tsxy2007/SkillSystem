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

	virtual void PostPlacedNewNode() override;
	virtual void AllocateDefaultPins() override;
	virtual bool CanDuplicateNode() const override { return false; }
	virtual bool CanUserDeleteNode() const override { return false; }
	virtual bool HasErrors()const override { return false; }
	virtual bool RefreshNodeClass() override { return false; }
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const  override;

	// Get Icon resource name for title bar
	virtual FName GetNameIcon() const override;
	virtual FText GetTooltipText() const override;

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostEditUndo() override;

	virtual FText GetDescription() const override;

	// notify skill tree about 
};
