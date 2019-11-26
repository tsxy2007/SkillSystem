// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SkillTreeManager.generated.h"

class USkill;
class USTCompositeNode;
/**
 * 
 */
UCLASS()
class SKILLSYSTEM_API USkillTreeManager : public UObject
{
	GENERATED_UCLASS_BODY()
	

public:
	bool LoadTree(USkill& Asset, USTCompositeNode* Root, uint16& InstanceMemorySize);

};
