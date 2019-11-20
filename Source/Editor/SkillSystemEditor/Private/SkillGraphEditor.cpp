// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillGraphEditor.h"
#include "Editor/EditorEngine.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Commands/GenericCommands.h"
#include "SkillEdGraph.h"
#include "Skill.h"

#define LOCTEXT_NAMESPACE "SkillGraphEditor"
FSkillGraphEditor::FSkillGraphEditor()
{
	UEditorEngine* Editor = (UEditorEngine*)GEngine;
	if (Editor!=nullptr)
	{
		Editor->RegisterForUndo(this);
	}
}

FSkillGraphEditor::~FSkillGraphEditor()
{
	UEditorEngine* Editor = (UEditorEngine*)GEngine;
	if (Editor != nullptr)
	{
		Editor->UnregisterForUndo(this);
	}
}

void FSkillGraphEditor::OnSelectedNodesChanged(const TSet<class UObject*>& NewSelection)
{

}

void FSkillGraphEditor::PostUndo(bool bSuccess)
{
	if (bSuccess)
	{
		if (TSharedPtr<SGraphEditor> CurrentGraphEditor = UpdateGraphEdptr.Pin())
		{
			CurrentGraphEditor->ClearSelectionSet();
			CurrentGraphEditor->NotifyGraphChanged();
		}
		FSlateApplication::Get().DismissAllMenus();
	}
}

void FSkillGraphEditor::PostRedo(bool bSuccess)
{
	if (bSuccess)
	{
		if (TSharedPtr<SGraphEditor> CurrentGraphEditor = UpdateGraphEdptr.Pin())
		{
			CurrentGraphEditor->ClearSelectionSet();
			CurrentGraphEditor->NotifyGraphChanged();
		}
		FSlateApplication::Get().DismissAllMenus();
	}
}

void FSkillGraphEditor::CreateCommandList()
{
	if (GraphEditorCommands.IsValid())
	{
		return;
	}
	GraphEditorCommands = MakeShareable(new FUICommandList);
	GraphEditorCommands->MapAction(FGenericCommands::Get().SelectAll,
		FExecuteAction::CreateRaw(this, &FSkillGraphEditor::SelectAllNodes),
		FCanExecuteAction::CreateRaw(this,&FSkillGraphEditor::CanSelectAllNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Delete,
		FExecuteAction::CreateRaw(this, &FSkillGraphEditor::DeleteSelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FSkillGraphEditor::CanDeleteNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Copy,
		FExecuteAction::CreateRaw(this, &FSkillGraphEditor::CopySelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FSkillGraphEditor::CanCopyNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Cut,
		FExecuteAction::CreateRaw(this, &FSkillGraphEditor::CutSelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FSkillGraphEditor::CanCutNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Paste,
		FExecuteAction::CreateRaw(this, &FSkillGraphEditor::PasteNodes),
		FCanExecuteAction::CreateRaw(this, &FSkillGraphEditor::CanPasteNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Duplicate,
		FExecuteAction::CreateRaw(this, &FSkillGraphEditor::DuplicateNodes),
		FCanExecuteAction::CreateRaw(this, &FSkillGraphEditor::CanDuplicateNodes)
	);
}

void FSkillGraphEditor::SelectAllNodes()
{
	if (TSharedPtr<SGraphEditor> CurrentGraphEditor = UpdateGraphEdptr.Pin())
	{
		CurrentGraphEditor->SelectAllNodes();
	}
}

bool FSkillGraphEditor::CanSelectAllNodes()
{
	return true;
}

void FSkillGraphEditor::DeleteSelectedNodes()
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = UpdateGraphEdptr.Pin();
	if (!CurrentGraphEditor.IsValid())
	{
		return;
	}

	const FScopedTransaction Transaction (FGenericCommands::Get().Delete->GetDescription());

	CurrentGraphEditor->GetCurrentGraph()->Modify();

	const FGraphPanelSelectionSet SelectedNodes = CurrentGraphEditor->GetSelectedNodes();
	CurrentGraphEditor->ClearSelectionSet();

	for (FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes);NodeIt;++NodeIt)
	{
		if (UEdGraphNode* Node = Cast<UEdGraphNode>(*NodeIt))
		{
			if (Node->CanUserDeleteNode())
			{
				Node->Modify();
				Node->DestroyNode();
			}
		}
	}

}

bool FSkillGraphEditor::CanDeleteNodes() const
{
	const FGraphPanelSelectionSet SelectNodes = GetSelectedNodes();
	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		if (Node && Node->CanUserDeleteNode())
		{
			return true;
		}
	}
	return false;
}

void FSkillGraphEditor::DeleteSelectedDulicatableNodes()
{
}

void FSkillGraphEditor::CutSelectedNodes()
{
}

bool FSkillGraphEditor::CanCutNodes()
{
	return false;
}

void FSkillGraphEditor::CopySelectedNodes()
{
}

bool FSkillGraphEditor::CanCopyNodes() const
{
	return false;
}

void FSkillGraphEditor::PasteNodes()
{
}

void FSkillGraphEditor::PasteNodesHere(const FVector2D & Location)
{
}

bool FSkillGraphEditor::CanPasteNodes() const
{
	return false;
}

void FSkillGraphEditor::DuplicateNodes()
{
}

bool FSkillGraphEditor::CanDuplicateNodes() const
{
	return false;
}

FGraphPanelSelectionSet FSkillGraphEditor::GetSelectedNodes() const
{
	FGraphPanelSelectionSet CurrentSelection;
	if (TSharedPtr<SGraphEditor> FocusedGraphEd = UpdateGraphEdptr.Pin())
	{
		CurrentSelection = FocusedGraphEd->GetSelectedNodes();
	}
	return CurrentSelection;
}

void FSkillGraphEditor::OnPackageSaved(const FString& PackageFileName, UObject* Outer)
{
	USkillEdGraph* MyGraph = SkillTree ? Cast<USkillEdGraph>(SkillTree->BTGraph) : NULL;
	if (MyGraph)
	{
		const bool bUpdated = false;
		if (bUpdated)
		{
			MyGraph->UpdateAsset(USkillEdGraph::ClearDebuggerFlags);
		}
	}
}

void FSkillGraphEditor::OnClassListUpdated()
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = UpdateGraphEdptr.Pin();
	if (!CurrentGraphEditor.IsValid())
	{
		return;
	}
	USkillEdGraph* MyGraph = Cast<USkillEdGraph>(CurrentGraphEditor->GetCurrentGraph());
	if (MyGraph)
	{
		const bool bUpdated = MyGraph->UpdateUnknownNodeClasses();
		if (bUpdated)
		{
			FGraphPanelSelectionSet CurrentSelection = GetSelectedNodes();
			OnSelectedNodesChanged(CurrentSelection);
			MyGraph->UpdateAsset();
		}
	}
}

#undef LOCTEXT_NAMESPACE