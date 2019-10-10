// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SharedPointer.h"
class FSkillEditor;
class FExtender;
class FToolBarBuilder;

/**
 * 
 */
class FSkillTreeEditorToolbar : public TSharedFromThis<FSkillTreeEditorToolbar>
{
public:
	FSkillTreeEditorToolbar(TSharedPtr<FSkillEditor> InSkillTreeEditor);
	
	void AddModesToolbar(TSharedPtr<FExtender> Extender);
	void AddDebuggerToolBar(TSharedPtr<FExtender> Extender);
	void AddSkillTreeToolBar(TSharedPtr<FExtender> Extender);
public:
	void FillModesToolbar(FToolBarBuilder& ToolbarBuilder);
	void FillDebuggerToolbar(FToolBarBuilder& ToolbarBuilder);
	void FillSkillTreeToolbar(FToolBarBuilder& ToolbarBuilder);
protected:
	TWeakPtr<FSkillEditor> SkillTreeEditor;
};
