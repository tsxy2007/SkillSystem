// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraph.h"
#include "SkillEdGraph.generated.h"

/**
 * 
 */
UCLASS()
class SKILLSYSTEMEDITOR_API USkillEdGraph : public UEdGraph
{
	GENERATED_UCLASS_BODY()
	
	enum EUpdateFlags
	{
		RebuildGraph = 0,
		ClearDebuggerFlags = 1,
		KeepRebuildCounter = 2,
	};

	UPROPERTY()
	int32 ModCounter;
public:
	UPROPERTY()
	int32 GrapVersion;

	virtual void OnCreated();
	virtual void OnLoaded();
	virtual void Initialize();

	virtual void UpdateAsset(int32 UpdateFlags = 0);
	virtual void UpdateVersion();
	virtual void MarkVersion();

	void CreateSTFromGraph(class USkillTreeGraphNode* RootEdNode);

	virtual void OnSubNodeDropped();
	virtual void OnNodesPasted(const FString& ImportStr);
	virtual void OnSave();

	bool UpdateUnknownNodeClasses();
	bool UpdateDeprecatedClasses();
	void RemoveOrphanedNodes();
	void UpdateClassData();

	bool IsLocked()const;
	void LockedUpdates();
	void UnlockUpdates();

	void RebuildChildOrder(UEdGraphNode* ParentNode);

	virtual void Serialize(FArchive& Ar) override;


	void AutoArrange();
protected:
	uint32 bLockUpdates : 1;
	virtual void CollectAllNodeInstance(TSet<UObject*>& NodeInstances);
	virtual bool CanRemoveNestdObject(UObject* TestObject)const;
	virtual void OnNodeInstanceRemoved(UObject* NodeInstance);

	UEdGraphPin* FindGraphNodePin(UEdGraphNode* Node, EEdGraphPinDirection Dir);
};
