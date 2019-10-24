// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillTreeConnectionDrawingPolicy.h"
#include "SkillTreeColors.h"
#include "SkillTreeGraphNode.h"

FSkillTreeConnectionDrawingPolicy::FSkillTreeConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float ZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements, UEdGraph* InGraphObj)
	:FSTGraphConnectionDrawingPolicy(InBackLayerID,InFrontLayerID,ZoomFactor,InClippingRect,InDrawElements,InGraphObj)
{

}

void FSkillTreeConnectionDrawingPolicy::DetermineWiringStyle(UEdGraphPin* OutputPin, UEdGraphPin* InputPin, FConnectionParams& Params)
{
	Params.AssociatedPin1 = OutputPin;
	Params.AssociatedPin2 = InputPin;
	Params.WireThickness = 1.5f;

	Params.WireColor = SkillTreeColors::Connection::Default;

	const bool bDeemphasizeUnhoveredPins = HoveredPins.Num() > 0;
	if (bDeemphasizeUnhoveredPins)
	{
		ApplyHoverDeemphasis(OutputPin, InputPin, Params.WireThickness, Params.WireColor);
	}

	USkillTreeGraphNode* FromNode = OutputPin ? Cast<USkillTreeGraphNode>(OutputPin->GetOwningNode()) : nullptr;
	USkillTreeGraphNode* ToNode = InputPin ? Cast<USkillTreeGraphNode>(InputPin->GetOwningNode()) : nullptr;
	if (ToNode && FromNode)
	{
		if ((ToNode->bDebuggerMarkCurrentlyActive&&FromNode->bDebuggerMarkCurrentlyActive) ||
			(ToNode->bDebuggerMarkPreviouslyActive&&FromNode->bDebuggerMarkPreviouslyActive))
		{
			Params.WireThickness = 10.f;
			Params.bDrawBubbles = true;
		}
	}
}
