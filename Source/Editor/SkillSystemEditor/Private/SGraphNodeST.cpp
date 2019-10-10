// Fill out your copyright notice in the Description page of Project Settings.


#include "SGraphNodeST.h"
#include "SlateOptMacros.h"
#include "Framework/Application/SlateApplication.h"
#include "Editor.h"
#include "SkillGraphNode.h"
#include "SGraphPanel.h"
#include "ScopedTransaction.h"
#include "SkillEdGraph.h"

#define LOCTEXT_NAMESPACE "SKillGraph"

TSharedRef<FDragSTGraphNode> FDragSTGraphNode::New(const TSharedRef<SGraphPanel>& InGraphPanel, const TSharedRef<SGraphNode>& InDraggedNode)
{
	TSharedRef<FDragSTGraphNode> Operation = MakeShared<FDragSTGraphNode>();
	Operation->StartTime = FPlatformTime::Seconds();
	Operation->GraphPanel = InGraphPanel;
	Operation->DraggedNodes.Add(InDraggedNode);
	Operation->DecoratorAdjust = FSlateApplication::Get().GetCursorSize();
	Operation->Construct();
	return Operation;
}

TSharedRef<FDragSTGraphNode> FDragSTGraphNode::New(const TSharedRef<SGraphPanel>& InGraphPanel, const TArray<TSharedRef<SGraphNode>>& InDraggedNode)
{
	TSharedRef<FDragSTGraphNode> Operation = MakeShareable(new FDragSTGraphNode);
	Operation->StartTime = FPlatformTime::Seconds();
	Operation->GraphPanel = InGraphPanel;
	Operation->DraggedNodes.Append(InDraggedNode);
	Operation->DecoratorAdjust = FSlateApplication::Get().GetCursorSize();
	Operation->Construct();
	return Operation;
}

USkillGraphNode * FDragSTGraphNode::GetDropTargetNode() const
{
	return Cast<USkillGraphNode>(GetHoveredNode());
}

// SGraphNodeST begin----------------------------------------------------------------------------

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SGraphNodeST::Construct(const FArguments& InArgs, USkillGraphNode* InNode)
{
	SetCursor(EMouseCursor::CardinalCross);
	GraphNode = InNode;
	UpdateGraphNode();
	bDragMarkerVisible = false;
}
TSharedPtr<SToolTip> SGraphNodeST::GetComplexTooltip()
{
	return nullptr;
}

void SGraphNodeST::OnDragEnter(const FGeometry & MyGeometry, const FDragDropEvent & DragDropEvent)
{
	TSharedPtr<FDragNode> DragConnectionOp = DragDropEvent.GetOperationAs<FDragNode>();
	if (DragConnectionOp.IsValid())
	{
		TSharedPtr<SGraphNode> SubNode = GetSubNodeUnderCursor(MyGeometry, DragDropEvent);
		DragConnectionOp->SetHoveredNode(SubNode.IsValid() ? SubNode : SharedThis(this));

		USkillGraphNode* TestNode = Cast<USkillGraphNode>(GraphNode);
		if (DragConnectionOp->IsValidOperation() && TestNode&&TestNode->IsSubNode())
		{
			SetDragMarker(true);
		}
	}
	SGraphNode::OnDragEnter(MyGeometry, DragDropEvent);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

FReply SGraphNodeST::OnDragOver(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
	TSharedPtr<FDragNode> DragConnectionOp = DragDropEvent.GetOperationAs<FDragNode>();
	if (DragConnectionOp.IsValid())
	{
		TSharedPtr<SGraphNode> SubNode = GetSubNodeUnderCursor(MyGeometry, DragDropEvent);
		DragConnectionOp->SetHoveredNode(SubNode.IsValid() ? SubNode : SharedThis(this));
	}
	return SGraphNode::OnDragOver(MyGeometry,DragDropEvent);
}

FReply SGraphNodeST::OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
	SetDragMarker(false);
	TSharedPtr<FDragSTGraphNode> DragNodeOp = DragDropEvent.GetOperationAs<FDragSTGraphNode>();
	if (DragNodeOp.IsValid())
	{
		if (!DragNodeOp->IsValidOperation())
		{
			return FReply::Handled();
		}
		const float DragTime = float(FPlatformTime::Seconds() - DragNodeOp->StartTime);
		if (DragTime < 0.5f)
		{
			return FReply::Handled();
		}

		USkillGraphNode* MyNode = Cast<USkillGraphNode>(GraphNode);
		if (MyNode == nullptr || MyNode->IsSubNode())
		{
			return FReply::Unhandled();
		}

		const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "GraphEd_DragDropNode", "Drag&Drop Node"));
		bool bReorderOperation = true;
		const TArray<TSharedRef<SGraphNode>>& DraggedNodes = DragNodeOp->GetNodes();
		for (int32 Idx = 0; Idx < DraggedNodes.Num(); Idx++)
		{
			USkillGraphNode* DraggedNode = Cast<USkillGraphNode>(DraggedNodes[Idx]->GetNodeObj());
			if (DraggedNode&& DraggedNode->ParentNode)
			{
				if (DraggedNode->ParentNode != GraphNode)
				{
					bReorderOperation = false;
				}
				DraggedNode->ParentNode->RemoveSubNode(DraggedNode);
			}
		}

		USkillGraphNode* DropTargetNode = DragNodeOp->GetDropTargetNode();
		const int32 InsertIndex = MyNode->FindSubNodeDropIndex(DropTargetNode);

		for (int32 i = 0 ;i<DraggedNodes.Num();i++)
		{
			USkillGraphNode* DraggedTestNode = Cast<USkillGraphNode>(DraggedNodes[i]->GetNodeObj());
			DraggedTestNode->Modify();
			DraggedTestNode->ParentNode = MyNode;

			MyNode->Modify();
			MyNode->InsertSubNodeAt(DraggedTestNode, InsertIndex);
		}
		if (bReorderOperation)
		{
			UpdateGraphNode();
		}
		else
		{
			USkillEdGraph* MyGraph = MyNode->GetSkillGraph();
			if (MyGraph)
			{
				MyGraph->OnSubNodeDropped();
			}
		}
	}
	return SGraphNode::OnDrop(MyGeometry, DragDropEvent);
}

void SGraphNodeST::OnDragLeave(const FDragDropEvent& DragDropEvent)
{
	TSharedPtr<FDragNode> DragNodeOp = DragDropEvent.GetOperationAs<FDragNode>();
	if (DragNodeOp.IsValid())
	{
		DragNodeOp->SetHoveredNode(TSharedPtr<SGraphNode>(NULL));
	}
	SetDragMarker(false);
	SGraphNode::OnDragLeave(DragDropEvent);
}

FReply SGraphNodeST::OnMouseMove(const FGeometry & MyGeometry, const FPointerEvent & MouseEvent)
{
	if (MouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton) && !(GEditor->bIsSimulatingInEditor || GEditor->PlayWorld))
	{
		USkillGraphNode* TestNode = Cast<USkillGraphNode>(GraphNode);
		if (TestNode && TestNode->IsSubNode())
		{
			const TSharedRef<SGraphPanel>& Panel = GetOwnerPanel().ToSharedRef();
			const TSharedRef<SGraphNode>& Node = SharedThis(this);
			return FReply::Handled().BeginDragDrop(FDragSTGraphNode::New(Panel, Node));
		}
	}

	if (!MouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton) && bDragMarkerVisible)
	{
		SetDragMarker(false);
	}

	return FReply::Unhandled();
}

void SGraphNodeST::SetOwner(const TSharedRef<SGraphPanel>& OwnerPanel)
{
	SGraphNode::SetOwner(OwnerPanel);
	for (auto& ChildWidget : SubNodes)
	{
		if (ChildWidget.IsValid())
		{
			ChildWidget->SetOwner(OwnerPanel);
			OwnerPanel->AttachGraphEvents(ChildWidget);
		}
	}
}

void SGraphNodeST::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
{
	PinToAdd->SetOwner(SharedThis(this));
	const UEdGraphPin* PinObj = PinToAdd->GetPinObj();
	const bool bAdvanceParameter = PinObj && PinObj->bAdvancedView;

	if (bAdvanceParameter)
	{
		PinToAdd->SetVisibility(TAttribute<EVisibility>(PinToAdd, &SGraphPin::IsPinVisibleAsAdvanced));
	}

	if (PinToAdd->GetDirection() == EEdGraphPinDirection::EGPD_Input)
	{
		LeftNodeBox->AddSlot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.FillHeight(1.f)
			[
				PinToAdd
			];
		InputPins.Add(PinToAdd);
	}
	else
	{
		RightNodeBox->AddSlot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.FillHeight(1.f)
			[
				PinToAdd
			];
		OutputPins.Add(PinToAdd);
	}
}

FReply SGraphNodeST::OnMouseDown(const FGeometry & SenderGeometry, const FPointerEvent & MouseEvent)
{
	if (MouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton) && !(GEditor->bIsSimulatingInEditor || GEditor->PlayWorld))
	{
		USkillGraphNode* TestNode = Cast<USkillGraphNode>(GraphNode);
		if (TestNode&&TestNode->IsSubNode())
		{
			const TSharedRef<SGraphPanel>& Panel = GetOwnerPanel().ToSharedRef();
			const TSharedRef<SGraphNode>& Node = SharedThis(this);
			TSharedRef<FDragSTGraphNode> InGraphNode = FDragSTGraphNode::New(Panel, Node);
			return FReply::Handled().BeginDragDrop(InGraphNode);
		}
	}

	if (!MouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton)&&bDragMarkerVisible)
	{
		SetDragMarker(false);
	}

	return FReply::Unhandled();
}

void SGraphNodeST::AddSubNode(TSharedPtr<SGraphNode> SubNodeWidget)
{
	SubNodes.Add(SubNodeWidget);
}

TSharedPtr<SGraphNode> SGraphNodeST::GetSubNodeUnderCursor(const FGeometry & WidgetGeometry, const FPointerEvent & MouseEvent)
{
	TSharedPtr<SGraphNode> ResultNode;
	TSet<TSharedRef<SWidget>> SubWidgetsSet;
	for (int32 i = 0; i < SubNodes.Num(); i++)
	{
		SubWidgetsSet.Add(SubNodes[i].ToSharedRef());
	}
	TMap<TSharedRef<SWidget>, FArrangedWidget> Result;
	FindChildGeometries(WidgetGeometry, SubWidgetsSet, Result);

	if (Result.Num() > 0)
	{
		FArrangedChildren ArrangedChildren(EVisibility::Visible);
		Result.GenerateValueArray(ArrangedChildren.GetInternalArray());
		const int32 HoveredIndex = SWidget::FindChildUnderMouse(ArrangedChildren, MouseEvent);
		if (HoveredIndex != INDEX_NONE)
		{
			ResultNode = StaticCastSharedRef<SGraphNode>(ArrangedChildren[HoveredIndex].Widget);
		}
	}

	return ResultNode;
}

EVisibility SGraphNodeST::GetDragOverMarkerVisibility() const
{
	return bDragMarkerVisible ? EVisibility::Visible : EVisibility::Collapsed;
}

void SGraphNodeST::SetDragMarker(bool bEnabled)
{
	bDragMarkerVisible = bEnabled;
}

FText SGraphNodeST::GetDescription() const
{
	USkillGraphNode* MyNode = CastChecked<USkillGraphNode>(GraphNode);
	return IsValid(MyNode) ? MyNode->GetDescription() : FText::GetEmpty();
}

EVisibility SGraphNodeST::GetDescriptionVisibility() const
{
	TSharedPtr<SGraphPanel> MyOwnerPanel = GetOwnerPanel();
	return (!MyOwnerPanel.IsValid() || MyOwnerPanel->GetCurrentLOD() > EGraphRenderingLOD::LowDetail) ? EVisibility::Visible : EVisibility::Collapsed;
}

FText SGraphNodeST::GetPreviewCornerText() const
{
	return FText::GetEmpty();
}

const FSlateBrush * SGraphNodeST::GetNameIcon() const
{
	return FEditorStyle::GetBrush(TEXT("Graph.StateNode.Icon"));
}

// SGraphPinST begin ------------------------------------------------------------------------------
void SGraphPinST::Construct(const FArguments & InArgs, UEdGraphPin * InPin)
{
	this->SetCursor(EMouseCursor::Default);
	bShowLabel = true;

	GraphPinObj = InPin;
	check(GraphPinObj != nullptr);

	const UEdGraphSchema* Schema = GraphPinObj->GetSchema();
	check(Schema);

	SBorder::Construct(SBorder::FArguments()
		.BorderImage(this, &SGraphPinST::GetPinBorder)
		.BorderBackgroundColor(this, &SGraphPinST::GetPinColor)
		.OnMouseButtonDown(this, &SGraphPinST::OnPinMouseDown)
		.Cursor(this, &SGraphPinST::GetPinCursor)
		.Padding(FMargin(10.f))
	);
}

FSlateColor SGraphPinST::GetPinColor() const
{
	return FSlateColor(IsHovered() ? FLinearColor::Yellow : FLinearColor::Black);
}

TSharedRef<SWidget> SGraphPinST::GetDefaultValueWidget()
{
	return SNew(STextBlock);
}

const FSlateBrush * SGraphPinST::GetPinBorder() const
{
	return FEditorStyle::GetBrush(TEXT("Graph.StateNode.Body"));
}

// SGraphPinST End---------------------------------------------------------------------------------
#undef LOCTEXT_NAMESPACE
