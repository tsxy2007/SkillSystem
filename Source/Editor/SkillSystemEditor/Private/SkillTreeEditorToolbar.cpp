// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillTreeEditorToolbar.h"
#include "Misc/Attribute.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Textures/SlateIcon.h"
#include "Framework/Commands/UIAction.h"
#include "Widgets/SWidget.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SSpacer.h"
#include "Widgets/Text/STextBlock.h"
#include "Framework/MultiBox/MultiBoxExtender.h"
#include "Widgets/Input/SComboButton.h"
#include "EditorStyleSet.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "SkillEditor.h"
#include "WorkflowOrientedApp/SModeWidget.h"
#include "SkillTreeEditorCommands.h"

#define LOCTEXT_NAMESPACE "SkillTreeEditorToolbar"

class SSkillTreeModeSeparator : public SBorder
{
public:
	SLATE_BEGIN_ARGS(SSkillTreeModeSeparator){}
	SLATE_END_ARGS()
public:
	void Construct(const FArguments& InArgs)
	{
		SBorder::Construct(
			SBorder::FArguments()
			.BorderImage(FEditorStyle::GetBrush("BlueprintEditor.PipelineSeparator"))
			.Padding(0.0f)
		);
	}

	virtual FVector2D ComputeDesiredSize(float) const override
	{
		const float Height = 20.f;
		const float Thickness = 16.f;
		return FVector2D(Thickness, Height);
	}
};

FSkillTreeEditorToolbar::FSkillTreeEditorToolbar(TSharedPtr<FSkillEditor> InSkillTreeEditor)
	:SkillTreeEditor(InSkillTreeEditor)
{

}

void FSkillTreeEditorToolbar::AddModesToolbar(TSharedPtr<FExtender> Extender)
{
	check(SkillTreeEditor.IsValid());
	TSharedPtr<FSkillEditor> SkillTreeEditorPtr = SkillTreeEditor.Pin();
	

	Extender->AddToolBarExtension(
		"Asset",
		EExtensionHook::After,
		SkillTreeEditorPtr->GetToolkitCommands(),
		FToolBarExtensionDelegate::CreateRaw(this, &FSkillTreeEditorToolbar::FillModesToolbar));
}

void FSkillTreeEditorToolbar::AddDebuggerToolBar(TSharedPtr<FExtender> Extender)
{
	struct Local
	{
		static void FillToolbar(FToolBarBuilder& ToolbarBuilder, TWeakPtr<FSkillEditor> SkillTreeEditor)
		{
			TSharedPtr<FSkillEditor> BehaviorTreeEditorPtr = SkillTreeEditor.Pin();

			const bool bCanShowDebugger = BehaviorTreeEditorPtr->IsDebuggerReady();
			if (bCanShowDebugger)
			{
				TSharedRef<SWidget> SelectionBox = SNew(SComboButton)
					//.OnGetMenuContent(BehaviorTreeEditorPtr.Get(), &FSkillEditor::OnGetDebuggerActorsMenu)
					.ButtonContent()
					[
						SNew(STextBlock)
						.ToolTipText(LOCTEXT("SelectDebugActor", "Pick actor to debug"))
					//.Text(BehaviorTreeEditorPtr.Get(), &FSkillEditor::GetDebuggerActorDesc)
					];

				ToolbarBuilder.BeginSection("CachedState");
				{
					ToolbarBuilder.AddToolBarButton(FSTDebuggerCommands::Get().BackOver);
					ToolbarBuilder.AddToolBarButton(FSTDebuggerCommands::Get().BackInto);
					ToolbarBuilder.AddToolBarButton(FSTDebuggerCommands::Get().ForwardInto);
					ToolbarBuilder.AddToolBarButton(FSTDebuggerCommands::Get().ForwardOver);
					ToolbarBuilder.AddToolBarButton(FSTDebuggerCommands::Get().StepOut);
				}
				ToolbarBuilder.EndSection();
				ToolbarBuilder.BeginSection("World");
				{
					ToolbarBuilder.AddToolBarButton(FSTDebuggerCommands::Get().PausePlaySession);
					ToolbarBuilder.AddToolBarButton(FSTDebuggerCommands::Get().ResumePlaySession);
					ToolbarBuilder.AddToolBarButton(FSTDebuggerCommands::Get().StopPlaySession);
					ToolbarBuilder.AddSeparator();
					ToolbarBuilder.AddWidget(SelectionBox);
				}
				ToolbarBuilder.EndSection();
			}
		}
	};

	TSharedPtr<FSkillEditor> SkillTreeEditorPtr = SkillTreeEditor.Pin();
	TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
	ToolbarExtender->AddToolBarExtension("Asset", EExtensionHook::After, SkillTreeEditorPtr->GetToolkitCommands(), FToolBarExtensionDelegate::CreateStatic(&Local::FillToolbar, SkillTreeEditor));
	SkillTreeEditorPtr->AddToolbarExtender(ToolbarExtender);
}

void FSkillTreeEditorToolbar::AddSkillTreeToolBar(TSharedPtr<FExtender> Extender)
{
	check(SkillTreeEditor.IsValid());
	TSharedPtr<FSkillEditor> SkillTreeEditorPtr = SkillTreeEditor.Pin();
	TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
	ToolbarExtender->AddToolBarExtension("Asset", EExtensionHook::After, SkillTreeEditorPtr->GetToolkitCommands(), FToolBarExtensionDelegate::CreateRaw(this, &FSkillTreeEditorToolbar::FillSkillTreeToolbar));
	SkillTreeEditorPtr->AddToolbarExtender(ToolbarExtender);
}

void FSkillTreeEditorToolbar::FillModesToolbar(FToolBarBuilder& ToolbarBuilder)
{
	check(SkillTreeEditor.IsValid());
	TSharedPtr<FSkillEditor> SkillTreeEditorPtr = SkillTreeEditor.Pin();

	TAttribute<FName> GetActiveMode(SkillTreeEditorPtr.ToSharedRef(), &FSkillEditor::GetCurrentMode);
	FOnModeChangeRequested SetActiveMode = FOnModeChangeRequested::CreateSP(SkillTreeEditorPtr.ToSharedRef(), &FSkillEditor::SetCurrentMode);

	// Left side padding
	SkillTreeEditorPtr->AddToolbarWidget(SNew(SSpacer).Size(FVector2D(4.0f, 1.0f)));

	SkillTreeEditorPtr->AddToolbarWidget(
		SNew(SModeWidget, FSkillEditor::GetLocalizedMode(FSkillEditor::SkillMode), FSkillEditor::SkillMode)
		.OnGetActiveMode(GetActiveMode)
		.OnSetActiveMode(SetActiveMode)
		.CanBeSelected(SkillTreeEditorPtr.Get(), &FSkillEditor::CanAccessSkillTreeMode)
		.ToolTipText(LOCTEXT("SkillTreeModeButtonTooltip", "Switch to Skill Tree Mode"))
		.IconImage(FEditorStyle::GetBrush("BTEditor.SwitchToBehaviorTreeMode"))
		.SmallIconImage(FEditorStyle::GetBrush("BTEditor.SwitchToBehaviorTreeMode.Small"))
	);

	SkillTreeEditorPtr->AddToolbarWidget(SNew(SSkillTreeModeSeparator));
}

void FSkillTreeEditorToolbar::FillDebuggerToolbar(FToolBarBuilder& ToolbarBuilder)
{
	
}

void FSkillTreeEditorToolbar::FillSkillTreeToolbar(FToolBarBuilder& ToolbarBuilder)
{
	check(SkillTreeEditor.IsValid());
	TSharedPtr<FSkillEditor> SkillTreeEditorPtr = SkillTreeEditor.Pin();

	if (!SkillTreeEditorPtr->IsDebuggerReady() && SkillTreeEditorPtr->GetCurrentMode() == FSkillEditor::SkillMode)
	{

		ToolbarBuilder.BeginSection("SkillTree");
		{
			const FText NewTaskLabel = LOCTEXT("NewTask_Label", "New Task");
			const FText NewTaskTooltip = LOCTEXT("NewTask_ToolTip", "Create a new task node Blueprint from a base class");
			const FSlateIcon NewTaskIcon = FSlateIcon(FEditorStyle::GetStyleSetName(), "BTEditor.Graph.NewTask");

			ToolbarBuilder.AddToolBarButton(
				FUIAction(
					FExecuteAction::CreateSP(SkillTreeEditorPtr.Get(), &FSkillEditor::CreateNewTask),
					FCanExecuteAction::CreateSP(SkillTreeEditorPtr.Get(), &FSkillEditor::CanCreateNewTask),
					FIsActionChecked(),
					FIsActionButtonVisible::CreateSP(SkillTreeEditorPtr.Get(), &FSkillEditor::IsNewTaskButtonVisible)
				),
				NAME_None,
				NewTaskLabel,
				NewTaskTooltip,
				NewTaskIcon
			);

			ToolbarBuilder.AddComboButton(
				FUIAction(
					FExecuteAction(),
					FCanExecuteAction::CreateSP(SkillTreeEditorPtr.Get(), &FSkillEditor::CanCreateNewTask),
					FIsActionChecked(),
					FIsActionButtonVisible::CreateSP(SkillTreeEditorPtr.Get(), &FSkillEditor::IsNewTaskComboVisible)
				),
				 FOnGetContent::CreateSP(SkillTreeEditorPtr.Get(), &FSkillEditor::HandleCreateNewTaskMenu),
				NewTaskLabel,
				NewTaskTooltip,
				NewTaskIcon
			);
		}
		ToolbarBuilder.EndSection();
	}
}
#undef LOCTEXT_NAMESPACE