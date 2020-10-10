#include "SDirectoryView.h"
#include "FTreeItem.h"
#include "SAssetTreeItem.h"
#include "FAssetDragDropOp.h"
#include "ContentBrowserUtils.h"
#include "FileManager.h"

void SDirectoryView::Construct(const FArguments& InArgs)
{
	if (!TreeViewPtr.IsValid())
	{
		SAssignNew(TreeViewPtr, STreeView< TSharedPtr<FTreeItem> >)
			.TreeItemsSource(&TreeRootItems)
			.OnGenerateRow(this, &SDirectoryView::GenerateTreeRow)
			.OnItemScrolledIntoView(this, &SDirectoryView::TreeItemScrolledIntoView)
			.ItemHeight(18)
			.SelectionMode(ESelectionMode::Multi)
			.OnSelectionChanged(this, &SDirectoryView::TreeSelectionChanged)
			.OnExpansionChanged(this, &SDirectoryView::TreeExpansionChanged)
			.OnGetChildren(this, &SDirectoryView::GetChildrenForTree)
			.OnSetExpansionRecursive(this, &SDirectoryView::SetTreeItemExpansionRecursive)
			.OnContextMenuOpening(this, &SDirectoryView::MakePathViewContextMenu)
			.ClearSelectionOnClick(false);
	}
}

TArray<FString> SDirectoryView::GetSelectedPaths() const
{
	TArray<FString> RetArray;

	TArray<TSharedPtr<FTreeItem>> Items = TreeViewPtr->GetSelectedItems();
	for (int32 ItemIdx = 0; ItemIdx < Items.Num(); ++ItemIdx)
	{
		RetArray.Add(Items[ItemIdx]->FolderPath);
	}
	return RetArray;
}

TSharedRef<ITableRow> SDirectoryView::GenerateTreeRow(TSharedPtr<FTreeItem> TreeItem, const TSharedRef<STableViewBase>& OwnerTable)
{
	check(TreeItem.IsValid());

	return
		SNew(STableRow< TSharedPtr<FTreeItem> >, OwnerTable)
		.OnDragDetected(this, &SDirectoryView::OnFolderDragDetected)
		[
			SNew(SAssetTreeItem)
			.TreeItem(TreeItem)
			.OnNameChanged(this, &SDirectoryView::FolderNameChanged)
			.OnVerifyNameChanged(this, &SDirectoryView::VerifyFolderNameChanged)
			.OnAssetsOrPathsDragDropped(this, &SDirectoryView::TreeAssetsOrPathsDropped)
			.OnFilesDragDropped(this, &SDirectoryView::TreeFilesDropped)
			.IsItemExpanded(this, &SDirectoryView::IsTreeItemExpanded, TreeItem)
			.HighlightText(this, &SDirectoryView::GetHighlightText)
			.IsSelected(this, &SDirectoryView::IsTreeItemSelected, TreeItem)
		];
}

void SDirectoryView::TreeItemScrolledIntoView(TSharedPtr<FTreeItem> TreeItem, const TSharedPtr<ITableRow>& Widget)
{
	if (TreeItem->bNamingFolder && Widget.IsValid() && Widget->GetContent().IsValid())
	{
		TreeItem->OnRenamedRequestEvent.Broadcast();
	}
}

void SDirectoryView::TreeSelectionChanged(TSharedPtr< FTreeItem > TreeItem, ESelectInfo::Type SelectInfo)
{
	//if (ShouldAllowTreeItemChangedDelegate())
	{
		const TArray<TSharedPtr<FTreeItem>> SelectedItems = TreeViewPtr->GetSelectedItems();

		LastSelectedPaths.Empty();
		for (int32 ItemIdx = 0; ItemIdx < SelectedItems.Num(); ++ItemIdx)
		{
			const TSharedPtr<FTreeItem> Item = SelectedItems[ItemIdx];
			if (!ensure(Item.IsValid()))
			{
				// All items must exist
				continue;
			}

			// Keep track of the last paths that we broadcasted for selection reasons when filtering
			LastSelectedPaths.Add(Item->FolderPath);
		}

		if (OnPathSelected.IsBound())
		{
			if (TreeItem.IsValid())
			{
				OnPathSelected.Execute(TreeItem->FolderPath);
			}
			else
			{
				OnPathSelected.Execute(TEXT(""));
			}
		}
	}

	if (TreeItem.IsValid())
	{
		// Prioritize the asset registry scan for the selected path
		//FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
		//AssetRegistryModule.Get().PrioritizeSearchPath(TreeItem->FolderPath / TEXT(""));
	}
}

void SDirectoryView::TreeExpansionChanged(TSharedPtr< FTreeItem > TreeItem, bool bIsExpanded)
{
	//if (ShouldAllowTreeItemChangedDelegate())
	{
		TSet<TSharedPtr<FTreeItem>> ExpandedItemSet;
		TreeViewPtr->GetExpandedItems(ExpandedItemSet);
		const TArray<TSharedPtr<FTreeItem>> ExpandedItems = ExpandedItemSet.Array();

		LastExpandedPaths.Empty();
		for (int32 ItemIdx = 0; ItemIdx < ExpandedItems.Num(); ++ItemIdx)
		{
			const TSharedPtr<FTreeItem> Item = ExpandedItems[ItemIdx];
			if (!ensure(Item.IsValid()))
			{
				// All items must exist
				continue;
			}

			// Keep track of the last paths that we broadcasted for expansion reasons when filtering
			LastExpandedPaths.Add(Item->FolderPath);
		}
	}
}

void SDirectoryView::GetChildrenForTree(TSharedPtr< FTreeItem > TreeItem, TArray< TSharedPtr<FTreeItem> >& OutChildren)
{
	TreeItem->SortChildrenIfNeeded();
	OutChildren = TreeItem->Children;
}

void SDirectoryView::SetTreeItemExpansionRecursive(TSharedPtr< FTreeItem > TreeItem, bool bInExpansionState)
{
	TreeViewPtr->SetItemExpansion(TreeItem, bInExpansionState);

	// Recursively go through the children.
	for (auto It = TreeItem->Children.CreateIterator(); It; ++It)
	{
		SetTreeItemExpansionRecursive(*It, bInExpansionState);
	}
}

TSharedPtr<SWidget> SDirectoryView::MakePathViewContextMenu()
{
	if (TreeViewPtr->GetSelectedItems().Num() <= 0 || !bAllowContextMenu)
	{
		return NULL;
	}

	//if (!OnGetFolderContextMenu.IsBound())
	//{
	//	return NULL;
	//}

	const TArray<FString> SelectedPaths = GetSelectedPaths();
	//return OnGetFolderContextMenu.Execute(SelectedPaths, OnGetPathContextMenuExtender, FOnCreateNewFolder::CreateSP(this, &SPathView::OnCreateNewFolder));
	return SNullWidget::NullWidget;
}

FReply SDirectoryView::OnFolderDragDetected(const FGeometry& Geometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		TArray<TSharedPtr<FTreeItem>> SelectedItems = TreeViewPtr->GetSelectedItems();
		if (SelectedItems.Num())
		{
			TArray<FString> PathNames;

			for (auto ItemIt = SelectedItems.CreateConstIterator(); ItemIt; ++ItemIt)
			{
				PathNames.Add((*ItemIt)->FolderPath);
			}

			return FReply::Handled().BeginDragDrop(FAssetDragDropOp::New(PathNames));
		}
	}

	return FReply::Unhandled();


}

void SDirectoryView::FolderNameChanged(const TSharedPtr< FTreeItem >& TreeItem, const FString& OldPath, const FVector2D& MessageLocation, const ETextCommit::Type CommitType)
{
	TArray<FMovedContentFolder> MovedFolders;
	// Verify the name of the folder
	FText Reason;
	if (ContentBrowserUtils::IsValidFolderName(TreeItem->FolderName, Reason))
	{
		if (CommitType == ETextCommit::OnCleared)
		{
			FString NewPathOnDisk;
			if (FPackageName::TryConvertLongPackageNameToFilename(TreeItem->FolderPath, NewPathOnDisk) && !IFileManager::Get().DirectoryExists(*NewPathOnDisk))
			{
				// Clearing the rename box on a newly created folder cancels the entire creation process
				RemoveFolderItem(TreeItem);
				return;
			}
		}

		TSharedPtr< FTreeItem > ExistingItem;
		if (FolderAlreadyExists(TreeItem, ExistingItem))
		{
			// The folder already exists, remove it so selection is simple
			RemoveFolderItem(ExistingItem);
		}

		// The folder did not already exist
		bool bWasItemSelected = TreeViewPtr->IsItemSelected(TreeItem);

		// Reselect the path to notify that selection has changed
		if (bWasItemSelected)
		{
			//FScopedPreventTreeItemChangedDelegate DelegatePrevention(SharedThis(this));
			TreeViewPtr->SetItemSelection(TreeItem, false);
		}

		// If we weren't a root node, make sure our parent is sorted
		if (TreeItem->Parent.IsValid())
		{
			TreeItem->Parent.Pin()->RequestSortChildren();
			TreeViewPtr->RequestTreeRefresh();
		}

		if (bWasItemSelected)
		{
			// Set the selection again
			TreeViewPtr->SetItemSelection(TreeItem, true);

			// Scroll back into view if position has changed
			TreeViewPtr->RequestScrollIntoView(TreeItem);
		}

		// Update either the asset registry or the native class hierarchy so this folder will persist
		/*
		if (ContentBrowserUtils::IsClassPath(TreeItem->FolderPath))
		{
			// todo: jdale - CLASS - This will need updating to support renaming of class folders (SAssetView has similar logic - needs abstracting)
			TSharedRef<FNativeClassHierarchy> NativeClassHierarchy = FContentBrowserSingleton::Get().GetNativeClassHierarchy();
			NativeClassHierarchy->AddFolder(TreeItem->FolderPath);
		}
		else
		*/
		{
			// ensure the folder exists on disk
			FString NewPathOnDisk;
			if (FPackageName::TryConvertLongPackageNameToFilename(TreeItem->FolderPath, NewPathOnDisk) && IFileManager::Get().MakeDirectory(*NewPathOnDisk, true))
			{
				//TSharedRef<FEmptyFolderVisibilityManager> EmptyFolderVisibilityManager = FContentBrowserSingleton::Get().GetEmptyFolderVisibilityManager();
				//EmptyFolderVisibilityManager->SetAlwaysShowPath(TreeItem->FolderPath);

				//FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
				//if (AssetRegistryModule.Get().AddPath(TreeItem->FolderPath) && TreeItem->FolderPath != OldPath)
				{
					MovedFolders.Add(FMovedContentFolder(OldPath, TreeItem->FolderPath));
					// move any assets in our folder
					TArray<FAssetData> AssetsInFolder;
					//AssetRegistryModule.Get().GetAssetsByPath(*OldPath, AssetsInFolder, true);
					TArray<UObject*> ObjectsInFolder;
					ContentBrowserUtils::GetObjectsInAssetData(AssetsInFolder, ObjectsInFolder);
					ContentBrowserUtils::MoveAssets(ObjectsInFolder, TreeItem->FolderPath, OldPath);

					// Now check to see if the original folder is empty, if so we can delete it
					TArray<FAssetData> AssetsInOriginalFolder;
					//AssetRegistryModule.Get().GetAssetsByPath(*OldPath, AssetsInOriginalFolder, true);
					if (AssetsInOriginalFolder.Num() == 0)
					{
						TArray<FString> FoldersToDelete;
						FoldersToDelete.Add(OldPath);
						ContentBrowserUtils::DeleteFolders(FoldersToDelete);
					}
					OnFolderPathChanged.ExecuteIfBound(MovedFolders);
				}
			}
		}
	}
	else
	{
		// Remove the item
		RemoveFolderItem(TreeItem);

		// Display the reason why the folder was invalid
		//FSlateRect MessageAnchor(MessageLocation.X, MessageLocation.Y, MessageLocation.X, MessageLocation.Y);
		//ContentBrowserUtils::DisplayMessage(Reason, MessageAnchor, SharedThis(this));
	}

}

bool SDirectoryView::VerifyFolderNameChanged(const FString& InName, FText& OutErrorMessage, const FString& InFolderPath) const
{
	return ContentBrowserUtils::IsValidFolderPathForCreate(FPaths::GetPath(InFolderPath), InName, OutErrorMessage);
}

void SDirectoryView::TreeAssetsOrPathsDropped(const TArray<FAssetData>& AssetList, const TArray<FString>& AssetPaths, const TSharedPtr<FTreeItem>& TreeItem)
{
	DragDropHandler::HandleDropOnAssetFolder(
		SharedThis(this),
		AssetList,
		AssetPaths,
		TreeItem->FolderPath,
		TreeItem->DisplayName,
		DragDropHandler::FExecuteCopyOrMove::CreateSP(this, &SDirectoryView::ExecuteTreeDropCopy),
		DragDropHandler::FExecuteCopyOrMove::CreateSP(this, &SDirectoryView::ExecuteTreeDropMove),
		DragDropHandler::FExecuteCopyOrMove::CreateSP(this, &SDirectoryView::ExecuteTreeDropAdvancedCopy)
	);

}

void SDirectoryView::TreeFilesDropped(const TArray<FString>& FileNames, const TSharedPtr<FTreeItem>& TreeItem)
{
	//FAssetToolsModule& AssetToolsModule = FModuleManager::Get().LoadModuleChecked<FAssetToolsModule>("AssetTools");
	//AssetToolsModule.Get().ImportAssets(FileNames, TreeItem->FolderPath);

}

bool SDirectoryView::IsTreeItemExpanded(TSharedPtr<FTreeItem> TreeItem) const
{
	return TreeViewPtr->IsItemExpanded(TreeItem);
}

FText SDirectoryView::GetHighlightText() const
{
	return SearchBoxFolderFilter->GetRawFilterText();
}

bool SDirectoryView::IsTreeItemSelected(TSharedPtr<FTreeItem> TreeItem) const
{
	return TreeViewPtr->IsItemSelected(TreeItem);
}

void SDirectoryView::RemoveFolderItem(const TSharedPtr< FTreeItem >& TreeItem)
{
	if (TreeItem.IsValid())
	{
		if (TreeItem->Parent.IsValid())
		{
			// Remove this item from it's parent's list
			TreeItem->Parent.Pin()->Children.Remove(TreeItem);
		}
		else
		{
			// This was a root node, remove from the root list
			TreeRootItems.Remove(TreeItem);
		}

		TreeViewPtr->RequestTreeRefresh();
	}
}

bool SDirectoryView::FolderAlreadyExists(const TSharedPtr< FTreeItem >& TreeItem, TSharedPtr< FTreeItem >& ExistingItem)
{
	ExistingItem.Reset();

	if (TreeItem.IsValid())
	{
		if (TreeItem->Parent.IsValid())
		{
			// This item has a parent, try to find it in its parent's children
			TSharedPtr<FTreeItem> ParentItem = TreeItem->Parent.Pin();

			for (auto ChildIt = ParentItem->Children.CreateConstIterator(); ChildIt; ++ChildIt)
			{
				const TSharedPtr<FTreeItem>& Child = *ChildIt;
				if (Child != TreeItem && Child->FolderName == TreeItem->FolderName)
				{
					// The item is in its parent already
					ExistingItem = Child;
					break;
				}
			}
		}
		else
		{
			// This item is part of the root set
			for (auto RootIt = TreeRootItems.CreateConstIterator(); RootIt; ++RootIt)
			{
				const TSharedPtr<FTreeItem>& Root = *RootIt;
				if (Root != TreeItem && Root->FolderName == TreeItem->FolderName)
				{
					// The item is part of the root set already
					ExistingItem = Root;
					break;
				}
			}
		}
	}

	return ExistingItem.IsValid();


}

void SDirectoryView::ExecuteTreeDropCopy(TArray<FAssetData> AssetList, TArray<FString> AssetPaths, FString DestinationPath)
{
	if (AssetList.Num() > 0)
	{
		TArray<UObject*> DroppedObjects;
		ContentBrowserUtils::GetObjectsInAssetData(AssetList, DroppedObjects);

		ContentBrowserUtils::CopyAssets(DroppedObjects, DestinationPath);
	}

	if (AssetPaths.Num() > 0 && ContentBrowserUtils::CopyFolders(AssetPaths, DestinationPath))
	{
		TSharedPtr<FTreeItem> RootItem = FindItemRecursive(DestinationPath);
		if (RootItem.IsValid())
		{
			TreeViewPtr->SetItemExpansion(RootItem, true);

			// Select all the new folders
			TreeViewPtr->ClearSelection();
			for (const FString& AssetPath : AssetPaths)
			{
				const FString SubFolderName = FPackageName::GetLongPackageAssetName(AssetPath);
				const FString NewPath = DestinationPath + TEXT("/") + SubFolderName;

				TSharedPtr<FTreeItem> Item = FindItemRecursive(NewPath);
				if (Item.IsValid())
				{
					TreeViewPtr->SetItemSelection(Item, true);
					TreeViewPtr->RequestScrollIntoView(Item);
				}
			}
		}
	}

}

void SDirectoryView::ExecuteTreeDropAdvancedCopy(TArray<FAssetData> AssetList, TArray<FString> AssetPaths, FString DestinationPath)
{
	//ContentBrowserUtils::BeginAdvancedCopyPackages(AssetList, AssetPaths, DestinationPath);

}

void SDirectoryView::ExecuteTreeDropMove(TArray<FAssetData> AssetList, TArray<FString> AssetPaths, FString DestinationPath)
{
	if (AssetList.Num() > 0)
	{
		TArray<UObject*> DroppedObjects;
		ContentBrowserUtils::GetObjectsInAssetData(AssetList, DroppedObjects);

		ContentBrowserUtils::MoveAssets(DroppedObjects, DestinationPath);
	}

	// Prepare to fixup any asset paths that are favorites
	TArray<FMovedContentFolder> MovedFolders;
	for (const FString& OldPath : AssetPaths)
	{
		const FString SubFolderName = FPackageName::GetLongPackageAssetName(OldPath);
		const FString NewPath = DestinationPath + TEXT("/") + SubFolderName;
		MovedFolders.Add(FMovedContentFolder(OldPath, NewPath));
	}

	if (AssetPaths.Num() > 0 && ContentBrowserUtils::MoveFolders(AssetPaths, DestinationPath))
	{
		TSharedPtr<FTreeItem> RootItem = FindItemRecursive(DestinationPath);
		if (RootItem.IsValid())
		{
			TreeViewPtr->SetItemExpansion(RootItem, true);

			// Select all the new folders
			TreeViewPtr->ClearSelection();
			for (const FString& AssetPath : AssetPaths)
			{
				const FString SubFolderName = FPackageName::GetLongPackageAssetName(AssetPath);
				const FString NewPath = DestinationPath + TEXT("/") + SubFolderName;
				TSharedPtr<FTreeItem> Item = FindItemRecursive(NewPath);
				if (Item.IsValid())
				{
					TreeViewPtr->SetItemSelection(Item, true);
					TreeViewPtr->RequestScrollIntoView(Item);
				}
			}
		}

		OnFolderPathChanged.ExecuteIfBound(MovedFolders);
	}

}

TSharedPtr<FTreeItem> SDirectoryView::FindItemRecursive(const FString& Path) const
{
	for (auto TreeItemIt = TreeRootItems.CreateConstIterator(); TreeItemIt; ++TreeItemIt)
	{
		if ((*TreeItemIt)->FolderPath == Path)
		{
			// This root item is the path
			return *TreeItemIt;
		}

		// Try to find the item under this root
		TSharedPtr<FTreeItem> Item = (*TreeItemIt)->FindItemRecursive(Path);
		if (Item.IsValid())
		{
			// The item was found under this root
			return Item;
		}
	}

	return TSharedPtr<FTreeItem>();
}

