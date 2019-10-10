// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Toolkits/IToolkitHost.h"
#include "AssetTypeActions_Base.h"
/**
 * 
 */
class SKILLSYSTEMEDITOR_API FAssetTypeActions_Skill : public FAssetTypeActions_Base
{
public:
	// IAssetTypeActions Implementation
	virtual FText GetName() const override { return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_Skill", "Skill"); }
	virtual FColor GetTypeColor() const override { return FColor(149, 70, 255); }
	virtual UClass* GetSupportedClass() const override;
	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor = TSharedPtr<IToolkitHost>()) override;
	virtual uint32 GetCategories() override;
	virtual void PerformAssetDiff(UObject* Asset1, UObject* Asset2, const struct FRevisionInfo& OldRevision, const struct FRevisionInfo& NewRevision) const override;

private:

	/* Called to open the Behavior Tree defaults view, this opens whatever text dif tool the user has */
	void OpenInDefaults(class USkill* OldBehaviorTree, class USkill* NewBehaviorTree) const;
};