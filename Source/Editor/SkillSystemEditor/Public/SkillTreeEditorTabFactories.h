// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Misc/Attribute.h"
#include "EdGraph/EdGraph.h"
#include "Widgets/SWidget.h"
#include "WorkflowOrientedApp/WorkflowTabFactory.h"
#include "GraphEditor.h"
#include "SkillEditor.h"
#include "WorkflowOrientedApp/WorkflowUObjectDocuments.h"

struct FSkillTreeDetailSummoner : public FWorkflowTabFactory
{
public:
	FSkillTreeDetailSummoner(TSharedPtr<class FSkillEditor> InSkillEditorPtr);
	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
	virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;
protected:
	TWeakPtr<class FSkillEditor> SkillEditorPtr;
};

struct FSkillTreeSearchSummoner : public FWorkflowTabFactory
{
public:
	FSkillTreeSearchSummoner(TSharedPtr<class FSkillEditor> InSkillEditorPtr);
	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
	virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;
protected:
	TWeakPtr<class FSkillEditor> SkillEditorPtr;
};

struct FSkillGraphEditorSummoner : public  FDocumentTabFactoryForObjects<UEdGraph>
{
public:
	DECLARE_DELEGATE_RetVal_OneParam(TSharedRef<SGraphEditor>, FOnCreateGraphEditorWidget, UEdGraph*);
public:
	FSkillGraphEditorSummoner(TSharedPtr<class FSkillEditor> InBehaviorTreeEditorPtr, FOnCreateGraphEditorWidget CreateGraphEditorWidgetCallback);

	virtual void OnTabActivated(TSharedPtr<SDockTab> Tab) const override;
	virtual void OnTabRefreshed(TSharedPtr<SDockTab> Tab) const override;

protected:
	virtual TAttribute<FText> ConstructTabNameForObject(UEdGraph* DocumentID) const override;
	virtual TSharedRef<SWidget> CreateTabBodyForObject(const FWorkflowTabSpawnInfo& Info, UEdGraph* DocumentID) const override;
	virtual const FSlateBrush* GetTabIconForObject(const FWorkflowTabSpawnInfo& Info, UEdGraph* DocumentID) const override;
	virtual void SaveState(TSharedPtr<SDockTab> Tab, TSharedPtr<FTabPayload> Payload) const override;

protected:
	TWeakPtr<class FSkillEditor> SkillTreeEditorPtr;
	FOnCreateGraphEditorWidget OnCreateGraphEditorWidget;
};