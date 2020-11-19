// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "AssetData.h"
#include "ICodeBrowserSingleton.h"

class FCollectionAssetRegistryBridge;
class FEmptyFolderVisibilityManager;
class FNativeClassHierarchy;
class FSpawnTabArgs;
class FTabManager;
class FViewport;
class SCodeBrowser;
class UFactory;

#define MAX_CONTENT_BROWSERS 4

/**
 * Content browser module singleton implementation class
 */
class FCodeBrowserSingleton : public ICodeBrowserSingleton
{
public:
	/** Constructor, Destructor */
	FCodeBrowserSingleton();
	virtual ~FCodeBrowserSingleton();

	// ICodeBrowserSingleton interface
	virtual TSharedRef<class SWidget> CreateCodeBrowser( const FName InstanceName, TSharedPtr<SDockTab> ContainingTab, const FCodeBrowserConfig* CodeBrowserConfig ) override;
	virtual TSharedRef<class SWidget> CreateAssetPicker(const FAssetPickerConfig& AssetPickerConfig) override;
	virtual TSharedRef<class SWidget> CreatePathPicker(const FPathPickerConfig& PathPickerConfig) override;
	virtual TSharedRef<class SWidget> CreateCollectionPicker(const FCollectionPickerConfig& CollectionPickerConfig) override;
	virtual void CreateOpenAssetDialog(const FOpenAssetDialogConfig& OpenAssetConfig, const FOnAssetsChosenForOpen& OnAssetsChosenForOpen, const FOnAssetDialogCancelled& OnAssetDialogCancelled) override;
	virtual TArray<FFileData> CreateModalOpenAssetDialog(const FOpenAssetDialogConfig& InConfig) override;
	virtual void CreateSaveAssetDialog(const FSaveAssetDialogConfig& SaveAssetConfig, const FOnObjectPathChosenForSave& OnAssetNameChosenForSave, const FOnAssetDialogCancelled& OnAssetDialogCancelled) override;
	virtual FString CreateModalSaveAssetDialog(const FSaveAssetDialogConfig& SaveAssetConfig) override;
	virtual bool HasPrimaryCodeBrowser() const override;
	virtual void FocusPrimaryCodeBrowser(bool bFocusSearch) override;
	virtual void CreateNewAsset(const FString& DefaultAssetName, const FString& PackagePath, UClass* AssetClass, UFactory* Factory) override;
	virtual void SyncBrowserToAssets(const TArray<struct FFileData>& AssetDataList, bool bAllowLockedBrowsers = false, bool bFocusCodeBrowser = true, const FName& InstanceName = FName(), bool bNewSpawnBrowser = false) override;
	virtual void SyncBrowserToAssets(const TArray<UObject*>& AssetList, bool bAllowLockedBrowsers = false, bool bFocusCodeBrowser = true, const FName& InstanceName = FName(), bool bNewSpawnBrowser = false) override;
	virtual void SyncBrowserToFolders(const TArray<FString>& FolderList, bool bAllowLockedBrowsers = false, bool bFocusCodeBrowser = true, const FName& InstanceName = FName(), bool bNewSpawnBrowser = false) override;
	virtual void SyncBrowserTo(const FCodeBrowserSelection& ItemSelection, bool bAllowLockedBrowsers = false, bool bFocusCodeBrowser = true, const FName& InstanceName = FName(), bool bNewSpawnBrowser = false) override;
	virtual void GetSelectedAssets(TArray<FFileData>& SelectedAssets) override;
	virtual void GetSelectedFolders(TArray<FString>& SelectedFolders) override;
	virtual void GetSelectedPathViewFolders(TArray<FString>& SelectedFolders) override;
	virtual void CaptureThumbnailFromViewport(FViewport* InViewport, TArray<FFileData>& SelectedAssets) override;
	virtual void SetSelectedPaths(const TArray<FString>& FolderPaths, bool bNeedsRefresh = false) override;
	virtual void ForceShowPluginContent(bool bEnginePlugin) override;


	/** Gets the content browser singleton as a FCodeBrowserSingleton */
	static FCodeBrowserSingleton& Get();
	
	/** Sets the current primary content browser. */
	void SetPrimaryCodeBrowser(const TSharedRef<SCodeBrowser>& NewPrimaryBrowser);

	/** Notifies the singleton that a browser was closed */
	void CodeBrowserClosed(const TSharedRef<SCodeBrowser>& ClosedBrowser);

	TSharedRef<FNativeClassHierarchy> GetNativeClassHierarchy();

	TSharedRef<FEmptyFolderVisibilityManager> GetEmptyFolderVisibilityManager();

	/** Single storage location for content browser favorites */
	TArray<FString> FavoriteFolderPaths;

private:

	/** Util to get or create the content browser that should be used by the various Sync functions */
	TSharedPtr<SCodeBrowser> FindCodeBrowserToSync(bool bAllowLockedBrowsers, const FName& InstanceName = FName(), bool bNewSpawnBrowser = false);

	/** Shared code to open an asset dialog window with a config */
	void SharedCreateAssetDialogWindow(const TSharedRef<class SAssetDialog>& AssetDialog, const FSharedAssetDialogConfig& InConfig, bool bModal) const;

	/** 
	 * Delegate handlers
	 **/
	void OnEditorLoadSelectedAssetsIfNeeded();

	/** Sets the primary content browser to the next valid browser in the list of all browsers */
	void ChooseNewPrimaryBrowser();

	/** Gives focus to the specified content browser */
	void FocusCodeBrowser(const TSharedPtr<SCodeBrowser>& BrowserToFocus);

	/** Summons a new content browser */
	FName SummonNewBrowser(bool bAllowLockedBrowsers = false);

	/** Handler for a request to spawn a new content browser tab */
	TSharedRef<SDockTab> SpawnCodeBrowserTab( const FSpawnTabArgs& SpawnTabArgs, int32 BrowserIdx );

	/** Handler for a request to spawn a new content browser tab */
	FText GetCodeBrowserTabLabel(int32 BrowserIdx);

	/** Returns true if this content browser is locked (can be used even when closed) */
	bool IsLocked(const FName& InstanceName) const;

	/** Returns a localized name for the tab/menu entry with index */
	static FText GetCodeBrowserLabelWithIndex( int32 BrowserIdx );

public:
	/** The tab identifier/instance name for content browser tabs */
	FName CodeBrowserTabIDs[MAX_CONTENT_BROWSERS];

private:
	TArray<TWeakPtr<SCodeBrowser>> AllCodeBrowsers;

	TMap<FName, TWeakPtr<FTabManager>> BrowserToLastKnownTabManagerMap;

	TWeakPtr<SCodeBrowser> PrimaryCodeBrowser;

	TSharedPtr<FNativeClassHierarchy> NativeClassHierarchy;

	TSharedRef<FEmptyFolderVisibilityManager> EmptyFolderVisibilityManager;

	TSharedRef<FCollectionAssetRegistryBridge> CollectionAssetRegistryBridge;

	/** An incrementing int32 which is used when making unique settings strings */
	int32 SettingsStringID;
};
