#include "FDecoratedDragDropOp.h"

void FDecoratedDragDropOp::ResetToDefaultToolTip()
{
	CurrentHoverText = DefaultHoverText;
	CurrentIconBrush = DefaultHoverIcon;
	CurrentIconColorAndOpacity = DefaultHoverIconColorAndOpacity;
}

TSharedPtr<SWidget> FDecoratedDragDropOp::GetDefaultDecorator() const
{
	// Create hover widget
	return SNew(SBorder)
		.BorderImage(FEditorStyle::GetBrush("Graph.ConnectorFeedback.Border"))
		.Content()
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(0.0f, 0.0f, 3.0f, 0.0f)
		.VAlign(VAlign_Center)
		[
			SNew(SImage)
			.Image(this, &FDecoratedDragDropOp::GetIcon)
		.ColorAndOpacity(this, &FDecoratedDragDropOp::GetIconColorAndOpacity)
		]

	+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
			SNew(STextBlock)
			.Text(this, &FDecoratedDragDropOp::GetHoverText)
		]
		];
}

FText FDecoratedDragDropOp::GetHoverText() const
{
	return CurrentHoverText;
}

const FSlateBrush* FDecoratedDragDropOp::GetIcon() const
{
	return CurrentIconBrush;
}

FSlateColor FDecoratedDragDropOp::GetIconColorAndOpacity() const
{
	return CurrentIconColorAndOpacity;
}

void FDecoratedDragDropOp::SetToolTip(const FText& Text, const FSlateBrush* Icon)
{
	CurrentHoverText = Text;
	CurrentIconBrush = Icon;
}

void FDecoratedDragDropOp::SetupDefaults()
{
	DefaultHoverText = CurrentHoverText;
	DefaultHoverIcon = CurrentIconBrush;
	DefaultHoverIconColorAndOpacity = CurrentIconColorAndOpacity;
}

FText FDecoratedDragDropOp::GetDefaultHoverText() const
{
	return DefaultHoverText;
}

