// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "SkillSystemEditor.h"
#include "AssetTypeActions_Skill.h"
#include "SkillEditor.h"
#include "PropertyEditorModule.h"
#include "STNode.h"
#include "STGraphNode.h"
#include "PropertyEditorDelegates.h"
#include "EdGraphUtilities.h"
#include "SGraphNode_SkillTree.h"
#include "SGraphNodeST.h"

IMPLEMENT_GAME_MODULE(FSkillSystemEditorModule, SkillSystemEditor);
DEFINE_LOG_CATEGORY(LogSkillEditor);
#define LOCTEXT_NAMESPACE "SkillSystemEditorModule"


const FName FSkillSystemEditorModule::SkillTreeEditorAppIdentifier(TEXT("SkillTreeEditorApp"));

class FGraphPanelNodeFactory_SKillTree : public FGraphPanelNodeFactory
{
public:
	virtual TSharedPtr<class SGraphNode> CreateNode(UEdGraphNode* Node) const override
	{
		if (USTGraphNode* STNode = Cast<USTGraphNode>(Node))
		{
			return SNew(SGraphNode_SkillTree, STNode);
		}
		return NULL;
	}
};

TSharedPtr<FGraphPanelNodeFactory> GraphPanelNodeFactory_SKill;

void FSkillSystemEditorModule::StartupModule()
{
	MenuExtensibilityManager = MakeShareable(new FExtensibilityManager);
	ToolBarExtensibilityManager = MakeShareable(new FExtensibilityManager);

	GraphPanelNodeFactory_SKill = MakeShareable(new FGraphPanelNodeFactory_SKillTree);
	FEdGraphUtilities::RegisterVisualNodeFactory(GraphPanelNodeFactory_SKill);

	IAssetTools& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	TSharedPtr<FAssetTypeActions_Skill> BehaviorTreeAssetTypeAction = MakeShareable(new FAssetTypeActions_Skill);
	ItemDataAssetTypeActions.Add(BehaviorTreeAssetTypeAction);
	AssetToolsModule.RegisterAssetTypeActions(BehaviorTreeAssetTypeAction.ToSharedRef());

	if (GIsEditor)
	{
		// register AI category so that AI assets can register to it
		SkillAssetCategoryBit = AssetToolsModule.RegisterAdvancedAssetCategory(FName(TEXT("Skill")), LOCTEXT("SkillAssetCategory", "Custom Skill"));
	}

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	//PropertyModule.RegisterCustomPropertyTypeLayout("BlackboardKeySelector", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FBlackboardSelectorDetails::MakeInstance));
	//PropertyModule.RegisterCustomClassLayout("BTDecorator_Blackboard", FOnGetDetailCustomizationInstance::CreateStatic(&FBlackboardDecoratorDetails::MakeInstance));
	//PropertyModule.RegisterCustomClassLayout("BTDecorator", FOnGetDetailCustomizationInstance::CreateStatic(&FBehaviorDecoratorDetails::MakeInstance));
	PropertyModule.NotifyCustomizationModuleChanged();
}

void FSkillSystemEditorModule::ShutdownModule()
{
	if (!UObjectInitialized())
	{
		return;
	}

	MenuExtensibilityManager.Reset();
	ToolBarExtensibilityManager.Reset();
	ClassCache.Reset();

	if (GraphPanelNodeFactory_SKill.IsValid())
	{
		FEdGraphUtilities::UnregisterVisualNodeFactory(GraphPanelNodeFactory_SKill);
		GraphPanelNodeFactory_SKill.Reset();
	}

	if (FModuleManager::Get().IsModuleLoaded(TEXT("AssetTools")))
	{
		IAssetTools& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
		for (auto& AssetTypeAction : ItemDataAssetTypeActions)
		{
			if (AssetTypeAction.IsValid())
			{
				AssetToolsModule.UnregisterAssetTypeActions(AssetTypeAction.ToSharedRef());
			}
		}
	}
	ItemDataAssetTypeActions.Empty();

	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		//PropertyModule.UnregisterCustomPropertyTypeLayout("BlackboardKeySelector");
		//PropertyModule.UnregisterCustomClassLayout("BTDecorator_Blackboard");
		//PropertyModule.UnregisterCustomClassLayout("BTDecorator");
		PropertyModule.NotifyCustomizationModuleChanged();
	}
}

TSharedRef<FSkillEditor> FSkillSystemEditorModule::CreateSkillSystemEditor(EToolkitMode::Type Mode, TSharedPtr<class IToolkitHost> InEditWithinLevelEditor, UObject* InSkill)
{
	if (!ClassCache.IsValid())
	{
		ClassCache = MakeShareable(new FSkillGraphNodeClassHelper(USTNode::StaticClass()));
		//FGraphNodeClassHelper::AddObservedBlueprintClasses(UBTTask_BlueprintBase::StaticClass());
		//FGraphNodeClassHelper::AddObservedBlueprintClasses(UBTDecorator_BlueprintBase::StaticClass());
		//FGraphNodeClassHelper::AddObservedBlueprintClasses(UBTService_BlueprintBase::StaticClass());
		ClassCache->UpdateAvailableBlueprintClasses();
	}

	TSharedPtr< FSkillEditor >NewSkillTreeEditor = MakeShareable(new FSkillEditor);
	/*if (!NewSkillTreeEditor.IsValid())
	{
		NewSkillTreeEditor = MakeShareable(new FSkillEditor);
	}*/
	if (NewSkillTreeEditor.IsValid())
	{
		NewSkillTreeEditor->InitSkillEditor(Mode, InEditWithinLevelEditor, InSkill);
	}
	return NewSkillTreeEditor.ToSharedRef();
}

#undef LOCTEXT_NAMESPACE