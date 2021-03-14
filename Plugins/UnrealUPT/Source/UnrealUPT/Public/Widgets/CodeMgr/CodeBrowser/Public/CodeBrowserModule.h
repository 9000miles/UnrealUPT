// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetData.h"
#include "Modules/ModuleInterface.h"
#include "CodeBrowserDelegates.h"

class ICodeBrowserSingleton;
struct FARFilter;
class FMainMRUFavoritesList;

/**
 * Content browser module
 */
class FCodeBrowserModule : public IModuleInterface
{
public:

	/**  */
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnFilterChanged, const FARFilter& /*NewFilter*/, bool /*bIsPrimaryBrowser*/);
	/** */
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnSearchBoxChanged, const FText& /*InSearchText*/, bool /*bIsPrimaryBrowser*/);
	/** */
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnAssetSelectionChanged, const TArray<FFileData>& /*NewSelectedAssets*/, bool /*bIsPrimaryBrowser*/);
	/** */
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnSourcesViewChanged, bool /*bExpanded*/);
	/** */
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnAssetPathChanged, const FString& /*NewPath*/);
	/** */
	DECLARE_MULTICAST_DELEGATE(FOnRequestNewFolder);
	/** */
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnRenameFolder, const TArray<struct FMovedContentFolder>& /*MovedFolders*/);
	/** */
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnDeletedFolder, TArray<FString>& /*SelectedPaths*/);
	//隐藏指定类
	DECLARE_DELEGATE_OneParam(FOnHideSpecifiedClassNames, TArray<FName>&);
	//CodeBrowser希望显示的根目录
	DECLARE_DELEGATE_OneParam(FOnGetDirectoryWantToDisplay, TArray<FString>& /*DirectoryWantToDisplay*/);
	/**
	 * Called right after the plugin DLL has been loaded and the plugin object has been created
	 */
	virtual void StartupModule();

	/**
	 * Called before the plugin is unloaded, right before the plugin object is destroyed.
	 */
	virtual void ShutdownModule();

	/** Gets the content browser singleton */
	virtual ICodeBrowserSingleton& Get() const;

	/** Delegates to be called to extend the content browser menus */
	virtual TArray<FCodeBrowserMenuExtender_SelectedPaths>& GetAllAssetContextMenuExtenders() { return AssetContextMenuExtenders; }
	virtual TArray<FCodeBrowserMenuExtender_SelectedPaths>& GetAllPathViewContextMenuExtenders() { return PathViewContextMenuExtenders; }
	virtual TArray<FCodeBrowserMenuExtender>& GetAllCollectionListContextMenuExtenders() { return CollectionListContextMenuExtenders; }
	virtual TArray<FCodeBrowserMenuExtender>& GetAllCollectionViewContextMenuExtenders() { return CollectionViewContextMenuExtenders; }
	virtual TArray<FCodeBrowserMenuExtender_SelectedAssets>& GetAllAssetViewContextMenuExtenders() { return AssetViewContextMenuExtenders; }
	virtual TArray<FCodeBrowserMenuExtender>& GetAllAssetViewViewMenuExtenders() { return AssetViewViewMenuExtenders; }

	/** Delegates to call to extend the command/keybinds for content browser */
	virtual TArray<FCodeBrowserCommandExtender>& GetAllCodeBrowserCommandExtenders() { return CodeBrowserCommandExtenders; }

	/** Delegates to be called to add extra state indicators on the asset view items */
	virtual TArray<FOnGenerateAssetViewExtraStateIndicators>& GetAllAssetViewExtraStateIconGenerators() { return AssetViewExtraStateIconGenerators; }

	/** Delegates to be called to add extra state indicators on the asset view items */
	virtual TArray<FOnGenerateAssetViewExtraStateIndicators>& GetAllAssetViewExtraStateTooltipGenerators() { return AssetViewExtraStateTooltipGenerators; }

	/** Delegates to be called to extend the drag-and-drop support of the asset view */
	virtual TArray<FAssetViewDragAndDropExtender>& GetAssetViewDragAndDropExtenders() { return AssetViewDragAndDropExtenders; }

	/** Delegate accessors */
	FOnFilterChanged& GetOnFilterChanged() { return OnFilterChanged; }
	FOnSearchBoxChanged& GetOnSearchBoxChanged() { return OnSearchBoxChanged; }
	FOnAssetSelectionChanged& GetOnAssetSelectionChanged() { return OnAssetSelectionChanged; }
	FOnSourcesViewChanged& GetOnSourcesViewChanged() { return OnSourcesViewChanged; }
	FOnAssetPathChanged& GetOnAssetPathChanged() { return OnAssetPathChanged; }
	FOnRequestNewFolder& GetOnRequestNewFolder() { return OnRequestNewFolder; }
	FOnRenameFolder& GetOnRenameFolder() { return OnRenameFolder; }
	FOnDeletedFolder& GetOnDeletedFolder() { return OnFolderDeleted; }
	FOnHideSpecifiedClassNames& GetOnHideSpecifiedClassNames() { return OnHideSpecifiedClassNames; }
	FOnGetDirectoryWantToDisplay& GetOnDirectoryWantToDisplay() { return OnGetDirectoryWantToDisplay; }

	FMainMRUFavoritesList* GetRecentlyOpenedAssets() const
	{
		return RecentlyOpenedAssets.Get();
	};

	static const FName NumberOfRecentAssetsName;

private:
	/** Resize the recently opened asset list */
	void ResizeRecentAssetList(FName InName);

private:
	ICodeBrowserSingleton* CodeBrowserSingleton;
	TSharedPtr<class FCodeBrowserSpawner> CodeBrowserSpawner;

	/** All extender delegates for the content browser menus */
	TArray<FCodeBrowserMenuExtender_SelectedPaths> AssetContextMenuExtenders;
	TArray<FCodeBrowserMenuExtender_SelectedPaths> PathViewContextMenuExtenders;
	TArray<FCodeBrowserMenuExtender> CollectionListContextMenuExtenders;
	TArray<FCodeBrowserMenuExtender> CollectionViewContextMenuExtenders;
	TArray<FCodeBrowserMenuExtender_SelectedAssets> AssetViewContextMenuExtenders;
	TArray<FCodeBrowserMenuExtender> AssetViewViewMenuExtenders;
	TArray<FCodeBrowserCommandExtender> CodeBrowserCommandExtenders;

	/** All delegates generating extra state indicators */
	TArray<FOnGenerateAssetViewExtraStateIndicators> AssetViewExtraStateIconGenerators;
	TArray<FOnGenerateAssetViewExtraStateIndicators> AssetViewExtraStateTooltipGenerators;

	/** All extender delegates for the drag-and-drop support of the asset view */
	TArray<FAssetViewDragAndDropExtender> AssetViewDragAndDropExtenders;

	TUniquePtr<FMainMRUFavoritesList> RecentlyOpenedAssets;

	FOnFilterChanged OnFilterChanged;
	FOnSearchBoxChanged OnSearchBoxChanged;
	FOnAssetSelectionChanged OnAssetSelectionChanged;
	FOnSourcesViewChanged OnSourcesViewChanged;
	FOnAssetPathChanged OnAssetPathChanged;
	FOnRequestNewFolder OnRequestNewFolder;
	FOnRenameFolder OnRenameFolder;
	FOnDeletedFolder OnFolderDeleted;
	FOnHideSpecifiedClassNames OnHideSpecifiedClassNames;
	FOnGetDirectoryWantToDisplay OnGetDirectoryWantToDisplay;
};
