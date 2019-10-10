// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "STNode.h"
#include "STCompositeNode.generated.h"

class USTCompositeNode;

struct FSTCompositeMemory
{
	int8 CurrentChild;
	int8 OverrideChild;
};

UENUM()
enum class ESTChildIndex : uint8
{
	FirstNode,
	TaskNode,
};

UENUM()
namespace ESTDecoratorLogic
{
	enum Type
	{
		Invalid,
		Test,
		And,
		Or,
		Not,
	};
}

USTRUCT()
struct FSTDecoratorLogic
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		TEnumAsByte<ESTDecoratorLogic::Type> Operation;

	UPROPERTY()
		uint16 Number;

	FSTDecoratorLogic():Operation(ESTDecoratorLogic::Invalid),Number(0){}
	FSTDecoratorLogic(uint8 InOperationi,uint16 InNumber) : Operation(InOperationi),Number(InNumber){}
};

USTRUCT()
struct FSTCompositeChild
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		USTCompositeNode* ChildComposite;

};

UCLASS(Abstract)
class SKILLSYSTEM_API USTCompositeNode : public USTNode
{
	GENERATED_UCLASS_BODY()
public:
	UPROPERTY()
	TArray<FSTCompositeChild> Children;

	~USTCompositeNode();

	void InitializeComposite(uint16 InLastExecuionIndex);


	int32 GetChildIndex(const USTNode& ChildNode) const;



#if WITH_EDITOR
	virtual bool CanAbortLowerPriority() const;
	virtual bool CanAbortSelf()const;
#endif
};

