#include "ContentBrowserUtils.h"
#include "Linux/LinuxPlatformMisc.h"
#include "FileHelper.h"
#include "AssetRegistry/Public/AssetData.h"
#include "Paths.h"
#include "CoreGlobals.h"
#include "DragAndDrop.h"
#include "Geometry.h"
#include "FAssetDragDropOp.h"
#include "MultiBoxBuilder.h"
#include "SlateApplication.h"

#define LOCTEXT_NAMESPACE "ContentBrowserUtils"

bool ContentBrowserUtils::IsValidFolderName(const FString& FolderName, FText& Reason)
{
	// Check length of the folder name
	if (FolderName.Len() == 0)
	{
		Reason = LOCTEXT("InvalidFolderName_IsTooShort", "Please provide a name for this folder.");
		return false;
	}

	if (FolderName.Len() > FPlatformMisc::GetMaxPathLength())
	{
		Reason = FText::Format(LOCTEXT("InvalidFolderName_TooLongForCooking", "Filename '{0}' is too long; this may interfere with cooking for consoles. Unreal filenames should be no longer than {1} characters."),
			FText::FromString(FolderName), FText::AsNumber(FPlatformMisc::GetMaxPathLength()));
		return false;
	}

	const FString InvalidChars = INVALID_LONGPACKAGE_CHARACTERS TEXT("/"); // Slash is an invalid character for a folder name

	// See if the name contains invalid characters.
	FString Char;
	for (int32 CharIdx = 0; CharIdx < FolderName.Len(); ++CharIdx)
	{
		Char = FolderName.Mid(CharIdx, 1);

		if (InvalidChars.Contains(*Char))
		{
			FString ReadableInvalidChars = InvalidChars;
			ReadableInvalidChars.ReplaceInline(TEXT("\r"), TEXT(""));
			ReadableInvalidChars.ReplaceInline(TEXT("\n"), TEXT(""));
			ReadableInvalidChars.ReplaceInline(TEXT("\t"), TEXT(""));

			Reason = FText::Format(LOCTEXT("InvalidFolderName_InvalidCharacters", "A folder name may not contain any of the following characters: {0}"), FText::FromString(ReadableInvalidChars));
			return false;
		}
	}

	return FFileHelper::IsFilenameValidForSaving(FolderName, Reason);
}

void ContentBrowserUtils::GetObjectsInAssetData(const TArray<FAssetData>& AssetList, TArray<UObject*>& OutDroppedObjects)
{
	for (int32 AssetIdx = 0; AssetIdx < AssetList.Num(); ++AssetIdx)
	{
		const FAssetData& AssetData = AssetList[AssetIdx];

		UObject* Obj = AssetData.GetAsset();
		if (Obj)
		{
			OutDroppedObjects.Add(Obj);
		}
	}
}


void ContentBrowserUtils::MoveAssets(const TArray<UObject*>& Assets, const FString& DestPath, const FString& SourcePath)
{
	check(DestPath.Len() > 0);

	//FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
	//TArray<FAssetRenameData> AssetsAndNames;
	for (auto AssetIt = Assets.CreateConstIterator(); AssetIt; ++AssetIt)
	{
		UObject* Asset = *AssetIt;

		if (!ensure(Asset))
		{
			continue;
		}

		FString PackagePath;
		FString ObjectName = Asset->GetName();

		if (SourcePath.Len())
		{
			const FString CurrentPackageName = Asset->GetOutermost()->GetName();

			// This is a relative operation
			if (!ensure(CurrentPackageName.StartsWith(SourcePath)))
			{
				continue;
			}

			// Collect the relative path then use it to determine the new location
			// For example, if SourcePath = /Game/MyPath and CurrentPackageName = /Game/MyPath/MySubPath/MyAsset
			//     /Game/MyPath/MySubPath/MyAsset -> /MySubPath

			const int32 ShortPackageNameLen = FPackageName::GetLongPackageAssetName(CurrentPackageName).Len();
			const int32 RelativePathLen = CurrentPackageName.Len() - ShortPackageNameLen - SourcePath.Len() - 1; // -1 to exclude the trailing "/"
			const FString RelativeDestPath = CurrentPackageName.Mid(SourcePath.Len(), RelativePathLen);

			PackagePath = DestPath + RelativeDestPath;
		}
		else
		{
			// Only a DestPath was supplied, use it
			PackagePath = DestPath;
		}

		//new(AssetsAndNames) FAssetRenameData(Asset, PackagePath, ObjectName);
	}

	//if (AssetsAndNames.Num() > 0)
	//{
	//	AssetToolsModule.Get().RenameAssetsWithDialog(AssetsAndNames);
	//}
}

void ContentBrowserUtils::GetAssetsInPaths(const TArray<FString>& InPaths, TArray<FAssetData>& OutAssetDataList)
{
	//// Load the asset registry module
	//FAssetRegistryModule& AssetRegistryModule = FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

	//// Form a filter from the paths
	//FARFilter Filter;
	//Filter.bRecursivePaths = true;
	//for (int32 PathIdx = 0; PathIdx < InPaths.Num(); ++PathIdx)
	//{
	//	new (Filter.PackagePaths) FName(*InPaths[PathIdx]);
	//}

	//// Query for a list of assets in the selected paths
	//AssetRegistryModule.Get().GetAssets(Filter, OutAssetDataList);
}

bool ContentBrowserUtils::DeleteFolders(const TArray<FString>& PathsToDelete)
{
	// Get a list of assets in the paths to delete
	TArray<FAssetData> AssetDataList;
	GetAssetsInPaths(PathsToDelete, AssetDataList);

	const int32 NumAssetsInPaths = AssetDataList.Num();
	bool bAllowFolderDelete = false;
	if (NumAssetsInPaths == 0)
	{
		// There were no assets, allow the folder delete.
		bAllowFolderDelete = true;
	}
	else
	{
		// Load all the assets in the folder and attempt to delete them.
		// If it was successful, allow the folder delete.

		// Get a list of object paths for input into LoadAssetsIfNeeded
		TArray<FString> ObjectPaths;
		for (auto AssetIt = AssetDataList.CreateConstIterator(); AssetIt; ++AssetIt)
		{
			ObjectPaths.Add((*AssetIt).ObjectPath.ToString());
		}

		// Load all the assets in the selected paths
		//TArray<UObject*> LoadedAssets;
		//if (ContentBrowserUtils::LoadAssetsIfNeeded(ObjectPaths, LoadedAssets))
		//{
		//	// Make sure we loaded all of them
		//	if (LoadedAssets.Num() == NumAssetsInPaths)
		//	{
		//		TArray<UObject*> ToDelete = LoadedAssets;
		//		ObjectTools::AddExtraObjectsToDelete(ToDelete);
		//		const int32 NumAssetsDeleted = ContentBrowserUtils::DeleteAssets(ToDelete);
		//		if (NumAssetsDeleted == ToDelete.Num())
		//		{
		//			// Successfully deleted all assets in the specified path. Allow the folder to be removed.
		//			bAllowFolderDelete = true;
		//		}
		//		else
		//		{
		//			// Not all the assets in the selected paths were deleted
		//		}
		//	}
		//	else
		//	{
		//		// Not all the assets in the selected paths were loaded
		//	}
		//}
		//else
		//{
		//	// The user declined to load some assets or some assets failed to load
		//}
	}

	//if (bAllowFolderDelete)
	//{
	//	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	//	for (const FString& PathToDelete : PathsToDelete)
	//	{
	//		if (DeleteEmptyFolderFromDisk(PathToDelete))
	//		{
	//			AssetRegistryModule.Get().RemovePath(PathToDelete);
	//		}
	//	}

	//	return true;
	//}

	return false;
}

bool ContentBrowserUtils::DoesFolderExist(const FString& FolderPath)
{
	// todo: jdale - CLASS - Will need updating to handle class folders

	//TArray<FString> SubPaths;
	//FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	//AssetRegistryModule.Get().GetSubPaths(FPaths::GetPath(FolderPath), SubPaths, false);

	//for (auto SubPathIt(SubPaths.CreateConstIterator()); SubPathIt; SubPathIt++)
	//{
	//	if (*SubPathIt == FolderPath)
	//	{
	//		return true;
	//	}
	//}

	return false;
}


bool ContentBrowserUtils::IsValidFolderPathForCreate(const FString& InFolderPath, const FString& NewFolderName, FText& OutErrorMessage)
{
	if (!ContentBrowserUtils::IsValidFolderName(NewFolderName, OutErrorMessage))
	{
		return false;
	}

	const FString NewFolderPath = InFolderPath / NewFolderName;

	if (ContentBrowserUtils::DoesFolderExist(NewFolderPath))
	{
		OutErrorMessage = LOCTEXT("RenameFolderAlreadyExists", "A folder already exists at this location with this name.");
		return false;
	}

	// Make sure we are not creating a folder path that is too long
	if (NewFolderPath.Len() > FPlatformMisc::GetMaxPathLength() - 32)
	{
		// The full path for the folder is too long
		OutErrorMessage = FText::Format(LOCTEXT("RenameFolderPathTooLong",
			"The full path for the folder is too deep, the maximum is '{0}'. Please choose a shorter name for the folder or create it in a shallower folder structure."),
			FText::AsNumber(FPlatformMisc::GetMaxPathLength()));
		// Return false to indicate that the user should enter a new name for the folder
		return false;
	}

	//const bool bDisplayL10N = GetDefault<UContentBrowserSettings>()->GetDisplayL10NFolder();
	//if (!bDisplayL10N && ContentBrowserUtils::IsLocalizationFolder(NewFolderPath))
	//{
	//	OutErrorMessage = LOCTEXT("LocalizationFolderReserved", "The L10N folder is reserved for localized content and is currently hidden.");
	//	return false;
	//}

	return true;
}

void ContentBrowserUtils::CopyAssets(const TArray<UObject*>& Assets, const FString& DestPath)
{
	//TArray<UObject*> NewObjects;
	//ObjectTools::DuplicateObjects(Assets, TEXT(""), DestPath, /*bOpenDialog=*/false, &NewObjects);

	//// If any objects were duplicated, report the success
	//if (NewObjects.Num())
	//{
	//	FFormatNamedArguments Args;
	//	Args.Add(TEXT("Number"), NewObjects.Num());
	//	const FText Message = FText::Format(LOCTEXT("AssetsDroppedCopy", "{Number} asset(s) copied"), Args);
	//	FSlateNotificationManager::Get().AddNotification(FNotificationInfo(Message));

	//	// Now branch the files in source control if possible
	//	check(Assets.Num() == NewObjects.Num());
	//	for (int32 ObjectIndex = 0; ObjectIndex < Assets.Num(); ObjectIndex++)
	//	{
	//		UObject* SourceAsset = Assets[ObjectIndex];
	//		UObject* DestAsset = NewObjects[ObjectIndex];
	//		SourceControlHelpers::BranchPackage(DestAsset->GetOutermost(), SourceAsset->GetOutermost());
	//	}
	//}
}

bool ContentBrowserUtils::CopyFolders(const TArray<FString>& InSourcePathNames, const FString& DestPath)
{
	TMap<FString, TArray<UObject*> > SourcePathToLoadedAssets;

	// Make sure the destination path is not in the source path list
	TArray<FString> SourcePathNames = InSourcePathNames;
	SourcePathNames.Remove(DestPath);


	// Load the Asset Registry to update paths during the copy
	//FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	// For every path which contained valid assets...
	for (auto PathIt = SourcePathToLoadedAssets.CreateConstIterator(); PathIt; ++PathIt)
	{
		// Put dragged folders in a sub-folder under the destination path
		const FString SourcePath = PathIt.Key();
		FString SubFolderName = FPackageName::GetLongPackageAssetName(SourcePath);
		FString Destination = DestPath + TEXT("/") + SubFolderName;

		// Add the new path to notify sources views
/*		{
			TSharedRef<FEmptyFolderVisibilityManager> EmptyFolderVisibilityManager = FContentBrowserSingleton::Get().GetEmptyFolderVisibilityManager();
			EmptyFolderVisibilityManager->SetAlwaysShowPath(Destination);
		}
		AssetRegistryModule.Get().AddPath(Destination)*/;

		// If any assets were in this path...
		if (PathIt.Value().Num() > 0)
		{
			// Copy assets and supply a source path to indicate it is relative
			//ObjectTools::DuplicateObjects(PathIt.Value(), SourcePath, Destination, /*bOpenDialog=*/false);
		}

		// Attempt to copy the folder color to the new path location
		if (FPaths::FileExists(GEditorPerProjectIni))
		{
			FString ColorStr;
			if (GConfig->GetString(TEXT("PathColor"), *SourcePath, ColorStr, GEditorPerProjectIni))
			{
				// Add the new path
				GConfig->SetString(TEXT("PathColor"), *Destination, *ColorStr, GEditorPerProjectIni);
			}
		}
	}

	return true;
}

bool ContentBrowserUtils::MoveFolders(const TArray<FString>& InSourcePathNames, const FString& DestPath)
{
	TMap<FString, TArray<UObject*> > SourcePathToLoadedAssets;
	FString DestPathWithTrailingSlash = DestPath / "";

	// Do not allow parent directories to be moved to themselves or children.
	TArray<FString> SourcePathNames = InSourcePathNames;
	TArray<FString> SourcePathNamesToRemove;
	for (auto SourcePathIt = SourcePathNames.CreateConstIterator(); SourcePathIt; ++SourcePathIt)
	{
		if (DestPathWithTrailingSlash.StartsWith(*SourcePathIt / ""))
		{
			SourcePathNamesToRemove.Add(*SourcePathIt);
		}
	}
	for (auto SourcePathToRemoveIt = SourcePathNamesToRemove.CreateConstIterator(); SourcePathToRemoveIt; ++SourcePathToRemoveIt)
	{
		SourcePathNames.Remove(*SourcePathToRemoveIt);
	}


	// Load the Asset Registry to update paths during the move
	//FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

	// For every path which contained valid assets...
	for (auto PathIt = SourcePathToLoadedAssets.CreateConstIterator(); PathIt; ++PathIt)
	{
		// Put dragged folders in a sub-folder under the destination path
		const FString SourcePath = PathIt.Key();
		const FString SubFolderName = FPackageName::GetLongPackageAssetName(SourcePath);
		const FString Destination = DestPathWithTrailingSlash + SubFolderName;

		// Add the new path to notify sources views
		//{
		//	TSharedRef<FEmptyFolderVisibilityManager> EmptyFolderVisibilityManager = FContentBrowserSingleton::Get().GetEmptyFolderVisibilityManager();
		//	EmptyFolderVisibilityManager->SetAlwaysShowPath(Destination);
		//}
		//AssetRegistryModule.Get().AddPath(Destination);

		// If any assets were in this path...
		if (PathIt.Value().Num() > 0)
		{
			// Move assets and supply a source path to indicate it is relative
			ContentBrowserUtils::MoveAssets(PathIt.Value(), Destination, PathIt.Key());
		}

		// Attempt to remove the old path
		//if (DeleteEmptyFolderFromDisk(SourcePath))
		//{
		//	AssetRegistryModule.Get().RemovePath(SourcePath);
		//}

		// Attempt to move the folder color to the new path location
		if (FPaths::FileExists(GEditorPerProjectIni))
		{
			FString ColorStr;
			if (GConfig->GetString(TEXT("PathColor"), *SourcePath, ColorStr, GEditorPerProjectIni))
			{
				// Remove the old path
				GConfig->RemoveKey(TEXT("PathColor"), *SourcePath, GEditorPerProjectIni);

				// Add the new path
				GConfig->SetString(TEXT("PathColor"), *Destination, *ColorStr, GEditorPerProjectIni);
			}
		}
	}

	return true;
}



bool DragDropHandler::ValidateDragDropOnAssetFolder(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent, const FString& TargetPath, bool& OutIsKnownDragOperation)
{
	OutIsKnownDragOperation = false;

	TSharedPtr<FDragDropOperation> Operation = DragDropEvent.GetOperation();
	if (!Operation.IsValid())
	{
		return false;
	}

	bool bIsValidDrag = false;
	TOptional<EMouseCursor::Type> NewDragCursor;


	if (Operation->IsOfType<FAssetDragDropOp>())
	{
		TSharedPtr<FAssetDragDropOp> DragDropOp = StaticCastSharedPtr<FAssetDragDropOp>(Operation);
		const TArray<FAssetData>& DroppedAssets = DragDropOp->GetAssets();
		const TArray<FString>& DroppedAssetPaths = DragDropOp->GetAssetPaths();

		OutIsKnownDragOperation = true;

		int32 NumAssetItems, NumClassItems;
		ContentBrowserUtils::CountItemTypes(DroppedAssets, NumAssetItems, NumClassItems);

		int32 NumAssetPaths, NumClassPaths;
		ContentBrowserUtils::CountPathTypes(DroppedAssetPaths, NumAssetPaths, NumClassPaths);

		if (DroppedAssetPaths.Num() == 1 && DroppedAssetPaths[0] == TargetPath)
		{
			DragDropOp->SetToolTip(LOCTEXT("OnDragFoldersOverFolder_CannotSelfDrop", "Cannot move or copy a folder onto itself"), FEditorStyle::GetBrush(TEXT("Graph.ConnectorFeedback.Error")));
		}
		else if (bIsAssetPath)
		{
			const int32 TotalAssetDropItems = NumAssetItems + NumAssetPaths;
			const int32 TotalClassDropItems = NumClassItems + NumClassPaths;

			if (TotalAssetDropItems > 0)
			{
				bIsValidDrag = true;

				const FText FirstItemText = DroppedAssets.Num() > 0 ? FText::FromName(DroppedAssets[0].AssetName) : FText::FromString(DroppedAssetPaths[0]);
				const FText MoveOrCopyText = (TotalAssetDropItems > 1)
					? FText::Format(LOCTEXT("OnDragAssetsOverFolder_MultipleAssetItems", "Move or copy '{0}' and {1} {1}|plural(one=other,other=others)"), FirstItemText, TotalAssetDropItems - 1)
					: FText::Format(LOCTEXT("OnDragAssetsOverFolder_SingularAssetItems", "Move or copy '{0}'"), FirstItemText);

				if (TotalClassDropItems > 0)
				{
					DragDropOp->SetToolTip(FText::Format(LOCTEXT("OnDragAssetsOverFolder_AssetAndClassItems", "{0}\n\n{1} C++ {1}|plural(one=item,other=items) will be ignored as they cannot be moved or copied"), MoveOrCopyText, NumClassItems), FEditorStyle::GetBrush(TEXT("Graph.ConnectorFeedback.OKWarn")));
				}
				else
				{
					DragDropOp->SetToolTip(MoveOrCopyText, FEditorStyle::GetBrush(TEXT("Graph.ConnectorFeedback.OK")));
				}
			}
			else if (TotalClassDropItems > 0)
			{
				DragDropOp->SetToolTip(LOCTEXT("OnDragAssetsOverFolder_OnlyClassItems", "C++ items cannot be moved or copied"), FEditorStyle::GetBrush(TEXT("Graph.ConnectorFeedback.Error")));
			}
		}
		else
		{
			DragDropOp->SetToolTip(FText::Format(LOCTEXT("OnDragAssetsOverFolder_InvalidFolder", "'{0}' is not a valid place to drop assets or folders"), FText::FromString(TargetPath)), FEditorStyle::GetBrush(TEXT("Graph.ConnectorFeedback.Error")));
		}
	}
	else if (Operation->IsOfType<FExternalDragOperation>())
	{
		TSharedPtr<FExternalDragOperation> DragDropOp = StaticCastSharedPtr<FExternalDragOperation>(Operation);
		OutIsKnownDragOperation = true;
		bIsValidDrag = DragDropOp->HasFiles();
	}

	// Set the default slashed circle if this drag is invalid and a drag operation hasn't set NewDragCursor to something custom
	if (!bIsValidDrag && !NewDragCursor.IsSet())
	{
		NewDragCursor = EMouseCursor::SlashedCircle;
	}
	Operation->SetCursorOverride(NewDragCursor);

	return bIsValidDrag;
}

void DragDropHandler::HandleDropOnAssetFolder(const TSharedRef<SWidget>& ParentWidget, const TArray<FAssetData>& AssetList, const TArray<FString>& AssetPaths, const FString& TargetPath, const FText& TargetDisplayName, FExecuteCopyOrMove CopyActionHandler, FExecuteCopyOrMove MoveActionHandler, FExecuteCopyOrMove AdvancedCopyActionHandler)
{
	// Remove any classes from the asset list
	TArray<FAssetData> FinalAssetList = AssetList;
	FinalAssetList.RemoveAll([](const FAssetData& AssetData)
	{
		return AssetData.AssetClass == NAME_Class;
	});

	// Remove any class paths from the list
	TArray<FString> FinalAssetPaths = AssetPaths;
	FinalAssetPaths.RemoveAll([](const FString& AssetPath)
	{
		return true;
	});

	FMenuBuilder MenuBuilder(/*bInShouldCloseWindowAfterMenuSelection=*/true, nullptr);
	const FText MoveCopyHeaderString = FText::Format(LOCTEXT("AssetViewDropMenuHeading", "Move/Copy to {0}"), TargetDisplayName);
	MenuBuilder.BeginSection("PathAssetMoveCopy", MoveCopyHeaderString);
	{
		MenuBuilder.AddMenuEntry(
			LOCTEXT("DragDropMove", "Move Here"),
			LOCTEXT("DragDropMoveTooltip", "Move the dragged items to this folder, preserving the structure of any copied folders."),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateLambda([=]() { MoveActionHandler.ExecuteIfBound(FinalAssetList, FinalAssetPaths, TargetPath); }))
		);
		MenuBuilder.AddMenuEntry(
			LOCTEXT("DragDropCopy", "Copy Here"),
			LOCTEXT("DragDropCopyTooltip", "Copy the dragged items to this folder, preserving the structure of any copied folders."),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateLambda([=]() { CopyActionHandler.ExecuteIfBound(FinalAssetList, FinalAssetPaths, TargetPath); }))
		);
		MenuBuilder.AddMenuEntry(
			LOCTEXT("DragDropAdvancedCopy", "Advanced Copy Here"),
			LOCTEXT("DragDropAdvancedCopyTooltip", "Copy the dragged items and any specified dependencies to this folder, afterwards fixing up any dependencies on copied files to the new files."),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateLambda([=]() { AdvancedCopyActionHandler.ExecuteIfBound(FinalAssetList, FinalAssetPaths, TargetPath); }))
		);
	}
	MenuBuilder.EndSection();

	FSlateApplication::Get().PushMenu(
		ParentWidget,
		FWidgetPath(),
		MenuBuilder.MakeWidget(),
		FSlateApplication::Get().GetCursorPos(),
		FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu)
	);
}



#undef LOCTEXT_NAMESPACE
