// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillFactory.h"
#include "Skill.h"

#define LOCTEXT_NAMESPACE "SkillFactory"
USkillFactory::USkillFactory(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	SupportedClass = USkill::StaticClass();
	bCreateNew = true;
	bEditAfterNew = true;
}

UObject* USkillFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	check(Class->IsChildOf(USkill::StaticClass()));
	return NewObject<USkill>(InParent, Class, Name, Flags);;
}

bool USkillFactory::CanCreateNew() const
{
	return true;
}
FText USkillFactory::GetDisplayName() const
{
	return LOCTEXT("SkillFactoryDescription", "Skill");;
}

#undef LOCTEXT_NAMESPACE
