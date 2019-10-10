// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Layout/Visibility.h"
#include "Styling/SlateColor.h"
#include "Input/DragAndDrop.h"
#include "Input/Reply.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SWidget.h"
#include "SGraphNode.h"
#include "SGraphPin.h"
#include "Editor/GraphEditor/Private/DragNode.h"

class SGraphPanel;
class SToolTip;
class USkillGraphNode;

class FDragSTGraphNode : public FDragNode
{
public:
	DRAG_DROP_OPERATOR_TYPE(FDragSTGraphNode, FDragNode)

	static TSharedRef<FDragSTGraphNode> New(const TSharedRef<SGraphPanel>& InGraphPanel, const TSharedRef<SGraphNode>& InDraggedNode);
	static TSharedRef<FDragSTGraphNode> New(const TSharedRef<SGraphPanel>& InGraphPanel, const TArray<TSharedRef<SGraphNode>>& InDraggedNode);

	USkillGraphNode* GetDropTargetNode()const;
	
	double StartTime;

protected:
	typedef FDragNode Super;
};

/**
 * 
 */
class SKILLSYSTEMEDITOR_API SGraphNodeST : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SGraphNodeST){}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs,USkillGraphNode* InNode);

	// begin
	virtual TSharedPtr<SToolTip> GetComplexTooltip() override;
	virtual void OnDragEnter(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;
	virtual FReply OnDragOver(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;
	virtual FReply OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;
	virtual void OnDragLeave(const FDragDropEvent& DragDropEvent) override;
	virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void SetOwner(const TSharedRef<SGraphPanel>& OwnerPanel) override;
	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;
	//end

	// handle mouse down on the node
	FReply OnMouseDown(const FGeometry& SenderGeometry, const FPointerEvent& MouseEvent);

	// add subnode widget inside current node
	virtual void AddSubNode(TSharedPtr<SGraphNode> SubNodeWidget);

	// gets server net node if one is found under mouse cursor
	TSharedPtr<SGraphNode> GetSubNodeUnderCursor(const FGeometry& WidgetGeometry, const FPointerEvent& MouseEvent);

	// gets drag over marker visibility
	EVisibility GetDragOverMarkerVisibility() const;

	// set drag marker visible or collapsed on this node
	void SetDragMarker(bool bEnabled);

protected:
	TArray<TSharedPtr<SGraphNode>> SubNodes;

	uint32 bDragMarkerVisible : 1;

	virtual FText GetDescription()const;
	virtual EVisibility GetDescriptionVisibility()const;

	virtual FText GetPreviewCornerText()const;
	virtual const FSlateBrush* GetNameIcon()const;
};

class SKILLSYSTEMEDITOR_API SGraphPinST : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SGraphPinST){}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InPin);
protected:

	// Begin SGraphPin Interface
	virtual FSlateColor GetPinColor() const override;
	virtual TSharedRef<class SWidget> GetDefaultValueWidget() override;
	// End SGraphPin Interface

	const FSlateBrush* GetPinBorder() const;
};