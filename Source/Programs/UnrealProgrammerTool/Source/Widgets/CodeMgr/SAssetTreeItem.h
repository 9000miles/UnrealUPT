// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "AssetRegistry/Public/AssetData.h"


class SEditableTextBox;
struct FTreeItem;

/** A single item in the asset tree. Represents a folder. */
class SAssetTreeItem : public SCompoundWidget
{
public:
	DECLARE_DELEGATE_FourParams(FOnNameChanged, const TSharedPtr<FTreeItem>& /*TreeItem*/, const FString& /*OldPath*/, const FVector2D& /*MessageLocation*/, const ETextCommit::Type /*CommitType*/);
	DECLARE_DELEGATE_RetVal_ThreeParams(bool, FOnVerifyNameChanged, const FString& /*InName*/, FText& /*OutErrorMessage*/, const FString& /*FolderPath*/);
	DECLARE_DELEGATE_ThreeParams(FOnAssetsOrPathsDragDropped, const TArray<FAssetData>& /*AssetList*/, const TArray<FString>& /*AssetPaths*/, const TSharedPtr<FTreeItem>& /*TreeItem*/);
	DECLARE_DELEGATE_TwoParams(FOnFilesDragDropped, const TArray<FString>& /*FileNames*/, const TSharedPtr<FTreeItem>& /*TreeItem*/);

	SLATE_BEGIN_ARGS(SAssetTreeItem)
		: _TreeItem(TSharedPtr<FTreeItem>())
		, _IsItemExpanded(false)
	{}

	/** Data for the folder this item represents */
	SLATE_ARGUMENT(TSharedPtr<FTreeItem>, TreeItem)

		/** Delegate for when the user commits a new name to the folder */
		SLATE_EVENT(FOnNameChanged, OnNameChanged)

		/** Delegate for when the user is typing a new name for the folder */
		SLATE_EVENT(FOnVerifyNameChanged, OnVerifyNameChanged)

		/** Delegate for when assets or asset paths are dropped on this folder */
		SLATE_EVENT(FOnAssetsOrPathsDragDropped, OnAssetsOrPathsDragDropped)

		/** Delegate for when a list of files is dropped on this folder from an external source */
		SLATE_EVENT(FOnFilesDragDropped, OnFilesDragDropped)

		/** True when this item has children and is expanded */
		SLATE_ATTRIBUTE(bool, IsItemExpanded)

		/** The string in the title to highlight (used when searching folders) */
		SLATE_ATTRIBUTE(FText, HighlightText)

		SLATE_ATTRIBUTE(FSlateFontInfo, FontOverride)

		/** Callback to check if the widget is selected, should only be hooked up if parent widget is handling selection or focus. */
		SLATE_EVENT(FIsSelected, IsSelected)
		SLATE_END_ARGS()

		/** Constructs this widget with InArgs */
		void Construct(const FArguments& InArgs);

	~SAssetTreeItem();

	virtual void OnDragEnter(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;
	virtual void OnDragLeave(const FDragDropEvent& DragDropEvent) override;
	virtual FReply OnDragOver(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;
	virtual FReply OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

private:
	/** Used by OnDragEnter, OnDragOver, and OnDrop to check and update the validity of the drag operation */
	bool ValidateDragDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent, bool& OutIsKnownDragOperation) const;

	/** Handles verifying name changes */
	bool VerifyNameChanged(const FText& InName, FText& OutError) const;

	/** Handles committing a name change */
	void HandleNameCommitted(const FText& NewText, ETextCommit::Type /*CommitInfo*/);

	/** Returns false if this folder is in the process of being created */
	bool IsReadOnly() const;

	/** Returns true if this folder is special and not a real path (like the Classes folder) */
	bool IsValidAssetPath() const;

	/** Gets the brush used to draw the folder icon */
	const FSlateBrush* GetFolderIcon() const;

	/** Gets the color used to draw the folder icon */
	FSlateColor GetFolderColor() const;

	/** Returns the text of the folder name */
	FText GetNameText() const;

	/** Returns the text to use for the folder tooltip */
	FText GetToolTipText() const;

	/** Returns the image for the border around this item. Used for drag/drop operations */
	const FSlateBrush* GetBorderImage() const;

	/** Returns the visibility of the editable folder name */
	EVisibility GetEditableTextVisibility() const;

	/** Returns the visibility of the non-editable folder name */
	EVisibility GetStaticTextVisibility() const;

private:
	enum class EFolderType : uint8
	{
		Normal,
		Code,
		Developer,
	};

	/** The data for this item */
	TWeakPtr<FTreeItem> TreeItem;

	/** The name of the asset as an editable text box */
	TSharedPtr<SEditableTextBox> EditableName;

	/** Delegate for when a list of assets or asset paths are dropped on this folder */
	FOnAssetsOrPathsDragDropped OnAssetsOrPathsDragDropped;

	/** Delegate for when a list of files is dropped on this folder from an external source */
	FOnFilesDragDropped OnFilesDragDropped;

	/** Delegate for when the user commits a new name to the folder */
	FOnNameChanged OnNameChanged;

	/** Delegate for when a user is typing a name for the folder */
	FOnVerifyNameChanged OnVerifyNameChanged;

	/** True when this item has children and is expanded */
	TAttribute<bool> IsItemExpanded;

	/** The geometry last frame. Used when telling popup messages where to appear. */
	FGeometry LastGeometry;

	/** Brushes for the different folder states */
	const FSlateBrush* FolderOpenBrush;
	const FSlateBrush* FolderClosedBrush;
	const FSlateBrush* FolderOpenCodeBrush;
	const FSlateBrush* FolderClosedCodeBrush;
	const FSlateBrush* FolderDeveloperBrush;

	/** True when a drag is over this item with a drag operation that we know how to handle. The operation itself may not be valid to drop. */
	bool bDraggedOver;

	/** What type of stuff does this folder hold */
	EFolderType FolderType;

	/** Widget to display the name of the asset item and allows for renaming */
	TSharedPtr< SInlineEditableTextBlock > InlineRenameWidget;

	/** Handle to the registered EnterEditingMode delegate. */
	FDelegateHandle EnterEditingModeDelegateHandle;
};