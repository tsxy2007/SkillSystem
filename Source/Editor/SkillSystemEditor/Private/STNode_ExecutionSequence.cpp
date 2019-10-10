// Fill out your copyright notice in the Description page of Project Settings.


#include "STNode_ExecutionSequence.h"
#include "EdGraphSchema_K2.h"

USTNode_ExecutionSequence::USTNode_ExecutionSequence(const FObjectInitializer& Objectilializer)
	:Super(Objectilializer)
{

}

void USTNode_ExecutionSequence::AllocateDefaultPins()
{
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);

	//add two default pins
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, GetPinNameGivenIndex(0));
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, GetPinNameGivenIndex(1));

	Super::AllocateDefaultPins();
}

FText USTNode_ExecutionSequence::GetTooltipText() const
{
	return NSLOCTEXT("STNode", "ExecutePinInOrder_Tooltip", "Executes a series of pins in order");
}

FLinearColor USTNode_ExecutionSequence::GetNodeTitleColor() const
{
	return FLinearColor::White;
}

FText USTNode_ExecutionSequence::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return NSLOCTEXT("STNode", "Sequence", "Sequence");
}

FSlateIcon USTNode_ExecutionSequence::GetIconAndTint(FLinearColor& OutColor) const
{
	static FSlateIcon Icon("EditorStyle", "GraphEditor.Sequence_16x");
	return Icon;
}

void USTNode_ExecutionSequence::AddInputPin()
{

}

bool USTNode_ExecutionSequence::CanAddPin() const
{

	return true;
}

FName USTNode_ExecutionSequence::GetPinNameGivenIndex(int32 Index) const
{
	return *FString::Printf(TEXT("%s_%d"), *UEdGraphSchema_K2::PN_Then.ToString(), Index);
}
