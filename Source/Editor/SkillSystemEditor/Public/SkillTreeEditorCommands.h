// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
/**
 * 
 */

class FSTCommonCommands : public TCommands<FSTCommonCommands>
{
public:
	FSTCommonCommands();

	TSharedPtr<FUICommandInfo> SearchST;

	/** Initialize commands */
	virtual void RegisterCommands() override;
};

class FSTDebuggerCommands : public TCommands<FSTDebuggerCommands>
{
public:
	FSTDebuggerCommands();

	TSharedPtr<FUICommandInfo> BackInto;
	TSharedPtr<FUICommandInfo> BackOver;
	TSharedPtr<FUICommandInfo> ForwardInto;
	TSharedPtr<FUICommandInfo> ForwardOver;
	TSharedPtr<FUICommandInfo> StepOut;

	TSharedPtr<FUICommandInfo> PausePlaySession;
	TSharedPtr<FUICommandInfo> ResumePlaySession;
	TSharedPtr<FUICommandInfo> StopPlaySession;

	TSharedPtr<FUICommandInfo> CurrentValues;
	TSharedPtr<FUICommandInfo> SavedValues;

	/** Initialize commands */
	virtual void RegisterCommands() override;
};