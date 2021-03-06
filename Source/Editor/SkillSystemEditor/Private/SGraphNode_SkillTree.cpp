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
#include "STGraphNode.h"
#include "SkillGraphNode_Root.h"
#include "SkillGraphNode_Service.h"
#include "SkillGraphNode_Composite.h"
#include "STCompositeNode.h"
#include "SkillEdGraph.h"

#define LOCTEXT_NAMESPACE "SkillTreeEditor"

namespace
{
	static const bool bShowExecutionIndexInEditorMode = true;
}

static USkillTreeGraphNode* GetParentNode(UEdGraphNode* GraphNode)
{
	USkillTreeGraphNode* STGraphNode = Cast<USkillTreeGraphNode>(GraphNode);
	if (STGraphNode->ParentNode != nullptr)
	{
		STGraphNode = Cast<USkillTreeGraphNode>(STGraphNode->ParentNode);
	}
	UEdGraphPin* MyInputPin = STGraphNode->GetInputPin();
	UEdGraphPin* MyParentOutputPin = nullptr;
	if (MyInputPin != nullptr && MyInputPin->LinkedTo.Num() > 0)
	{
		MyParentOutputPin = MyInputPin->LinkedTo[0];
		if (MyParentOutputPin != nullptr)
		{
			if (MyParentOutputPin->GetOwningNode() != nullptr)
			{
				return CastChecked<USkillTreeGraphNode>(MyParentOutputPin->GetOwningNode());
			}
		}
	}
	return nullptr;
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

void SGraphNode_SkillTree::Construct(const FArguments& InArgs,USTGraphNode* InGraphNode)
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

	USTGraphNode* STNode = Cast<USTGraphNode>(GraphNode);
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
					+ SVerticalBox::Slot()
						.AutoHeight()
						[
							SNew(SBox)
							.MinDesiredHeight(NodePadding.Bottom)
						[
							SAssignNew(RightNodeBox, SVerticalBox)
							+ SVerticalBox::Slot()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						.Padding(20.f, 0.f)
						.FillHeight(1.f)
						[
							SAssignNew(OutputPinBox, SHorizontalBox)
						]
						]
						]
				]
				+SOverlay::Slot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Top)
				[
					SNew(SBorder)
					.BorderBackgroundColor(SkillTreeColors::Action::DragMarker)
					.ColorAndOpacity(SkillTreeColors::Action::DragMarker)
					.BorderImage(FEditorStyle::GetBrush("BTEditor.Graph.BTNode.Body"))
					.Visibility(this, &SGraphNode_SkillTree::GetDragOverMarkerVisibility)
					[
						SNew(SBox)
						.HeightOverride(4)
					]
				]
				// Blueprint indicator overlay
				+SOverlay::Slot()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Top)
				[
					SNew(SImage)
					.Image(FEditorStyle::GetBrush(TEXT("BTEditor.Graph.BTNode.Blueprint")))
					.Visibility(this, &SGraphNode_SkillTree::GetBlueprintIconVisibility)
				]
			]
		];

	// Create comment bubble
	TSharedPtr<SCommentBubble> CommentBubble;
	const FSlateColor CommentColor = GetDefault<UGraphEditorSettings>()->DefaultCommentNodeTitleColor;

	SAssignNew(CommentBubble, SCommentBubble)
		.GraphNode(GraphNode)
		.Text(this, &SGraphNode::GetNodeComment)
		.OnTextCommitted(this, &SGraphNode::OnCommentTextCommitted)
		.ColorAndOpacity(CommentColor)
		.AllowPinning(true)
		.EnableTitleBarBubble(true)
		.EnableBubbleCtrls(true)
		.GraphLOD(this, &SGraphNode::GetCurrentLOD)
		.IsGraphNodeHovered(this, &SGraphNode::IsHovered);

	GetOrAddSlot(ENodeZone::TopCenter)
		.SlotOffset(TAttribute<FVector2D>(CommentBubble.Get(), &SCommentBubble::GetOffset))
		.SlotSize(TAttribute<FVector2D>(CommentBubble.Get(), &SCommentBubble::GetSize))
		.AllowScaling(TAttribute<bool>(CommentBubble.Get(), &SCommentBubble::IsScalingAllowed))
		.VAlign(VAlign_Top)
		[
			CommentBubble.ToSharedRef()
		];
	
	CreatePinWidgets();
}

void SGraphNode_SkillTree::CreatePinWidgets()
{
	USTGraphNode* StateNode = CastChecked<USTGraphNode>(GraphNode);
	for (int32 PinIdx = 0;PinIdx < StateNode->Pins.Num();PinIdx++)
	{
		UEdGraphPin* MyPin = StateNode->Pins[PinIdx];
		if (!MyPin->bHidden)
		{
			TSharedPtr<SGraphPin> NewPin = SNew(SSkillTreePin, MyPin)
				.ToolTipText(this, &SGraphNode_SkillTree::GetPinTooltip, MyPin);
			AddPin(NewPin.ToSharedRef());
		}
	}
}

void SGraphNode_SkillTree::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
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
			.Padding(20.f, 0.f)
			[
				PinToAdd
			];
		InputPins.Add(PinToAdd);
	}
	else
	{
		const bool bIsSingleTaskPin = PinObj && (PinObj->PinType.PinCategory == USkillTreeEditorTypes::PinCategory_SingleTask);
		if (bIsSingleTaskPin)
		{
			OutputPinBox->AddSlot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.FillWidth(0.4f)
				.Padding(0, 0, 20.0f, 0)
				[
					PinToAdd
				];
		}
		else
		{
			OutputPinBox->AddSlot()
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.FillWidth(1.0f)
				[
					PinToAdd
				];
		}
		OutputPins.Add(PinToAdd);
	}
}

TSharedPtr<SToolTip> SGraphNode_SkillTree::GetComplexTooltip()
{
	return IDocumentation::Get()->CreateToolTip(TAttribute<FText>(this,&SGraphNode::GetNodeTooltip),nullptr,GraphNode->GetDocumentationLink(),GraphNode->GetDocumentationExcerptName());
}

void SGraphNode_SkillTree::GetOverlayBrushes(bool bSelected, const FVector2D WidgetSize, TArray<FOverlayBrushInfo>& Brushes) const
{
	USkillTreeGraphNode* STNode = Cast<USkillTreeGraphNode>(GraphNode);
	if (STNode == nullptr)
	{
		return;
	}
	//TODO: Debug
}

TArray<FOverlayWidgetInfo> SGraphNode_SkillTree::GetOverlayWidgets(bool bSelected, const FVector2D& WidgetSize) const
{
	TArray<FOverlayWidgetInfo> Widgets;
	check(NodeBody.IsValid());
	check(IndexOverlay.IsValid());

	FVector2D Origin(0.f, 0.f);

	//build overlay for service sub-nodes

	FOverlayWidgetInfo Overlay(IndexOverlay);
	Overlay.OverlayOffset = FVector2D(WidgetSize.X - (IndexOverlay->GetDesiredSize().X*0.5f), Origin.Y);
	Widgets.Add(Overlay);
	Origin.Y += NodeBody->GetDesiredSize().Y;
	for (const auto& ServiceWidget : ServicesWidgets)
	{
		TArray<FOverlayWidgetInfo> OverlayWidgets = ServiceWidget->GetOverlayWidgets(bSelected, WidgetSize);
		for (auto& OverlayWidget : OverlayWidgets)
		{
			OverlayWidget.OverlayOffset.Y += Origin.Y;
		}
		Widgets.Append(OverlayWidgets);
		Origin.Y += ServiceWidget->GetDesiredSize().Y;
	}
	return Widgets;
}

TSharedRef<SGraphNode> SGraphNode_SkillTree::GetNodeUnderMouse(const FGeometry & MyGeometry, const FPointerEvent & MouseEvent)
{
	TSharedPtr<SGraphNode> SubNode = GetSubNodeUnderCursor(MyGeometry, MouseEvent);
	return SubNode.IsValid()?SubNode.ToSharedRef():StaticCastSharedRef<SGraphNode>(AsShared());
}

void SGraphNode_SkillTree::MoveTo(const FVector2D & NewPosition, FNodeSet & NodeFilter)
{
	SGraphNodeST::MoveTo(NewPosition, NodeFilter);
	USTGraphNode* STGraphNode = Cast<USTGraphNode>(GraphNode);
	if (STGraphNode && !STGraphNode->IsSubNode())
	{
		USkillEdGraph* STGraph = STGraphNode->GetSTGraph();
		if (STGraph)
		{
			for (int32 i = 0; i < STGraphNode->Pins.Num(); i++)
			{
				UEdGraphPin* Pin = STGraphNode->Pins[i];
				if (Pin	&& Pin->Direction == EGPD_Input && Pin->LinkedTo.Num() == 1)
				{
					UEdGraphPin* ParentPin = Pin->LinkedTo[0];
					if (ParentPin)
					{
						STGraph->RebuildChildOrder(ParentPin->GetOwningNode());
					}
				}
			}
		}
	}
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
	USTGraphNode* STGraphNode = Cast<USTGraphNode>(GraphNode);
	USTGraphNode* STParentNode = STGraphNode ? Cast<USTGraphNode>(STGraphNode->ParentNode) : nullptr;
	const bool bSelectSubNode = STParentNode && GetOwnerPanel()->SelectionManager.SelectedNodes.Contains(GraphNode);
	USTNode* NodeInstance = STGraphNode ? Cast<USTNode>(STGraphNode->NodeInstance) : nullptr;
	
	const bool bIsConnectedTreeRoot = STGraphNode && 
		STGraphNode->IsA<USkillGraphNode_Root>() && 
		STGraphNode->Pins.IsValidIndex(0) &&
		STGraphNode->Pins[0]->LinkedTo.Num() > 0;
	const bool bIsDisconnected = NodeInstance && NodeInstance->GetExecutionIndex() == MAX_uint16;
	const bool bIsService = STGraphNode && STGraphNode->IsA(USkillGraphNode_Service::StaticClass());
	//const bool bIsRootDecorator = STGraphNode;
	//const bool bIsInjected = STGraphNode&& STGraphNode->binje

	return bSelectSubNode ? SkillTreeColors::NodeBorder::Selected : bIsDisconnected ? SkillTreeColors::NodeBorder::Root : SkillTreeColors::NodeBorder::Inactive;
}

FSlateColor SGraphNode_SkillTree::GetBackgroundColor() const
{
	USTGraphNode* STGraphNode = Cast<USTGraphNode>(GraphNode);
	
	FLinearColor NodeColor = SkillTreeColors::NodeBody::Default;
	if (STGraphNode && STGraphNode->HasErrors())
	{
		NodeColor = SkillTreeColors::NodeBody::Error;
	}
	else if (Cast<USkillGraphNode_Composite>(GraphNode))
	{
		check(STGraphNode);
		USTCompositeNode* CompositeNodeInstance = Cast<USTCompositeNode>(STGraphNode->NodeInstance);
		const bool bIsScoped = CompositeNodeInstance && false;
		NodeColor = bIsScoped ? SkillTreeColors::NodeBody::CompositeScoped : SkillTreeColors::NodeBody::Composite;
	}
	else if (Cast<USkillGraphNode_Service>(GraphNode))
	{
		NodeColor = SkillTreeColors::NodeBody::Service;
	}
	else if (Cast<USkillGraphNode_Root>(GraphNode) && GraphNode->Pins.IsValidIndex(0)&& GraphNode->Pins[0]->LinkedTo.Num()>0)
	{
		NodeColor = SkillTreeColors::NodeBody::Root;
	}

	return (FlashAlpha > 0.f) ? FMath::Lerp(NodeColor, FlashColor, FlashAlpha) : NodeColor;
}

const FSlateBrush * SGraphNode_SkillTree::GetNameIcon() const
{
	USTGraphNode* STGraphNode = Cast<USTGraphNode>(GraphNode);
	return /*STGraphNode != nullptr ? FEditorStyle::GetBrush(STGraphNode->GetNameIcon()) : */FEditorStyle::GetBrush(TEXT("BTEditor.Graph.BTNode.Icon"));
}

EVisibility SGraphNode_SkillTree::GetBlueprintIconVisibility() const
{
	return EVisibility();
}

EVisibility SGraphNode_SkillTree::GetIndexVisibility() const
{
	// always hide the index on the root node
	if (GraphNode->IsA(USkillGraphNode_Root::StaticClass()))
	{
		return EVisibility::Collapsed;
	}
	USTGraphNode* StateNode = CastChecked<USTGraphNode>(GraphNode);
	UEdGraphPin* MyInputPin = StateNode->GetInputPin();
	UEdGraphPin* MyParentOutputPin = nullptr;
	if (MyInputPin != nullptr && MyInputPin->LinkedTo.Num() > 0)
	{
		MyParentOutputPin = MyInputPin->LinkedTo[0];
	}

	CA_SUPPRESS(6235);
	const bool bCanShowIndex = (bShowExecutionIndexInEditorMode || GEditor->bIsSimulatingInEditor || GEditor->PlayWorld != NULL) || (MyParentOutputPin && MyParentOutputPin->LinkedTo.Num() > 1);

	TSharedPtr<SGraphPanel> MyOwnerPanel = GetOwnerPanel();
	return (bCanShowIndex && (!MyOwnerPanel.IsValid() || MyOwnerPanel->GetCurrentLOD() > EGraphRenderingLOD::LowDetail)) ? EVisibility::Visible : EVisibility::Collapsed;
}

FText SGraphNode_SkillTree::GetIndexText() const
{
	USTGraphNode* StateNode = CastChecked<USTGraphNode>(GraphNode);
	UEdGraphPin* MyInputPin = StateNode->GetInputPin();
	UEdGraphPin* MyParentOutputPin = nullptr;
	if (MyInputPin != nullptr && MyInputPin->LinkedTo.Num() > 0)
	{
		MyParentOutputPin = MyInputPin->LinkedTo[0];
	}
	int32 Index = 0;
	CA_SUPPRESS(6235);
	if (bShowExecutionIndexInEditorMode || GEditor->bIsSimulateInEditorQueued || GEditor->PlayWorld != nullptr)
	{
		USTNode* STNode = Cast<USTNode>(StateNode->NodeInstance);
		Index = (STNode&&STNode->GetExecutionIndex() < 0xfff) ? STNode->GetExecutionIndex() : -1;
	}
	else
	{
		if (MyParentOutputPin != nullptr)
		{
			for (Index = 0 ;Index<MyParentOutputPin->LinkedTo.Num();Index++)
			{
				if (MyParentOutputPin->LinkedTo[Index] == MyInputPin)
				{
					break;
				}
			}
		}
	}
	return FText::AsNumber(Index);
}

FText SGraphNode_SkillTree::GetIndexTooltipText() const
{
	CA_SUPPRESS(6235);
	if (bShowExecutionIndexInEditorMode|| GEditor->bIsSimulatingInEditor||GEditor->PlayWorld != nullptr)
	{
		return LOCTEXT("ExecutionIndexToolTip", "Execution Index: this shows the order in which nodes are executed.");
	}
	else
	{
		return LOCTEXT("ChildIndexToolTip", "Child index: this shows the order in which child nodes are executed.");
	}
	return FText();
}

FSlateColor SGraphNode_SkillTree::GetIndexColor(bool bHovered) const
{
	USkillTreeGraphNode* ParentNode = GetParentNode(GraphNode);
	const bool bHighlightHover = bHovered || (ParentNode && ParentNode->bHighlightChildNodeIndices);
	static const FName HoveredColor("BTEditor.Graph.BTNode.Index.HoveredColor");
	static const FName DefaultColor("BTEditor.Graph.BTNode.Index.Color");

	return bHighlightHover ? FEditorStyle::Get().GetSlateColor(HoveredColor) : FEditorStyle::Get().GetSlateColor(DefaultColor);
}

void SGraphNode_SkillTree::OnIndexHoverStateChanged(bool bHovered)
{
}

FText SGraphNode_SkillTree::GetPinTooltip(UEdGraphPin * GraphPinObj) const
{
	return FText();
}

#undef LOCTEXT_NAMESPACE