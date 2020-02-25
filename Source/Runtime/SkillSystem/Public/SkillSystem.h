

#pragma once

#include "CoreMinimal.h"
#include "ModuleManager.h"
class USkillTreeManager;
DECLARE_LOG_CATEGORY_EXTERN(LogSkill, Verbose, All);
/** Module interface for this game's loading screens */
class FSkillSystemModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	virtual bool IsGameModule() const override;

	virtual void SetManager(class USkillTreeManager* NewManager);
	virtual class USkillTreeManager* GetCurrentManager();
	static FSkillSystemModule& Get();


	TWeakObjectPtr<USkillTreeManager> SkillTreeManager;
};


