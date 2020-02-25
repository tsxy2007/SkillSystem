// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "SkillSystem.h"
#include "SkillTreeManager.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/World.h"

IMPLEMENT_GAME_MODULE(FSkillSystemModule, SkillSystem);
DEFINE_LOG_CATEGORY(LogSkill);

void FSkillSystemModule::StartupModule()
{
	
}

void FSkillSystemModule::ShutdownModule()
{
	
}

bool FSkillSystemModule::IsGameModule() const
{
	return true;
}

void FSkillSystemModule::SetManager(class USkillTreeManager* NewManager)
{
	SkillTreeManager = NewManager;
}

class USkillTreeManager* FSkillSystemModule::GetCurrentManager()
{
	return SkillTreeManager.Get();
}

FSkillSystemModule& FSkillSystemModule::Get()
{
	static const FName FunctionalTesting(TEXT("SkillSystem"));
	return FModuleManager::Get().LoadModuleChecked<FSkillSystemModule>(FunctionalTesting);
}

