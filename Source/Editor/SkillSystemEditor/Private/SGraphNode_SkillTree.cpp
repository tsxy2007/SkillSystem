// Fill out your copyright notice in the Description page of Project Settings.


#include "SGraphNode_SkillTree.h"
#include "Types/SlateStructs.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SToolTip.h"
#include "STNode.h"
#include "Editor.h"
#include "GraphEditorSettings.h"
#include "SGraphPanel.h"
#include "SCommentBubble.h"
#include "SGraphPreviewer.h"
#include "NodeFactory.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/Tasks/BTTask_RunBehavior.h"
#include "IDocumentation.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"
#include "SLevelOfDetailBranchNode.h"
#include "SkillTreeColors.h"

#define LOCTEXT_NAMESPACE "SkillTreeEditor"

namespace
{
	static const bool bShowExecutionIndexInEditorMode = true;
}

class SSkillTreePin : public SGraphPinST
{
	SLATE_BEGIN_ARGS(SSkillTreePin) {}
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs, UEdGraphPin* InPin);
protected:
	virtual FSlateColor GetPinColor() const override;
};

void SSkillTreePin::Construct(const FArguments& InArgs, UEdGraphPin* InPin)
{
	SGraphPinST::Construct(SGraphPinST::FArguments(), InPin);
}

FSlateColor SSkillTreePin::GetPinColor() const
{
	return GraphPinObj->bIsDiffing ? SkillTreeColors::Pin::Diff : IsHovered() ? SkillTreeColors::Pin::Hover :
		(GraphPinObj->PinType.PinCategory == USkillTreeEditorTypes::PinCategory_SingleComposite) ? SkillTreeColors::Pin::CompositeOnly :
		(GraphPinObj->PinType.PinCategory == USkillTreeEditorTypes::PinCategory_SingleTask) ? SkillTreeColors::Pin::TaskOnly :
		(GraphPinObj->PinType.PinCategory == USkillTreeEditorTypes::PinCategory_SingleNode) ? SkillTreeColors::Pin::SingleNode :
		SkillTreeColors::Pin::Default;
}

class SSkillTreeIndex : public SCompoundWidget
{
public:
	DECLARE_DELEGATE_OneParam(FOnHoverStateChanged, bool);
	DECLARE_DELEGATE_RetVal_OneParam(FSlateColor, FOnGetIndexColor, bool /* bHovered */);

	SLATE_BEGIN_ARGS(SSkillTreeIndex){}
		SLATE_ATTRIBUTE(FText,Text)
		SLATE_EVENT(FOnHoverStateChanged,OnHoverStateChanged)
		SLATE_EVENT(FOnGetIndexColor,OnGetIndexColor)
	SLATE_END_ARGS()
public:
		void Construct(const FArguments& InArgs)
		{
			OnHoverStateChangedEvent = InArgs._OnHoverStateChanged;
			OnGetIndexColorEvent = InArgs._OnGetIndexColor;
			const FSlateBrush* IndexBrush = FEditorStyle::GetBrush(TEXT("BTEditor.Graph.BTNode.Index"));

			ChildSlot
			[
				SNew(SOverlay)
				+SOverlay::Slot()
				.HAlign(EHorizontalAlignment::HAlign_Fill)
				.VAlign(EVerticalAlignment::VAlign_Fill)
				[
					SNew(SBox)
					.WidthOverride(IndexBrush->ImageSize.X)
					.HeightOverride(IndexBrush->ImageSize.Y)
				]
				+SOverlay::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SNew(SBorder)
					.BorderImage(IndexBrush)
					.BorderBackgroundColor(this, &SSkillTreeIndex::GetColor)
					.Padding(FMargin(4.0f, 0.0f, 4.0f, 1.0f))
					.VAlign(VAlign_Center)
					.HAlign(HAlign_Center)
					[
						SNew(STextBlock)
						.Text(InArgs._Text)
					.Font(FEditorStyle::GetFontStyle("BTEditor.Graph.BTNode.IndexText"))
					]
				]
			];
		}

		virtual void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override 
		{
			OnHoverStateChangedEvent.ExecuteIfBound(true);
			SCompoundWidget::OnMouseEnter(MyGeometry, MouseEvent);
		}

		virtual void OnMouseLeave(const FPointerEvent& MouseEvent) override
		{
			OnHoverStateChangedEvent.ExecuteIfBound(false);
			SCompoundWidget::OnMouseLeave(MouseEvent);
		}

		FSlateColor GetColor()const
		{
			return OnGetIndexColorEvent.Execute(IsHovered());
		}
private:
	FOnHoverStateChanged OnHoverStateChangedEvent;
	FOnGetIndexColor OnGetIndexColorEvent;
};

void SGraphNode_SkillTree::Construct(const FArguments& InArgs,USkillGraphNode* InGraphNode)
{
	DebuggerStateDuration = 0.0f;
	DebuggerStateCounter = INDEX_NONE;
	bSupperessDebuggerTriggers = false;
	SGraphNodeST::Construct(SGraphNodeST::FArguments(), InGraphNode);
}


void SGraphNode_SkillTree::UpdateGraphNode()
{
	bDragMarkerVisible = false;
	InputPins.Empty();
	OutputPins.Empty();

	if (ServicesBox.IsValid())
	{
		ServicesBox->ClearChildren();
	}
	else
	{
		SAssignNew(ServicesBox, SVerticalBox);
	}
	RightNodeBox.Reset();
	LeftNodeBox.Reset();
	ServicesWidgets.Reset();
	SubNodes.Reset();
	OutputPins.Reset();

	USkillGraphNode* STNode = Cast<USkillGraphNode>(GraphNode);
	if (STNode)
	{
	
	}

	TSharedPtr<SNodeTitle> NodeTile = SNew(SNodeTitle, GraphNode);

	TWeakPtr<SNodeTitle> WeakNodeTitle = NodeTile;
	auto GetNodeTitlePlaceHolderWidth = [WeakNodeTitle]()->FOptionalSize
	{
		TSharedPtr<SNodeTitle> NodeTitlePin = WeakNodeTitle.Pin();
		const float DesiredWidth = (NodeTitlePin.IsValid()) ? NodeTitlePin->GetTitleSize().X : 0.f;
		return FMath::Max(75.f, DesiredWidth);
	};

	auto GetNodeTitlePlaceHolderHeight = [WeakNodeTitle]()->FOptionalSize
	{
		TSharedPtr<SNodeTitle> NodeTitlePin = WeakNodeTitle.Pin();
		const float DesiredHeight = (NodeTitlePin.IsValid()) ? NodeTitlePin->GetTitleSize().Y : 0.f;
		return FMath::Max(22.f, DesiredHeight);
	};

	const FMargin NodePadding = FMargin(8.f);
	
	IndexOverlay = SNew(SSkillTreeIndex)
		.ToolTipText(this, &SGraphNode_SkillTree::GetIndexTooltipText)
		.Visibility(this, &SGraphNode_SkillTree::GetIndexVisibility)
		.Text(this, &SGraphNode_SkillTree::GetIndexText)
		.OnHoverStateChanged(this,&SGraphNode_SkillTree::OnIndexHoverStateChanged)
		.OnGetIndexColor(this,&SGraphNode_SkillTree::GetIndexColor);
	this->ContentScale.Bind(this, &SGraphNode::GetContentScale);
	this->GetOrAddSlot(ENodeZone::Center)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Center)
		[
			SNew(SBorder)
			.BorderImage(FEditorStyle::GetBrush("Graph.StateNode.Body"))
			.Padding(0.0f)
			.BorderBackgroundColor(this, &SGraphNode_SkillTree::GetBorderBackgroundColor)
			.OnMouseButtonDown(this,&SGraphNode_SkillTree::OnMouseDown)
			[
				SNew(SOverlay)
				+SOverlay::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SNew(SVerticalBox)
					+SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SBox)
						.MinDesiredHeight(NodePadding.Top)
						[
							SAssignNew(LeftNodeBox,SVerticalBox)
						]
					]
					// STATE NAME AREA
					+SVerticalBox::Slot()
					.Padding(FMargin(NodePadding.Left,0.f,NodePadding.Right,0.f))
					[
						SNew(SVerticalBox)
						+SVerticalBox::Slot()
						.AutoHeight()
						[
							SAssignNew(NodeBody,SBorder)
							.BorderImage(FEditorStyle::GetBrush("BTEditor.Graph.BTNode.Body"))
							.BorderBackgroundColor(this, &SGraphNode_SkillTree::GetBackgroundColor)
							.HAlign(HAlign_Fill)
							.VAlign(VAlign_Center)
							.Visibility(EVisibility::SelfHitTestInvisible)
						]
						+ SVerticalBox::Slot()
						.AutoHeight()
						[
							ServicesBox.ToSharedRef()
						]
					]
					// OUTPUT PIN AREA
					+SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SBox)
						.MinDesiredHeight(NodePadding.Bottom)
						[
							SAssignNew(RightNodeBox,SVerticalBox)
							+SVerticalBox::Slot()
							.HAlign(HAlign_Fill)
							.VAlign(VAlign_Fill)
							.Padding(20.f,0.f)
							.FillHeight(1.f)
							[
								SAssignNew(OutputPinBox,SHorizontalBox)
							]
						]
					]
				]
			]
		];
}

void SGraphNode_SkillTree::CreatePinWidgets()
{
}

void SGraphNode_SkillTree::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
{
}

TSharedPtr<SToolTip> SGraphNode_SkillTree::GetComplexTooltip()
{
	return TSharedPtr<SToolTip>();
}

void SGraphNode_SkillTree::GetOverlayBrushes(bool bSelected, const FVector2D WidgetSize, TArray<FOverlayBrushInfo>& Brushes) const
{
}

TSharedRef<SGraphNode> SGraphNode_SkillTree::GetNodeUnderMouse(const FGeometry & MyGeometry, const FPointerEvent & MouseEvent)
{
	TSharedPtr<SGraphNode> SubNode = GetSubNodeUnderCursor(MyGeometry, MouseEvent);
	return SubNode.IsValid()?SubNode.ToSharedRef():StaticCastSharedRef<SGraphNode>(AsShared());
}

void SGraphNode_SkillTree::MoveTo(const FVector2D & NewPosition, FNodeSet & NodeFilter)
{
}

FReply SGraphNode_SkillTree::OnMouseButtonDoubleClick(const FGeometry & InMyGeometry, const FPointerEvent & InMouseEvent)
{
	return FReply::Handled();
}

void SGraphNode_SkillTree::Tick(const FGeometry & AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
}

void SGraphNode_SkillTree::AddService(TSharedPtr<SGraphNode> ServiceWidget)
{
	ServicesBox->AddSlot().AutoHeight()
	[
		ServiceWidget.ToSharedRef()
	];
	ServicesWidgets.Add(ServiceWidget);
	AddSubNode(ServiceWidget);
}

EVisibility SGraphNode_SkillTree::GetDebuggerSearchFailedMarkerVisility() const
{
	return EVisibility();
}

FSlateColor SGraphNode_SkillTree::GetBorderBackgroundColor() const
{
	return FSlateColor();
}

FSlateColor SGraphNode_SkillTree::GetBackgroundColor() const
{
	return FSlateColor();
}

const FSlateBrush * SGraphNode_SkillTree::GetNameIcon() const
{
	return nullptr;
}

EVisibility SGraphNode_SkillTree::GetBlueprintIconVisibility() const
{
	return EVisibility();
}

EVisibility SGraphNode_SkillTree::GetIndexVisibility() const
{
	return EVisibility();
}

FText SGraphNode_SkillTree::GetIndexText() const
{

	return FText();
}

FText SGraphNode_SkillTree::GetIndexTooltipText() const
{
	return FText();
}

FSlateColor SGraphNode_SkillTree::GetIndexColor(bool bHovered) const
{
	return FSlateColor();
}

void SGraphNode_SkillTree::OnIndexHoverStateChanged(bool bHovered)
{
}

FText SGraphNode_SkillTree::GetPinTooltip(UEdGraphPin * GraphPinObj) const
{
	return FText();
}

#undef LOCTEXT_NAMESPACE