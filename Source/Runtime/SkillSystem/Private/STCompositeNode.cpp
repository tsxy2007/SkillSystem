// Fill out your copyright notice in the Description page of Project Settings.


#include "STCompositeNode.h"
#include "VisualLogger.h"

USTCompositeNode::USTCompositeNode(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	bApplyDecoratorScope = true;
}

USTCompositeNode::~USTCompositeNode()
{

}

void USTCompositeNode::InitializeComposite(uint16 InLastExecuionIndex)
{
	LastExecutionIndex = InLastExecuionIndex;
}

int32 USTCompositeNode::GetChildIndex(const USTNode& ChildNode) const
{
	return 0;
}

bool USTCompositeNode::IsApplyingDecoratorScope() const
{
	return bApplyDecoratorScope;
}

bool USTCompositeNode::CanAbortLowerPriority() const
{
	return false;
}

bool USTCompositeNode::CanAbortSelf() const
{
	return false;
}

void USTCompositeNode::DoActive()
{
	UE_VLOG(GetOuter(), LogTemp, Warning, TEXT("USTCompositeNode::DoActive = "));
	for (int32 i = 0; i < Children.Num(); i++)
	{
		Children[i].ChildComposite->DoActive();
	}
	DoDeactive();
}

void USTCompositeNode::DoDeactive()
{
	UE_VLOG(GetOuter(), LogTemp, Warning, TEXT("USTCompositeNode::DoDeactive = "));
}
