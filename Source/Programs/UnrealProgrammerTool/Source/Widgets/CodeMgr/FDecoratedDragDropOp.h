// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Input/DragAndDrop.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Text/STextBlock.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "DecoratedDragDrop"

class FDecoratedDragDropOp : public FDragDropOperation
{
public:
	DRAG_DROP_OPERATOR_TYPE(FDecoratedDragDropOp, FDragDropOperation)

		/** String to show as hover text */
		FText								CurrentHoverText;

	/** Icon to be displayed */
	const FSlateBrush*					CurrentIconBrush;

	/** The color of the icon to be displayed. */
	FSlateColor							CurrentIconColorAndOpacity;

	FDecoratedDragDropOp()
		: CurrentIconBrush(nullptr)
		, CurrentIconColorAndOpacity(FLinearColor::White)
		, DefaultHoverIcon(nullptr)
		, DefaultHoverIconColorAndOpacity(FLinearColor::White)
	{ }

	/** Overridden to provide public access */
	virtual void Construct() override
	{
		FDragDropOperation::Construct();
	}

	/** Set the decorator back to the icon and text defined by the default */
	virtual void ResetToDefaultToolTip();

	/** The widget decorator to use */
	virtual TSharedPtr<SWidget> GetDefaultDecorator() const override;

	FText GetHoverText() const;

	const FSlateBrush* GetIcon() const;

	FSlateColor GetIconColorAndOpacity() const;

	/** Set the text and icon for this tooltip */
	void SetToolTip(const FText& Text, const FSlateBrush* Icon);

	/** Setup some default values for the decorator */
	void SetupDefaults();

	/** Gets the default hover text for this drag drop op. */
	FText GetDefaultHoverText() const;

protected:

	/** Default string to show as hover text */
	FText								DefaultHoverText;

	/** Default icon to be displayed */
	const FSlateBrush*					DefaultHoverIcon;

	/** Default color and opacity for the default icon to be displayed. */
	FSlateColor							DefaultHoverIconColorAndOpacity;
};


#undef LOCTEXT_NAMESPACE
