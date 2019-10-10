// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SkillTreeEditorTypes.generated.h"


struct FNodeBounds
{
	FVector2D Position;
	FVector2D Size;
	FNodeBounds(FVector2D InPos, FVector2D InSize)
	{
		Position = InPos;
		Size = InSize;
	}
};

UCLASS()
class SKILLSYSTEMEDITOR_API USkillTreeEditorTypes : public UObject
{
	GENERATED_UCLASS_BODY()
public:
	static const FName PinCategory_MultipleNodes;
	static const FName PinCategory_SingleComposite;
	static const FName PinCategory_SingleTask;
	static const FName PinCategory_SingleNode;
};
