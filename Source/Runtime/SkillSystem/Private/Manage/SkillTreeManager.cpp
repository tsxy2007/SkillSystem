// Fill out your copyright notice in the Description page of Project Settings.


#include "Manage/SkillTreeManager.h"
#include "Skill.h"
#include "STCompositeNode.h"

USkillTreeManager::USkillTreeManager(const FObjectInitializer& Obj)
	:Super(Obj)
{

}

bool USkillTreeManager::LoadTree(USkill& Asset, USTCompositeNode* Root, uint16& InstanceMemorySize)
{

	return true;
}