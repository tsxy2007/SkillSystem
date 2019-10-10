

#pragma once

#include "CoreMinimal.h"
#include "ModuleManager.h"
#include "SkillGraphTypes.h"
#include "Modules/ModuleInterface.h"
#include "Toolkits/IToolkitHost.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "AssetTypeActions_Base.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSkillEditor, Verbose, All);
/** Module interface for this game's loading screens */
class FSkillSystemEditorModule : public IModuleInterface,
	public IHasMenuExtensibility, public IHasToolBarExtensibility
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	virtual TSharedPtr<FExtensibilityManager> GetMenuExtensibilityManager()override  { return MenuExtensibilityManager; }
	virtual TSharedPtr<FExtensibilityManager> GetToolBarExtensibilityManager()override { return ToolBarExtensibilityManager; }

	TSharedPtr<struct FSkillGraphNodeClassHelper> GetClassCache() { return ClassCache; }

	static FSkillSystemEditorModule& Get()
	{
		return FModuleManager::LoadModuleChecked< FSkillSystemEditorModule >("SkillSystemEditor");
	}

	TSharedRef<class FSkillEditor> CreateSkillSystemEditor(EToolkitMode::Type Mode,TSharedPtr<class IToolkitHost> InEditWithinLevelEditor,UObject* InSkill);
#if WITH_EDITOR
	virtual EAssetTypeCategories::Type GetSkillAssetCategoryBit() const { return SkillAssetCategoryBit; }
protected:
	EAssetTypeCategories::Type SkillAssetCategoryBit;
#endif
private:
	TSharedPtr<FExtensibilityManager> MenuExtensibilityManager;
	TSharedPtr<FExtensibilityManager> ToolBarExtensibilityManager;
	TArray<TSharedPtr<class FAssetTypeActions_Base>> ItemDataAssetTypeActions;
	//TSharedPtr< FSkillEditor > NewSkillTreeEditor;
	TSharedPtr<struct FSkillGraphNodeClassHelper> ClassCache;
public:
	static const FName SkillTreeEditorAppIdentifier;
};


