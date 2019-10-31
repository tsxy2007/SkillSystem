// Fill out your copyright notice in the Description page of Project Settings.


#include "STComposite_Selector.h"

USTComposite_Selector::USTComposite_Selector(const FObjectInitializer& Obj) : Super(Obj)
{
	NodeName = "Selector";
}

#if WITH_EDITOR

FName USTComposite_Selector::GetNodeIconName() const
{
	return FName("BTEditor.Graph.BTNode.Composite.Selector.Icon");
}

#endif