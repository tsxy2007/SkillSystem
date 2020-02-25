// Fill out your copyright notice in the Description page of Project Settings.


#include "Manage/SkillTreeManager.h"
#include "Skill.h"
#include "STCompositeNode.h"
#include "UObject/UObjectHash.h"
#include "UObject/Class.h"
#include "UObject/UObjectIterator.h"
#include "EngineGlobals.h"
#include "Engine/Engine.h"
#include "Stats/Stats.h"
#if WITH_EDITOR
#include "Kismet2/KismetEditorUtilities.h"
#endif // WITH_EDITOR
#include "SkillSystem.h"

DEFINE_STAT(STAT_ST_SkillTree_LoadTime);

struct FNodeInitializationData
{
	USTNode* Node;
	USTCompositeNode* ParentNode;
	uint16 ExecutionIndex;
	uint16 DataSize;
	uint16 SpecialDataSize;
	uint8 TreeDepth;

	FNodeInitializationData() {}
	FNodeInitializationData(USTNode* InNode, USTCompositeNode* InParentNode,
		uint16 InExecutionIndex, uint8 InTreeDepth, uint16 NodeMemory, uint16 SpecialNodeMemory = 0)
		: Node(InNode), ParentNode(InParentNode), ExecutionIndex(InExecutionIndex), TreeDepth(InTreeDepth)
	{
		SpecialDataSize = USkillTreeManager::GetAlignedDataSize(SpecialNodeMemory);

		const uint16 NodeMemorySize = NodeMemory + SpecialDataSize;
		DataSize = (NodeMemorySize <= 2) ? NodeMemorySize : USkillTreeManager::GetAlignedDataSize(NodeMemorySize);
	}

	struct FMemorySort
	{
		FORCEINLINE bool operator()(const FNodeInitializationData& A, const FNodeInitializationData& B) const
		{
			return A.DataSize > B.DataSize;
		}
	};
};

static void InitializeNodeHelper(USTCompositeNode* ParentNode, USTNode* NodeOb, uint8 TreeDepth, uint16& ExecutionIndex,
	TArray<FNodeInitializationData>& InitList, USkill& TreeAsset, UObject* NodeOuter)
{
	InitList.Add(FNodeInitializationData(NodeOb, ParentNode, ExecutionIndex, TreeDepth,NodeOb->GetInstanceMemorySize(),NodeOb->GetSpecialMemorySize()));
	NodeOb->InitializeFromAsset(TreeAsset);
	ExecutionIndex++;

	USTCompositeNode* CompositeOb = Cast<USTCompositeNode>(NodeOb);
	if (CompositeOb)
	{
		for (int32 ChildIndex = 0; ChildIndex < CompositeOb->Children.Num(); ChildIndex++)
		{
			FSTCompositeChild& ChildInfo = CompositeOb->Children[ChildIndex];

			USTNode* ChildNode = NULL;
			if (ChildInfo.ChildComposite)
			{
				ChildInfo.ChildComposite = Cast<USTCompositeNode>(StaticDuplicateObject(ChildInfo.ChildComposite, NodeOuter));
				ChildNode = ChildInfo.ChildComposite;
			}
			if (ChildNode)
			{
				InitializeNodeHelper(CompositeOb, ChildNode, TreeDepth + 1, ExecutionIndex, InitList, TreeAsset, NodeOuter);
			}
		}
		CompositeOb->InitializeComposite(ExecutionIndex - 1);
	}
}

USkillTreeManager* USkillTreeManager::Instance = nullptr;

USkillTreeManager::USkillTreeManager(const FObjectInitializer& Obj)
	:Super(Obj)
{

}

bool USkillTreeManager::LoadTree(USkill& Asset, USTCompositeNode*& Root, uint16& InstanceMemorySize)
{
	SCOPE_CYCLE_COUNTER(STAT_ST_SkillTree_LoadTime);

	for (int32 TemplateIndex = 0; TemplateIndex < LoadedTemplates.Num(); TemplateIndex++)
	{
		FSkillTreeTemplateInfo& TemplateInfo = LoadedTemplates[TemplateIndex];
		if (TemplateInfo.Asset == &Asset)
		{
			Root = TemplateInfo.Template;
			InstanceMemorySize = TemplateInfo.InstanceMemorySize;
			return true;
		}
	}

	if (Asset.RootNode)
	{
		FSkillTreeTemplateInfo TemplateInfo;
		TemplateInfo.Asset = &Asset;
		TemplateInfo.Template = Cast<USTCompositeNode>(StaticDuplicateObject(Asset.RootNode,this));

		TArray<FNodeInitializationData> InitList;
		uint16 ExecutionIndex = 0;
		InitializeNodeHelper(nullptr, TemplateInfo.Template, 0, ExecutionIndex, InitList, Asset, this);

#if USE_SKILLTREE_DEBUGGER
		for (int32 Index = 0; Index < InitList.Num() - 1; Index++)
		{
			InitList[Index].Node->InitializeExecutionOrder(InitList[Index + 1].Node);
		}
#endif

		InitList.Sort(FNodeInitializationData::FMemorySort());
		uint16 MemoryOffset = 0;
		for (int32 Index = 0; Index < InitList.Num(); Index++)
		{
			FNodeInitializationData & Data = InitList[Index];
			Data.Node->InitializeNode(Data.ParentNode, Data.ExecutionIndex
				, Data.SpecialDataSize + MemoryOffset, Data.TreeDepth);
			MemoryOffset += Data.DataSize;
		}
		TemplateInfo.InstanceMemorySize = MemoryOffset;

		LoadedTemplates.Add(TemplateInfo);
		Root = TemplateInfo.Template;
		InstanceMemorySize = TemplateInfo.InstanceMemorySize;
		return true;
	}

	return false;
}

USkillTreeManager* USkillTreeManager::Get(UObject* InWorldContext)
{
	USkillTreeManager* Manager = FSkillSystemModule::Get().GetCurrentManager();
	if (Manager == nullptr)
	{
		if (UWorld* World = GEngine->GetWorldFromContextObject(InWorldContext, EGetWorldErrorMode::LogAndReturnNull))
		{
			// Only add functional test managers for the PIE and Game Worlds.
			if (World->WorldType == EWorldType::PIE || World->WorldType == EWorldType::Game)
			{
				Manager = NewObject<USkillTreeManager>(World);
				FSkillSystemModule::Get().SetManager(Manager);
				Manager->AddToRoot();
				FWorldDelegates::OnWorldCleanup.AddUObject(Manager, &USkillTreeManager::OnWorldCleanedUp);
			}
		}
		else
		{
			ensureMsgf(false, TEXT("Tried to add a functional test manager to a non-game world."));
		}
	}

	return Manager;
}

void USkillTreeManager::OnWorldCleanedUp(UWorld* World, bool bSessionEnded, bool bCleanupResources)
{
	UWorld* MyWorld = GetWorld();
	if (MyWorld == World)
	{
		RemoveFromRoot();
	}
}

UWorld* USkillTreeManager::GetWorld() const
{
	return GEngine->GetWorldFromContextObjectChecked(GetOuter());
}

int32 USkillTreeManager::GetAlignedDataSize(int32 Size)
{
	return ((Size + 3)&~3);
}
