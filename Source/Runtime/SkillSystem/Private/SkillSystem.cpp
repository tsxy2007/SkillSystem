// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "SkillSystem.h"

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