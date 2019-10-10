
// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillEditorModes.h"
#include "SkillTreeEditorTabs.h"
#include "SkillTreeEditorTabFactories.h"
#include "SkillTreeEditorToolbar.h"
#include "SkillEditor.h"

FSkillEditorApplicationMode::FSkillEditorApplicationMode(TSharedPtr<class FSkillEditor> InSkillEditor)
	:FApplicationMode(FSkillEditor::SkillMode,FSkillEditor::GetLocalizedMode)
{
	SkillTreeEditor = InSkillEditor;

	SkillTreeEditorTabFactories.RegisterFactory(MakeShareable(new FSkillTreeDetailSummoner(InSkillEditor)));
	SkillTreeEditorTabFactories.RegisterFactory(MakeShareable(new FSkillTreeSearchSummoner(InSkillEditor)));

	TabLayout = FTabManager::NewLayout("Standalone_SkillTree_Layout_v1")
		->AddArea
		(
			FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.1f)
				->AddTab(InSkillEditor->GetToolbarTabId(), ETabState::OpenedTab)
				->SetHideTabWell(true)
			)
			->Split
			(
				FTabManager::NewSplitter()->SetOrientation(Orient_Horizontal)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.7f)
					->AddTab(FSkillTreeEditorTabs::GraphEditorID, ETabState::ClosedTab)
				)
				->Split
				(
					FTabManager::NewSplitter()->SetOrientation(Orient_Vertical)
					->SetSizeCoefficient(0.3f)
					->Split
					(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.6f)
						->AddTab(FSkillTreeEditorTabs::GraphDetailsID, ETabState::OpenedTab)
						->AddTab(FSkillTreeEditorTabs::SearchID, ETabState::ClosedTab)
					)
				)
			)
		);

	InSkillEditor->GetToolbarBuilder()->AddModesToolbar(ToolbarExtender);
	InSkillEditor->GetToolbarBuilder()->AddSkillTreeToolBar(ToolbarExtender);
	InSkillEditor->GetToolbarBuilder()->AddDebuggerToolBar(ToolbarExtender);
}

void FSkillEditorApplicationMode::RegisterTabFactories(TSharedPtr< class FTabManager> InTabManager)
{
	check(SkillTreeEditor.IsValid());
	TSharedPtr<FSkillEditor> SkillTreeEditorPtr = SkillTreeEditor.Pin();
	SkillTreeEditorPtr->RegisterToolbarTab(InTabManager.ToSharedRef());
	SkillTreeEditorPtr->PushTabFactories(SkillTreeEditorTabFactories);
	FApplicationMode::RegisterTabFactories(InTabManager);
}

void FSkillEditorApplicationMode::PreDeactivateMode()
{
	FApplicationMode::PreDeactivateMode();
	check(SkillTreeEditor.IsValid());
	TSharedPtr<FSkillEditor> SkillTreeEditorPtr = SkillTreeEditor.Pin();
	SkillTreeEditorPtr->SaveEditedObjectState();
}

void FSkillEditorApplicationMode::PostActivateMode()
{
	check(SkillTreeEditor.IsValid());
	TSharedPtr<FSkillEditor> SkillTreeEditorPtr = SkillTreeEditor.Pin();
	SkillTreeEditorPtr->RestoreBehaviorTree();

	FApplicationMode::PostActivateMode();
}
