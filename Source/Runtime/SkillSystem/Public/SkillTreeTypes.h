// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SkillTreeTypes.generated.h"

class USTNode;


#define USE_SKILLTREE_DEBUGGER (1&&WITH_EDITORONLY_DATA)

UENUM(BlueprintType)
namespace ESTNodeResult
{
	enum Type
	{
		Successed,
		Failed,
		Aborted,
		InProgress,
	};
}

namespace ESTExecutionMode
{
	enum Type
	{
		SingleRun,
		Looped,
	};
}

namespace ESTStopMode
{
	enum Type
	{
		Safe,
		Forced,
	};
}

namespace ESTMemoryInit
{
	enum Type
	{
		Initialize,
		RestoreSubtree,
	};
}

namespace ESTMemoryClear
{
	enum Type
	{
		Destory,
		StoreSubtree,
	};
}

UENUM()
namespace ESTFlowAbortMode
{
	enum Type
	{
		None			UMETA(DisplayName="Nothing"),
		LowerPriority	UMETA(DisplayName="Lower Priority"),
		Self			UMETA(DisplayName="Self"),
		Both			UMETA(DisplayName="Both"),
	};
}

namespace ESTActiveNode
{
	enum Type
	{
		Composite,
		ActiveTask,
		AbortingTask,
		InactiveTask,
	};
}

namespace ESTTaskStatus
{
	enum Type
	{
		Active,
		Aborting,
		Inactive,
	};
}

namespace ESTNodeUpdateMode
{
	enum Type
	{
		Unkown,
		Add,
		Remove,
	};
}



UCLASS(Abstract)
class SKILLSYSTEM_API USkillTreeTypes : public UObject
{
	GENERATED_BODY()

	static FString STLoggingContext;
public:
	//static FString DescribeNodeHelper(const USTNode* Node);
	//static FString DescribeNodeResult(ESTNodeResult::Type NodeResult);
	//static FString DescribeActiveNode(ESTFlowAbortMode::Type FlowAbortMode);

};
