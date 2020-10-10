// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

namespace ContentBrowserUtils
{
	/** Returns true if the supplied folder name can be used as part of a package name */
	bool IsValidFolderName(const FString& FolderName, FText& Reason);

	/** Get all the objects in a list of asset data */
	void GetObjectsInAssetData(const TArray<FAssetData>& AssetList, TArray<UObject*>& OutDroppedObjects);

	/**
	* Moves assets to a new path
	*
	* @param Assets The assets to move
	* @param DestPath The destination folder in which to move the assets
	* @param SourcePath If non-empty, this will specify the base folder which will cause the move to maintain folder structure
	*/
	void MoveAssets(const TArray<UObject*>& Assets, const FString& DestPath, const FString& SourcePath = FString());

	/** Attempts to delete the specified folders and all assets inside them. Returns true if the operation succeeded. */
	bool DeleteFolders(const TArray<FString>& PathsToDelete);

	/** Gets an array of assets inside the specified folders */
	void GetAssetsInPaths(const TArray<FString>& InPaths, TArray<FAssetData>& OutAssetDataList);

	/** Returns true if the specified folder name in the specified path is available for folder creation */
	bool IsValidFolderPathForCreate(const FString& FolderPath, const FString& NewFolderName, FText& OutErrorMessage);

	/**
	 * Copies assets to a new path
	 * @param Assets The assets to copy
	 * @param DestPath The destination folder in which to copy the assets
	 */
	void CopyAssets(const TArray<UObject*>& Assets, const FString& DestPath);

	/** Copies all assets in all source paths to the destination path, preserving path structure */
	bool CopyFolders(const TArray<FString>& InSourcePathNames, const FString& DestPath);


	/** Moves all assets in all source paths to the destination path, preserving path structure */
	bool MoveFolders(const TArray<FString>& InSourcePathNames, const FString& DestPath);


	/** Returns true if the path specified exists as a folder in the asset registry */
	bool DoesFolderExist(const FString& FolderPath);

}

/** Common Content Browser drag-drop handler logic */
namespace DragDropHandler
{
	DECLARE_DELEGATE_ThreeParams(FExecuteCopyOrMove, TArray<FAssetData> /*AssetList*/, TArray<FString> /*AssetPaths*/, FString /*TargetPath*/);

	/** Used by OnDragEnter, OnDragOver, and OnDrop to check and update the validity of a drag-drop operation on an asset folder in the Content Browser */
	bool ValidateDragDropOnAssetFolder(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent, const FString& TargetPath, bool& OutIsKnownDragOperation);

	/** Handle assets or asset paths being dropped onto an asset folder in the Content Browser - this drop should have been externally validated by ValidateDragDropOnAssetFolder */
	void HandleDropOnAssetFolder(const TSharedRef<SWidget>& ParentWidget, const TArray<FAssetData>& AssetList, const TArray<FString>& AssetPaths, const FString& TargetPath, const FText& TargetDisplayName, FExecuteCopyOrMove CopyActionHandler, FExecuteCopyOrMove MoveActionHandler, FExecuteCopyOrMove AdvancedCopyActionHandler);
}
