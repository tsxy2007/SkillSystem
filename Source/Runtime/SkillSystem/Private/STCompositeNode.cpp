// Fill out your copyright notice in the Description page of Project Settings.


#include "STCompositeNode.h"

USTCompositeNode::USTCompositeNode(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{

}

USTCompositeNode::~USTCompositeNode()
{

}

void USTCompositeNode::InitializeComposite(uint16 InLastExecuionIndex)
{

}

int32 USTCompositeNode::GetChildIndex(const USTNode& ChildNode) const
{
	return 0;
}

bool USTCompositeNode::CanAbortLowerPriority() const
{
	return false;
}

bool USTCompositeNode::CanAbortSelf() const
{
	return false;
}
