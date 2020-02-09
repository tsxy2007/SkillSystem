// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillEditor.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"
#include "AssetToolsModule.h"
#include "Components/TextBlock.h"
#include "SkillTreeEditorToolbar.h"
#include "GraphEditorActions.h"
#include "SkillSystemEditor.h"
#include "Toolkits/IToolkitHost.h"
#include "Widgets/Text/STextBlock.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Engine/Blueprint.h"
#include "Widgets/Layout/SBorder.h"
#include "UObject/Package.h"
#include "BehaviorTree/BTDecorator.h"
#include "BehaviorTree/BTCompositeNode.h"
#include "Modules/ModuleManager.h"
#include "EditorStyleSet.h"
#include "Editor/UnrealEdEngine.h"
#include "Factories/DataAssetFactory.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "UnrealEdGlobals.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "WorkflowOrientedApp/WorkflowTabFactory.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"
#include "WorkflowOrientedApp/WorkflowUObjectDocuments.h"
#include "PropertyEditorModule.h"
#include "ClassViewerModule.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "ClassViewerFilter.h"

#include "Skill.h"
#include "SkillTreeEditorCommands.h"
#include "SkillTreeEditorTabs.h"
#include "SkillEdGraph.h"
#include "SkillEditorModes.h"
#include "SkillTreeEditorTabFactories.h"
#include "EdGraphSchema_SkillTree.h"

#define LOCTEXT_NAMESPACE "SkillEditor"

const FName FSkillEditor::SkillMode(TEXT("Skill"));
FSkillEditor::FSkillEditor()
{
	SelectedNodesCount = 0;
}

FSkillEditor::~FSkillEditor()
{

}

void FSkillEditor::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	DocumentManager->SetTabManager(InTabManager);
	FWorkflowCentricApplication::RegisterTabSpawners(InTabManager);
} 

void FSkillEditor::RegisterToolbarTab(const TSharedRef<class FTabManager>& InTabManager)
{
	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);
}

void FSkillEditor::InitSkillEditor(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, UObject* InObject)
{

	USkill* SkillTreeToEdit = Cast<USkill>(InObject);

	if (SkillTreeToEdit != nullptr)
	{
		Skill = SkillTreeToEdit;
	}

	TSharedPtr<FSkillEditor> ThisPtr(SharedThis(this));
	if (!DocumentManager.IsValid())
	{
		DocumentManager = MakeShareable(new FDocumentTracker);
		DocumentManager->Initialize(ThisPtr);
		// Register the document factories
		{
			TSharedRef<FDocumentTabFactory> GraphEditorFactory = MakeShareable(new FSkillGraphEditorSummoner(ThisPtr,
				FSkillGraphEditorSummoner::FOnCreateGraphEditorWidget::CreateSP(this, &FSkillEditor::CreateGraphEditorWidget)
			));

			// also store off a reference to the grapheditor factory so we can find all the tabs spawned by it later.
			GraphEditorTabFactoryPtr = GraphEditorFactory;
			DocumentManager->RegisterDocumentFactory(GraphEditorFactory);
		}
	}

	TArray<UObject*> ObjectsToEdit;
	if (Skill != nullptr)
	{
		ObjectsToEdit.Add(Skill);
	}

	if (!ToolbarBuilder.IsValid())
	{
		ToolbarBuilder = MakeShareable(new FSkillTreeEditorToolbar(SharedThis(this)));
	}

	const TArray<UObject*>* EditedObjects = GetObjectsCurrentlyBeingEdited();
	if (EditedObjects == nullptr || EditedObjects->Num() == 0)
	{
		FGraphEditorCommands::Register();
		FSTCommonCommands::Register();
		FSTDebuggerCommands::Register();


		const TSharedRef<FTabManager::FLayout> DummyLayout = FTabManager::NewLayout("NullLayout")->AddArea(FTabManager::NewPrimaryArea());
		const bool bCreateDefaultStandaloneMenu = true;
		const bool bCreateDefaultToolbar = true;
		InitAssetEditor(Mode, InitToolkitHost, FSkillSystemEditorModule::SkillTreeEditorAppIdentifier, DummyLayout, bCreateDefaultStandaloneMenu, bCreateDefaultToolbar, ObjectsToEdit);

	
		BindCommonCommands();
		ExtendMenu();
		CreateInternalWidgets();


		FSkillSystemEditorModule& SkillSystemEditorModule = FModuleManager::LoadModuleChecked<FSkillSystemEditorModule>("SkillSystemEditor");
		AddMenuExtender(SkillSystemEditorModule.GetMenuExtensibilityManager()->GetAllExtenders(GetToolkitCommands(), GetEditingObjects()));

		AddApplicationMode(SkillMode, MakeShareable(new FSkillEditorApplicationMode(SharedThis(this))));
	}
	else
	{
	
	}

	if (SkillTreeToEdit)
	{
		SetCurrentMode(SkillMode);
	}
	OnClassListUpdated();
	RegenerateMenusAndToolbars();
}

FName FSkillEditor::GetToolkitFName() const
{

	return FName("Skill Tree");
}

FText FSkillEditor::GetBaseToolkitName() const
{
	return LOCTEXT("AppLabel","SkillTree");
}

FString FSkillEditor::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("WorldCentricTabPrefix", "SkillTree ").ToString();
}

FLinearColor FSkillEditor::GetWorldCentricTabColorScale() const
{
	return FLinearColor(0.0f, 0.0f, 0.2f, 0.5f);
}

FText FSkillEditor::GetToolkitName() const
{
	const UObject* EditingObject = GetCurrentMode() == SkillMode ? (UObject*)Skill : nullptr;
	if (EditingObject != nullptr)
	{
		return FAssetEditorToolkit::GetLabelForObject(EditingObject);
	}
	return FText();
}

FText FSkillEditor::GetToolkitToolTipText() const
{
	const UObject* EditingObject = GetCurrentMode() == SkillMode ? (UObject*)Skill : nullptr;
	if (EditingObject != nullptr)
	{
		return FAssetEditorToolkit::GetToolTipTextForObject(EditingObject);
	}
	return FText();
}


void FSkillEditor::InitializeDebuggerState(class FBehaviorTreeDebugger* ParentDebugger) const
{

}

UEdGraphNode* FSkillEditor::FindInjectedNode(int32 Index) const
{
	return nullptr;
}

void FSkillEditor::DoubleClickNode(class UEdGraphNode* Node)
{

}

void FSkillEditor::FocusWindow(UObject* ObjectToFocusOn /*= NULL*/)
{
	if (ObjectToFocusOn == Skill)
	{
		SetCurrentMode(SkillMode);
	}
	FWorkflowCentricApplication::FocusWindow(ObjectToFocusOn);
}

bool FSkillEditor::GetBoundsForSelectedNodes(class FSlateRect& Rect, float Padding) const
{
	return true;
}

void FSkillEditor::PostUndo(bool bSuccess)
{

}

void FSkillEditor::PostRedo(bool bSuccess)
{

}

void FSkillEditor::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, UProperty* PropertyThatChanged)
{

}

FText FSkillEditor::GetLocalizedMode(FName InMode)
{
	static TMap<FName, FText> LocModes;
	if (LocModes.Num() == 0)
	{
		LocModes.Add(SkillMode, LOCTEXT("SkillMode", "Skill"));
	}
	check(InMode != NAME_None);
	const FText* OutDesc = LocModes.Find(InMode);
	check(OutDesc);
	return *OutDesc;
}

FGraphAppearanceInfo FSkillEditor::GetGraphAppearance() const
{
	FGraphAppearanceInfo AppearanceInfo;
	AppearanceInfo.CornerText = LOCTEXT("AppearanceCornerText", "Skill Tree");

	const int32 StepIdx = 0;
	AppearanceInfo.PIENotifyText = LOCTEXT("InactiveLabel", "INACTIVE");
	return AppearanceInfo;
}

void FSkillEditor::OnNodeDoubleClicked(class UEdGraphNode* Node)
{

}

void FSkillEditor::OnGraphEditorFocused(const TSharedRef<SGraphEditor>& InGraphEditor)
{
	UpdateGraphEdptr = InGraphEditor;

	FGraphPanelSelectionSet CurrentSelection = InGraphEditor->GetSelectedNodes();
	OnSelectedNodesChanged(CurrentSelection);
}

void FSkillEditor::OnNodeTitleCommitted(const FText& NewText, ETextCommit::Type CommitInfo, UEdGraphNode* NodeBeingChanged)
{

}

bool FSkillEditor::IsDebuggerReady() const
{
	return false;
}

USkill* FSkillEditor::GetSKillTree() const
{
	return Skill;
}

TSharedRef<SWidget> FSkillEditor::SpawnProperties()
{
	return SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.FillHeight(1.f)
		.HAlign(HAlign_Fill)
		[
			DetailsView.ToSharedRef()
		];
}

TSharedRef<SWidget> FSkillEditor::SpawnSearch()
{
	return SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.FillHeight(1.f)
		.HAlign(HAlign_Fill)
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("Search")))
		];
}

void FSkillEditor::BindCommonCommands()
{
	ToolkitCommands->MapAction(FSTCommonCommands::Get().SearchST,
		FExecuteAction::CreateSP(this, &FSkillEditor::SearchTree),
		FCanExecuteAction::CreateSP(this, &FSkillEditor::CanSearchTree));

}

void FSkillEditor::ExtendMenu()
{
	struct Local
	{
		static void FillEditMenu(FMenuBuilder& MenuBuilder)
		{
			MenuBuilder.BeginSection("EditSearch", LOCTEXT("EditMenu_SearchHeading", "Search"));
			{
				MenuBuilder.AddMenuEntry(FSTCommonCommands::Get().SearchST);
			}
			MenuBuilder.EndSection();
		}
	};

	TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender);

	MenuExtender->AddMenuExtension(
		"EditHistory",
		EExtensionHook::After,
		GetToolkitCommands(),
		FMenuExtensionDelegate::CreateStatic(&Local::FillEditMenu)
	);
	AddMenuExtender(MenuExtender);
}

void FSkillEditor::CreateInternalWidgets()
{
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs DetailViewArgs(false, false, true, FDetailsViewArgs::HideNameArea, false);
	DetailViewArgs.NotifyHook = this;
	DetailViewArgs.DefaultsOnlyVisibility = EEditDefaultsOnlyNodeVisibility::Hide;
	DetailsView = PropertyEditorModule.CreateDetailView(DetailViewArgs);
	DetailsView->SetObject(NULL);
	DetailsView->SetIsPropertyEditingEnabledDelegate(FIsPropertyEditingEnabled::CreateSP(this, &FSkillEditor::IsPropertyEditable));
	DetailsView->OnFinishedChangingProperties().AddSP(this, &FSkillEditor::OnFinishedChangingProperties);
}

void FSkillEditor::OnClassListUpdated()
{
	FSkillGraphEditor::OnClassListUpdated();
	// Ìí¼Ó£»
	//const int32 NumTaskBP = FGraphNodeClassHelper::GetObservedBlueprintClassCount()

}

void FSkillEditor::SearchTree()
{
	TabManager->InvokeTab(FSkillTreeEditorTabs::SearchID);
}

bool FSkillEditor::CanSearchTree() const
{
	return true;
}

bool FSkillEditor::CanAccessSkillTreeMode() const
{
	return Skill != nullptr;
}

void FSkillEditor::OnAddInputPin()
{

}

bool FSkillEditor::CanAddInputPin() const
{
	return true;
}

void FSkillEditor::OnRemoveInputPin()
{

}

bool FSkillEditor::CanRemoveInputPin() const
{
	return true;
}

void FSkillEditor::OnEnableBreakpoint()
{

}

bool FSkillEditor::CanEnableBreakpoint() const
{
	return true;
}

void FSkillEditor::OnToggleBreakpoint()
{

}

bool FSkillEditor::CanToggleBreakpoint() const
{
	return true;
}

void FSkillEditor::OnDisableBreakpoint()
{

}

bool FSkillEditor::CanDisableBreakpoint() const
{
	return true;
}

void FSkillEditor::OnAddBreakpoint()
{

}

bool FSkillEditor::CanAddBreakpoint() const
{
	return true;
}

void FSkillEditor::OnRemoveBreakpoint()
{

}

bool FSkillEditor::CanRemoveBreakpoint() const
{
	return true;
}

bool FSkillEditor::InEditingMode(bool bGraphIsEditable) const
{
	return true;
}

void FSkillEditor::CreateNewTask() const
{
	
}

bool FSkillEditor::CanCreateNewTask() const
{
	return !IsDebuggerReady();
}

bool FSkillEditor::IsNewTaskButtonVisible() const
{
	return true;
}

bool FSkillEditor::IsNewTaskComboVisible() const
{
	return true;
}

template<typename Type>
class FNewNodeClassFilter : public IClassViewerFilter
{
public:
	virtual bool IsClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const UClass* InClass, TSharedRef<class FClassViewerFilterFuncs> InFilterFuncs) override
	{
		if (InClass != nullptr)
		{
			return InClass->IsChildOf(Type::StaticClass());
		}
		return false;
	}
	virtual bool IsUnloadedClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const TSharedRef<const class IUnloadedBlueprintData> InUnloadedClassData, TSharedRef<class FClassViewerFilterFuncs> InFilterFuncs) override
	{
		return InUnloadedClassData->IsChildOf(Type::StaticClass());
	}
};

TSharedRef<SWidget> FSkillEditor::HandleCreateNewTaskMenu() const
{
	FClassViewerInitializationOptions Options;
	Options.bShowUnloadedBlueprints = true;
	Options.ClassFilter = MakeShareable(new FNewNodeClassFilter<USTGraphNode>());

	FOnClassPicked OnPicked(FOnClassPicked::CreateSP(this, &FSkillEditor::HandleNewNodeClassPicked));

	return FModuleManager::LoadModuleChecked<FClassViewerModule>("ClassViewer").CreateClassViewer(Options, OnPicked);
}

void FSkillEditor::HandleNewNodeClassPicked(UClass* InClass) const
{

}

bool FSkillEditor::IsPropertyEditable() const
{
	TSharedPtr<SGraphEditor> FocusedGraph = UpdateGraphEdptr.Pin();
	return FocusedGraph.IsValid() && FocusedGraph->GetCurrentGraph() && FocusedGraph->GetCurrentGraph()->bEditable;
}

void FSkillEditor::OnPackageSaved(const FString& PackageFileName, UObject* Outer)
{
	USkillEdGraph* MyGraph = Skill ? Cast<USkillEdGraph>(Skill->BTGraph) : NULL;
	if (MyGraph)
	{
		const bool bUpdated = false; // 
		if (bUpdated)
		{
			MyGraph->UpdateAsset(USkillEdGraph::ClearDebuggerFlags);
		}
	}
}

void FSkillEditor::OnFinishedChangingProperties(const FPropertyChangedEvent& PropertyChangedEvent)
{
	
	if (PropertyChangedEvent.Property)
	{
		if (PropertyChangedEvent.Property->GetName() == TEXT("FlowAbortMode"))
		{
			
		}
	}

	if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetName() == TEXT("SkillAsset"))
	{
		USkillEdGraph * MyGraph = Cast<USkillEdGraph>(Skill->BTGraph);
	}
}

void FSkillEditor::RestoreBehaviorTree()
{
	USkillEdGraph * MyGraph = Cast<USkillEdGraph>(Skill->BTGraph);
	const bool bNewGraph = MyGraph == nullptr;
	if (MyGraph == nullptr)
	{
		Skill->BTGraph = FBlueprintEditorUtils::CreateNewGraph(Skill, TEXT("Skill Tree"), USkillEdGraph::StaticClass(), UEdGraphSchema_SkillTree::StaticClass());
		MyGraph = Cast<USkillEdGraph>(Skill->BTGraph);

		const UEdGraphSchema* Schema = MyGraph->GetSchema();
		Schema->CreateDefaultNodesForGraph(*MyGraph);
		MyGraph->OnCreated();
	}
	else
	{
		MyGraph->OnLoaded();
	}
	MyGraph->Initialize();

	TSharedRef<FTabPayload_UObject> Payload = FTabPayload_UObject::Make(MyGraph);
	TSharedPtr<SDockTab> DocumentTab = DocumentManager->OpenDocument(Payload, bNewGraph ? FDocumentTracker::OpenNewDocument : FDocumentTracker::RestorePreviousDocument);
	if (Skill->LastEditedDocuments.Num()>0)
	{
		TSharedRef<SGraphEditor> GraphEditor = StaticCastSharedRef<SGraphEditor>(DocumentTab->GetContent());
		GraphEditor->SetViewLocation(Skill->LastEditedDocuments[0].SavedViewOffset, Skill->LastEditedDocuments[0].SavedZoomAmount);
	}
	// ²»È«
	const bool bIncreaseVersionNum = false;
	if (bNewGraph)
	{
		MyGraph->UpdateAsset(true);
	}
	else
	{
		MyGraph->UpdateAsset(true);
	}

}

void FSkillEditor::SaveEditedObjectState()
{
	Skill->LastEditedDocuments.Empty();
	DocumentManager->SaveAllState();
}

//
//void FSkillEditor::OnClassListUpdated()
//{
//
//}

void FSkillEditor::SaveAsset_Execute()
{
	if (Skill)
	{
		USkillEdGraph* SkillGraph = Cast<USkillEdGraph>(Skill->BTGraph);
		if (SkillGraph)
		{
			SkillGraph->OnSave();
		}
	}
	ISkillEditor::SaveAsset_Execute();
}

TSharedRef<class SGraphEditor> FSkillEditor::CreateGraphEditorWidget(UEdGraph* InGraph)
{
	check(InGraph != NULL);
	if (!GraphEditorCommands.IsValid())
	{
		CreateCommandList();
		GraphEditorCommands->MapAction(FGraphEditorCommands::Get().RemoveExecutionPin,
			FExecuteAction::CreateSP(this, &FSkillEditor::OnRemoveInputPin),
			FCanExecuteAction::CreateSP(this, &FSkillEditor::CanRemoveInputPin)
		);

		GraphEditorCommands->MapAction(FGraphEditorCommands::Get().AddExecutionPin,
			FExecuteAction::CreateSP(this, &FSkillEditor::OnAddInputPin),
			FCanExecuteAction::CreateSP(this,&FSkillEditor::CanAddInputPin)
			);

		// Debug actions
		GraphEditorCommands->MapAction(FGraphEditorCommands::Get().AddBreakpoint,
			FExecuteAction::CreateSP(this, &FSkillEditor::OnAddBreakpoint),
			FCanExecuteAction::CreateSP(this, &FSkillEditor::CanAddBreakpoint),
			FIsActionChecked(),
			FIsActionButtonVisible::CreateSP(this, &FSkillEditor::CanAddBreakpoint)
		);

		GraphEditorCommands->MapAction(FGraphEditorCommands::Get().RemoveBreakpoint,
			FExecuteAction::CreateSP(this, &FSkillEditor::OnRemoveBreakpoint),
			FCanExecuteAction::CreateSP(this, &FSkillEditor::CanRemoveBreakpoint),
			FIsActionChecked(),
			FIsActionButtonVisible::CreateSP(this, &FSkillEditor::CanRemoveBreakpoint)
		);

		GraphEditorCommands->MapAction(FGraphEditorCommands::Get().EnableBreakpoint,
			FExecuteAction::CreateSP(this, &FSkillEditor::OnEnableBreakpoint),
			FCanExecuteAction::CreateSP(this, &FSkillEditor::CanEnableBreakpoint),
			FIsActionChecked(),
			FIsActionButtonVisible::CreateSP(this, &FSkillEditor::CanEnableBreakpoint)
		);

		GraphEditorCommands->MapAction(FGraphEditorCommands::Get().DisableBreakpoint,
			FExecuteAction::CreateSP(this, &FSkillEditor::OnDisableBreakpoint),
			FCanExecuteAction::CreateSP(this, &FSkillEditor::CanDisableBreakpoint),
			FIsActionChecked(),
			FIsActionButtonVisible::CreateSP(this, &FSkillEditor::CanDisableBreakpoint)
		);

		GraphEditorCommands->MapAction(FGraphEditorCommands::Get().ToggleBreakpoint,
			FExecuteAction::CreateSP(this, &FSkillEditor::OnToggleBreakpoint),
			FCanExecuteAction::CreateSP(this, &FSkillEditor::CanToggleBreakpoint),
			FIsActionChecked(),
			FIsActionButtonVisible::CreateSP(this, &FSkillEditor::CanToggleBreakpoint)
		);
	}

	SGraphEditor::FGraphEditorEvents InEnvents;
	InEnvents.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP(this, &FSkillEditor::OnSelectedNodesChanged);
	InEnvents.OnNodeDoubleClicked = FSingleNodeEvent::CreateSP(this, &FSkillEditor::OnNodeDoubleClicked);
	InEnvents.OnTextCommitted = FOnNodeTextCommitted::CreateSP(this, &FSkillEditor::OnNodeTitleCommitted);

	TSharedRef<SWidget> TitleBarWidget =
		SNew(SBorder)
		.BorderImage(FEditorStyle::GetBrush(TEXT("Graph.TitleBackground")))
		.HAlign(HAlign_Fill)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.HAlign(HAlign_Center)
			.FillWidth(1.f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("SkillTreeGraphLabel","Skill Tree"))
				.TextStyle(FEditorStyle::Get(),TEXT("GraphBreadcrumbButtonText"))
			]
		];
	const bool bGraphIsEditable = InGraph->bEditable;
	return SNew(SGraphEditor)
		.AdditionalCommands(GraphEditorCommands)
		.IsEditable(this, &FSkillEditor::InEditingMode, bGraphIsEditable)
		.Appearance(this, &FSkillEditor::GetGraphAppearance)
		.TitleBar(TitleBarWidget)
		.GraphToEdit(InGraph)
		.GraphEvents(InEnvents);
}

void FSkillEditor::OnSelectedNodesChanged(const TSet<class UObject*>& NewSelection)
{
	SelectedNodesCount = NewSelection.Num();

	USkillEdGraph* MyGraph = Cast<USkillEdGraph>(Skill->BTGraph);
	if (SelectedNodesCount)
	{

	}


}
#undef LOCTEXT_NAMESPACE