// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "SkillTreeTypes.h"
#include "GameplayTaskOwnerInterface.h"
#include "STNode.generated.h"

class UWorld;
class USkill;
class USTCompositeNode;
class USkillTreeComponent;
struct FSkillTreeSearchData;

struct FSTInstanceNodeMemory
{
	int32 NodeIdx;
};
/**
 * 
 */
UCLASS()
class SKILLSYSTEM_API USTNode : public UObject , public IGameplayTaskOwnerInterface
{
	GENERATED_UCLASS_BODY()
public:
	virtual UWorld* GetWorld() const override;
	
	// fill in data about tree structure
	void InitializeNode(USTCompositeNode* InParentNode,uint16 InExecutionIndex,uint16 InMemoryOffset,uint8 InTreeDepth);

	// initialize any asset related data
	virtual void InitializeFromAsset(USkill& Asset);

	//// initialize memory block
	//virtual void InitializeMemory(USkillTreeComponent& OwnerComp, uint8* NodeMemory, ESTMemoryInit::Type InitType)const;

	//// cleanup memory block
	//virtual void CleanupMemory(USkillTreeComponent& OwnerComp, uint8* NodeMemory, ESTMemoryClear::Type CleanupType) const;


	//// called when node instance is added to tree
	//virtual void OnInstanceCreated(USkillTreeComponent& OwnerComp);


	//~Begin GamePlayTaskOwnerInterface
	virtual UGameplayTasksComponent* GetGameplayTasksComponent(const UGameplayTask& Task) const  override;
	virtual AActor* GetGameplayTaskOwner(const UGameplayTask* Task) const override;
	virtual AActor* GetGameplayTaskAvatar(const UGameplayTask* Task) const override;
	virtual uint8 GetGameplayTaskDefaultPriority() const override;
	virtual void OnGameplayTaskInitialized(UGameplayTask& Task) override;
	virtual void OnGameplayTaskActivated(UGameplayTask& Task) override;
	virtual void OnGameplayTaskDeactivated(UGameplayTask& Task) override;
	//~End GamePlayTaskOwnerInterface


	USkill* GetTreeAsset() const;

	USTCompositeNode* GetParentNode()const;
#if USE_SKILLTREE_DEBUGGER
	USTNode* GetNextNode()const;
#endif
	uint16 GetExecutionIndex()const;
	uint16 GetMemoryOffset()const;
	uint8 GetTreeDepth()const;
	void MarkInjectedNode();
	bool IsInjected()const;
	void ForceInstancing(bool bEnable);
	bool HasInstance()const;
	bool IsInstanced()const;
private:
	UPROPERTY()
		USkill* TreeAsset;

	UPROPERTY()
		USTCompositeNode* ParentNode;

#if USE_SKILLTREE_DEBUGGER
	USTNode* NextExecutionNode;
#endif

	uint16 ExecutionIndex;

	uint16 MemoryOffset;

	uint8 TreeDepth;

	uint8 bIsInstanced : 1;

	uint8 bIsInjected : 1;

protected:
	uint8 bCreateNodeInstance : 1;

	uint8 bOwnsGamePlayTasks : 1;

};


FORCEINLINE USkill* USTNode::GetTreeAsset()const
{
	return TreeAsset;
}

FORCEINLINE USTCompositeNode* USTNode::GetParentNode() const
{
	return ParentNode;
}
#if USE_SKILLTREE_DEBUGGER
FORCEINLINE USTNode* USTNode::GetNextNode()const
{
	return NextExecutionNode;
}
#endif

FORCEINLINE uint16 USTNode::GetExecutionIndex()const
{
	return ExecutionIndex;
}

FORCEINLINE uint16 USTNode::GetMemoryOffset()const
{
	return MemoryOffset;
}

FORCEINLINE uint8 USTNode::GetTreeDepth()const
{
	return TreeDepth;
}

FORCEINLINE void USTNode::MarkInjectedNode()
{
	bIsInjected = true;
}

FORCEINLINE bool USTNode::IsInjected()const
{
	return bIsInjected;
}

FORCEINLINE void USTNode::ForceInstancing(bool bEnable)
{
	check(ParentNode == NULL);
	bCreateNodeInstance = bEnable;
}

FORCEINLINE bool USTNode::HasInstance()const
{
	return bCreateNodeInstance;
}

FORCEINLINE bool USTNode::IsInstanced()const
{
	return bIsInstanced;
}
