// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillTreeEditorTabFactories.h"
#include "SkillTreeEditorTabs.h"
#include "Engine/Blueprint.h"
#include "EditorStyleSet.h"
#include "Widgets/Docking/SDockTab.h"
#define LOCTEXT_NAMESPACE "SkillTreeEditorFactories"
FSkillTreeDetailSummoner::FSkillTreeDetailSummoner(TSharedPtr<class FSkillEditor> InSkillEditorPtr)
	:FWorkflowTabFactory(FSkillTreeEditorTabs::GraphDetailsID,InSkillEditorPtr)
	,SkillEditorPtr(InSkillEditorPtr)
{
	TabLabel = LOCTEXT("SkillTreeDetailsLabel","Details");
	TabIcon = FSlateIcon(FEditorStyle::GetStyleSetName(), "Kismet.Tabs.Components");
	bIsSingleton = true;

	ViewMenuDescription = LOCTEXT("SkillTreeDetailsView", "Details");
	ViewMenuTooltip = LOCTEXT("SkillTreeDetailsView_ToolTip", "Show the details view");
}

TSharedRef<SWidget> FSkillTreeDetailSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	check(SkillEditorPtr.IsValid());
	return SkillEditorPtr.Pin()->SpawnProperties();
}

FText FSkillTreeDetailSummoner::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	return LOCTEXT("SkillTreeDetailsTabToolTip", "The Skill tree details tab allows editing of the properties of skill tree nodes");
}

FSkillTreeSearchSummoner::FSkillTreeSearchSummoner(TSharedPtr<class FSkillEditor> InSkillEditorPtr)
	:FWorkflowTabFactory(FSkillTreeEditorTabs::SearchID,InSkillEditorPtr)
	,SkillEditorPtr(InSkillEditorPtr)
{
	TabLabel = LOCTEXT("BehaviorTreeSearchLabel", "Search");
	TabIcon = FSlateIcon(FEditorStyle::GetStyleSetName(), "Kismet.Tabs.FindResults");

	bIsSingleton = true;

	ViewMenuDescription = LOCTEXT("SkillTreeSearchView", "Search");
	ViewMenuTooltip = LOCTEXT("SkillTreeSearchView_ToolTip", "Show the skill tree search tab");
}

TSharedRef<SWidget> FSkillTreeSearchSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	return SkillEditorPtr.Pin()->SpawnSearch();
}

FText FSkillTreeSearchSummoner::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	return LOCTEXT("SkillTreeSearchTabTooltip", "The Skill tree search tab allows searching within Skill tree nodes");
}

FSkillGraphEditorSummoner::FSkillGraphEditorSummoner(TSharedPtr<class FSkillEditor> InSkillTreeEditorPtr, FOnCreateGraphEditorWidget CreateGraphEditorWidgetCallback)
	:FDocumentTabFactoryForObjects<UEdGraph>(FSkillTreeEditorTabs::GraphEditorID, InSkillTreeEditorPtr)
	,SkillTreeEditorPtr(InSkillTreeEditorPtr)
	,OnCreateGraphEditorWidget(CreateGraphEditorWidgetCallback)
{

}

void FSkillGraphEditorSummoner::OnTabActivated(TSharedPtr<SDockTab> Tab) const
{
	check(SkillTreeEditorPtr.IsValid());
	TSharedRef<SGraphEditor> GraphEditor = StaticCastSharedRef<SGraphEditor>(Tab->GetContent());
	SkillTreeEditorPtr.Pin()->OnGraphEditorFocused(GraphEditor);
}

void FSkillGraphEditorSummoner::OnTabRefreshed(TSharedPtr<SDockTab> Tab) const
{
	TSharedRef<SGraphEditor> GraphEditor = StaticCastSharedRef<SGraphEditor>(Tab->GetContent());
	GraphEditor->NotifyGraphChanged();
}

TAttribute<FText> FSkillGraphEditorSummoner::ConstructTabNameForObject(UEdGraph* DocumentID) const
{
	return TAttribute<FText>(FText::FromString(DocumentID->GetName()));
}

TSharedRef<SWidget> FSkillGraphEditorSummoner::CreateTabBodyForObject(const FWorkflowTabSpawnInfo& Info, UEdGraph* DocumentID) const
{
	return OnCreateGraphEditorWidget.Execute(DocumentID);
}

const FSlateBrush* FSkillGraphEditorSummoner::GetTabIconForObject(const FWorkflowTabSpawnInfo& Info, UEdGraph* DocumentID) const
{
	return FEditorStyle::GetBrush("NoBrush");
}

void FSkillGraphEditorSummoner::SaveState(TSharedPtr<SDockTab> Tab, TSharedPtr<FTabPayload> Payload) const
{
	check(SkillTreeEditorPtr.IsValid());
	check(SkillTreeEditorPtr.Pin()->GetSKillTree());

	TSharedPtr<SGraphEditor> GraphEditor = StaticCastSharedRef<SGraphEditor>(Tab->GetContent());
	FVector2D ViewLocation;
	float ZoomAmount;
	GraphEditor->GetViewLocation(ViewLocation, ZoomAmount);
	UEdGraph* Graph = FTabPayload_UObject::CastChecked<UEdGraph>(Payload);
	SkillTreeEditorPtr.Pin()->GetSKillTree()->LastEditedDocuments.Add(FEditedDocumentInfo(Graph, ViewLocation, ZoomAmount));
}
#undef LOCTEXT_NAMESPACE