// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillTreeComponent.h"
#include "Skill.h"
#include "SkillTreeManager.h"
#include "VisualLogger.h"
#include "STCompositeNode.h"

// Sets default values for this component's properties
USkillTreeComponent::USkillTreeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USkillTreeComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void USkillTreeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USkillTreeComponent::StartTree(USkill& Asset)
{

	// ¼ÓÔØSkill 
	USkillTreeManager* STManager = USkillTreeManager::Get(this);
	if (STManager == nullptr)
	{
		UE_VLOG(GetOwner(), LogTemp, Warning, TEXT("USkillTreeComponent::StartTree"));
		return;
	}

	if (!CanCastSkill(Asset))
	{
		return;
	}

	USTCompositeNode* RootNode = nullptr;
	uint16 InstanceMemorySize = 0;
	STManager->LoadTree(Asset, RootNode, InstanceMemorySize);
	if (RootNode)
	{
		RootNode->DoActive();
	}
}

void USkillTreeComponent::RunSkillTree(USkill* Skill)
{
	if (Skill)
	{
		StartTree(*Skill);
	}
}

bool USkillTreeComponent::CanCastSkill(USkill& Skill)
{

	return true;
}

