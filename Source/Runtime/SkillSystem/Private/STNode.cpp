// Fill out your copyright notice in the Description page of Project Settings.


#include "STNode.h"

USTNode::USTNode(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{

}

UWorld* USTNode::GetWorld() const
{
	if (GetOuter() == nullptr)
	{
		return nullptr;
	}
	if (Cast<UPackage>(GetOuter()) != nullptr)
	{
		return Cast<UWorld>(GetOuter()->GetOuter());
	}
	return GetOuter()->GetWorld();
}

void USTNode::InitializeNode(USTCompositeNode* InParentNode, uint16 InExecutionIndex, uint16 InMemoryOffset, uint8 InTreeDepth)
{

}

void USTNode::InitializeFromAsset(USkill& Asset)
{

}

void USTNode::InitializeMemory(USkillTreeComponent& OwnerComp, uint8* NodeMemory, ESTMemoryInit::Type InitType) const
{

}

void USTNode::CleanupMemory(USkillTreeComponent& OwnerComp, uint8* NodeMemory, ESTMemoryClear::Type CleanupType) const
{

}

void USTNode::OnInstanceCreated(USkillTreeComponent& OwnerComp)
{

}

UGameplayTasksComponent * USTNode::GetGameplayTasksComponent(const UGameplayTask & Task) const
{
	return nullptr;
}

AActor * USTNode::GetGameplayTaskOwner(const UGameplayTask * Task) const
{
	return nullptr;
}

AActor * USTNode::GetGameplayTaskAvatar(const UGameplayTask * Task) const
{
	return nullptr;
}

uint8 USTNode::GetGameplayTaskDefaultPriority() const
{
	return uint8();
}

void USTNode::OnGameplayTaskInitialized(UGameplayTask & Task)
{
}

void USTNode::OnGameplayTaskActivated(UGameplayTask & Task)
{
}

void USTNode::OnGameplayTaskDeactivated(UGameplayTask & Task)
{
}



#if WITH_EDITOR

FName USTNode::GetNodeIconName() const
{
	return NAME_None;
}
FString USTNode::GetNodeName() const
{
	return NodeName.Len() ? NodeName : USkillTreeTypes::GetShortTypeName(this);
}
#endif
