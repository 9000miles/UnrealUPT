// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.


#include "CodeBrowserSingleton.h"
#include "Textures/SlateIcon.h"
#include "Misc/ConfigCacheIni.h"
#include "Widgets/SWindow.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/Layout/SBox.h"
#include "Framework/Docking/WorkspaceItem.h"
#include "Framework/Docking/TabManager.h"
#include "EditorStyleSet.h"
#include "Editor.h"
#include "CodeBrowserLog.h"
#include "CodeBrowserUtils.h"
#include "SAssetPicker.h"
#include "SPathPicker.h"
#include "SCollectionPicker.h"
#include "SCodeBrowser.h"
#include "CodeBrowserModule.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"
#include "IDocumentation.h"
#include "Interfaces/IMainFrameModule.h"
#include "SAssetDialog.h"
#include "TutorialMetaData.h"
#include "Widgets/Docking/SDockTab.h"
#include "NativeClassHierarchy.h"
#include "EmptyFolderVisibilityManager.h"
#include "CollectionAssetRegistryBridge.h"
#include "CodeBrowserCommands.h"

#define LOCTEXT_NAMESPACE "CodeBrowser"

FCodeBrowserSingleton::FCodeBrowserSingleton()
	: EmptyFolderVisibilityManager(MakeShared<FEmptyFolderVisibilityManager>())
	, CollectionAssetRegistryBridge(MakeShared<FCollectionAssetRegistryBridge>())
	, SettingsStringID(0)
{
	// Register the tab spawners for all content browsers
	const FSlateIcon CodeBrowserIcon(FEditorStyle::GetStyleSetName(), "CodeBrowser.TabIcon");
	const IWorkspaceMenuStructure& MenuStructure = WorkspaceMenu::GetMenuStructure();
	TSharedRef<FWorkspaceItem> CodeBrowserGroup = MenuStructure.GetToolsCategory()->AddGroup(
		LOCTEXT( "WorkspaceMenu_CodeBrowserCategory", "Content Browser" ),
		LOCTEXT( "CodeBrowserMenuTooltipText", "Open a Content Browser tab." ),
		CodeBrowserIcon,
		true);

	for ( int32 BrowserIdx = 0; BrowserIdx < ARRAY_COUNT(CodeBrowserTabIDs); BrowserIdx++ )
	{
		const FName TabID = FName(*FString::Printf(TEXT("CodeBrowserTab%d"), BrowserIdx + 1));
		CodeBrowserTabIDs[BrowserIdx] = TabID;

		const FText DefaultDisplayName = GetCodeBrowserLabelWithIndex( BrowserIdx );

		FGlobalTabmanager::Get()->RegisterNomadTabSpawner( TabID, FOnSpawnTab::CreateRaw(this, &FCodeBrowserSingleton::SpawnCodeBrowserTab, BrowserIdx) )
			.SetDisplayName(DefaultDisplayName)
			.SetTooltipText( LOCTEXT( "CodeBrowserMenuTooltipText", "Open a Content Browser tab." ) )
			.SetGroup( CodeBrowserGroup )
			.SetIcon(CodeBrowserIcon);
	}

	// Register a couple legacy tab ids
	FGlobalTabmanager::Get()->AddLegacyTabType( "LevelEditorCodeBrowser", "CodeBrowserTab1" );
	FGlobalTabmanager::Get()->AddLegacyTabType( "MajorCodeBrowserTab", "CodeBrowserTab2" );

	// Register to be notified when properties are edited
	FEditorDelegates::LoadSelectedAssetsIfNeeded.AddRaw(this, &FCodeBrowserSingleton::OnEditorLoadSelectedAssetsIfNeeded);

	FCodeBrowserCommands::Register();
}

FCodeBrowserSingleton::~FCodeBrowserSingleton()
{
	FEditorDelegates::LoadSelectedAssetsIfNeeded.RemoveAll(this);

	if ( FSlateApplication::IsInitialized() )
	{
		for ( int32 BrowserIdx = 0; BrowserIdx < ARRAY_COUNT(CodeBrowserTabIDs); BrowserIdx++ )
		{
			FGlobalTabmanager::Get()->UnregisterNomadTabSpawner( CodeBrowserTabIDs[BrowserIdx] );
		}
	}
}

TSharedRef<SWidget> FCodeBrowserSingleton::CreateAssetPicker(const FAssetPickerConfig& AssetPickerConfig)
{
	return SNew( SAssetPicker )
		.IsEnabled( FSlateApplication::Get().GetNormalExecutionAttribute() )
		.AssetPickerConfig(AssetPickerConfig);
}

TSharedRef<SWidget> FCodeBrowserSingleton::CreatePathPicker(const FPathPickerConfig& PathPickerConfig)
{
	return SNew( SPathPicker )
		.IsEnabled( FSlateApplication::Get().GetNormalExecutionAttribute() )
		.PathPickerConfig(PathPickerConfig);
}

TSharedRef<class SWidget> FCodeBrowserSingleton::CreateCollectionPicker(const FCollectionPickerConfig& CollectionPickerConfig)
{
	return SNew( SCollectionPicker )
		.IsEnabled( FSlateApplication::Get().GetNormalExecutionAttribute() )
		.CollectionPickerConfig(CollectionPickerConfig);
}

void FCodeBrowserSingleton::CreateOpenAssetDialog(const FOpenAssetDialogConfig& InConfig,
													 const FOnAssetsChosenForOpen& InOnAssetsChosenForOpen,
													 const FOnAssetDialogCancelled& InOnAssetDialogCancelled)
{
	const bool bModal = false;
	TSharedRef<SAssetDialog> AssetDialog = SNew(SAssetDialog, InConfig);
	AssetDialog->SetOnAssetsChosenForOpen(InOnAssetsChosenForOpen);
	AssetDialog->SetOnAssetDialogCancelled(InOnAssetDialogCancelled);
	SharedCreateAssetDialogWindow(AssetDialog, InConfig, bModal);
}

TArray<FFileData> FCodeBrowserSingleton::CreateModalOpenAssetDialog(const FOpenAssetDialogConfig& InConfig)
{
	struct FModalResults
	{
		void OnAssetsChosenForOpen(const TArray<FFileData>& SelectedAssets)
		{
			SavedResults = SelectedAssets;
		}

		TArray<FFileData> SavedResults;
	};

	FModalResults ModalWindowResults;
	FOnAssetsChosenForOpen OnAssetsChosenForOpenDelegate = FOnAssetsChosenForOpen::CreateRaw(&ModalWindowResults, &FModalResults::OnAssetsChosenForOpen);

	const bool bModal = true;
	TSharedRef<SAssetDialog> AssetDialog = SNew(SAssetDialog, InConfig);
	AssetDialog->SetOnAssetsChosenForOpen(OnAssetsChosenForOpenDelegate);
	SharedCreateAssetDialogWindow(AssetDialog, InConfig, bModal);

	return ModalWindowResults.SavedResults;
}

void FCodeBrowserSingleton::CreateSaveAssetDialog(const FSaveAssetDialogConfig& InConfig,
													 const FOnObjectPathChosenForSave& InOnObjectPathChosenForSave,
													 const FOnAssetDialogCancelled& InOnAssetDialogCancelled)
{
	const bool bModal = false;
	TSharedRef<SAssetDialog> AssetDialog = SNew(SAssetDialog, InConfig);
	AssetDialog->SetOnObjectPathChosenForSave(InOnObjectPathChosenForSave);
	AssetDialog->SetOnAssetDialogCancelled(InOnAssetDialogCancelled);
	SharedCreateAssetDialogWindow(AssetDialog, InConfig, bModal);
}

FString FCodeBrowserSingleton::CreateModalSaveAssetDialog(const FSaveAssetDialogConfig& InConfig)
{
	struct FModalResults
	{
		void OnObjectPathChosenForSave(const FString& ObjectPath)
		{
			SavedResult = ObjectPath;
		}

		FString SavedResult;
	};

	FModalResults ModalWindowResults;
	FOnObjectPathChosenForSave OnObjectPathChosenForSaveDelegate = FOnObjectPathChosenForSave::CreateRaw(&ModalWindowResults, &FModalResults::OnObjectPathChosenForSave);

	const bool bModal = true;
	TSharedRef<SAssetDialog> AssetDialog = SNew(SAssetDialog, InConfig);
	AssetDialog->SetOnObjectPathChosenForSave(OnObjectPathChosenForSaveDelegate);
	SharedCreateAssetDialogWindow(AssetDialog, InConfig, bModal);

	return ModalWindowResults.SavedResult;
}

bool FCodeBrowserSingleton::HasPrimaryCodeBrowser() const
{
	if ( PrimaryCodeBrowser.IsValid() )
	{
		// There is a primary content browser
		return true;
	}
	else
	{
		for (int32 BrowserIdx = 0; BrowserIdx < AllCodeBrowsers.Num(); ++BrowserIdx)
		{
			if ( AllCodeBrowsers[BrowserIdx].IsValid() )
			{
				// There is at least one valid content browser
				return true;
			}
		}

		// There were no valid content browsers
		return false;
	}
}

void FCodeBrowserSingleton::FocusPrimaryCodeBrowser(bool bFocusSearch)
{
	// See if the primary content browser is still valid
	if ( !PrimaryCodeBrowser.IsValid() )
	{
		ChooseNewPrimaryBrowser();
	}

	if ( PrimaryCodeBrowser.IsValid() )
	{
		FocusCodeBrowser( PrimaryCodeBrowser.Pin() );
	}
	else
	{
		// If we couldn't find a primary content browser, open one
		SummonNewBrowser();
	}

	// Do we also want to focus on the search box of the content browser?
	if ( bFocusSearch && PrimaryCodeBrowser.IsValid() )
	{
		PrimaryCodeBrowser.Pin()->SetKeyboardFocusOnSearch();
	}
}

void FCodeBrowserSingleton::CreateNewAsset(const FString& DefaultAssetName, const FString& PackagePath, UClass* AssetClass, UFactory* Factory)
{
	FocusPrimaryCodeBrowser(false);

	if ( PrimaryCodeBrowser.IsValid() )
	{
		PrimaryCodeBrowser.Pin()->CreateNewAsset(DefaultAssetName, PackagePath, AssetClass, Factory);
	}
}

TSharedPtr<SCodeBrowser> FCodeBrowserSingleton::FindCodeBrowserToSync(bool bAllowLockedBrowsers, const FName& InstanceName, bool bNewSpawnBrowser)
{
	TSharedPtr<SCodeBrowser> CodeBrowserToSync;

	if (InstanceName.IsValid() && !InstanceName.IsNone())
	{
		for (int32 BrowserIdx = 0; BrowserIdx < AllCodeBrowsers.Num(); ++BrowserIdx)
		{
			if (AllCodeBrowsers[BrowserIdx].IsValid() && AllCodeBrowsers[BrowserIdx].Pin()->GetInstanceName() == InstanceName)
			{
				return AllCodeBrowsers[BrowserIdx].Pin();
			}	
		}

		return CodeBrowserToSync;
	}

	if ( !PrimaryCodeBrowser.IsValid() )
	{
		ChooseNewPrimaryBrowser();
	}

	if ( PrimaryCodeBrowser.IsValid() && (bAllowLockedBrowsers || !PrimaryCodeBrowser.Pin()->IsLocked()) )
	{
		// If wanting to spawn a new browser window, don't set the BrowserToSync in order to summon a new browser
		if (!bNewSpawnBrowser)
		{
			// If the primary content browser is not locked, sync it
			CodeBrowserToSync = PrimaryCodeBrowser.Pin();
		}
	}
	else
	{
		// If there is no primary or it is locked, find the first non-locked valid browser
		for (int32 BrowserIdx = 0; BrowserIdx < AllCodeBrowsers.Num(); ++BrowserIdx)
		{
			if ( AllCodeBrowsers[BrowserIdx].IsValid() && (bAllowLockedBrowsers || !AllCodeBrowsers[BrowserIdx].Pin()->IsLocked()) )
			{
				CodeBrowserToSync = AllCodeBrowsers[BrowserIdx].Pin();
				break;
			}
		}
	}

	if ( !CodeBrowserToSync.IsValid() )
	{
		// There are no valid, unlocked browsers, attempt to summon a new one.
		const FName NewBrowserName = SummonNewBrowser(bAllowLockedBrowsers);

		// Now try to find a non-locked valid browser again, now that a new one may exist
		for (int32 BrowserIdx = 0; BrowserIdx < AllCodeBrowsers.Num(); ++BrowserIdx)
		{
			if ((AllCodeBrowsers[BrowserIdx].IsValid() && (NewBrowserName == NAME_None && (bAllowLockedBrowsers || !AllCodeBrowsers[BrowserIdx].Pin()->IsLocked()))) || (AllCodeBrowsers[BrowserIdx].Pin()->GetInstanceName() == NewBrowserName))
			{
				CodeBrowserToSync = AllCodeBrowsers[BrowserIdx].Pin();
				break;
			}
		}
	}

	if ( !CodeBrowserToSync.IsValid() )
	{
		UE_LOG( LogCodeBrowser, Log, TEXT( "Unable to sync content browser, all browsers appear to be locked" ) );
	}

	return CodeBrowserToSync;
}

void FCodeBrowserSingleton::SyncBrowserToAssets(const TArray<FFileData>& AssetDataList, bool bAllowLockedBrowsers, bool bFocusCodeBrowser, const FName& InstanceName, bool bNewSpawnBrowser)
{
	TSharedPtr<SCodeBrowser> CodeBrowserToSync = FindCodeBrowserToSync(bAllowLockedBrowsers, InstanceName, bNewSpawnBrowser);

	if ( CodeBrowserToSync.IsValid() )
	{
		// Finally, focus and sync the browser that was found
		if (bFocusCodeBrowser)
		{
			FocusCodeBrowser(CodeBrowserToSync);
		}
		CodeBrowserToSync->SyncToAssets(AssetDataList);
	}
}

void FCodeBrowserSingleton::SyncBrowserToAssets(const TArray<UObject*>& AssetList, bool bAllowLockedBrowsers, bool bFocusCodeBrowser, const FName& InstanceName, bool bNewSpawnBrowser)
{
	// Convert UObject* array to FFileData array
	TArray<FFileData> AssetDataList;
	for (int32 AssetIdx = 0; AssetIdx < AssetList.Num(); ++AssetIdx)
	{
		if ( AssetList[AssetIdx] )
		{
			AssetDataList.Add( FFileData(AssetList[AssetIdx]) );
		}
	}

	SyncBrowserToAssets(AssetDataList, bAllowLockedBrowsers, bFocusCodeBrowser, InstanceName, bNewSpawnBrowser);
}

void FCodeBrowserSingleton::SyncBrowserToFolders(const TArray<FString>& FolderList, bool bAllowLockedBrowsers, bool bFocusCodeBrowser, const FName& InstanceName, bool bNewSpawnBrowser)
{
	TSharedPtr<SCodeBrowser> CodeBrowserToSync = FindCodeBrowserToSync(bAllowLockedBrowsers, InstanceName, bNewSpawnBrowser);

	if ( CodeBrowserToSync.IsValid() )
	{
		// Finally, focus and sync the browser that was found
		if (bFocusCodeBrowser)
		{
			FocusCodeBrowser(CodeBrowserToSync);
		}
		CodeBrowserToSync->SyncToFolders(FolderList);
	}
}

void FCodeBrowserSingleton::SyncBrowserTo(const FCodeBrowserSelection& ItemSelection, bool bAllowLockedBrowsers, bool bFocusCodeBrowser, const FName& InstanceName, bool bNewSpawnBrowser)
{
	TSharedPtr<SCodeBrowser> CodeBrowserToSync = FindCodeBrowserToSync(bAllowLockedBrowsers, InstanceName, bNewSpawnBrowser);

	if ( CodeBrowserToSync.IsValid() )
	{
		// Finally, focus and sync the browser that was found
		if (bFocusCodeBrowser)
		{
			FocusCodeBrowser(CodeBrowserToSync);
		}
		CodeBrowserToSync->SyncTo(ItemSelection);
	}
}

void FCodeBrowserSingleton::GetSelectedAssets(TArray<FFileData>& SelectedAssets)
{
	if ( PrimaryCodeBrowser.IsValid() )
	{
		PrimaryCodeBrowser.Pin()->GetSelectedAssets(SelectedAssets);
	}
}

void FCodeBrowserSingleton::GetSelectedFolders(TArray<FString>& SelectedFolders)
{
	if (PrimaryCodeBrowser.IsValid())
	{
		PrimaryCodeBrowser.Pin()->GetSelectedFolders(SelectedFolders);
	}
}

void FCodeBrowserSingleton::GetSelectedPathViewFolders(TArray<FString>& SelectedFolders)
{
	if (PrimaryCodeBrowser.IsValid())
	{
		SelectedFolders = PrimaryCodeBrowser.Pin()->GetSelectedPathViewFolders();
	}
}

void FCodeBrowserSingleton::CaptureThumbnailFromViewport(FViewport* InViewport, TArray<FFileData>& SelectedAssets)
{
	CodeBrowserUtils::CaptureThumbnailFromViewport(InViewport, SelectedAssets);
}


void FCodeBrowserSingleton::OnEditorLoadSelectedAssetsIfNeeded()
{
	if ( PrimaryCodeBrowser.IsValid() )
	{
		PrimaryCodeBrowser.Pin()->LoadSelectedObjectsIfNeeded();
	}
}

FCodeBrowserSingleton& FCodeBrowserSingleton::Get()
{
	static const FName ModuleName = "CodeBrowser";
	FCodeBrowserModule& Module = FModuleManager::GetModuleChecked<FCodeBrowserModule>(ModuleName);
	return static_cast<FCodeBrowserSingleton&>(Module.Get());
}

void FCodeBrowserSingleton::SetPrimaryCodeBrowser(const TSharedRef<SCodeBrowser>& NewPrimaryBrowser)
{
	if ( PrimaryCodeBrowser.IsValid() && PrimaryCodeBrowser.Pin().ToSharedRef() == NewPrimaryBrowser )
	{
		// This is already the primary content browser
		return;
	}

	if ( PrimaryCodeBrowser.IsValid() )
	{
		PrimaryCodeBrowser.Pin()->SetIsPrimaryCodeBrowser(false);
	}

	PrimaryCodeBrowser = NewPrimaryBrowser;
	NewPrimaryBrowser->SetIsPrimaryCodeBrowser(true);
}

void FCodeBrowserSingleton::CodeBrowserClosed(const TSharedRef<SCodeBrowser>& ClosedBrowser)
{
	// Find the browser in the all browsers list
	for (int32 BrowserIdx = AllCodeBrowsers.Num() - 1; BrowserIdx >= 0; --BrowserIdx)
	{
		if ( !AllCodeBrowsers[BrowserIdx].IsValid() || AllCodeBrowsers[BrowserIdx].Pin() == ClosedBrowser )
		{
			AllCodeBrowsers.RemoveAt(BrowserIdx);
		}
	}

	if ( !PrimaryCodeBrowser.IsValid() || ClosedBrowser == PrimaryCodeBrowser.Pin() )
	{
		ChooseNewPrimaryBrowser();
	}

	BrowserToLastKnownTabManagerMap.Add(ClosedBrowser->GetInstanceName(), ClosedBrowser->GetTabManager());
}

TSharedRef<FNativeClassHierarchy> FCodeBrowserSingleton::GetNativeClassHierarchy()
{
	if(!NativeClassHierarchy.IsValid())
	{
		NativeClassHierarchy = MakeShareable(new FNativeClassHierarchy());
	}
	return NativeClassHierarchy.ToSharedRef();
}

TSharedRef<FEmptyFolderVisibilityManager> FCodeBrowserSingleton::GetEmptyFolderVisibilityManager()
{
	return EmptyFolderVisibilityManager;
}

void FCodeBrowserSingleton::SharedCreateAssetDialogWindow(const TSharedRef<SAssetDialog>& AssetDialog, const FSharedAssetDialogConfig& InConfig, bool bModal) const
{
	const FVector2D DefaultWindowSize(1152.0f, 648.0f);
	const FVector2D WindowSize = InConfig.WindowSizeOverride.IsZero() ? DefaultWindowSize : InConfig.WindowSizeOverride;
	const FText WindowTitle = InConfig.DialogTitleOverride.IsEmpty() ? LOCTEXT("GenericAssetDialogWindowHeader", "Asset Dialog") : InConfig.DialogTitleOverride;

	TSharedRef<SWindow> DialogWindow =
		SNew(SWindow)
		.Title(WindowTitle)
		.ClientSize(WindowSize);

	DialogWindow->SetContent(AssetDialog);

	IMainFrameModule& MainFrameModule = FModuleManager::LoadModuleChecked<IMainFrameModule>(TEXT("MainFrame"));
	const TSharedPtr<SWindow>& MainFrameParentWindow = MainFrameModule.GetParentWindow();
	if (MainFrameParentWindow.IsValid())
	{
		if (bModal)
		{
			FSlateApplication::Get().AddModalWindow(DialogWindow, MainFrameParentWindow.ToSharedRef());
		}
		else if (FGlobalTabmanager::Get()->GetRootWindow().IsValid())
		{
			FSlateApplication::Get().AddWindowAsNativeChild(DialogWindow, MainFrameParentWindow.ToSharedRef());
		}
		else
		{
			FSlateApplication::Get().AddWindow(DialogWindow);
		}
	}
	else
	{
		if (ensureMsgf(!bModal, TEXT("Could not create asset dialog because modal windows must have a parent and this was called at a time where the mainframe window does not exist.")))
		{
			FSlateApplication::Get().AddWindow(DialogWindow);
		}
	}
}

void FCodeBrowserSingleton::ChooseNewPrimaryBrowser()
{
	// Find the first valid browser and trim any invalid browsers along the way
	for (int32 BrowserIdx = 0; BrowserIdx < AllCodeBrowsers.Num(); ++BrowserIdx)
	{
		if ( AllCodeBrowsers[BrowserIdx].IsValid() )
		{
			if (AllCodeBrowsers[BrowserIdx].Pin()->CanSetAsPrimaryCodeBrowser())
			{
				SetPrimaryCodeBrowser(AllCodeBrowsers[BrowserIdx].Pin().ToSharedRef());
				break;
			}
		}
		else
		{
			// Trim any invalid content browsers
			AllCodeBrowsers.RemoveAt(BrowserIdx);
			BrowserIdx--;
		}
	}
}

void FCodeBrowserSingleton::FocusCodeBrowser(const TSharedPtr<SCodeBrowser>& BrowserToFocus)
{
	if ( BrowserToFocus.IsValid() )
	{
		TSharedRef<SCodeBrowser> Browser = BrowserToFocus.ToSharedRef();
		TSharedPtr<FTabManager> TabManager = Browser->GetTabManager();
		if ( TabManager.IsValid() )
		{
			TabManager->InvokeTab(Browser->GetInstanceName());
		}
	}
}

FName FCodeBrowserSingleton::SummonNewBrowser(bool bAllowLockedBrowsers)
{
	TSet<FName> OpenBrowserIDs;

	// Find all currently open browsers to help find the first open slot
	for (int32 BrowserIdx = AllCodeBrowsers.Num() - 1; BrowserIdx >= 0; --BrowserIdx)
	{
		const TWeakPtr<SCodeBrowser>& Browser = AllCodeBrowsers[BrowserIdx];
		if ( Browser.IsValid() )
		{
			OpenBrowserIDs.Add(Browser.Pin()->GetInstanceName());
		}
	}
	
	FName NewTabName;
	for ( int32 BrowserIdx = 0; BrowserIdx < ARRAY_COUNT(CodeBrowserTabIDs); BrowserIdx++ )
	{
		FName TestTabID = CodeBrowserTabIDs[BrowserIdx];
		if ( !OpenBrowserIDs.Contains(TestTabID) && (bAllowLockedBrowsers || !IsLocked(TestTabID)) )
		{
			// Found the first index that is not currently open
			NewTabName = TestTabID;
			break;
		}
	}

	if ( NewTabName != NAME_None )
	{
		const TWeakPtr<FTabManager>& TabManagerToInvoke = BrowserToLastKnownTabManagerMap.FindRef(NewTabName);
		if ( TabManagerToInvoke.IsValid() )
		{
			TabManagerToInvoke.Pin()->InvokeTab(NewTabName);
		}
		else
		{
			FGlobalTabmanager::Get()->InvokeTab(NewTabName);
		}
	}
	else
	{
		// No available slots... don't summon anything
	}

	return NewTabName;
}

TSharedRef<SWidget> FCodeBrowserSingleton::CreateCodeBrowser( const FName InstanceName, TSharedPtr<SDockTab> ContainingTab, const FCodeBrowserConfig* CodeBrowserConfig )
{
	TSharedRef<SCodeBrowser> NewBrowser =
		SNew( SCodeBrowser, InstanceName, CodeBrowserConfig )
		.IsEnabled( FSlateApplication::Get().GetNormalExecutionAttribute() )
		.ContainingTab( ContainingTab );

	AllCodeBrowsers.Add( NewBrowser );

	if( !PrimaryCodeBrowser.IsValid() )
	{
		ChooseNewPrimaryBrowser();
	}

	return NewBrowser;
}


TSharedRef<SDockTab> FCodeBrowserSingleton::SpawnCodeBrowserTab( const FSpawnTabArgs& SpawnTabArgs, int32 BrowserIdx )
{	
	TAttribute<FText> Label = TAttribute<FText>::Create( TAttribute<FText>::FGetter::CreateRaw( this, &FCodeBrowserSingleton::GetCodeBrowserTabLabel, BrowserIdx ) );

	TSharedRef<SDockTab> NewTab = SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		.Label( Label )
		.ToolTip( IDocumentation::Get()->CreateToolTip( Label, nullptr, "Shared/CodeBrowser", "Tab" ) );

	TSharedRef<SWidget> NewBrowser = CreateCodeBrowser( SpawnTabArgs.GetTabId().TabType, NewTab, nullptr );

	if ( !PrimaryCodeBrowser.IsValid() )
	{
		ChooseNewPrimaryBrowser();
	}

	// Add wrapper for tutorial highlighting
	TSharedRef<SBox> Wrapper =
		SNew(SBox)
		.AddMetaData<FTutorialMetaData>(FTutorialMetaData(TEXT("CodeBrowser"), TEXT("CodeBrowserTab1")))
		[
			NewBrowser
		];

	NewTab->SetContent( Wrapper );

	return NewTab;
}

bool FCodeBrowserSingleton::IsLocked(const FName& InstanceName) const
{
	// First try all the open browsers, as their locked state might be newer than the configs
	for (int32 AllBrowsersIdx = AllCodeBrowsers.Num() - 1; AllBrowsersIdx >= 0; --AllBrowsersIdx)
	{
		const TWeakPtr<SCodeBrowser>& Browser = AllCodeBrowsers[AllBrowsersIdx];
		if ( Browser.IsValid() && Browser.Pin()->GetInstanceName() == InstanceName )
		{
			return Browser.Pin()->IsLocked();
		}
	}

	// Fallback to getting the locked state from the config instead
	bool bIsLocked = false;
	GConfig->GetBool(*SCodeBrowser::SettingsIniSection, *(InstanceName.ToString() + TEXT(".Locked")), bIsLocked, GEditorPerProjectIni);
	return bIsLocked;
}

FText FCodeBrowserSingleton::GetCodeBrowserLabelWithIndex( int32 BrowserIdx )
{
	return FText::Format(LOCTEXT("CodeBrowserTabNameWithIndex", "Content Browser {0}"), FText::AsNumber(BrowserIdx + 1));
}

FText FCodeBrowserSingleton::GetCodeBrowserTabLabel(int32 BrowserIdx)
{
	int32 NumOpenCodeBrowsers = 0;
	for (int32 AllBrowsersIdx = AllCodeBrowsers.Num() - 1; AllBrowsersIdx >= 0; --AllBrowsersIdx)
	{
		const TWeakPtr<SCodeBrowser>& Browser = AllCodeBrowsers[AllBrowsersIdx];
		if ( Browser.IsValid() )
		{
			NumOpenCodeBrowsers++;
		}
		else
		{
			AllCodeBrowsers.RemoveAt(AllBrowsersIdx);
		}
	}

	if ( NumOpenCodeBrowsers > 1 )
	{
		return GetCodeBrowserLabelWithIndex( BrowserIdx );
	}
	else
	{
		return LOCTEXT("CodeBrowserTabName", "Content Browser");
	}
}

void FCodeBrowserSingleton::SetSelectedPaths(const TArray<FString>& FolderPaths, bool bNeedsRefresh/* = false*/)
{
	// Make sure we have a valid browser
	if (!PrimaryCodeBrowser.IsValid())
	{
		ChooseNewPrimaryBrowser();

		if (!PrimaryCodeBrowser.IsValid())
		{
			SummonNewBrowser();
		}
	}

	if (PrimaryCodeBrowser.IsValid())
	{
		PrimaryCodeBrowser.Pin()->SetSelectedPaths(FolderPaths, bNeedsRefresh);
	}
}

void FCodeBrowserSingleton::ForceShowPluginContent(bool bEnginePlugin)
{
	if (!PrimaryCodeBrowser.IsValid())
	{
		ChooseNewPrimaryBrowser();

		if (!PrimaryCodeBrowser.IsValid())
		{
			SummonNewBrowser();
		}
	}

	if (PrimaryCodeBrowser.IsValid())
	{
		PrimaryCodeBrowser.Pin()->ForceShowPluginContent(bEnginePlugin);
	}
}

#undef LOCTEXT_NAMESPACE
