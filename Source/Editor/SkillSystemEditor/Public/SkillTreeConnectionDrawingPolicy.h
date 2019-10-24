// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "STGraphConnectionDrawingPolicy.h"
/**
 * 
 */
class SKILLSYSTEMEDITOR_API FSkillTreeConnectionDrawingPolicy : public FSTGraphConnectionDrawingPolicy
{
public:
	FSkillTreeConnectionDrawingPolicy(int32 InBackLayerID,int32 InFrontLayerID,float ZoomFactor,const FSlateRect& InClippingRect,FSlateWindowElementList& InDrawElements,UEdGraph* InGraphObj);
	
	virtual void DetermineWiringStyle(UEdGraphPin* OutputPin, UEdGraphPin* InputPin, FConnectionParams& Params) override;
};
