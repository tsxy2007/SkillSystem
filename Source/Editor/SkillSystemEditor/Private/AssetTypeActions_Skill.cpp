// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetTypeActions_Skill.h"
#include "Framework/Application/SlateApplication.h"
#include "Modules/ModuleManager.h"
#include "SkillSystem.h"
#include "Skill.h"
#include "SkillSystemEditor.h"
#include "SkillEditor.h"

#define  LOCTEXT_NAMESPACE "AssetTypeActions"
UClass* FAssetTypeActions_Skill::GetSupportedClass() const
{
	return USkill::StaticClass();
}

void FAssetTypeActions_Skill::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor /*= TSharedPtr<IToolkitHost>()*/)
{
	EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;
	for (auto Object : InObjects)
	{
		auto Skill = Cast<USkill>(Object);
		if (Skill != nullptr)
		{
			bool  bFoundInAsset = false;
			bool bOPen= false;
			{
				FSkillEditor* ExistingInstance = static_cast<FSkillEditor*>(FAssetEditorManager::Get().FindEditorForAsset(Skill,bOPen));
				if (ExistingInstance != nullptr && ExistingInstance->GetSKillTree() == nullptr)
				{
					ExistingInstance->InitSkillEditor(Mode,EditWithinLevelEditor,Skill);
					bFoundInAsset = true;
				}
			}
			if (!bFoundInAsset)
			{
				FSkillSystemEditorModule& SkillSystemEditorModule = FModuleManager::LoadModuleChecked<FSkillSystemEditorModule>("SkillSystemEditor").Get();
				TSharedRef<FSkillEditor> ExistingInstance = SkillSystemEditorModule.CreateSkillSystemEditor(Mode,EditWithinLevelEditor,Skill);
			}
		}
	}
}

uint32 FAssetTypeActions_Skill::GetCategories()
{
	FSkillSystemEditorModule& SkillSystemEditorModule = FModuleManager::LoadModuleChecked<FSkillSystemEditorModule>("SkillSystemEditor").Get();
	return SkillSystemEditorModule.GetSkillAssetCategoryBit();
}

void FAssetTypeActions_Skill::PerformAssetDiff(UObject* Asset1, UObject* Asset2, const struct FRevisionInfo& OldRevision, const struct FRevisionInfo& NewRevision) const
{

}

void FAssetTypeActions_Skill::OpenInDefaults(class USkill* OldBehaviorTree, class USkill* NewBehaviorTree) const
{

}
#undef LOCTEXT_NAMESPACE