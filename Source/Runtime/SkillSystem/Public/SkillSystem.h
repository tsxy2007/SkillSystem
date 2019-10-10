

#pragma once

#include "CoreMinimal.h"
#include "ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSkill, Verbose, All);
/** Module interface for this game's loading screens */
class FSkillSystemModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	virtual bool IsGameModule() const override;


	static FSkillSystemModule& Get()
	{
		return FModuleManager::LoadModuleChecked< FSkillSystemModule >("SkillSystem");
	}

};


