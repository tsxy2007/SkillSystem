// Fill out your copyright notice in the Description page of Project Settings.


#include "EdGraphSchema_SkillTree.h"
#include "ScopedTransaction.h"
#include "STGraphNode.h"
#include "EdGraph/EdGraph.h"
#include "SkillSystemEditor.h"
#include "SkillGraphTypes.h"
#include "STNode.h"
#include "SkillGraphNode_Composite.h"
#include "SkillGraphNode_Root.h"
#include "SkillTreeConnectionDrawingPolicy.h"
#include "SkillEdGraph.h"
#include "Modules/ModuleManager.h"
#include "SkillTreeEditorTypes.h"
#include "STCompositeNode.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "GraphEditorActions.h"
#include "SkillEditor.h"
#include "Toolkits/ToolkitManager.h"
#include "EdGraphNode_Comment.h"
#include "Skill.h"

#define LOCTEXT_NAMESPACE "SkillTreeEditor"

int32 UEdGraphSchema_SkillTree::CurrentCacheRefreshID = 0;

UEdGraphNode* FSkillTreeSchemaAction_AutoArrange::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode /*= true*/)
{
	USkillEdGraph* Graph = Cast<USkillEdGraph>(ParentGraph);
	if (Graph)
	{
		Graph->AutoArrange();
	}
	return NULL;
}

UEdGraphNode* FSkillTreeSchemaAction_AddComment::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode /*= true*/)
{
	UEdGraphNode_Comment* const CommentTemplate = NewObject<UEdGraphNode_Comment>();
	FVector2D SpawnLocation = Location;
	TSharedPtr<ISkillEditor> STEditor;
	if (USkill* const STAsset = Cast<USkill>(ParentGraph->GetOuter()))
	{
		TSharedPtr<IToolkit> STAssetEditor = FToolkitManager::Get().FindEditorForAsset(STAsset);
		if (STAssetEditor.IsValid())
		{
			STEditor = StaticCastSharedPtr<ISkillEditor>(STAssetEditor);
		}
	}
	FSlateRect Bounds;
	if (STEditor.IsValid()&&STEditor->GetBoundsForSelectedNodes(Bounds,50.f))
	{
		CommentTemplate->SetBounds(Bounds);
		SpawnLocation.X = CommentTemplate->NodePosX;
		SpawnLocation.Y = CommentTemplate->NodePosY;

	}
	UEdGraphNode* const NewNode = FEdGraphSchemaAction_NewNode::SpawnNodeFromTemplate<UEdGraphNode_Comment>(ParentGraph, CommentTemplate, SpawnLocation, bSelectNewNode);
	return NewNode;
}


UEdGraphSchema_SkillTree::UEdGraphSchema_SkillTree(const FObjectInitializer& Obj) : Super(Obj)
{
}

void UEdGraphSchema_SkillTree::CreateDefaultNodesForGraph(UEdGraph& Graph) const
{
	FGraphNodeCreator<USkillGraphNode_Root> NodeCreator(Graph);
	USkillGraphNode_Root * MyNode = NodeCreator.CreateNode();
	NodeCreator.Finalize();
	SetNodeMetaData(MyNode, FNodeMetadata::DefaultGraphNode);
}

void UEdGraphSchema_SkillTree::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	const FName PinCategory = ContextMenuBuilder.FromPin ? ContextMenuBuilder.FromPin->PinType.PinCategory : USkillTreeEditorTypes::PinCategory_MultipleNodes;
	const bool bNoParent = (ContextMenuBuilder.FromPin == NULL);
	const bool bOnlyTasks = (PinCategory == USkillTreeEditorTypes::PinCategory_SingleTask);
	const bool bOnlyComposite = (PinCategory == USkillTreeEditorTypes::PinCategory_SingleComposite);
	const bool bAllowComposites = bNoParent || !bOnlyTasks || bOnlyComposite;
	const bool bAllowTasks = bNoParent || !bOnlyComposite || bOnlyTasks;

	FSkillSystemEditorModule& EditorModule = FModuleManager::GetModuleChecked<FSkillSystemEditorModule>(TEXT("SkillSystemEditor"));

	FSkillGraphNodeClassHelper* ClassCache = EditorModule.GetClassCache().Get();

	if (bAllowComposites)
	{
		FCategorizedGraphActionListBuilder CompositesBuilder(TEXT("Composites"));
		TArray<FSkillGraphNodeClassData> NodeClasses;
		ClassCache->GatherClasses(USTCompositeNode::StaticClass(), NodeClasses);
		
		for (const auto& NodeClass : NodeClasses)
		{
			const FText NodeTypeName = FText::FromString(FName::NameToDisplayString(NodeClass.ToString(), false));
			TSharedPtr<FSTSchemaAction_NewNode> AddOpAction = USTGraphSchema::AddNewNodeAction(CompositesBuilder, NodeClass.GetCategory(), NodeTypeName, FText::GetEmpty());
			UClass* GraphNodeClass = USkillGraphNode_Composite::StaticClass();

			USkillTreeGraphNode* OpNode = NewObject<USkillTreeGraphNode>(ContextMenuBuilder.OwnerOfTemporaries, GraphNodeClass);
			OpNode->ClassData = NodeClass;
			AddOpAction->NodeTemplate = OpNode;
		}
		ContextMenuBuilder.Append(CompositesBuilder);
	}
	if (bAllowTasks)
	{
		// TODO: TASK;
		FCategorizedGraphActionListBuilder TaskBuilder(TEXT("Tasks"));
		TArray<FSkillGraphNodeClassData> NodeClasses;
		//ClassCache->GatherClasses(USTTaskNode::)
	}
	if (bNoParent)
	{
		TSharedPtr<FSkillTreeSchemaAction_AutoArrange> Action = TSharedPtr<FSkillTreeSchemaAction_AutoArrange>(
			new FSkillTreeSchemaAction_AutoArrange(FText::GetEmpty(), LOCTEXT("AutoArrange", "Auto Arrange"), FText::GetEmpty(), 0)
			);
		ContextMenuBuilder.AddAction(Action);
	}
}

void UEdGraphSchema_SkillTree::GetContextMenuActions(const UEdGraph* CurrentGraph, const UEdGraphNode* InGraphNode, const UEdGraphPin* InGraphPin, class FMenuBuilder* MenuBuilder, bool bIsDebugging) const
{
	if (InGraphNode && !InGraphPin)
	{
		const USkillTreeGraphNode* STGraphNode = Cast<const USkillTreeGraphNode>(InGraphNode);
		if (STGraphNode && STGraphNode->CanPlaceBreakpoints())
		{
			MenuBuilder->BeginSection("EdGraphSchemaBreakpoints", LOCTEXT("BreakpointsHeader", "Breakpoints"));
			{
				MenuBuilder->AddMenuEntry(FGraphEditorCommands::Get().ToggleBreakpoint);
				MenuBuilder->AddMenuEntry(FGraphEditorCommands::Get().AddBreakpoint);
				MenuBuilder->AddMenuEntry(FGraphEditorCommands::Get().RemoveBreakpoint);
				MenuBuilder->AddMenuEntry(FGraphEditorCommands::Get().EnableBreakpoint);
				MenuBuilder->AddMenuEntry(FGraphEditorCommands::Get().DisableBreakpoint);
			}
			MenuBuilder->EndSection();
		}
	}
	Super::GetContextMenuActions(CurrentGraph, InGraphNode, InGraphPin, MenuBuilder, bIsDebugging);
}

const FPinConnectionResponse UEdGraphSchema_SkillTree::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
{
	if (A->GetOwningNode() == B->GetOwningNode())
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("PinErrorSameNode", "Both are on the same node"));
	}
	return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, LOCTEXT("PinConnect", "Connect nodes"));
}

const FPinConnectionResponse UEdGraphSchema_SkillTree::CanMergeNodes(const UEdGraphNode* A, const UEdGraphNode* B) const
{
	if (A == B)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Both are the same node"));
	}
	//TODO: debug
	return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT(""));
}

FLinearColor UEdGraphSchema_SkillTree::GetPinTypeColor(const FEdGraphPinType& PinType) const
{
	return FColor::White;
}

class FConnectionDrawingPolicy* UEdGraphSchema_SkillTree::CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, class FSlateWindowElementList& InDrawElements, class UEdGraph* InGraphObj) const
{
	return new FSkillTreeConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, InZoomFactor, InClippingRect, InDrawElements, InGraphObj);
}

int32 UEdGraphSchema_SkillTree::GetNodeSelectionCount(const UEdGraph* Graph) const
{
	if (Graph)
	{
		TSharedPtr<ISkillEditor> STEditor;
		if (USkill* STAsset  = Cast<USkill>(Graph->GetOuter()))
		{
			TSharedPtr<IToolkit> STAssetEditor = FToolkitManager::Get().FindEditorForAsset(STAsset);
			if (STAssetEditor.IsValid())
			{
				STEditor = StaticCastSharedPtr<ISkillEditor>(STAssetEditor);
			}
		}
		if (STEditor.IsValid())
		{
			return STEditor->GetSelectedNodesCount();
		}
	}
	return 0;
}

bool UEdGraphSchema_SkillTree::IsCacheVisualizationOutOfDate(int32 InVisualizationCacheID) const
{
	return CurrentCacheRefreshID != InVisualizationCacheID;
}

int32 UEdGraphSchema_SkillTree::GetCurrentVisualizationCacheID() const
{
	return CurrentCacheRefreshID;
}

void UEdGraphSchema_SkillTree::ForceVisualizationCacheClear() const
{
	++CurrentCacheRefreshID;
}

TSharedPtr<FEdGraphSchemaAction> UEdGraphSchema_SkillTree::GetCreateCommentAction() const
{
	return TSharedPtr<FEdGraphSchemaAction>(static_cast<FEdGraphSchemaAction*>(new FSkillTreeSchemaAction_AddComment));
}

void UEdGraphSchema_SkillTree::GetGraphNodeContextActions(FGraphContextMenuBuilder& ContextMenuBuilder, int32 SubNodeFlags) const
{
	Super::GetGraphNodeContextActions(ContextMenuBuilder, SubNodeFlags);
	// TODO: 
}

void UEdGraphSchema_SkillTree::GetSubNodeClasses(int32 SubNodeFlags, TArray<FSkillGraphNodeClassData>& ClassData, UClass*& GraphNodeClass) const
{
	FSkillSystemEditorModule& EditorModule = FModuleManager::GetModuleChecked<FSkillSystemEditorModule>(TEXT("SkillSystemEditor"));
	FSkillGraphNodeClassHelper* ClassCache = EditorModule.GetClassCache().Get();
	
	// TODO: вс╫з╣Ц
}
#undef LOCTEXT_NAMESPACE