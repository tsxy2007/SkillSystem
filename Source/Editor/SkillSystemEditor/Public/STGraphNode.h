// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "EdGraph/EdGraphNode.h"
#include "SkillGraphTypes.h"
#include "STGraphNode.generated.h"

/**
 * 
 */
UCLASS()
class SKILLSYSTEMEDITOR_API USTGraphNode : public UEdGraphNode
{
	GENERATED_UCLASS_BODY()

public:
	UPROPERTY()
	struct FSkillGraphNodeClassData ClassData;

	UPROPERTY()
	UObject* NodeInstance;

	UPROPERTY(transient)
	USTGraphNode* ParentNode;

	UPROPERTY()
	TArray<USTGraphNode*> SubNodes;

	UPROPERTY()
	int32 CopySubNodeIndex;

	UPROPERTY()
	uint32 bIsReadOnly : 1;

	UPROPERTY()
	uint32 bIsSubNode : 1;

	UPROPERTY()
	FString ErrorMessage;
public:

	// ~begin UEdGraphNode Interface
	virtual class USkillEdGraph* GetSTGraph();
	virtual void AutowireNewNode(UEdGraphPin* FromPin) override;
	virtual void PostPlacedNewNode() override;
	virtual void PrepareForCopying() override;
	virtual bool CanDuplicateNode() const override;
	virtual bool CanUserDeleteNode() const override;
	virtual void DestroyNode() override;
	virtual FText GetTooltipText() const override;
	virtual void NodeConnectionListChanged() override;
	virtual bool CanCreateUnderSpecifiedSchema(const UEdGraphSchema* Schema) const override;
	virtual void FindDiffs(class UEdGraphNode* OtherNode, FDiffResults& Results) override;
	virtual FString GetPropertyNameAndValueForDiff(const UProperty* Prop, const uint8* PropertyAddr) const override;
	//~end UEdGraphNode Interface

	// ~Begin UOBject Interface
#if WITH_EDITOR
	virtual void PostEditImport() override;
	virtual void PostEditUndo() override;
#endif
	//~End UObject Interface
	
	//@return the input pin for this state
	virtual UEdGraphPin* GetInputPin(int32 InputIndex = 0) const;
	//@return the output pin for this state
	virtual UEdGraphPin* GetOutputPin(int32 OutputIndex = 0) const;
	virtual UEdGraphPin* GetBoundGraph()const { return nullptr; }

	virtual FText GetDescription()const;
	virtual void PostCopyNode();

	void AddSubNode(USTGraphNode* SubNode, class UEdGraph* ParentGraph);
	void RemoveSubNode(USTGraphNode* SubNode);
	virtual void RemoveAllSubNodes();
	virtual void OnSubNodeRemoved(USTGraphNode* SubNode);
	virtual void OnSubNodeAdded(USTGraphNode* SubNode);

	virtual int32 FindSubNodeDropIndex(USTGraphNode* SubNode)const;
	virtual void InsertSubNodeAt(USTGraphNode* SubNode, int32 DropIndex);

	/*check if node is subnode*/
	virtual bool IsSubNode()const;

	/**/
	virtual void InitializeInstance();

	// 
	virtual bool RefreshNodeClass();

	//
	virtual void UpdateNodeClassData();

	//
	bool UsesBlueprint()const;

	//
	virtual bool HasErrors()const;

	//
	static void UpdateNodeClassDataFrom(UClass* InstanceClass, FSkillGraphNodeClassData& UpdateData);

protected:
	virtual void ResetNodeOwner();
};
