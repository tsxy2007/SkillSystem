// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"
#include "WorkflowOrientedApp/ApplicationMode.h"
#include "Framework/Docking/TabManager.h"

/**
 * 
 */
class SKILLSYSTEMEDITOR_API FSkillEditorApplicationMode : public FApplicationMode
{
public:
	FSkillEditorApplicationMode(TSharedPtr<class FSkillEditor> InSkillEditor);
	virtual void RegisterTabFactories(TSharedPtr< class FTabManager> InTabManager) override;
	virtual void PreDeactivateMode() override;
	virtual void PostActivateMode() override;
protected:
	TWeakPtr<class FSkillEditor> SkillTreeEditor;
	FWorkflowAllowedTabSet SkillTreeEditorTabFactories;
};