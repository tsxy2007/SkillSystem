// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillTreeEditorCommands.h"
#include "EditorStyleSet.h"
#define LOCTEXT_NAMESPACE "SkillTreeEditorCommands"

FSTCommonCommands::FSTCommonCommands()
	:TCommands<FSTCommonCommands>("STEditor.Common",LOCTEXT("Common","Common"),NAME_None,FEditorStyle::GetStyleSetName())
{

}

void FSTCommonCommands::RegisterCommands()
{
	UI_COMMAND(SearchST, "Search", "Search this Behavior Tree.", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::F));
}

FSTDebuggerCommands::FSTDebuggerCommands()
	: TCommands<FSTDebuggerCommands>("STEditor.Debugger", LOCTEXT("Debugger", "Debugger"), NAME_None, FEditorStyle::GetStyleSetName())
{

}

void FSTDebuggerCommands::RegisterCommands()
{
	UI_COMMAND(BackInto, "Back: Into", "Show state from previous step, can go into subtrees", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(BackOver, "Back: Over", "Show state from previous step, don't go into subtrees", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(ForwardInto, "Forward: Into", "Show state from next step, can go into subtrees", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(ForwardOver, "Forward: Over", "Show state from next step, don't go into subtrees", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(StepOut, "Step Out", "Show state from next step, leave current subtree", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(PausePlaySession, "Pause", "Pause simulation", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(ResumePlaySession, "Resume", "Resume simulation", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(StopPlaySession, "Stop", "Stop simulation", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(CurrentValues, "Current", "View current values", EUserInterfaceActionType::RadioButton, FInputChord());
	UI_COMMAND(SavedValues, "Saved", "View saved values", EUserInterfaceActionType::RadioButton, FInputChord());
}
#undef LOCTEXT_NAMESPACE