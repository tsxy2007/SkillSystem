// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "SkillGraphTypes.h"
#include "SkillGraphNode.generated.h"

/**
 * 
 */
UCLASS()
class SKILLSYSTEMEDITOR_API USkillGraphNode : public UEdGraphNode
{
	GENERATED_UCLASS_BODY()

public:

	//~beging UEDGraphNode Interface
	virtual class USkillEdGraph* GetSkillGraph();
	virtual void AutowireNewNode(UEdGraphPin* FromPin) override;
	virtual void PostPlacedNewNode()override;
	virtual void PrepareForCopying() override;
	virtual bool CanDuplicateNode() const override;
	virtual bool CanUserDeleteNode() const override;
	virtual void DestroyNode() override;
	virtual FText GetTooltipText() const override;
	virtual void NodeConnectionListChanged() override;
	virtual bool CanCreateUnderSpecifiedSchema(const UEdGraphSchema* Schema) const override;
	virtual void FindDiffs(class UEdGraphNode* OtherNode, FDiffResults& Results) override;
	virtual FString GetPropertyNameAndValueForDiff(const UProperty* Prop, const uint8* PropertyAddr) const override;
	//~End UedGraphNode Interface

	//~Begin UObject Interface
	virtual void PostEditImport() override;
	virtual void PostEditUndo() override;
	//~End UObject

	virtual UEdGraphPin* GetInputPin(int32 InputIndex = 0)const;
	virtual UEdGraphPin* GetOutputPin(int32 InputIndex = 0)const;
	virtual UEdGraph* GetBoundGraph()const { return NULL; }

	virtual FName GetNameIcon()const;


	void AddSubNode(USkillGraphNode* SubNode, class UEdGraph* ParentGraph);
	void RemoveSubNode(USkillGraphNode* SubNode);
	virtual void RemoveAllSubNodes();
	virtual void OnSubNodeRemoved(USkillGraphNode* SubNode);
	virtual void OnSubNodeAdded(USkillGraphNode* SubNode);

	virtual int32 FindSubNodeDropIndex(USkillGraphNode* SubNode) const;
	virtual void InsertSubNodeAt(USkillGraphNode* SubNode, int32 DropIndex);

	virtual bool IsSubNode()const;

	virtual FText GetDescription() const;

	virtual void PostCopyNode();

	virtual void InitializeInstance();

	virtual bool RefreshNodeClass();

	virtual void UpdateNodeClassData();

	bool UsesBlueprint()const;

	virtual bool HasErrors()const;

	static void UpdateNodeClassDataFrom(UClass* InstanceClass, FSkillGraphNodeClassData& UpdatedData);

protected:
	virtual void ResetNodeOwner();
public:
	UPROPERTY()
		FSkillGraphNodeClassData ClassData;

	UPROPERTY(transient)
		USkillGraphNode* ParentNode;

	UPROPERTY()
		TArray<USkillGraphNode*> SubNodes;

	UPROPERTY()
		UObject* NodeInstance;

	UPROPERTY()
		FString ErrorMessage;

	UPROPERTY()
		int32 CopySubNodeIndex;

	UPROPERTY()
		uint32 bIsReadOnly : 1;

	UPROPERTY()
		uint32 bIsSubNode : 1;
};
