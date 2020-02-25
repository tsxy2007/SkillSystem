// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SkillTreeManager.generated.h"

class USkill;
class USTCompositeNode;

USTRUCT()
struct FSkillTreeTemplateInfo
{
	GENERATED_USTRUCT_BODY()

	/** behavior tree asset */
	UPROPERTY()
	USkill* Asset;

	/** initialized template */
	UPROPERTY(transient)
	USTCompositeNode* Template;

	/** size required for instance memory */
	uint16 InstanceMemorySize;
};
DECLARE_STATS_GROUP(TEXT("Skill Tree"), STATGROUP_SkillTree, STATCAT_Advanced);
DECLARE_CYCLE_STAT_EXTERN(TEXT("Load Time"), STAT_ST_SkillTree_LoadTime, STATGROUP_SkillTree, );
/**
 * 
 */
UCLASS()
class SKILLSYSTEM_API USkillTreeManager : public UObject
{
	GENERATED_UCLASS_BODY()
	

public:
	bool LoadTree(USkill& Asset, USTCompositeNode*& Root, uint16& InstanceMemorySize);

	static USkillTreeManager* Get(UObject* InWorldContext);
	void OnWorldCleanedUp(UWorld* World, bool bSessionEnded, bool bCleanupResources);
	virtual UWorld* GetWorld() const override;
	static int32 GetAlignedDataSize(int32 Size);
protected:
	UPROPERTY()
	TArray<FSkillTreeTemplateInfo> LoadedTemplates;
private:
	static USkillTreeManager* Instance;
};
