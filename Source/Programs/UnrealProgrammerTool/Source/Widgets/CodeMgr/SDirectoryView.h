// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "STreeView.h"
#include "TextFilter.h"
#include "AssetRegistry/Public/AssetData.h"

/** Called when a path is selected in the path picker */
DECLARE_DELEGATE_OneParam(FOnPathSelected, const FString& /*Path*/);

/** Called to request the menu when right clicking on an asset */
//DECLARE_DELEGATE_RetVal_ThreeParams(TSharedPtr<SWidget>, FOnGetFolderContextMenu, const TArray<FString>& /*SelectedPaths*/, FContentBrowserMenuExtender_SelectedPaths /*MenuExtender*/, FOnCreateNewFolder /*CreationDelegate*/);

/** Called when a favorite folder is moved or renamed in the content browser */
DECLARE_DELEGATE_OneParam(FOnFolderPathChanged, const TArray<struct FMovedContentFolder>& /*MovedFolders*/);


typedef TTextFilter< const FString& > FolderTextFilter;
struct FTreeItem;

struct FMovedContentFolder
{
	FString OldPath;
	FString NewPath;

	FMovedContentFolder(const FString& InOldPath, const FString& InNewPath);
};

class SDirectoryView : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SDirectoryView)
	{ }
	SLATE_END_ARGS()

		void Construct(const FArguments& InArgs);

public:
	/** Returns all selected paths in the tree view */
	TArray<FString> GetSelectedPaths() const;


private:
	/** Creates a list item for the tree view */
	virtual TSharedRef<ITableRow> GenerateTreeRow(TSharedPtr<FTreeItem> TreeItem, const TSharedRef<STableViewBase>& OwnerTable);

	/** Handles focusing a folder widget after it has been created with the intent to rename */
	void TreeItemScrolledIntoView(TSharedPtr<FTreeItem> TreeItem, const TSharedPtr<ITableRow>& Widget);

	/** Handler for tree view selection changes */
	void TreeSelectionChanged(TSharedPtr< FTreeItem > TreeItem, ESelectInfo::Type SelectInfo);

	/** Handler for tree view expansion changes */
	void TreeExpansionChanged(TSharedPtr< FTreeItem > TreeItem, bool bIsExpanded);

	/** Handler for returning a list of children associated with a particular tree node */
	void GetChildrenForTree(TSharedPtr< FTreeItem > TreeItem, TArray< TSharedPtr<FTreeItem> >& OutChildren);

	/** Handler for recursively expanding/collapsing items in the tree view */
	void SetTreeItemExpansionRecursive(TSharedPtr< FTreeItem > TreeItem, bool bInExpansionState);

	/** Gets the content for a context menu */
	TSharedPtr<SWidget> MakePathViewContextMenu();

	/** Handler for tree view folders are dragged */
	FReply OnFolderDragDetected(const FGeometry& Geometry, const FPointerEvent& MouseEvent);

	/** Handler for when a name was given to a new folder */
	void FolderNameChanged(const TSharedPtr< FTreeItem >& TreeItem, const FString& OldPath, const FVector2D& MessageLocation, const ETextCommit::Type CommitType);

	/** Handler used to verify the name of a new folder */
	bool VerifyFolderNameChanged(const FString& InName, FText& OutErrorMessage, const FString& InFolderPath) const;

	/** Handler for when assets or asset paths are dropped on a tree item */
	virtual void TreeAssetsOrPathsDropped(const TArray<FAssetData>& AssetList, const TArray<FString>& AssetPaths, const TSharedPtr<FTreeItem>& TreeItem);

	/** Handler for when asset paths are dropped on a tree item */
	void TreeFilesDropped(const TArray<FString>& FileNames, const TSharedPtr<FTreeItem>& TreeItem);

	/** True if the specified item is expanded in the asset tree */
	bool IsTreeItemExpanded(TSharedPtr<FTreeItem> TreeItem) const;

	/** Gets the string to highlight in tree items (used in folder searching) */
	FText GetHighlightText() const;

	/** True if the specified item is selected in the asset tree */
	bool IsTreeItemSelected(TSharedPtr<FTreeItem> TreeItem) const;

	/** Removes the supplied folder from the tree. */
	void RemoveFolderItem(const TSharedPtr< FTreeItem >& TreeItem);

	/** Returns true if the supplied folder item already exists in the tree. If so, ExistingItem will be set to the found item. */
	bool FolderAlreadyExists(const TSharedPtr< FTreeItem >& TreeItem, TSharedPtr< FTreeItem >& ExistingItem);

	/** Handler for the user selecting to copy assets or asset paths to the specified folder */
	void ExecuteTreeDropCopy(TArray<FAssetData> AssetList, TArray<FString> AssetPaths, FString DestinationPath);

	/** Handler for the user selecting to copy assets or asset paths - and dependencies - to the specified folder */
	void ExecuteTreeDropAdvancedCopy(TArray<FAssetData> AssetList, TArray<FString> AssetPaths, FString DestinationPath);

	/** Handler for the user selecting to move assets or asset paths to the specified folder */
	virtual void ExecuteTreeDropMove(TArray<FAssetData> AssetList, TArray<FString> AssetPaths, FString DestinationPath);

	/** Finds the item that represents the specified path, if it exists. */
	TSharedPtr<FTreeItem> FindItemRecursive(const FString& Path) const;

private:
	/** The tree view widget */
	TSharedPtr< STreeView< TSharedPtr<FTreeItem>> > TreeViewPtr;
	/** The list of folders in the tree */
	TArray< TSharedPtr<FTreeItem> > TreeRootItems;

	/** The The TextFilter attached to the SearchBox widget */
	TSharedPtr< FolderTextFilter > SearchBoxFolderFilter;

private:	
	/** The paths that were last reported by OnPathSelected event. Used in preserving selection when filtering folders */
	TSet< FString > LastSelectedPaths;

	/** The paths that were last reported by OnPathExpanded event. Used in preserving expansion when filtering folders */
	TSet< FString > LastExpandedPaths;

	/** Delegate to invoke when selection changes. */
	FOnPathSelected OnPathSelected;

	/** Delegate that handles if any folder paths changed as a result of a move, rename, etc. in the path view*/
	FOnFolderPathChanged OnFolderPathChanged;

	/** Delegate to invoke when generating the context menu for a folder */
	//FOnGetFolderContextMenu OnGetFolderContextMenu;

	/** If false, the context menu will not open when right clicking an item in the tree */
	bool bAllowContextMenu;

};