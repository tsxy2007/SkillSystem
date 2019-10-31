// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "STCompositeNode.h"
#include "STComposite_Selector.generated.h"

/**
 * 
 */
UCLASS()
class SKILLSYSTEM_API USTComposite_Selector : public USTCompositeNode
{
	GENERATED_UCLASS_BODY()
public:

#if WITH_EDITOR
	virtual FName GetNodeIconName()const override;
#endif
	
};
