// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.


#include "SCodeBrowser.h"
#include "Factories/Factory.h"
#include "Framework/Commands/UIAction.h"
#include "Textures/SlateIcon.h"
#include "Framework/Commands/UICommandList.h"
#include "Misc/ConfigCacheIni.h"
#include "Misc/FeedbackContext.h"
#include "Misc/ScopedSlowTask.h"
#include "Widgets/SBoxPanel.h"
#include "Layout/WidgetPath.h"
#include "SlateOptMacros.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/Layout/SWrapBox.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/Layout/SSplitter.h"
#include "Framework/Docking/TabManager.h"
#include "EditorStyleSet.h"
#include "EditorFontGlyphs.h"
#include "Settings/EditorSettings.h"
#include "Editor.h"
#include "FileHelpers.h"
#include "AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "Widgets/Navigation/SBreadcrumbTrail.h"
#include "CodeBrowserLog.h"
#include "FrontendFilters.h"
#include "CodeBrowserSingleton.h"
#include "CodeBrowserUtils.h"
#include "SAssetSearchBox.h"
#include "SFilterList.h"
#include "SPathView.h"
#include "SCollectionView.h"
#include "SAssetView.h"
#include "AssetContextMenu.h"
#include "NewAssetOrClassContextMenu.h"
#include "PathContextMenu.h"
#include "CodeBrowserModule.h"
#include "CodeBrowserCommands.h"
#include "Widgets/Docking/SDockTab.h"
#include "Framework/Commands/GenericCommands.h"
#include "IAddContentDialogModule.h"
#include "Engine/Selection.h"
#include "NativeClassHierarchy.h"
#include "AddToProjectConfig.h"
#include "GameProjectGenerationModule.h"
#include "Toolkits/GlobalEditorCommonCommands.h"

#define LOCTEXT_NAMESPACE "CodeBrowser"

const FString SCodeBrowser::SettingsIniSection = TEXT("CodeBrowser");

SCodeBrowser::~SCodeBrowser()
{
	// Remove listeners for when collections/paths are renamed/deleted
	if (FCollectionManagerModule::IsModuleAvailable())
	{
		FCollectionManagerModule& CollectionManagerModule = FCollectionManagerModule::GetModule();

		CollectionManagerModule.Get().OnCollectionRenamed().RemoveAll(this);
		CollectionManagerModule.Get().OnCollectionDestroyed().RemoveAll(this);
	}

	FAssetRegistryModule* AssetRegistryModule = FModuleManager::GetModulePtr<FAssetRegistryModule>(TEXT("AssetRegistry"));
	if (AssetRegistryModule != nullptr)
	{
		AssetRegistryModule->Get().OnPathRemoved().RemoveAll(this);
	}
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SCodeBrowser::Construct( const FArguments& InArgs, const FName& InInstanceName, const FCodeBrowserConfig* Config )
{
	if ( InArgs._ContainingTab.IsValid() )
	{
		// For content browsers that are placed in tabs, save settings when the tab is closing.
		ContainingTab = InArgs._ContainingTab;
		InArgs._ContainingTab->SetOnPersistVisualState( SDockTab::FOnPersistVisualState::CreateSP( this, &SCodeBrowser::OnContainingTabSavingVisualState ) );
		InArgs._ContainingTab->SetOnTabClosed( SDockTab::FOnTabClosedCallback::CreateSP( this, &SCodeBrowser::OnContainingTabClosed ) );
		InArgs._ContainingTab->SetOnTabActivated( SDockTab::FOnTabActivatedCallback::CreateSP( this, &SCodeBrowser::OnContainingTabActivated ) );
	}
	
	bIsLocked = InArgs._InitiallyLocked;
	bAlwaysShowCollections = Config != nullptr ? Config->bAlwaysShowCollections : false;
	bCanSetAsPrimaryBrowser = Config != nullptr ? Config->bCanSetAsPrimaryBrowser : true;

	HistoryManager.SetOnApplyHistoryData(FOnApplyHistoryData::CreateSP(this, &SCodeBrowser::OnApplyHistoryData));
	HistoryManager.SetOnUpdateHistoryData(FOnUpdateHistoryData::CreateSP(this, &SCodeBrowser::OnUpdateHistoryData));

	PathContextMenu = MakeShareable(new FPathContextMenu( AsShared() ));
	PathContextMenu->SetOnNewAssetRequested( FNewAssetOrClassContextMenu::FOnNewAssetRequested::CreateSP(this, &SCodeBrowser::NewAssetRequested) );
	PathContextMenu->SetOnNewClassRequested( FNewAssetOrClassContextMenu::FOnNewClassRequested::CreateSP(this, &SCodeBrowser::NewClassRequested) );
	PathContextMenu->SetOnImportAssetRequested(FNewAssetOrClassContextMenu::FOnImportAssetRequested::CreateSP(this, &SCodeBrowser::ImportAsset));
	PathContextMenu->SetOnRenameFolderRequested(FPathContextMenu::FOnRenameFolderRequested::CreateSP(this, &SCodeBrowser::OnRenameFolderRequested));
	PathContextMenu->SetOnFolderDeleted(FPathContextMenu::FOnFolderDeleted::CreateSP(this, &SCodeBrowser::OnOpenedFolderDeleted));
	PathContextMenu->SetOnFolderFavoriteToggled(FPathContextMenu::FOnFolderFavoriteToggled::CreateSP(this, &SCodeBrowser::ToggleFolderFavorite));
	FrontendFilters = MakeShareable(new FAssetFilterCollectionType());
	TextFilter = MakeShareable( new FFrontendFilter_Text() );

	static const FName DefaultForegroundName("DefaultForeground");

	BindCommands();

	ChildSlot
	[
		SNew(SVerticalBox)

		// Path and history
		+SVerticalBox::Slot()
		.AutoHeight()
		.Padding( 0, 0, 0, 0 )
		[
			SNew( SWrapBox )
			.UseAllottedWidth( true )
			.InnerSlotPadding( FVector2D( 5, 2 ) )

			+ SWrapBox::Slot()
			.FillLineWhenWidthLessThan( 600 )
			.FillEmptySpace( true )
			[
				SNew( SHorizontalBox )

				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				[
					SNew( SBorder )
#pragma region // CodeBrowser左上角 AddNew，Import，SaveAll - 
					//.Visibility_Lambda([this] { if (CHECK_ENGINE_IS_COURSEWARE_EDITOR == false) return EVisibility::Visible;	return EVisibility::Collapsed; })
#pragma endregion
					.Padding( FMargin( 3 ) )
					.BorderImage( FEditorStyle::GetBrush( "ToolPanel.GroupBorder" ) )
					[
						SNew( SHorizontalBox )

						// New
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.VAlign( VAlign_Center )
						.HAlign( HAlign_Left )
						[
							SNew( SComboButton )
							.ComboButtonStyle( FEditorStyle::Get(), "ToolbarComboButton" )
							.ButtonStyle(FEditorStyle::Get(), "FlatButton.Success")
							.ForegroundColor(FLinearColor::White)
							.ContentPadding(FMargin(6, 2))
							.OnGetMenuContent_Lambda( [this]{ return MakeAddNewContextMenu( true, false ); } )
							.ToolTipText( this, &SCodeBrowser::GetAddNewToolTipText )
							.IsEnabled( this, &SCodeBrowser::IsAddNewEnabled )
							.AddMetaData<FTagMetaData>(FTagMetaData(TEXT("CodeBrowserNewAsset")))
							.HasDownArrow(false)
							.ButtonContent()
							[
								SNew( SHorizontalBox )

								// New Icon
								+ SHorizontalBox::Slot()
								.VAlign(VAlign_Center)
								.AutoWidth()
								[
									SNew(STextBlock)
									.TextStyle(FEditorStyle::Get(), "CodeBrowser.TopBar.Font")
									.Font(FEditorStyle::Get().GetFontStyle("FontAwesome.11"))
									.Text(FEditorFontGlyphs::File)
								]

								// New Text
								+ SHorizontalBox::Slot()
								.AutoWidth()
								.VAlign(VAlign_Center)
								.Padding(4, 0, 0, 0)
								[
									SNew( STextBlock )
									.TextStyle( FEditorStyle::Get(), "CodeBrowser.TopBar.Font" )
									.Text( LOCTEXT( "NewButton", "Add New" ) )
								]

								// Down Arrow
								+ SHorizontalBox::Slot()
								.VAlign(VAlign_Center)
								.AutoWidth()
								.Padding(4, 0, 0, 0)
								[
									SNew(STextBlock)
									.TextStyle(FEditorStyle::Get(), "CodeBrowser.TopBar.Font")
									.Font(FEditorStyle::Get().GetFontStyle("FontAwesome.10"))
									.Text(FEditorFontGlyphs::Caret_Down)
								]
							]
						]

						// Import
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.VAlign( VAlign_Center )
						.HAlign( HAlign_Left )
						.Padding(6, 0)
						[
							SNew( SButton )
							.ButtonStyle(FEditorStyle::Get(), "FlatButton")
							.ToolTipText( this, &SCodeBrowser::GetImportTooltipText )
							.IsEnabled( this, &SCodeBrowser::IsImportEnabled )
							.OnClicked( this, &SCodeBrowser::HandleImportClicked )
							.ContentPadding(FMargin(6, 2))
							.AddMetaData<FTagMetaData>(FTagMetaData(TEXT("CodeBrowserImportAsset")))
							[
								SNew( SHorizontalBox )

								// Import Icon
								+ SHorizontalBox::Slot()
								.VAlign(VAlign_Center)
								.AutoWidth()
								[
									SNew(STextBlock)
									.TextStyle(FEditorStyle::Get(), "CodeBrowser.TopBar.Font")
									.Font(FEditorStyle::Get().GetFontStyle("FontAwesome.11"))
									.Text(FEditorFontGlyphs::Download)
								]

								// Import Text
								+ SHorizontalBox::Slot()
								.AutoWidth()
								.VAlign(VAlign_Center)
								.Padding(4, 0, 0, 0)
								[
									SNew( STextBlock )
									.TextStyle( FEditorStyle::Get(), "CodeBrowser.TopBar.Font" )
									.Text( LOCTEXT( "Import", "Import" ) )
								]
							]
						]

						// Save
						+ SHorizontalBox::Slot()
						.FillWidth(1.0f)
						.VAlign(VAlign_Center)
						.HAlign(HAlign_Left)
						[
							SNew( SButton )
							.ButtonStyle(FEditorStyle::Get(), "FlatButton")
							.ToolTipText( LOCTEXT( "SaveDirtyPackagesTooltip", "Save all modified assets." ) )
							.ContentPadding(FMargin(6, 2))
							.OnClicked( this, &SCodeBrowser::OnSaveClicked )
							.AddMetaData<FTagMetaData>(FTagMetaData(TEXT("CodeBrowserSaveDirtyPackages")))
							[
								SNew( SHorizontalBox )

								// Save All Icon
								+ SHorizontalBox::Slot()
								.VAlign(VAlign_Center)
								.AutoWidth()
								[
									SNew(STextBlock)
									.TextStyle(FEditorStyle::Get(), "CodeBrowser.TopBar.Font")
									.Font(FEditorStyle::Get().GetFontStyle("FontAwesome.11"))
									.Text(FEditorFontGlyphs::Floppy_O)
								]

								// Save All Text
								+ SHorizontalBox::Slot()
								.AutoWidth()
								.VAlign(VAlign_Center)
								.Padding(4, 0, 0, 0)
								[
									SNew( STextBlock )
									.TextStyle( FEditorStyle::Get(), "CodeBrowser.TopBar.Font" )
									.Text( LOCTEXT( "SaveAll", "Save All" ) )
								]
							]
						]
					]
				]
			]

			+ SWrapBox::Slot()
			.FillEmptySpace( true )
			[
				SNew(SBorder)
				.Padding(FMargin(3))
				.BorderImage( FEditorStyle::GetBrush("ToolPanel.GroupBorder") )
				[
					SNew(SHorizontalBox)

					// History Back Button
					+SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(SVerticalBox)

						+ SVerticalBox::Slot()
						.FillHeight(1.0f)
						[
							SNew(SButton)
							.VAlign(EVerticalAlignment::VAlign_Center)
							.ButtonStyle(FEditorStyle::Get(), "FlatButton")
							.ForegroundColor( FEditorStyle::GetSlateColor(DefaultForegroundName) )
							.ToolTipText( this, &SCodeBrowser::GetHistoryBackTooltip )
							.ContentPadding( FMargin(1, 0) )
							.OnClicked(this, &SCodeBrowser::BackClicked)
							.IsEnabled(this, &SCodeBrowser::IsBackEnabled)
							.AddMetaData<FTagMetaData>(FTagMetaData(TEXT("CodeBrowserHistoryBack")))
							[
								SNew(STextBlock)
								.TextStyle(FEditorStyle::Get(), "CodeBrowser.TopBar.Font")
								.Font(FEditorStyle::Get().GetFontStyle("FontAwesome.11"))
								.Text(FText::FromString(FString(TEXT("\xf060"))) /*fa-arrow-left*/)
							]
						]
					]

					// History Forward Button
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(SVerticalBox)

						+ SVerticalBox::Slot()
						.FillHeight(1.0f)
						[
							SNew(SButton)
							.VAlign(EVerticalAlignment::VAlign_Center)
							.ButtonStyle(FEditorStyle::Get(), "FlatButton")
							.ForegroundColor( FEditorStyle::GetSlateColor(DefaultForegroundName) )
							.ToolTipText( this, &SCodeBrowser::GetHistoryForwardTooltip )
							.ContentPadding( FMargin(1, 0) )
							.OnClicked(this, &SCodeBrowser::ForwardClicked)
							.IsEnabled(this, &SCodeBrowser::IsForwardEnabled)
							.AddMetaData<FTagMetaData>(FTagMetaData(TEXT("CodeBrowserHistoryForward")))
							[
								SNew(STextBlock)
								.TextStyle(FEditorStyle::Get(), "CodeBrowser.TopBar.Font")
								.Font(FEditorStyle::Get().GetFontStyle("FontAwesome.11"))
								.Text(FText::FromString(FString(TEXT("\xf061"))) /*fa-arrow-right*/)
							]
						]
					]

					// Separator
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.Padding(3, 0)
					[
						SNew(SSeparator)
						.Orientation(Orient_Vertical)
					]

					// Path picker
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.VAlign( VAlign_Fill )
					[
						SAssignNew( PathPickerButton, SComboButton )
						.Visibility( ( Config != nullptr ? Config->bUsePathPicker : true ) ? EVisibility::Visible : EVisibility::Collapsed )
						.ButtonStyle(FEditorStyle::Get(), "FlatButton")
						.ForegroundColor(FLinearColor::White)
						.ToolTipText( LOCTEXT( "PathPickerTooltip", "Choose a path" ) )
						.OnGetMenuContent( this, &SCodeBrowser::GetPathPickerContent )
						.HasDownArrow( false )
						.AddMetaData<FTagMetaData>(FTagMetaData(TEXT("CodeBrowserPathPicker")))
						.ContentPadding(FMargin(3, 3))
						.ButtonContent()
						[
							SNew(STextBlock)
							.TextStyle(FEditorStyle::Get(), "CodeBrowser.TopBar.Font")
							.Font(FEditorStyle::Get().GetFontStyle("FontAwesome.11"))
							.Text(FText::FromString(FString(TEXT("\xf07c"))) /*fa-folder-open*/)
						]
					]

					// Path
					+ SHorizontalBox::Slot()
					.VAlign(VAlign_Center)
					.HAlign(HAlign_Left)
					.FillWidth(1.0f)
					.Padding(FMargin(0))
					[
						SAssignNew(PathBreadcrumbTrail, SBreadcrumbTrail<FString>)
						.ButtonContentPadding(FMargin(2, 2))
						.ButtonStyle(FEditorStyle::Get(), "FlatButton")
						.DelimiterImage(FEditorStyle::GetBrush("CodeBrowser.PathDelimiter"))
						.TextStyle(FEditorStyle::Get(), "CodeBrowser.PathText")
						.ShowLeadingDelimiter(false)
						.InvertTextColorOnHover(false)
						.OnCrumbClicked(this, &SCodeBrowser::OnPathClicked)
						.GetCrumbMenuContent(this, &SCodeBrowser::OnGetCrumbDelimiterContent)
						.AddMetaData<FTagMetaData>(FTagMetaData(TEXT("CodeBrowserPath")))
					]

					// Lock button
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.VAlign(VAlign_Center)
					[
						SNew(SVerticalBox)
						.Visibility( ( Config != nullptr ? Config->bCanShowLockButton : true ) ? EVisibility::SelfHitTestInvisible : EVisibility::Collapsed )

						+ SVerticalBox::Slot()
						.FillHeight(1.0f)
						[
							SNew(SButton)
							.VAlign(EVerticalAlignment::VAlign_Center)
							.ButtonStyle(FEditorStyle::Get(), "FlatButton")
							.ToolTipText( LOCTEXT("LockToggleTooltip", "Toggle lock. If locked, this browser will ignore Find in Content Browser requests.") )
							.ContentPadding( FMargin(1, 0) )
							.OnClicked(this, &SCodeBrowser::ToggleLockClicked)
							.AddMetaData<FTagMetaData>(FTagMetaData(TEXT("CodeBrowserLock")))
							[
								SNew(SImage)
								.Image( this, &SCodeBrowser::GetToggleLockImage)
							]
						]
					]
				]
			]
		]

		// Assets/tree
		+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		.Padding(0,2,0,0)
		[
			// The tree/assets splitter
			SAssignNew(PathAssetSplitterPtr, SSplitter)
			// Sources View
			+ SSplitter::Slot()
			.Value(0.25f)
			[
				SAssignNew(PathFavoriteSplitterPtr, SSplitter)
				.Orientation(EOrientation::Orient_Vertical)
				.MinimumSlotHeight(70.0f)
				.Visibility( this, &SCodeBrowser::GetSourcesViewVisibility )
				+ SSplitter::Slot()
				.Value(.2f)
				[
					SNew(SBorder)

					.Visibility(this, &SCodeBrowser::GetFavoriteFolderVisibility)
					.Padding(FMargin(3))
					.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
					[
						SAssignNew(FavoritePathViewPtr, SFavoritePathView)
						.OnPathSelected(this, &SCodeBrowser::FavoritePathSelected)
						.OnGetFolderContextMenu(this, &SCodeBrowser::GetFolderContextMenu, true)
						.OnGetPathContextMenuExtender(this, &SCodeBrowser::GetPathContextMenuExtender)
						.FocusSearchBoxWhenOpened(false)
						.ShowTreeTitle(true)
						.ShowSeparator(false)
						.AllowClassesFolder(true)
						.SearchContent()
						[
							SNew(SVerticalBox)
							.AddMetaData<FTagMetaData>(FTagMetaData(TEXT("CodeBrowserSourcesToggle1")))
							+ SVerticalBox::Slot()
							.FillHeight(1.0f)
							.Padding(0, 0, 2, 0)
							[
								SNew(SButton)
								.VAlign(EVerticalAlignment::VAlign_Center)
								.ButtonStyle(FEditorStyle::Get(), "ToggleButton")
								.ToolTipText(LOCTEXT("SourcesTreeToggleTooltip", "Show or hide the sources panel"))
								.ContentPadding(FMargin(1, 0))
								.ForegroundColor(FEditorStyle::GetSlateColor(DefaultForegroundName))
								.OnClicked(this, &SCodeBrowser::SourcesViewExpandClicked)
								[
									SNew(SImage)
									.Image(this, &SCodeBrowser::GetSourcesToggleImage)
								]
							]
						]
					]
				]
				+ SSplitter::Slot()
				.Value(0.8f)
				[
					SAssignNew(PathCollectionSplitterPtr, SSplitter)
					.Style( FEditorStyle::Get(), "CodeBrowser.Splitter" )
					.Orientation( Orient_Vertical )
					// Path View
					+ SSplitter::Slot()
					.Value(0.9f)
					[
						SNew(SBorder)
						.Visibility( ( Config != nullptr ? Config->bShowAssetPathTree : true ) ? EVisibility::Visible : EVisibility::Collapsed )
						.Padding(FMargin(3))
						.BorderImage( FEditorStyle::GetBrush("ToolPanel.GroupBorder") )
						[
							SAssignNew( PathViewPtr, SPathView )
							.OnPathSelected( this, &SCodeBrowser::PathSelected )
							.OnGetFolderContextMenu( this, &SCodeBrowser::GetFolderContextMenu, true )
							.OnGetPathContextMenuExtender( this, &SCodeBrowser::GetPathContextMenuExtender )
							.FocusSearchBoxWhenOpened( false )
							.SearchBarVisibility(this, &SCodeBrowser::GetAlternateSearchBarVisibility)
							.ShowTreeTitle( false )
							.ShowSeparator( false )
							.AllowClassesFolder( true )
							.AddMetaData<FTagMetaData>(FTagMetaData(TEXT("CodeBrowserSources")))
							.SearchContent()
							[
								SNew(SVerticalBox)
								.AddMetaData<FTagMetaData>(FTagMetaData(TEXT("CodeBrowserSourcesToggle1")))
								.Visibility(this, &SCodeBrowser::GetAlternateSearchBarVisibility)
								+ SVerticalBox::Slot()
								.FillHeight(1.0f)
								.Padding(0, 0, 2, 0)
								[
									SNew(SButton)
									.VAlign(EVerticalAlignment::VAlign_Center)
									.ButtonStyle(FEditorStyle::Get(), "ToggleButton")
									.ToolTipText(LOCTEXT("SourcesTreeToggleTooltip", "Show or hide the sources panel"))
									.ContentPadding(FMargin(1, 0))
									.ForegroundColor(FEditorStyle::GetSlateColor(DefaultForegroundName))
									.OnClicked(this, &SCodeBrowser::SourcesViewExpandClicked)
									[
										SNew(SImage)
										.Image(this, &SCodeBrowser::GetSourcesToggleImage)
									]
								]
							]
						]
					]

					// Collection View
					+ SSplitter::Slot()
					.Value(0.9f)
					[
						SNew(SBorder)
						.Visibility( this, &SCodeBrowser::GetCollectionViewVisibility )
						.Padding(FMargin(3))
						.BorderImage( FEditorStyle::GetBrush("ToolPanel.GroupBorder") )
						[
							SAssignNew(CollectionViewPtr, SCollectionView)
							.OnCollectionSelected(this, &SCodeBrowser::CollectionSelected)
							.AddMetaData<FTagMetaData>(FTagMetaData(TEXT("CodeBrowserCollections")))
							.AllowCollectionDrag(true)
							.AllowQuickAssetManagement(true)
						]
					]
				]
			]

			// Asset View
			+ SSplitter::Slot()
			.Value(0.75f)
			[
				SNew(SBorder)
				.Padding(FMargin(3))
				.BorderImage( FEditorStyle::GetBrush("ToolPanel.GroupBorder") )
				[
					SNew(SVerticalBox)

					// Search and commands
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(0, 0, 0, 2)
					[
						SNew(SHorizontalBox)
						//.Visibility_Lambda([this] { if (CHECK_ENGINE_IS_COURSEWARE_EDITOR == false) return EVisibility::Visible; return EVisibility::Collapsed; })

						// Expand/collapse sources button
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.Padding( 0, 0, 4, 0 )
						[
							SNew( SVerticalBox )
							.Visibility(( Config != nullptr ? Config->bUseSourcesView : true ) ? EVisibility::SelfHitTestInvisible : EVisibility::Collapsed)
							.AddMetaData<FTagMetaData>(FTagMetaData(TEXT("CodeBrowserSourcesToggle2")))
							+ SVerticalBox::Slot()
							.FillHeight( 1.0f )
							[
								SNew( SButton )
								.VAlign( EVerticalAlignment::VAlign_Center )
								.ButtonStyle( FEditorStyle::Get(), "ToggleButton" )
								.ToolTipText( LOCTEXT( "SourcesTreeToggleTooltip", "Show or hide the sources panel" ) )
								.ContentPadding( FMargin( 1, 0 ) )
								.ForegroundColor( FEditorStyle::GetSlateColor(DefaultForegroundName) )
								.OnClicked( this, &SCodeBrowser::SourcesViewExpandClicked )
								.Visibility( this, &SCodeBrowser::GetPathExpanderVisibility )
								[
									SNew( SImage )
									.Image( this, &SCodeBrowser::GetSourcesToggleImage )
								]
							]
						]

						// Filter
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew( SComboButton )
							.ComboButtonStyle( FEditorStyle::Get(), "GenericFilters.ComboButtonStyle" )
							.ForegroundColor(FLinearColor::White)
							.ContentPadding(0)
							.ToolTipText( LOCTEXT( "AddFilterToolTip", "Add an asset filter." ) )
							.OnGetMenuContent( this, &SCodeBrowser::MakeAddFilterMenu )
							.HasDownArrow( true )
							.ContentPadding( FMargin( 1, 0 ) )
							.AddMetaData<FTagMetaData>(FTagMetaData(TEXT("CodeBrowserFiltersCombo")))
							.Visibility( ( Config != nullptr ? Config->bCanShowFilters : true ) ? EVisibility::Visible : EVisibility::Collapsed )
							.ButtonContent()
							[
								SNew(SHorizontalBox)

								+ SHorizontalBox::Slot()
								.AutoWidth()
								[
									SNew(STextBlock)
									.TextStyle(FEditorStyle::Get(), "GenericFilters.TextStyle")
									.Font(FEditorStyle::Get().GetFontStyle("FontAwesome.9"))
									.Text(FText::FromString(FString(TEXT("\xf0b0"))) /*fa-filter*/)
								]

								+ SHorizontalBox::Slot()
								.AutoWidth()
								.Padding(2,0,0,0)
								[
									SNew(STextBlock)
									.TextStyle(FEditorStyle::Get(), "GenericFilters.TextStyle")
									.Text(LOCTEXT("Filters", "Filters"))
								]
							]
						]

						// Search
						+SHorizontalBox::Slot()
						.Padding(4, 1, 0, 0)
						.FillWidth(1.0f)
						[
							SAssignNew(SearchBoxPtr, SAssetSearchBox)
							.HintText( this, &SCodeBrowser::GetSearchAssetsHintText )
							.OnTextChanged( this, &SCodeBrowser::OnSearchBoxChanged )
							.OnTextCommitted( this, &SCodeBrowser::OnSearchBoxCommitted )
							.PossibleSuggestions( this, &SCodeBrowser::GetAssetSearchSuggestions )
							.DelayChangeNotificationsWhileTyping( true )
							.Visibility( ( Config != nullptr ? Config->bCanShowAssetSearch : true ) ? EVisibility::Visible : EVisibility::Collapsed )
							.AddMetaData<FTagMetaData>(FTagMetaData(TEXT("CodeBrowserSearchAssets")))
						]

						// Save Search
						+SHorizontalBox::Slot()
						.AutoWidth()
						.VAlign(VAlign_Center)
						.Padding(2.0f, 0.0f, 0.0f, 0.0f)
						[
							SNew(SButton)
							.ButtonStyle(FEditorStyle::Get(), "FlatButton")
							.ToolTipText(LOCTEXT("SaveSearchButtonTooltip", "Save the current search as a dynamic collection."))
							.IsEnabled(this, &SCodeBrowser::IsSaveSearchButtonEnabled)
							.OnClicked(this, &SCodeBrowser::OnSaveSearchButtonClicked)
							.ContentPadding( FMargin(1, 1) )
							.Visibility( ( Config != nullptr ? Config->bCanShowAssetSearch : true ) ? EVisibility::Visible : EVisibility::Collapsed )
							[
								SNew(STextBlock)
								.TextStyle(FEditorStyle::Get(), "GenericFilters.TextStyle")
								.Font(FEditorStyle::Get().GetFontStyle("FontAwesome.10"))
								.Text(FEditorFontGlyphs::Floppy_O)
							]
						]
					]

					// Filters
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SAssignNew(FilterListPtr, SFilterList)
						.OnFilterChanged(this, &SCodeBrowser::OnFilterChanged)
						.OnGetContextMenu(this, &SCodeBrowser::GetFilterContextMenu)
						.Visibility( ( Config != nullptr ? Config->bCanShowFilters : true ) ? EVisibility::Visible : EVisibility::Collapsed )
						.FrontendFilters(FrontendFilters)
						.AddMetaData<FTagMetaData>(FTagMetaData(TEXT("CodeBrowserFilters")))
					]

					// Assets
					+ SVerticalBox::Slot()
					.FillHeight( 1.0f )
					.Padding( 0 )
					[
						SAssignNew(AssetViewPtr, SAssetView)
						.ThumbnailLabel( Config != nullptr ? Config->ThumbnailLabel : EThumbnailLabel::ClassName )
						.ThumbnailScale( Config != nullptr ? Config->ThumbnailScale : 0.18f )
						.InitialViewType( Config != nullptr ? Config->InitialAssetViewType : EAssetViewType::Tile )
						.ShowBottomToolbar( Config != nullptr ? Config->bShowBottomToolbar : true )
						.OnPathSelected(this, &SCodeBrowser::FolderEntered)
						.OnAssetSelected(this, &SCodeBrowser::OnAssetSelectionChanged)
						.OnAssetsActivated(this, &SCodeBrowser::OnAssetsActivated)
						.OnGetAssetContextMenu(this, &SCodeBrowser::OnGetAssetContextMenu)
						.OnGetFolderContextMenu(this, &SCodeBrowser::GetFolderContextMenu, false)
						.OnGetPathContextMenuExtender(this, &SCodeBrowser::GetPathContextMenuExtender)
						.OnFindInAssetTreeRequested(this, &SCodeBrowser::OnFindInAssetTreeRequested)
						.OnAssetRenameCommitted(this, &SCodeBrowser::OnAssetRenameCommitted)
						.AreRealTimeThumbnailsAllowed(this, &SCodeBrowser::IsHovered)
						.FrontendFilters(FrontendFilters)
						.HighlightedText(this, &SCodeBrowser::GetHighlightedText)
						.AllowThumbnailEditMode(true)
						.AllowThumbnailHintLabel(false)
						.CanShowFolders(Config != nullptr ? Config->bCanShowFolders : true)
						.CanShowClasses(Config != nullptr ? Config->bCanShowClasses : true)
						.CanShowRealTimeThumbnails( Config != nullptr ? Config->bCanShowRealTimeThumbnails : true)
						.CanShowDevelopersFolder( Config != nullptr ? Config->bCanShowDevelopersFolder : true)
						.CanShowCollections(true)
						.CanShowFavorites(true)
						.AddMetaData<FTagMetaData>(FTagMetaData(TEXT("CodeBrowserAssets")))
						.OnSearchOptionsChanged(this, &SCodeBrowser::HandleAssetViewSearchOptionsChanged)
					]
				]
			]
		]
	];

	AssetContextMenu = MakeShareable(new FAssetContextMenu(AssetViewPtr));
	AssetContextMenu->BindCommands(Commands);
	AssetContextMenu->SetOnFindInAssetTreeRequested( FOnFindInAssetTreeRequested::CreateSP(this, &SCodeBrowser::OnFindInAssetTreeRequested) );
	AssetContextMenu->SetOnRenameRequested( FAssetContextMenu::FOnRenameRequested::CreateSP(this, &SCodeBrowser::OnRenameRequested) );
	AssetContextMenu->SetOnRenameFolderRequested( FAssetContextMenu::FOnRenameFolderRequested::CreateSP(this, &SCodeBrowser::OnRenameFolderRequested) );
	AssetContextMenu->SetOnDuplicateRequested( FAssetContextMenu::FOnDuplicateRequested::CreateSP(this, &SCodeBrowser::OnDuplicateRequested) );
	AssetContextMenu->SetOnAssetViewRefreshRequested( FAssetContextMenu::FOnAssetViewRefreshRequested::CreateSP( this, &SCodeBrowser::OnAssetViewRefreshRequested) );
	FavoritePathViewPtr->SetTreeTitle(LOCTEXT("Favorites", "Favorites"));
	if( Config != nullptr && Config->SelectedCollectionName.Name != NAME_None )
	{
		// Select the specified collection by default
		FSourcesData DefaultSourcesData( Config->SelectedCollectionName );
		TArray<FString> SelectedPaths;
		AssetViewPtr->SetSourcesData( DefaultSourcesData );
	}
	else
	{
		// Select /Game by default
		FSourcesData DefaultSourcesData(FName("/Game"));
		TArray<FString> SelectedPaths;
		TArray<FString> SelectedFavoritePaths;
		SelectedPaths.Add(TEXT("/Game"));
		PathViewPtr->SetSelectedPaths(SelectedPaths);
		AssetViewPtr->SetSourcesData(DefaultSourcesData);
		FavoritePathViewPtr->SetSelectedPaths(SelectedFavoritePaths);
	}

	//Bind the path view filtering to the favorite path view search bar
	FavoritePathViewPtr->OnFavoriteSearchChanged.BindSP(PathViewPtr.Get(), &SPathView::OnAssetTreeSearchBoxChanged);
	FavoritePathViewPtr->OnFavoriteSearchCommitted.BindSP(PathViewPtr.Get(), &SPathView::OnAssetTreeSearchBoxCommitted);

	// Bind the favorites menu to update after folder changes in the path or asset view
	//PathViewPtr->OnFolderPathChanged.BindSP(FavoritePathViewPtr.Get(), &SFavoritePathView::FixupFavoritesFromExternalChange);
	PathViewPtr->OnFolderPathChanged.BindLambda([this](const TArray<struct FMovedContentFolder>& MovedFolders)
	{
		FCodeBrowserModule& CodeBrowserModule = FModuleManager::GetModuleChecked<FCodeBrowserModule>(TEXT("CodeBrowser"));
		FCodeBrowserModule::FOnRenameFolder& RenameFolderDelegate = CodeBrowserModule.GetOnRenameFolder();
		if (RenameFolderDelegate.IsBound())
		{
			RenameFolderDelegate.Broadcast(MovedFolders);
		}
		FavoritePathViewPtr.Get()->FixupFavoritesFromExternalChange(MovedFolders);
	});
	AssetViewPtr->OnFolderPathChanged.BindSP(FavoritePathViewPtr.Get(), &SFavoritePathView::FixupFavoritesFromExternalChange);

	FCodeBrowserModule& CodeBrowserModule = FModuleManager::Get().LoadModuleChecked<FCodeBrowserModule>("CodeBrowser");
	CodeBrowserModule.GetOnRequestNewFolder().AddLambda([&]() 
	{ 
		TArray<FString> SelectedPaths=PathViewPtr->GetSelectedPaths();
		//GetSelectedFolders(SelectedPaths);
		FOnCreateNewFolder NewFolderDelegate = FOnCreateNewFolder::CreateSP(PathViewPtr.Get(), &SPathView::OnCreateNewFolder);
		CreateNewFolder(SelectedPaths.Num() > 0 ? SelectedPaths[0] : FString(), NewFolderDelegate);
	});


	// Set the initial history data
	HistoryManager.AddHistoryData();

	// Load settings if they were specified
	this->InstanceName = InInstanceName;
	LoadSettings(InInstanceName);

	if( Config != nullptr )
	{
		// Make sure the sources view is initially visible if we were asked to show it
		if( ( bSourcesViewExpanded && ( !Config->bExpandSourcesView || !Config->bUseSourcesView ) ) ||
			( !bSourcesViewExpanded && Config->bExpandSourcesView && Config->bUseSourcesView ) )
		{
			SourcesViewExpandClicked();
		}
	}
	else
	{
		// in case we do not have a config, see what the global default settings are for the Sources Panel
		if (!bSourcesViewExpanded && GetDefault<UCodeBrowserSettings>()->bOpenSourcesPanelByDefault)
		{
			SourcesViewExpandClicked();
		}
	}

	// Bindings to manage history when items are deleted
	FCollectionManagerModule& CollectionManagerModule = FCollectionManagerModule::GetModule();
	CollectionManagerModule.Get().OnCollectionRenamed().AddSP(this, &SCodeBrowser::HandleCollectionRenamed);
	CollectionManagerModule.Get().OnCollectionDestroyed().AddSP(this, &SCodeBrowser::HandleCollectionRemoved);
	CollectionManagerModule.Get().OnCollectionUpdated().AddSP(this, &SCodeBrowser::HandleCollectionUpdated);

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	AssetRegistryModule.Get().OnPathRemoved().AddSP(this, &SCodeBrowser::HandlePathRemoved);

	// We want to be able to search the feature packs in the super search so we need the module loaded 
	IAddContentDialogModule& AddContentDialogModule = FModuleManager::LoadModuleChecked<IAddContentDialogModule>("AddContentDialog");

	// Update the breadcrumb trail path
	UpdatePath();
	OpenSourcesViewExpanded();
}

void SCodeBrowser::OpenSourcesViewExpanded()
{
	bSourcesViewExpanded = true;

	FCodeBrowserModule& CodeBrowserModule = FModuleManager::GetModuleChecked<FCodeBrowserModule>( TEXT("CodeBrowser") );
	FCodeBrowserModule::FOnSourcesViewChanged& SourcesViewChangedDelegate = CodeBrowserModule.GetOnSourcesViewChanged();
	if(SourcesViewChangedDelegate.IsBound())
	{
		SourcesViewChangedDelegate.Broadcast(true);
	}
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SCodeBrowser::BindCommands()
{
	Commands = TSharedPtr< FUICommandList >(new FUICommandList);

	Commands->MapAction(FGenericCommands::Get().Rename, FUIAction(
		FExecuteAction::CreateSP(this, &SCodeBrowser::HandleRenameCommand),
		FCanExecuteAction::CreateSP(this, &SCodeBrowser::HandleRenameCommandCanExecute)
	));

	Commands->MapAction(FGenericCommands::Get().Delete, FUIAction(
		FExecuteAction::CreateSP(this, &SCodeBrowser::HandleDeleteCommandExecute),
		FCanExecuteAction::CreateSP(this, &SCodeBrowser::HandleDeleteCommandCanExecute)
	));

	Commands->MapAction(FCodeBrowserCommands::Get().OpenAssetsOrFolders, FUIAction(
		FExecuteAction::CreateSP(this, &SCodeBrowser::HandleOpenAssetsOrFoldersCommandExecute)
	));

	Commands->MapAction(FCodeBrowserCommands::Get().PreviewAssets, FUIAction(
		FExecuteAction::CreateSP(this, &SCodeBrowser::HandlePreviewAssetsCommandExecute)
	));

	Commands->MapAction(FCodeBrowserCommands::Get().CreateNewFolder, FUIAction(
		FExecuteAction::CreateSP(this, &SCodeBrowser::HandleCreateNewFolderCommandExecute)
	));

	Commands->MapAction(FCodeBrowserCommands::Get().DirectoryUp, FUIAction(
		FExecuteAction::CreateSP(this, &SCodeBrowser::HandleDirectoryUpCommandExecute)
	));

	Commands->MapAction(FCodeBrowserCommands::Get().SaveSelectedAsset, FUIAction(
		FExecuteAction::CreateSP(this, &SCodeBrowser::HandleSaveAssetCommand),
		FCanExecuteAction::CreateSP(this, &SCodeBrowser::HandleSaveAssetCommandCanExecute)
	));

	Commands->MapAction(FCodeBrowserCommands::Get().SaveAllCurrentFolder, FUIAction(
		FExecuteAction::CreateSP(this, &SCodeBrowser::HandleSaveAllCurrentFolderCommand)
	));

	Commands->MapAction(FCodeBrowserCommands::Get().ResaveAllCurrentFolder, FUIAction(
		FExecuteAction::CreateSP(this, &SCodeBrowser::HandleResaveAllCurrentFolderCommand)
	));

	// Allow extenders to add commands
	FCodeBrowserModule& CodeBrowserModule = FModuleManager::GetModuleChecked<FCodeBrowserModule>(TEXT("CodeBrowser"));
	TArray<FCodeBrowserCommandExtender> CommmandExtenderDelegates = CodeBrowserModule.GetAllCodeBrowserCommandExtenders();

	for (int32 i = 0; i < CommmandExtenderDelegates.Num(); ++i)
	{
		if (CommmandExtenderDelegates[i].IsBound())
		{
			CommmandExtenderDelegates[i].Execute(Commands.ToSharedRef(), FOnCodeBrowserGetSelection::CreateSP(this, &SCodeBrowser::GetSelectionState));
		}
	}
}

EVisibility SCodeBrowser::GetCollectionViewVisibility() const
{
	return bAlwaysShowCollections ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility SCodeBrowser::GetFavoriteFolderVisibility() const
{
	return EVisibility::Visible;
}

EVisibility SCodeBrowser::GetAlternateSearchBarVisibility() const
{
	return EVisibility::Visible;
}

void SCodeBrowser::ToggleFolderFavorite(const TArray<FString>& FolderPaths)
{
	bool bAddedFavorite = false;
	for (FString FolderPath : FolderPaths)
	{
		if (CodeBrowserUtils::IsFavoriteFolder(FolderPath))
		{
			CodeBrowserUtils::RemoveFavoriteFolder(FolderPath, false);
		}
		else
		{
			CodeBrowserUtils::AddFavoriteFolder(FolderPath, false);
			bAddedFavorite = true;
		}
	}
	GConfig->Flush(false, GEditorPerProjectIni);
	FavoritePathViewPtr->Populate();
	if(bAddedFavorite)
	{	
		FavoritePathViewPtr->SetSelectedPaths(FolderPaths);
	}
}

void SCodeBrowser::HandleAssetViewSearchOptionsChanged()
{
	TextFilter->SetIncludeClassName(AssetViewPtr->IsIncludingClassNames());
	TextFilter->SetIncludeAssetPath(AssetViewPtr->IsIncludingAssetPaths());
	TextFilter->SetIncludeCollectionNames(AssetViewPtr->IsIncludingCollectionNames());
}

FText SCodeBrowser::GetHighlightedText() const
{
	return TextFilter->GetRawFilterText();
}

void SCodeBrowser::CreateNewAsset(const FString& DefaultAssetName, const FString& PackagePath, UClass* AssetClass, UFactory* Factory)
{
	AssetViewPtr->CreateNewAsset(DefaultAssetName, PackagePath, AssetClass, Factory);
}

bool SCodeBrowser::IsImportEnabled() const
{
	const FSourcesData& SourcesData = AssetViewPtr->GetSourcesData();
	return SourcesData.PackagePaths.Num() == 1 && !CodeBrowserUtils::IsClassPath(SourcesData.PackagePaths[0].ToString());
}

FText SCodeBrowser::GetImportTooltipText() const
{
	const FSourcesData& SourcesData = AssetViewPtr->GetSourcesData();

	if ( SourcesData.PackagePaths.Num() == 1 )
	{
		const FString CurrentPath = SourcesData.PackagePaths[0].ToString();
		if ( CodeBrowserUtils::IsClassPath( CurrentPath ) )
		{
			return LOCTEXT( "ImportAssetToolTip_InvalidClassPath", "Cannot import assets to class paths." );
		}
		else
		{
			return FText::Format( LOCTEXT( "ImportAssetToolTip", "Import to {0}..." ), FText::FromString( CurrentPath ) );
		}
	}
	else if ( SourcesData.PackagePaths.Num() > 1 )
	{
		return LOCTEXT( "ImportAssetToolTip_MultiplePaths", "Cannot import assets to multiple paths." );
	}
	
	return LOCTEXT( "ImportAssetToolTip_NoPath", "No path is selected as an import target." );
}

FReply SCodeBrowser::HandleImportClicked()
{
	ImportAsset( GetCurrentPath() );
	return FReply::Handled();
}

void SCodeBrowser::ImportAsset( const FString& InPath )
{
	if ( ensure( !InPath.IsEmpty() ) )
	{
		FAssetToolsModule& AssetToolsModule = FModuleManager::Get().LoadModuleChecked<FAssetToolsModule>( "AssetTools" );
		AssetToolsModule.Get().ImportAssetsWithDialog( InPath );
	}
}

void SCodeBrowser::PrepareToSync( const TArray<FFileData>& AssetDataList, const TArray<FString>& FolderPaths, const bool bDisableFiltersThatHideAssets )
{
	// Check to see if any of the assets require certain folders to be visible
	bool bDisplayDev = true;
	bool bDisplayEngine = true;
	bool bDisplayPlugins = true;
	bool bDisplayLocalized = true;
	if ( !bDisplayDev || !bDisplayEngine || !bDisplayPlugins || !bDisplayLocalized )
	{
		TSet<FString> PackagePaths = TSet<FString>(FolderPaths);
		for (const FFileData& AssetData : AssetDataList)
		{
			FString PackagePath;
			if (AssetData.AssetClass == NAME_Class)
			{
				// Classes are found in the /Classes_ roots
				TSharedRef<FNativeClassHierarchy> NativeClassHierarchy = FCodeBrowserSingleton::Get().GetNativeClassHierarchy();
				NativeClassHierarchy->GetClassPath(Cast<UClass>(AssetData.GetAsset()), PackagePath, false/*bIncludeClassName*/);
			}
			else
			{
				// All other assets are found by their package path
				PackagePath = AssetData.PackagePath.ToString();
			}

			PackagePaths.Add(PackagePath);
		}

		bool bRepopulate = false;
		for (const FString& PackagePath : PackagePaths)
		{
			const CodeBrowserUtils::ECBFolderCategory FolderCategory = CodeBrowserUtils::GetFolderCategory( PackagePath );
			if ( !bDisplayDev && FolderCategory == CodeBrowserUtils::ECBFolderCategory::DeveloperContent )
			{
				bDisplayDev = true;
				bRepopulate = true;
			}
			else if ( !bDisplayEngine && (FolderCategory == CodeBrowserUtils::ECBFolderCategory::EngineContent || FolderCategory == CodeBrowserUtils::ECBFolderCategory::EngineClasses) )
			{
				bDisplayEngine = true;
				bRepopulate = true;
			}
			else if ( !bDisplayPlugins && (FolderCategory == CodeBrowserUtils::ECBFolderCategory::PluginContent || FolderCategory == CodeBrowserUtils::ECBFolderCategory::PluginClasses) )
			{
				bDisplayPlugins = true;
				bRepopulate = true;
			}

			if (!bDisplayLocalized && CodeBrowserUtils::IsLocalizationFolder(PackagePath))
			{
				bDisplayLocalized = true;
				bRepopulate = true;
			}

			if (bDisplayDev && bDisplayEngine && bDisplayPlugins && bDisplayLocalized)
			{
				break;
			}
		}

		// If we have auto-enabled any flags, force a refresh
		if ( bRepopulate )
		{
			PathViewPtr->Populate();
			FavoritePathViewPtr->Populate();
		}
	}

	if ( bDisableFiltersThatHideAssets )
	{
		// Disable the filter categories
		FilterListPtr->DisableFiltersThatHideAssets(AssetDataList);
	}

	// Disable the filter search (reset the filter, then clear the search text)
	// Note: we have to remove the filter immediately, we can't wait for OnSearchBoxChanged to hit
	SetSearchBoxText(FText::GetEmpty());
	SearchBoxPtr->SetText(FText::GetEmpty());
	SearchBoxPtr->SetError(FText::GetEmpty());
}

void SCodeBrowser::SyncToAssets( const TArray<FFileData>& AssetDataList, const bool bAllowImplicitSync, const bool bDisableFiltersThatHideAssets )
{
	PrepareToSync(AssetDataList, TArray<FString>(), bDisableFiltersThatHideAssets);

	// Tell the sources view first so the asset view will be up to date by the time we request the sync
	PathViewPtr->SyncToAssets(AssetDataList, bAllowImplicitSync);
	FavoritePathViewPtr->SyncToAssets(AssetDataList, bAllowImplicitSync);
	AssetViewPtr->SyncToAssets(AssetDataList);
}

void SCodeBrowser::SyncToFolders( const TArray<FString>& FolderList, const bool bAllowImplicitSync )
{
	PrepareToSync(TArray<FFileData>(), FolderList, false);

	// Tell the sources view first so the asset view will be up to date by the time we request the sync
	PathViewPtr->SyncToFolders(FolderList, bAllowImplicitSync);
	FavoritePathViewPtr->SyncToFolders(FolderList, bAllowImplicitSync);
	AssetViewPtr->SyncToFolders(FolderList);
}

void SCodeBrowser::SyncTo( const FCodeBrowserSelection& ItemSelection, const bool bAllowImplicitSync, const bool bDisableFiltersThatHideAssets )
{
	PrepareToSync(ItemSelection.SelectedAssets, ItemSelection.SelectedFolders, bDisableFiltersThatHideAssets);

	// Tell the sources view first so the asset view will be up to date by the time we request the sync
	PathViewPtr->SyncTo(ItemSelection, bAllowImplicitSync);
	FavoritePathViewPtr->SyncTo(ItemSelection, bAllowImplicitSync);
	AssetViewPtr->SyncTo(ItemSelection);
}

void SCodeBrowser::SetIsPrimaryCodeBrowser(bool NewIsPrimary)
{
	if (!CanSetAsPrimaryCodeBrowser()) 
	{
		return;
	}

	bIsPrimaryBrowser = NewIsPrimary;

	if ( bIsPrimaryBrowser )
	{
		SyncGlobalSelectionSet();
	}
	else
	{
		USelection* EditorSelection = GEditor->GetSelectedObjects();
		if ( ensure( EditorSelection != NULL ) )
		{
			EditorSelection->DeselectAll();
		}
	}
}

bool SCodeBrowser::CanSetAsPrimaryCodeBrowser() const
{
	return bCanSetAsPrimaryBrowser;
}

TSharedPtr<FTabManager> SCodeBrowser::GetTabManager() const
{
	if ( ContainingTab.IsValid() )
	{
		return ContainingTab.Pin()->GetTabManager();
	}

	return NULL;
}

void SCodeBrowser::LoadSelectedObjectsIfNeeded()
{
	// Get the selected assets in the asset view
	const TArray<FFileData>& SelectedAssets = AssetViewPtr->GetSelectedAssets();

	// Load every asset that isn't already in memory
	for ( auto AssetIt = SelectedAssets.CreateConstIterator(); AssetIt; ++AssetIt )
	{
		const FFileData& AssetData = *AssetIt;
		const bool bShowProgressDialog = (!AssetData.IsAssetLoaded() && FEditorFileUtils::IsMapPackageAsset(AssetData.ObjectPath.ToString()));
		GWarn->BeginSlowTask(LOCTEXT("LoadingObjects", "Loading Objects..."), bShowProgressDialog);

		(*AssetIt).GetAsset();

		GWarn->EndSlowTask();
	}

	// Sync the global selection set if we are the primary browser
	if ( bIsPrimaryBrowser )
	{
		SyncGlobalSelectionSet();
	}
}

void SCodeBrowser::GetSelectedAssets(TArray<FFileData>& SelectedAssets)
{
	// Make sure the asset data is up to date
	AssetViewPtr->ProcessRecentlyLoadedOrChangedAssets();

	SelectedAssets = AssetViewPtr->GetSelectedAssets();
}

void SCodeBrowser::GetSelectedFolders(TArray<FString>& SelectedFolders)
{
	// Make sure the asset data is up to date
	AssetViewPtr->ProcessRecentlyLoadedOrChangedAssets();

	SelectedFolders = AssetViewPtr->GetSelectedFolders();
}

TArray<FString> SCodeBrowser::GetSelectedPathViewFolders()
{
	check(PathViewPtr.IsValid());
	return PathViewPtr->GetSelectedPaths();
}

void SCodeBrowser::SaveSettings() const
{
	const FString& SettingsString = InstanceName.ToString();

	GConfig->SetBool(*SettingsIniSection, *(SettingsString + TEXT(".SourcesExpanded")), bSourcesViewExpanded, GEditorPerProjectIni);
	GConfig->SetBool(*SettingsIniSection, *(SettingsString + TEXT(".Locked")), bIsLocked, GEditorPerProjectIni);

	for(int32 SlotIndex = 0; SlotIndex < PathAssetSplitterPtr->GetChildren()->Num(); SlotIndex++)
	{
		float SplitterSize = PathAssetSplitterPtr->SlotAt(SlotIndex).SizeValue.Get();
		GConfig->SetFloat(*SettingsIniSection, *(SettingsString + FString::Printf(TEXT(".VerticalSplitter.SlotSize%d"), SlotIndex)), SplitterSize, GEditorPerProjectIni);
	}
	
	for(int32 SlotIndex = 0; SlotIndex < PathCollectionSplitterPtr->GetChildren()->Num(); SlotIndex++)
	{
		float SplitterSize = PathCollectionSplitterPtr->SlotAt(SlotIndex).SizeValue.Get();
		GConfig->SetFloat(*SettingsIniSection, *(SettingsString + FString::Printf(TEXT(".HorizontalSplitter.SlotSize%d"), SlotIndex)), SplitterSize, GEditorPerProjectIni);
	}

	for (int32 SlotIndex = 0; SlotIndex < PathFavoriteSplitterPtr->GetChildren()->Num(); SlotIndex++)
	{
		float SplitterSize = PathFavoriteSplitterPtr->SlotAt(SlotIndex).SizeValue.Get();
		GConfig->SetFloat(*SettingsIniSection, *(SettingsString + FString::Printf(TEXT(".FavoriteSplitter.SlotSize%d"), SlotIndex)), SplitterSize, GEditorPerProjectIni);
	}

	// Save all our data using the settings string as a key in the user settings ini
	FilterListPtr->SaveSettings(GEditorPerProjectIni, SettingsIniSection, SettingsString);
	PathViewPtr->SaveSettings(GEditorPerProjectIni, SettingsIniSection, SettingsString);
	FavoritePathViewPtr->SaveSettings(GEditorPerProjectIni, SettingsIniSection, SettingsString + TEXT(".Favorites"));
	CollectionViewPtr->SaveSettings(GEditorPerProjectIni, SettingsIniSection, SettingsString);
	AssetViewPtr->SaveSettings(GEditorPerProjectIni, SettingsIniSection, SettingsString);
}

const FName SCodeBrowser::GetInstanceName() const
{
	return InstanceName;
}

bool SCodeBrowser::IsLocked() const
{
	return bIsLocked;
}

void SCodeBrowser::SetKeyboardFocusOnSearch() const
{
	// Focus on the search box
	FSlateApplication::Get().SetKeyboardFocus( SearchBoxPtr, EFocusCause::SetDirectly );
}

FReply SCodeBrowser::OnKeyDown( const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent )
{
	if( Commands->ProcessCommandBindings( InKeyEvent ) )
	{
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

FReply SCodeBrowser::OnPreviewMouseButtonDown( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent )
{
	// Clicking in a content browser will shift it to be the primary browser
	FCodeBrowserSingleton::Get().SetPrimaryCodeBrowser(SharedThis(this));

	return FReply::Unhandled();
}

FReply SCodeBrowser::OnMouseButtonDown( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent )
{
	// Mouse back and forward buttons traverse history
	if ( MouseEvent.GetEffectingButton() == EKeys::ThumbMouseButton)
	{
		HistoryManager.GoBack();
		return FReply::Handled();
	}
	else if ( MouseEvent.GetEffectingButton() == EKeys::ThumbMouseButton2)
	{
		HistoryManager.GoForward();
		return FReply::Handled();
	}

	return FReply::Unhandled();
}

FReply SCodeBrowser::OnMouseButtonDoubleClick( const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent )
{
	// Mouse back and forward buttons traverse history
	if ( InMouseEvent.GetEffectingButton() == EKeys::ThumbMouseButton)
	{
		HistoryManager.GoBack();
		return FReply::Handled();
	}
	else if ( InMouseEvent.GetEffectingButton() == EKeys::ThumbMouseButton2)
	{
		HistoryManager.GoForward();
		return FReply::Handled();
	}

	return FReply::Unhandled();
}

void SCodeBrowser::OnContainingTabSavingVisualState() const
{
	SaveSettings();
}

void SCodeBrowser::OnContainingTabClosed(TSharedRef<SDockTab> DockTab)
{
	FCodeBrowserSingleton::Get().CodeBrowserClosed( SharedThis(this) );
}

void SCodeBrowser::OnContainingTabActivated(TSharedRef<SDockTab> DockTab, ETabActivationCause InActivationCause)
{
	if(InActivationCause == ETabActivationCause::UserClickedOnTab)
	{
		FCodeBrowserSingleton::Get().SetPrimaryCodeBrowser(SharedThis(this));
	}
}

void SCodeBrowser::LoadSettings(const FName& InInstanceName)
{
	FString SettingsString = InInstanceName.ToString();

	// Test to see if we should load legacy settings from a previous instance name
	// First make sure there aren't any existing settings with the given instance name
	bool TestBool;
	if ( !GConfig->GetBool(*SettingsIniSection, *(SettingsString + TEXT(".SourcesExpanded")), TestBool, GEditorPerProjectIni) )
	{
		// If there were not any settings and we are Content Browser 1, see if we have any settings under the legacy name "LevelEditorCodeBrowser"
		if ( InInstanceName.ToString() == TEXT("CodeBrowserTab1") && GConfig->GetBool(*SettingsIniSection, TEXT("LevelEditorCodeBrowser.SourcesExpanded"), TestBool, GEditorPerProjectIni) )
		{
			// We have found some legacy settings with the old ID, use them. These settings will be saved out to the new id later
			SettingsString = TEXT("LevelEditorCodeBrowser");
		}
		// else see if we are Content Browser 2, and see if we have any settings under the legacy name "MajorCodeBrowserTab"
		else if ( InInstanceName.ToString() == TEXT("CodeBrowserTab2") && GConfig->GetBool(*SettingsIniSection, TEXT("MajorCodeBrowserTab.SourcesExpanded"), TestBool, GEditorPerProjectIni) )
		{
			// We have found some legacy settings with the old ID, use them. These settings will be saved out to the new id later
			SettingsString = TEXT("MajorCodeBrowserTab");
		}
	}

	// Now that we have determined the appropriate settings string, actually load the settings
	GConfig->GetBool(*SettingsIniSection, *(SettingsString + TEXT(".SourcesExpanded")), bSourcesViewExpanded, GEditorPerProjectIni);
	GConfig->GetBool(*SettingsIniSection, *(SettingsString + TEXT(".Locked")), bIsLocked, GEditorPerProjectIni);

	for(int32 SlotIndex = 0; SlotIndex < PathAssetSplitterPtr->GetChildren()->Num(); SlotIndex++)
	{
		float SplitterSize = PathAssetSplitterPtr->SlotAt(SlotIndex).SizeValue.Get();
		GConfig->GetFloat(*SettingsIniSection, *(SettingsString + FString::Printf(TEXT(".VerticalSplitter.SlotSize%d"), SlotIndex)), SplitterSize, GEditorPerProjectIni);
		PathAssetSplitterPtr->SlotAt(SlotIndex).SizeValue = SplitterSize;
	}
	
	for(int32 SlotIndex = 0; SlotIndex < PathCollectionSplitterPtr->GetChildren()->Num(); SlotIndex++)
	{
		float SplitterSize = PathCollectionSplitterPtr->SlotAt(SlotIndex).SizeValue.Get();
		GConfig->GetFloat(*SettingsIniSection, *(SettingsString + FString::Printf(TEXT(".HorizontalSplitter.SlotSize%d"), SlotIndex)), SplitterSize, GEditorPerProjectIni);
		PathCollectionSplitterPtr->SlotAt(SlotIndex).SizeValue = SplitterSize;
	}

	for (int32 SlotIndex = 0; SlotIndex < PathFavoriteSplitterPtr->GetChildren()->Num(); SlotIndex++)
	{
		float SplitterSize = PathFavoriteSplitterPtr->SlotAt(SlotIndex).SizeValue.Get();
		GConfig->GetFloat(*SettingsIniSection, *(SettingsString + FString::Printf(TEXT(".FavoriteSplitter.SlotSize%d"), SlotIndex)), SplitterSize, GEditorPerProjectIni);
		PathFavoriteSplitterPtr->SlotAt(SlotIndex).SizeValue = SplitterSize;
	}

	// Save all our data using the settings string as a key in the user settings ini
	FilterListPtr->LoadSettings(GEditorPerProjectIni, SettingsIniSection, SettingsString);
	PathViewPtr->LoadSettings(GEditorPerProjectIni, SettingsIniSection, SettingsString);
	FavoritePathViewPtr->LoadSettings(GEditorPerProjectIni, SettingsIniSection, SettingsString + TEXT(".Favorites"));
	CollectionViewPtr->LoadSettings(GEditorPerProjectIni, SettingsIniSection, SettingsString);
	AssetViewPtr->LoadSettings(GEditorPerProjectIni, SettingsIniSection, SettingsString);
}

void SCodeBrowser::SourcesChanged(const TArray<FString>& SelectedPaths, const TArray<FCollectionNameType>& SelectedCollections)
{
	FString NewSource = SelectedPaths.Num() > 0 ? SelectedPaths[0] : (SelectedCollections.Num() > 0 ? SelectedCollections[0].Name.ToString() : TEXT("None"));
	UE_LOG(LogCodeBrowser, VeryVerbose, TEXT("The content browser source was changed by the sources view to '%s'"), *NewSource);

	FSourcesData SourcesData;
	{
		TArray<FName> SelectedPathNames;
		SelectedPathNames.Reserve(SelectedPaths.Num());
		for (const FString& SelectedPath : SelectedPaths)
		{
			SelectedPathNames.Add(FName(*SelectedPath));
		}
		SourcesData = FSourcesData(MoveTemp(SelectedPathNames), SelectedCollections);
	}

	// A dynamic collection should apply its search query to the CB search, so we need to stash the current search so that we can restore it again later
	if (SourcesData.IsDynamicCollection())
	{
		// Only stash the user search term once in case we're switching between dynamic collections
		if (!StashedSearchBoxText.IsSet())
		{
			StashedSearchBoxText = TextFilter->GetRawFilterText();
		}

		FCollectionManagerModule& CollectionManagerModule = FCollectionManagerModule::GetModule();

		const FCollectionNameType& DynamicCollection = SourcesData.Collections[0];

		FString DynamicQueryString;
		CollectionManagerModule.Get().GetDynamicQueryText(DynamicCollection.Name, DynamicCollection.Type, DynamicQueryString);

		const FText DynamicQueryText = FText::FromString(DynamicQueryString);
		SetSearchBoxText(DynamicQueryText);
		SearchBoxPtr->SetText(DynamicQueryText);
	}
	else if (StashedSearchBoxText.IsSet())
	{
		// Restore the stashed search term
		const FText StashedText = StashedSearchBoxText.GetValue();
		StashedSearchBoxText.Reset();

		SetSearchBoxText(StashedText);
		SearchBoxPtr->SetText(StashedText);
	}

	if (!AssetViewPtr->GetSourcesData().IsEmpty())
	{
		// Update the current history data to preserve selection if there is a valid SourcesData
		HistoryManager.UpdateHistoryData();
	}

	// Change the filter for the asset view
	AssetViewPtr->SetSourcesData(SourcesData);

	// Add a new history data now that the source has changed
	HistoryManager.AddHistoryData();

	// Update the breadcrumb trail path
	UpdatePath();
}

void SCodeBrowser::FolderEntered(const FString& FolderPath)
{
	// Have we entered a sub-collection folder?
	FName CollectionName;
	ECollectionShareType::Type CollectionFolderShareType = ECollectionShareType::CST_All;
	if (CodeBrowserUtils::IsCollectionPath(FolderPath, &CollectionName, &CollectionFolderShareType))
	{
		const FCollectionNameType SelectedCollection(CollectionName, CollectionFolderShareType);

		TArray<FCollectionNameType> Collections;
		Collections.Add(SelectedCollection);
		CollectionViewPtr->SetSelectedCollections(Collections);

		CollectionSelected(SelectedCollection);
	}
	else
	{
		// set the path view to the incoming path
		TArray<FString> SelectedPaths;
		SelectedPaths.Add(FolderPath);
		PathViewPtr->SetSelectedPaths(SelectedPaths);

		PathSelected(SelectedPaths[0]);
	}
}

void SCodeBrowser::PathSelected(const FString& FolderPath)
{
	// You may not select both collections and paths
	CollectionViewPtr->ClearSelection();

	TArray<FString> SelectedPaths = PathViewPtr->GetSelectedPaths();
	// Selecting a folder shows it in the favorite list also
	FavoritePathViewPtr->SetSelectedPaths(SelectedPaths);
	TArray<FCollectionNameType> SelectedCollections;
	SourcesChanged(SelectedPaths, SelectedCollections);

	// Notify 'asset path changed' delegate
	FCodeBrowserModule& CodeBrowserModule = FModuleManager::GetModuleChecked<FCodeBrowserModule>( TEXT("CodeBrowser") );
	FCodeBrowserModule::FOnAssetPathChanged& PathChangedDelegate = CodeBrowserModule.GetOnAssetPathChanged();
	if(PathChangedDelegate.IsBound())
	{
		PathChangedDelegate.Broadcast(FolderPath);
	}

	// Update the context menu's selected paths list
	PathContextMenu->SetSelectedPaths(SelectedPaths);
}

void SCodeBrowser::FavoritePathSelected(const FString& FolderPath)
{
	// You may not select both collections and paths
	CollectionViewPtr->ClearSelection();

	TArray<FString> SelectedPaths = FavoritePathViewPtr->GetSelectedPaths();
	// Selecting a favorite shows it in the main list also
	PathViewPtr->SetSelectedPaths(SelectedPaths);
	TArray<FCollectionNameType> SelectedCollections;
	SourcesChanged(SelectedPaths, SelectedCollections);

	// Notify 'asset path changed' delegate
	FCodeBrowserModule& CodeBrowserModule = FModuleManager::GetModuleChecked<FCodeBrowserModule>(TEXT("CodeBrowser"));
	FCodeBrowserModule::FOnAssetPathChanged& PathChangedDelegate = CodeBrowserModule.GetOnAssetPathChanged();
	if (PathChangedDelegate.IsBound())
	{
		PathChangedDelegate.Broadcast(FolderPath);
	}

	// Update the context menu's selected paths list
	PathContextMenu->SetSelectedPaths(SelectedPaths);
}

TSharedRef<FExtender> SCodeBrowser::GetPathContextMenuExtender(const TArray<FString>& InSelectedPaths) const
{
	return PathContextMenu->MakePathViewContextMenuExtender(InSelectedPaths);
}

void SCodeBrowser::CollectionSelected(const FCollectionNameType& SelectedCollection)
{
	// You may not select both collections and paths
	PathViewPtr->ClearSelection();
	FavoritePathViewPtr->ClearSelection();

	TArray<FCollectionNameType> SelectedCollections = CollectionViewPtr->GetSelectedCollections();
	TArray<FString> SelectedPaths;

	if( SelectedCollections.Num() == 0  )
	{
		// just select the game folder
		SelectedPaths.Add(TEXT("/Game"));
		SourcesChanged(SelectedPaths, SelectedCollections);
	}
	else
	{
		SourcesChanged(SelectedPaths, SelectedCollections);
	}

}

void SCodeBrowser::PathPickerPathSelected(const FString& FolderPath)
{
	PathPickerButton->SetIsOpen(false);

	if ( !FolderPath.IsEmpty() )
	{
		TArray<FString> Paths;
		Paths.Add(FolderPath);
		PathViewPtr->SetSelectedPaths(Paths);
		FavoritePathViewPtr->SetSelectedPaths(Paths);
	}

	PathSelected(FolderPath);
}

void SCodeBrowser::SetSelectedPaths(const TArray<FString>& FolderPaths, bool bNeedsRefresh/* = false */)
{
	if (FolderPaths.Num() > 0)
	{
		if (bNeedsRefresh)
		{
			PathViewPtr->Populate();
			FavoritePathViewPtr->Populate();
		}

		PathViewPtr->SetSelectedPaths(FolderPaths);
		FavoritePathViewPtr->SetSelectedPaths(FolderPaths);
		PathSelected(FolderPaths[0]);
	}
}

void SCodeBrowser::ForceShowPluginContent(bool bEnginePlugin)
{
	if (AssetViewPtr.IsValid())
	{
		AssetViewPtr->ForceShowPluginFolder(bEnginePlugin);
	}
}

void SCodeBrowser::PathPickerCollectionSelected(const FCollectionNameType& SelectedCollection)
{
	PathPickerButton->SetIsOpen(false);

	TArray<FCollectionNameType> Collections;
	Collections.Add(SelectedCollection);
	CollectionViewPtr->SetSelectedCollections(Collections);

	CollectionSelected(SelectedCollection);
}

void SCodeBrowser::OnApplyHistoryData( const FHistoryData& History )
{
	PathViewPtr->ApplyHistoryData(History);
	FavoritePathViewPtr->ApplyHistoryData(History);
	CollectionViewPtr->ApplyHistoryData(History);
	AssetViewPtr->ApplyHistoryData(History);

	// Update the breadcrumb trail path
	UpdatePath();

	if (History.SourcesData.HasPackagePaths())
	{
		// Notify 'asset path changed' delegate
		FCodeBrowserModule& CodeBrowserModule = FModuleManager::GetModuleChecked<FCodeBrowserModule>(TEXT("CodeBrowser"));
		FCodeBrowserModule::FOnAssetPathChanged& PathChangedDelegate = CodeBrowserModule.GetOnAssetPathChanged();
		if (PathChangedDelegate.IsBound())
		{
			PathChangedDelegate.Broadcast(History.SourcesData.PackagePaths[0].ToString());
		}
	}
}

void SCodeBrowser::OnUpdateHistoryData(FHistoryData& HistoryData) const
{
	const FSourcesData& SourcesData = AssetViewPtr->GetSourcesData();
	const TArray<FFileData>& SelectedAssets = AssetViewPtr->GetSelectedAssets();

	const FText NewSource = SourcesData.HasPackagePaths() ? FText::FromName(SourcesData.PackagePaths[0]) : (SourcesData.HasCollections() ? FText::FromName(SourcesData.Collections[0].Name) : LOCTEXT("AllAssets", "All Assets"));

	HistoryData.HistoryDesc = NewSource;
	HistoryData.SourcesData = SourcesData;

	HistoryData.SelectionData.Reset();
	HistoryData.SelectionData.SelectedFolders = TSet<FString>(AssetViewPtr->GetSelectedFolders());
	for (const FFileData& SelectedAsset : SelectedAssets)
	{
		HistoryData.SelectionData.SelectedAssets.Add(SelectedAsset.ObjectPath);
	}
}

void SCodeBrowser::NewAssetRequested(const FString& SelectedPath, TWeakObjectPtr<UClass> FactoryClass)
{
	if ( ensure(SelectedPath.Len() > 0) && ensure(FactoryClass.IsValid()) )
	{
		UFactory* NewFactory = NewObject<UFactory>(GetTransientPackage(), FactoryClass.Get());
		// This factory may get gc'd as a side effect of various delegates potentially calling CollectGarbage so protect against it from being gc'd out from under us
		NewFactory->AddToRoot();

		FEditorDelegates::OnConfigureNewAssetProperties.Broadcast(NewFactory);
		if ( NewFactory->ConfigureProperties() )
		{
			FString DefaultAssetName;
			FString PackageNameToUse;

			static FName AssetToolsModuleName = FName("AssetTools");
			FAssetToolsModule& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>(AssetToolsModuleName);
			AssetToolsModule.Get().CreateUniqueAssetName(SelectedPath + TEXT("/") + NewFactory->GetDefaultNewAssetName(), TEXT(""), PackageNameToUse, DefaultAssetName);
			CreateNewAsset(DefaultAssetName, SelectedPath, NewFactory->GetSupportedClass(), NewFactory);
		}
		NewFactory->RemoveFromRoot();
	}
}

void SCodeBrowser::NewClassRequested(const FString& SelectedPath)
{
	// Parse out the on disk location for the currently selected path, this will then be used as the default location for the new class (if a valid project module location)
	FString ExistingFolderPath;
	if (!SelectedPath.IsEmpty())
	{
		TSharedRef<FNativeClassHierarchy> NativeClassHierarchy = FCodeBrowserSingleton::Get().GetNativeClassHierarchy();
		NativeClassHierarchy->GetFileSystemPath(SelectedPath, ExistingFolderPath);
	}

	FGameProjectGenerationModule::Get().OpenAddCodeToProjectDialog(
		FAddToProjectConfig()
		.InitialPath(ExistingFolderPath)
		.ParentWindow(FGlobalTabmanager::Get()->GetRootWindow())
	);
}

void SCodeBrowser::NewFolderRequested(const FString& SelectedPath)
{
	if( ensure(SelectedPath.Len() > 0) && AssetViewPtr.IsValid() )
	{
		CreateNewFolder(SelectedPath, FOnCreateNewFolder::CreateSP(AssetViewPtr.Get(), &SAssetView::OnCreateNewFolder));
	}
}

void SCodeBrowser::SetSearchBoxText(const FText& InSearchText)
{
	// Has anything changed? (need to test case as the operators are case-sensitive)
	if (!InSearchText.ToString().Equals(TextFilter->GetRawFilterText().ToString(), ESearchCase::CaseSensitive))
	{
		TextFilter->SetRawFilterText( InSearchText );
		SearchBoxPtr->SetError( TextFilter->GetFilterErrorText() );
		if(InSearchText.IsEmpty())
		{
			FrontendFilters->Remove(TextFilter);
			AssetViewPtr->SetUserSearching(false);
		}
		else
		{
			FrontendFilters->Add(TextFilter);
			AssetViewPtr->SetUserSearching(true);
		}
	}
}

void SCodeBrowser::OnSearchBoxChanged(const FText& InSearchText)
{
	SetSearchBoxText(InSearchText);

	// Broadcast 'search box changed' delegate
	FCodeBrowserModule& CodeBrowserModule = FModuleManager::GetModuleChecked<FCodeBrowserModule>( TEXT("CodeBrowser") );
	CodeBrowserModule.GetOnSearchBoxChanged().Broadcast(InSearchText, bIsPrimaryBrowser);	
}

void SCodeBrowser::OnSearchBoxCommitted(const FText& InSearchText, ETextCommit::Type CommitInfo)
{
	SetSearchBoxText(InSearchText);
}

bool SCodeBrowser::IsSaveSearchButtonEnabled() const
{
	return !TextFilter->GetRawFilterText().IsEmptyOrWhitespace();
}

FReply SCodeBrowser::OnSaveSearchButtonClicked()
{
	// Need to make sure we can see the collections view
	if (!bSourcesViewExpanded)
	{
		SourcesViewExpandClicked();
	}

	// We want to add any currently selected paths to the final saved query so that you get back roughly the same list of objects as what you're currently seeing
	FString SelectedPathsQuery;
	{
		const FSourcesData& SourcesData = AssetViewPtr->GetSourcesData();
		for (int32 SelectedPathIndex = 0; SelectedPathIndex < SourcesData.PackagePaths.Num(); ++SelectedPathIndex)
		{
			SelectedPathsQuery.Append(TEXT("Path:'"));
			SelectedPathsQuery.Append(SourcesData.PackagePaths[SelectedPathIndex].ToString());
			SelectedPathsQuery.Append(TEXT("'..."));

			if (SelectedPathIndex + 1 < SourcesData.PackagePaths.Num())
			{
				SelectedPathsQuery.Append(TEXT(" OR "));
			}
		}
	}

	// todo: should we automatically append any type filters too?

	// Produce the final query
	FText FinalQueryText;
	if (SelectedPathsQuery.IsEmpty())
	{
		FinalQueryText = TextFilter->GetRawFilterText();
	}
	else
	{
		FinalQueryText = FText::FromString(FString::Printf(TEXT("(%s) AND (%s)"), *TextFilter->GetRawFilterText().ToString(), *SelectedPathsQuery));
	}

	CollectionViewPtr->MakeSaveDynamicCollectionMenu(FinalQueryText);
	return FReply::Handled();
}

void SCodeBrowser::OnPathClicked( const FString& CrumbData )
{
	FSourcesData SourcesData = AssetViewPtr->GetSourcesData();

	if ( SourcesData.HasCollections() )
	{
		// Collection crumb was clicked. See if we've clicked on a different collection in the hierarchy, and change the path if required.
		TOptional<FCollectionNameType> CollectionClicked;
		{
			FString CollectionName;
			FString CollectionTypeString;
			if (CrumbData.Split(TEXT("?"), &CollectionName, &CollectionTypeString))
			{
				const int32 CollectionType = FCString::Atoi(*CollectionTypeString);
				if (CollectionType >= 0 && CollectionType < ECollectionShareType::CST_All)
				{
					CollectionClicked = FCollectionNameType(FName(*CollectionName), ECollectionShareType::Type(CollectionType));
				}
			}
		}

		if ( CollectionClicked.IsSet() && SourcesData.Collections[0] != CollectionClicked.GetValue() )
		{
			TArray<FCollectionNameType> Collections;
			Collections.Add(CollectionClicked.GetValue());
			CollectionViewPtr->SetSelectedCollections(Collections);

			CollectionSelected(CollectionClicked.GetValue());
		}
	}
	else if ( !SourcesData.HasPackagePaths() )
	{
		// No collections or paths are selected. This is "All Assets". Don't change the path when this is clicked.
	}
	else if ( SourcesData.PackagePaths.Num() > 1 || SourcesData.PackagePaths[0].ToString() != CrumbData )
	{
		// More than one path is selected or the crumb that was clicked is not the same path as the current one. Change the path.
		TArray<FString> SelectedPaths;
		SelectedPaths.Add(CrumbData);
		PathViewPtr->SetSelectedPaths(SelectedPaths);
		FavoritePathViewPtr->SetSelectedPaths(SelectedPaths);
		PathSelected(SelectedPaths[0]);
	}
}

void SCodeBrowser::OnPathMenuItemClicked(FString ClickedPath)
{
	OnPathClicked( ClickedPath );
}

TSharedPtr<SWidget> SCodeBrowser::OnGetCrumbDelimiterContent(const FString& CrumbData) const
{
	FSourcesData SourcesData = AssetViewPtr->GetSourcesData();

	TSharedPtr<SWidget> Widget = SNullWidget::NullWidget;
	TSharedPtr<SWidget> MenuWidget;

	if( SourcesData.HasCollections() )
	{
		TOptional<FCollectionNameType> CollectionClicked;
		{
			FString CollectionName;
			FString CollectionTypeString;
			if (CrumbData.Split(TEXT("?"), &CollectionName, &CollectionTypeString))
			{
				const int32 CollectionType = FCString::Atoi(*CollectionTypeString);
				if (CollectionType >= 0 && CollectionType < ECollectionShareType::CST_All)
				{
					CollectionClicked = FCollectionNameType(FName(*CollectionName), ECollectionShareType::Type(CollectionType));
				}
			}
		}

		if( CollectionClicked.IsSet() )
		{
			FCollectionManagerModule& CollectionManagerModule = FCollectionManagerModule::GetModule();

			TArray<FCollectionNameType> ChildCollections;
			CollectionManagerModule.Get().GetChildCollections(CollectionClicked->Name, CollectionClicked->Type, ChildCollections);

			if( ChildCollections.Num() > 0 )
			{
				FMenuBuilder MenuBuilder( true, nullptr );

				for( const FCollectionNameType& ChildCollection : ChildCollections )
				{
					const FString ChildCollectionCrumbData = FString::Printf(TEXT("%s?%s"), *ChildCollection.Name.ToString(), *FString::FromInt(ChildCollection.Type));

					MenuBuilder.AddMenuEntry(
						FText::FromName(ChildCollection.Name),
						FText::GetEmpty(),
						FSlateIcon(FEditorStyle::GetStyleSetName(), ECollectionShareType::GetIconStyleName(ChildCollection.Type)),
						FUIAction(FExecuteAction::CreateSP(this, &SCodeBrowser::OnPathMenuItemClicked, ChildCollectionCrumbData))
						);
				}

				MenuWidget = MenuBuilder.MakeWidget();
			}
		}
	}
	else if( SourcesData.HasPackagePaths() )
	{
		TArray<FString> SubPaths;
		const bool bRecurse = false;
		if( CodeBrowserUtils::IsClassPath(CrumbData) )
		{
			TSharedRef<FNativeClassHierarchy> NativeClassHierarchy = FCodeBrowserSingleton::Get().GetNativeClassHierarchy();

			FNativeClassHierarchyFilter ClassFilter;
			ClassFilter.ClassPaths.Add(FName(*CrumbData));
			ClassFilter.bRecursivePaths = bRecurse;

			NativeClassHierarchy->GetMatchingFolders(ClassFilter, SubPaths);
		}
		else
		{
			FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
			IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

			AssetRegistry.GetSubPaths( CrumbData, SubPaths, bRecurse );
		}

		if( SubPaths.Num() > 0 )
		{
			FMenuBuilder MenuBuilder( true, nullptr );

			for( int32 PathIndex = 0; PathIndex < SubPaths.Num(); ++PathIndex )
			{
				const FString& SubPath = SubPaths[PathIndex];

				// For displaying in the menu cut off the parent path since it is redundant
				FString PathWithoutParent = SubPath.RightChop( CrumbData.Len() + 1 );
				MenuBuilder.AddMenuEntry(
					FText::FromString(PathWithoutParent),
					FText::GetEmpty(),
					FSlateIcon(FEditorStyle::GetStyleSetName(), "CodeBrowser.BreadcrumbPathPickerFolder"),
					FUIAction(FExecuteAction::CreateSP(this, &SCodeBrowser::OnPathMenuItemClicked, SubPath))
					);
			}

			MenuWidget = MenuBuilder.MakeWidget();
		}
	}

	if( MenuWidget.IsValid() )
	{
		// Do not allow the menu to become too large if there are many directories
		Widget =
			SNew( SVerticalBox )
			+SVerticalBox::Slot()
			.MaxHeight( 400.0f )
			[
				MenuWidget.ToSharedRef()
			];
	}

	return Widget;
}

TSharedRef<SWidget> SCodeBrowser::GetPathPickerContent()
{
	FPathPickerConfig PathPickerConfig;

	FSourcesData SourcesData = AssetViewPtr->GetSourcesData();
	if ( SourcesData.HasPackagePaths() )
	{
		PathPickerConfig.DefaultPath = SourcesData.PackagePaths[0].ToString();
	}
	
	PathPickerConfig.OnPathSelected = FOnPathSelected::CreateSP(this, &SCodeBrowser::PathPickerPathSelected);
	PathPickerConfig.bAllowContextMenu = false;
	PathPickerConfig.bAllowClassesFolder = true;

	return SNew(SBox)
		.WidthOverride(300)
		.HeightOverride(500)
		.Padding(4)
		[
			SNew(SVerticalBox)

			// Path Picker
			+SVerticalBox::Slot()
			.FillHeight(1.f)
			[
				FCodeBrowserSingleton::Get().CreatePathPicker(PathPickerConfig)
			]

			// Collection View
			+SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0, 6, 0, 0)
			[
				SNew(SCollectionView)
				.AllowCollectionButtons(false)
				.OnCollectionSelected(this, &SCodeBrowser::PathPickerCollectionSelected)
				.AllowContextMenu(false)
			]
		];
}

FString SCodeBrowser::GetCurrentPath() const
{
	FString CurrentPath;
	const FSourcesData& SourcesData = AssetViewPtr->GetSourcesData();
	if ( SourcesData.HasPackagePaths() && SourcesData.PackagePaths[0] != NAME_None )
	{
		CurrentPath = SourcesData.PackagePaths[0].ToString();
	}

	return CurrentPath;
}

TSharedRef<SWidget> SCodeBrowser::MakeAddNewContextMenu(bool bShowGetContent, bool bShowImport)
{
	const FSourcesData& SourcesData = AssetViewPtr->GetSourcesData();

	int32 NumAssetPaths, NumClassPaths;
	CodeBrowserUtils::CountPathTypes(SourcesData.PackagePaths, NumAssetPaths, NumClassPaths);

	// Get all menu extenders for this context menu from the content browser module
	FCodeBrowserModule& CodeBrowserModule = FModuleManager::GetModuleChecked<FCodeBrowserModule>( TEXT("CodeBrowser") );
	TArray<FCodeBrowserMenuExtender_SelectedPaths> MenuExtenderDelegates = CodeBrowserModule.GetAllAssetContextMenuExtenders();
	
	// Delegate wants paths as FStrings
	TArray<FString> SelectPaths;
	for (FName PathName: SourcesData.PackagePaths)
	{
		SelectPaths.Add(PathName.ToString());
	}

	TArray<TSharedPtr<FExtender>> Extenders;
	for (int32 i = 0; i < MenuExtenderDelegates.Num(); ++i)
	{
		if (MenuExtenderDelegates[i].IsBound())
		{
			Extenders.Add(MenuExtenderDelegates[i].Execute(SelectPaths));
		}
	}
	TSharedPtr<FExtender> MenuExtender = FExtender::Combine(Extenders);

	FMenuBuilder MenuBuilder(/*bInShouldCloseWindowAfterMenuSelection=*/true, NULL, MenuExtender);

	// Only add "New Folder" item if we do not have a collection selected
	FNewAssetOrClassContextMenu::FOnNewFolderRequested OnNewFolderRequested;
	if (CollectionViewPtr->GetSelectedCollections().Num() == 0)
	{
		OnNewFolderRequested = FNewAssetOrClassContextMenu::FOnNewFolderRequested::CreateSP(this, &SCodeBrowser::NewFolderRequested);
	}


	// New feature packs don't depend on the current paths, so we always add this item if it was requested
	FNewAssetOrClassContextMenu::FOnGetContentRequested OnGetContentRequested;
	if(bShowGetContent)
	{
		OnGetContentRequested = FNewAssetOrClassContextMenu::FOnGetContentRequested::CreateSP(this, &SCodeBrowser::OnAddContentRequested);
	}

	// Only the asset items if we have an asset path selected
	FNewAssetOrClassContextMenu::FOnNewAssetRequested OnNewAssetRequested;
	FNewAssetOrClassContextMenu::FOnImportAssetRequested OnImportAssetRequested;
	if(NumAssetPaths > 0)
	{
		OnNewAssetRequested = FNewAssetOrClassContextMenu::FOnNewAssetRequested::CreateSP(this, &SCodeBrowser::NewAssetRequested);
		if(bShowImport)
		{
			OnImportAssetRequested = FNewAssetOrClassContextMenu::FOnImportAssetRequested::CreateSP(this, &SCodeBrowser::ImportAsset);
		}
	}

	// This menu always lets you create classes, but uses your default project source folder if the selected path is invalid for creating classes
	FNewAssetOrClassContextMenu::FOnNewClassRequested OnNewClassRequested = FNewAssetOrClassContextMenu::FOnNewClassRequested::CreateSP(this, &SCodeBrowser::NewClassRequested);

	FNewAssetOrClassContextMenu::MakeContextMenu(
		MenuBuilder, 
		SourcesData.PackagePaths, 
		OnNewAssetRequested,
		OnNewClassRequested,
		OnNewFolderRequested,
		OnImportAssetRequested,
		OnGetContentRequested
		);

	FDisplayMetrics DisplayMetrics;
	FSlateApplication::Get().GetCachedDisplayMetrics( DisplayMetrics );

	const FVector2D DisplaySize(
		DisplayMetrics.PrimaryDisplayWorkAreaRect.Right - DisplayMetrics.PrimaryDisplayWorkAreaRect.Left,
		DisplayMetrics.PrimaryDisplayWorkAreaRect.Bottom - DisplayMetrics.PrimaryDisplayWorkAreaRect.Top );

	return 
		SNew(SVerticalBox)

		+SVerticalBox::Slot()
		.MaxHeight(DisplaySize.Y * 0.9)
		[
			MenuBuilder.MakeWidget()
		];
}

bool SCodeBrowser::IsAddNewEnabled() const
{
	const FSourcesData& SourcesData = AssetViewPtr->GetSourcesData();
	return SourcesData.PackagePaths.Num() == 1;
}

FText SCodeBrowser::GetAddNewToolTipText() const
{
	const FSourcesData& SourcesData = AssetViewPtr->GetSourcesData();

	if ( SourcesData.PackagePaths.Num() == 1 )
	{
		const FString CurrentPath = SourcesData.PackagePaths[0].ToString();
		if ( CodeBrowserUtils::IsClassPath( CurrentPath ) )
		{
			return FText::Format( LOCTEXT("AddNewToolTip_AddNewClass", "Create a new class in {0}..."), FText::FromString(CurrentPath) );
		}
		else
		{
			return FText::Format( LOCTEXT("AddNewToolTip_AddNewAsset", "Create a new asset in {0}..."), FText::FromString(CurrentPath) );
		}
	}
	else if ( SourcesData.PackagePaths.Num() > 1 )
	{
		return LOCTEXT( "AddNewToolTip_MultiplePaths", "Cannot add assets or classes to multiple paths." );
	}
	
	return LOCTEXT( "AddNewToolTip_NoPath", "No path is selected as an add target." );
}

TSharedRef<SWidget> SCodeBrowser::MakeAddFilterMenu()
{
	return FilterListPtr->ExternalMakeAddFilterMenu();
}

TSharedPtr<SWidget> SCodeBrowser::GetFilterContextMenu()
{
	return FilterListPtr->ExternalMakeAddFilterMenu();
}

FReply SCodeBrowser::OnSaveClicked()
{
	CodeBrowserUtils::SaveDirtyPackages();
	return FReply::Handled();
}

void SCodeBrowser::OnAddContentRequested()
{
	IAddContentDialogModule& AddContentDialogModule = FModuleManager::LoadModuleChecked<IAddContentDialogModule>("AddContentDialog");
	FWidgetPath WidgetPath;
	FSlateApplication::Get().GeneratePathToWidgetChecked(AsShared(), WidgetPath);
	AddContentDialogModule.ShowDialog(WidgetPath.GetWindow());
}

void SCodeBrowser::OnAssetSelectionChanged(const FFileData& SelectedAsset)
{
	if ( bIsPrimaryBrowser )
	{
		SyncGlobalSelectionSet();
	}

	// Notify 'asset selection changed' delegate
	FCodeBrowserModule& CodeBrowserModule = FModuleManager::GetModuleChecked<FCodeBrowserModule>( TEXT("CodeBrowser") );
	FCodeBrowserModule::FOnAssetSelectionChanged& AssetSelectionChangedDelegate = CodeBrowserModule.GetOnAssetSelectionChanged();
	
	const TArray<FFileData>& SelectedAssets = AssetViewPtr->GetSelectedAssets();
	AssetContextMenu->SetSelectedAssets(SelectedAssets);
	CollectionViewPtr->SetSelectedAssets(SelectedAssets);
	if(AssetSelectionChangedDelegate.IsBound())
	{
		AssetSelectionChangedDelegate.Broadcast(SelectedAssets, bIsPrimaryBrowser);
	}
}

void SCodeBrowser::OnAssetsActivated(const TArray<FFileData>& ActivatedAssets, EAssetTypeActivationMethod::Type ActivationMethod)
{
	TMap< TSharedRef<IAssetTypeActions>, TArray<UObject*> > TypeActionsToObjects;
	TArray<UObject*> ObjectsWithoutTypeActions;

	const FText LoadingTemplate = LOCTEXT("LoadingAssetName", "Loading {0}...");
	const FText DefaultText = ActivatedAssets.Num() == 1 ? FText::Format(LoadingTemplate, FText::FromName(ActivatedAssets[0].AssetName)) : LOCTEXT("LoadingObjects", "Loading Objects...");
	FScopedSlowTask SlowTask(100, DefaultText);

	// Iterate over all activated assets to map them to AssetTypeActions.
	// This way individual asset type actions will get a batched list of assets to operate on
	for ( auto AssetIt = ActivatedAssets.CreateConstIterator(); AssetIt; ++AssetIt )
	{
		const FFileData& AssetData = *AssetIt;
		if (!AssetData.IsAssetLoaded() && FEditorFileUtils::IsMapPackageAsset(AssetData.ObjectPath.ToString()))
		{
			SlowTask.MakeDialog();
		}

		SlowTask.EnterProgressFrame(75.f/ActivatedAssets.Num(), FText::Format(LoadingTemplate, FText::FromName(AssetData.AssetName)));

		UObject* Asset = (*AssetIt).GetAsset();

		if ( Asset != NULL )
		{
			FAssetToolsModule& AssetToolsModule = FModuleManager::Get().LoadModuleChecked<FAssetToolsModule>("AssetTools");
			TWeakPtr<IAssetTypeActions> AssetTypeActions = AssetToolsModule.Get().GetAssetTypeActionsForClass(Asset->GetClass());
			if ( AssetTypeActions.IsValid() )
			{
				// Add this asset to the list associated with the asset type action object
				TArray<UObject*>& ObjList = TypeActionsToObjects.FindOrAdd(AssetTypeActions.Pin().ToSharedRef());
				ObjList.AddUnique(Asset);
			}
			else
			{
				ObjectsWithoutTypeActions.AddUnique(Asset);
			}
		}
	}

	// Now that we have created our map, activate all the lists of objects for each asset type action.
	for ( auto TypeActionsIt = TypeActionsToObjects.CreateConstIterator(); TypeActionsIt; ++TypeActionsIt )
	{
		SlowTask.EnterProgressFrame(25.f/TypeActionsToObjects.Num());

		const TSharedRef<IAssetTypeActions>& TypeActions = TypeActionsIt.Key();
		const TArray<UObject*>& ObjList = TypeActionsIt.Value();

		TypeActions->AssetsActivated(ObjList, ActivationMethod);
	}

	// Finally, open a simple asset editor for all assets which do not have asset type actions if activating with enter or double click
	if ( ActivationMethod == EAssetTypeActivationMethod::DoubleClicked || ActivationMethod == EAssetTypeActivationMethod::Opened )
	{
		CodeBrowserUtils::OpenEditorForAsset(ObjectsWithoutTypeActions);
	}
}

TSharedPtr<SWidget> SCodeBrowser::OnGetAssetContextMenu(const TArray<FFileData>& SelectedAssets)
{
	if ( SelectedAssets.Num() == 0 )
	{
#pragma region // CodeBrowser右键菜单
		if (CHECK_ENGINE_IS_COURSEWARE_EDITOR == false)
			return MakeAddNewContextMenu(false, true);
		else
			return SNullWidget::NullWidget;
#pragma endregion
	}
	else
	{
		return AssetContextMenu->MakeContextMenu(SelectedAssets, AssetViewPtr->GetSourcesData(), Commands);
	}
}

FReply SCodeBrowser::ToggleLockClicked()
{
	bIsLocked = !bIsLocked;

	return FReply::Handled();
}

const FSlateBrush* SCodeBrowser::GetToggleLockImage() const
{
	if ( bIsLocked )
	{
		return FEditorStyle::GetBrush("CodeBrowser.LockButton_Locked");
	}
	else
	{
		return FEditorStyle::GetBrush("CodeBrowser.LockButton_Unlocked");
	}
}

EVisibility SCodeBrowser::GetSourcesViewVisibility() const
{
	return bSourcesViewExpanded ? EVisibility::Visible : EVisibility::Collapsed;
}

const FSlateBrush* SCodeBrowser::GetSourcesToggleImage() const
{
	if ( bSourcesViewExpanded )
	{
		return FEditorStyle::GetBrush("CodeBrowser.HideSourcesView");
	}
	else
	{
		return FEditorStyle::GetBrush("CodeBrowser.ShowSourcesView");
	}
}

FReply SCodeBrowser::SourcesViewExpandClicked()
{
	bSourcesViewExpanded = !bSourcesViewExpanded;

	// Notify 'Sources View Expanded' delegate
	FCodeBrowserModule& CodeBrowserModule = FModuleManager::GetModuleChecked<FCodeBrowserModule>( TEXT("CodeBrowser") );
	FCodeBrowserModule::FOnSourcesViewChanged& SourcesViewChangedDelegate = CodeBrowserModule.GetOnSourcesViewChanged();
	if(SourcesViewChangedDelegate.IsBound())
	{
		SourcesViewChangedDelegate.Broadcast(bSourcesViewExpanded);
	}

	return FReply::Handled();
}

EVisibility SCodeBrowser::GetPathExpanderVisibility() const
{
	return bSourcesViewExpanded ? EVisibility::Collapsed : EVisibility::Visible;
}

FReply SCodeBrowser::BackClicked()
{
	HistoryManager.GoBack();

	return FReply::Handled();
}

FReply SCodeBrowser::ForwardClicked()
{
	HistoryManager.GoForward();

	return FReply::Handled();
}

bool SCodeBrowser::HandleRenameCommandCanExecute() const
{
	const TArray<TSharedPtr<FAssetViewItem>>& SelectedItems = AssetViewPtr->GetSelectedItems();
	if (SelectedItems.Num() > 0)
	{
		return AssetContextMenu->CanExecuteRename();
	}
	else
	{
		const TArray<FString>& SelectedPaths = PathViewPtr->GetSelectedPaths();
		if (SelectedPaths.Num() > 0)
		{
			return PathContextMenu->CanExecuteRename();
		}
	}
	return false;
}

bool SCodeBrowser::HandleSaveAssetCommandCanExecute() const
{
	const TArray<TSharedPtr<FAssetViewItem>>& SelectedItems = AssetViewPtr->GetSelectedItems();
	if (SelectedItems.Num() > 0)
	{
		return AssetContextMenu->CanExecuteSaveAsset();
	}

	return false;
}

void SCodeBrowser::HandleSaveAllCurrentFolderCommand() const
{
	PathContextMenu->ExecuteSaveFolder();
}

void SCodeBrowser::HandleResaveAllCurrentFolderCommand() const
{
	PathContextMenu->ExecuteResaveFolder();
}

void SCodeBrowser::HandleRenameCommand()
{
	const TArray<TSharedPtr<FAssetViewItem>>& SelectedItems = AssetViewPtr->GetSelectedItems();
	if (SelectedItems.Num() > 0)
	{
		AssetContextMenu->ExecuteRename();
	}
	else
	{
		const TArray<FString>& SelectedPaths = PathViewPtr->GetSelectedPaths();
		if (SelectedPaths.Num() > 0)
		{
			PathContextMenu->ExecuteRename();
		}
	}
}

void SCodeBrowser::HandleSaveAssetCommand()
{
	const TArray<TSharedPtr<FAssetViewItem>>& SelectedItems = AssetViewPtr->GetSelectedItems();
	if (SelectedItems.Num() > 0)
	{
		AssetContextMenu->ExecuteSaveAsset();
	}
}

bool SCodeBrowser::HandleDeleteCommandCanExecute() const
{
	const TArray<TSharedPtr<FAssetViewItem>>& SelectedItems = AssetViewPtr->GetSelectedItems();
	if (SelectedItems.Num() > 0)
	{
		return AssetContextMenu->CanExecuteDelete();
	}
	else
	{
		const TArray<FString>& SelectedPaths = PathViewPtr->GetSelectedPaths();
		if (SelectedPaths.Num() > 0)
		{
			return PathContextMenu->CanExecuteDelete();
		}
	}
	return false;
}

void SCodeBrowser::HandleDeleteCommandExecute()
{
	const TArray<TSharedPtr<FAssetViewItem>>& SelectedItems = AssetViewPtr->GetSelectedItems();
	if (SelectedItems.Num() > 0)
	{
		AssetContextMenu->ExecuteDelete();
	}
	else
	{
		const TArray<FString>& SelectedPaths = PathViewPtr->GetSelectedPaths();
		if (SelectedPaths.Num() > 0)
		{
			PathContextMenu->ExecuteDelete();
		}
	}
}

void SCodeBrowser::HandleOpenAssetsOrFoldersCommandExecute()
{
	AssetViewPtr->OnOpenAssetsOrFolders();
}

void SCodeBrowser::HandlePreviewAssetsCommandExecute()
{
	AssetViewPtr->OnPreviewAssets();
}

void SCodeBrowser::HandleCreateNewFolderCommandExecute()
{
	TArray<FString> SelectedPaths = PathViewPtr->GetSelectedPaths();

	// only create folders when a single path is selected
	const bool bCanCreateNewFolder = (SelectedPaths.Num() == 1) && CodeBrowserUtils::IsValidPathToCreateNewFolder(SelectedPaths[0]);

	if (bCanCreateNewFolder)
	{
		CreateNewFolder(
			SelectedPaths.Num() > 0
			? SelectedPaths[0]
			: FString(),
			FOnCreateNewFolder::CreateSP(AssetViewPtr.Get(), &SAssetView::OnCreateNewFolder));
	}
}

void SCodeBrowser::HandleDirectoryUpCommandExecute()
{
	TArray<FString> SelectedPaths = PathViewPtr->GetSelectedPaths();
	if(SelectedPaths.Num() == 1 && !CodeBrowserUtils::IsRootDir(SelectedPaths[0]))
	{
		FString ParentDir = SelectedPaths[0] / TEXT("..");
		FPaths::CollapseRelativeDirectories(ParentDir);
		FolderEntered(ParentDir);
	}
}

void SCodeBrowser::GetSelectionState(TArray<FFileData>& SelectedAssets, TArray<FString>& SelectedPaths)
{
	SelectedAssets.Reset();
	SelectedPaths.Reset();
	if (AssetViewPtr->HasAnyUserFocusOrFocusedDescendants())
	{
		SelectedAssets = AssetViewPtr->GetSelectedAssets();
		SelectedPaths = AssetViewPtr->GetSelectedFolders();
	}
	else if (PathViewPtr->HasAnyUserFocusOrFocusedDescendants())
	{
		SelectedPaths = PathViewPtr->GetSelectedPaths();
	}
}

bool SCodeBrowser::IsBackEnabled() const
{
	return HistoryManager.CanGoBack();
}

bool SCodeBrowser::IsForwardEnabled() const
{
	return HistoryManager.CanGoForward();
}

bool SCodeBrowser::CanExecuteDirectoryUp() const
{
	TArray<FString> SelectedPaths = PathViewPtr->GetSelectedPaths();
	return (SelectedPaths.Num() == 1 && !CodeBrowserUtils::IsRootDir(SelectedPaths[0]));
}

FText SCodeBrowser::GetHistoryBackTooltip() const
{
	if ( HistoryManager.CanGoBack() )
	{
		return FText::Format( LOCTEXT("HistoryBackTooltipFmt", "Back to {0}"), HistoryManager.GetBackDesc() );
	}
	return FText::GetEmpty();
}

FText SCodeBrowser::GetHistoryForwardTooltip() const
{
	if ( HistoryManager.CanGoForward() )
	{
		return FText::Format( LOCTEXT("HistoryForwardTooltipFmt", "Forward to {0}"), HistoryManager.GetForwardDesc() );
	}
	return FText::GetEmpty();
}

FText SCodeBrowser::GetDirectoryUpTooltip() const
{
	TArray<FString> SelectedPaths = PathViewPtr->GetSelectedPaths();
	if(SelectedPaths.Num() == 1 && !CodeBrowserUtils::IsRootDir(SelectedPaths[0]))
	{
		FString ParentDir = SelectedPaths[0] / TEXT("..");
		FPaths::CollapseRelativeDirectories(ParentDir);
		return FText::Format(LOCTEXT("DirectoryUpTooltip", "Up to {0}"), FText::FromString(ParentDir) );
	}

	return FText();
}

EVisibility SCodeBrowser::GetDirectoryUpToolTipVisibility() const
{
	EVisibility ToolTipVisibility = EVisibility::Collapsed;

	// if we have text in our tooltip, make it visible. 
	if(GetDirectoryUpTooltip().IsEmpty() == false)
	{
		ToolTipVisibility = EVisibility::Visible;
	}

	return ToolTipVisibility;
}

void SCodeBrowser::SyncGlobalSelectionSet()
{
	USelection* EditorSelection = GEditor->GetSelectedObjects();
	if ( !ensure( EditorSelection != NULL ) )
	{
		return;
	}

	// Get the selected assets in the asset view
	const TArray<FFileData>& SelectedAssets = AssetViewPtr->GetSelectedAssets();

	EditorSelection->BeginBatchSelectOperation();
	{
		TSet< UObject* > SelectedObjects;
		// Lets see what the user has selected and add any new selected objects to the global selection set
		for ( auto AssetIt = SelectedAssets.CreateConstIterator(); AssetIt; ++AssetIt )
		{
			// Grab the object if it is loaded
			if ( (*AssetIt).IsAssetLoaded() )
			{
				UObject* FoundObject = (*AssetIt).GetAsset();
				if( FoundObject != NULL && FoundObject->GetClass() != UObjectRedirector::StaticClass() )
				{
					SelectedObjects.Add( FoundObject );

					// Select this object!
					EditorSelection->Select( FoundObject );
				}
			}
		}


		// Now we'll build a list of objects that need to be removed from the global selection set
		for( int32 CurEditorObjectIndex = 0; CurEditorObjectIndex < EditorSelection->Num(); ++CurEditorObjectIndex )
		{
			UObject* CurEditorObject = EditorSelection->GetSelectedObject( CurEditorObjectIndex );
			if( CurEditorObject != NULL ) 
			{
				if( !SelectedObjects.Contains( CurEditorObject ) )
				{
					EditorSelection->Deselect( CurEditorObject );
				}
			}
		}
	}
	EditorSelection->EndBatchSelectOperation();
}

void SCodeBrowser::UpdatePath()
{
	FSourcesData SourcesData = AssetViewPtr->GetSourcesData();

	PathBreadcrumbTrail->ClearCrumbs();

	if ( SourcesData.HasPackagePaths() )
	{
		TArray<FString> Crumbs;
		SourcesData.PackagePaths[0].ToString().ParseIntoArray(Crumbs, TEXT("/"), true);

		FString CrumbPath = TEXT("/");
		for ( auto CrumbIt = Crumbs.CreateConstIterator(); CrumbIt; ++CrumbIt )
		{
			// If this is the root part of the path, try and get the localized display name to stay in sync with what we see in SPathView
			const FText DisplayName = (CrumbIt.GetIndex() == 0) ? CodeBrowserUtils::GetRootDirDisplayName(*CrumbIt) : FText::FromString(*CrumbIt);

			CrumbPath += *CrumbIt;
			PathBreadcrumbTrail->PushCrumb(DisplayName, CrumbPath);
			CrumbPath += TEXT("/");
		}
	}
	else if ( SourcesData.HasCollections() )
	{
		FCollectionManagerModule& CollectionManagerModule = FCollectionManagerModule::GetModule();
		TArray<FCollectionNameType> CollectionPathItems;

		// Walk up the parents of this collection so that we can generate a complete path (this loop also adds the child collection to the array)
		for (TOptional<FCollectionNameType> CurrentCollection = SourcesData.Collections[0]; 
			CurrentCollection.IsSet(); 
			CurrentCollection = CollectionManagerModule.Get().GetParentCollection(CurrentCollection->Name, CurrentCollection->Type)
			)
		{
			CollectionPathItems.Insert(CurrentCollection.GetValue(), 0);
		}

		// Now add each part of the path to the breadcrumb trail
		for (const FCollectionNameType& CollectionPathItem : CollectionPathItems)
		{
			const FString CrumbData = FString::Printf(TEXT("%s?%s"), *CollectionPathItem.Name.ToString(), *FString::FromInt(CollectionPathItem.Type));

			FFormatNamedArguments Args;
			Args.Add(TEXT("CollectionName"), FText::FromName(CollectionPathItem.Name));
			const FText DisplayName = FText::Format(LOCTEXT("CollectionPathIndicator", "{CollectionName} (Collection)"), Args);

			PathBreadcrumbTrail->PushCrumb(DisplayName, CrumbData);
		}
	}
	else
	{
		PathBreadcrumbTrail->PushCrumb(LOCTEXT("AllAssets", "All Assets"), TEXT(""));
	}
}

void SCodeBrowser::OnFilterChanged()
{
	FARFilter Filter = FilterListPtr->GetCombinedBackendFilter();
	AssetViewPtr->SetBackendFilter( Filter );

	// Notify 'filter changed' delegate
	FCodeBrowserModule& CodeBrowserModule = FModuleManager::GetModuleChecked<FCodeBrowserModule>( TEXT("CodeBrowser") );
	CodeBrowserModule.GetOnFilterChanged().Broadcast(Filter, bIsPrimaryBrowser);
}

FText SCodeBrowser::GetPathText() const
{
	FText PathLabelText;

	if ( IsFilteredBySource() )
	{
		const FSourcesData& SourcesData = AssetViewPtr->GetSourcesData();

		// At least one source is selected
		const int32 NumSources = SourcesData.PackagePaths.Num() + SourcesData.Collections.Num();

		if (NumSources > 0)
		{
			PathLabelText = FText::FromName(SourcesData.HasPackagePaths() ? SourcesData.PackagePaths[0] : SourcesData.Collections[0].Name);

			if (NumSources > 1)
			{
				PathLabelText = FText::Format(LOCTEXT("PathTextFmt", "{0} and {1} {1}|plural(one=other,other=others)..."), PathLabelText, NumSources - 1);
			}
		}
	}
	else
	{
		PathLabelText = LOCTEXT("AllAssets", "All Assets");
	}

	return PathLabelText;
}

bool SCodeBrowser::IsFilteredBySource() const
{
	const FSourcesData& SourcesData = AssetViewPtr->GetSourcesData();
	return !SourcesData.IsEmpty();
}

void SCodeBrowser::OnAssetRenameCommitted(const TArray<FFileData>& Assets)
{
	// After a rename is committed we allow an implicit sync so as not to
	// disorientate the user if they are looking at a parent folder

	const bool bAllowImplicitSync = true;
	const bool bDisableFiltersThatHideAssets = false;
	SyncToAssets(Assets, bAllowImplicitSync, bDisableFiltersThatHideAssets);
}

void SCodeBrowser::OnFindInAssetTreeRequested(const TArray<FFileData>& AssetsToFind)
{
	SyncToAssets(AssetsToFind);
}

void SCodeBrowser::OnRenameRequested(const FFileData& AssetData)
{
	AssetViewPtr->RenameAsset(AssetData);
}

void SCodeBrowser::OnRenameFolderRequested(const FString& FolderToRename)
{
	const TArray<FString>& SelectedFolders = AssetViewPtr->GetSelectedFolders();
	if (SelectedFolders.Num() > 0)
	{
		AssetViewPtr->RenameFolder(FolderToRename);
	}
	else
	{
		const TArray<FString>& SelectedPaths = PathViewPtr->GetSelectedPaths();
		if (SelectedPaths.Num() > 0)
		{
			PathViewPtr->RenameFolder(FolderToRename);
		}
	}
}

void SCodeBrowser::OnOpenedFolderDeleted(TArray<FString>& SelectedPaths)
{
	// Since the contents of the asset view have just been deleted, set the selected path to the default "/Game"
	TArray<FString> DefaultSelectedPaths;
	DefaultSelectedPaths.Add(TEXT("/Game"));
	PathViewPtr->SetSelectedPaths(DefaultSelectedPaths);
	FavoritePathViewPtr->SetSelectedPaths(DefaultSelectedPaths);
	FSourcesData DefaultSourcesData(FName("/Game"));
	AssetViewPtr->SetSourcesData(DefaultSourcesData);

	FCodeBrowserModule& CodeBrowserModule = FModuleManager::GetModuleChecked<FCodeBrowserModule>(TEXT("CodeBrowser"));
	FCodeBrowserModule::FOnDeletedFolder& DeletedFolderDelegate = CodeBrowserModule.GetOnDeletedFolder();
	if (DeletedFolderDelegate.IsBound())
	{
		DeletedFolderDelegate.Broadcast(SelectedPaths);
	}

	UpdatePath();
}

void SCodeBrowser::OnDuplicateRequested(const TWeakObjectPtr<UObject>& OriginalObject)
{
	UObject* Object = OriginalObject.Get();

	if ( Object )
	{
		AssetViewPtr->DuplicateAsset(FPackageName::GetLongPackagePath(Object->GetOutermost()->GetName()), OriginalObject);
	}
}

void SCodeBrowser::OnAssetViewRefreshRequested()
{
	AssetViewPtr->RequestSlowFullListRefresh();
}

void SCodeBrowser::HandleCollectionRemoved(const FCollectionNameType& Collection)
{
	AssetViewPtr->SetSourcesData(FSourcesData());

	auto RemoveHistoryDelegate = [&](const FHistoryData& HistoryData)
	{
		return (HistoryData.SourcesData.Collections.Num() == 1 &&
				HistoryData.SourcesData.PackagePaths.Num() == 0 &&
				HistoryData.SourcesData.Collections.Contains(Collection));
	};

	HistoryManager.RemoveHistoryData(RemoveHistoryDelegate);
}

void SCodeBrowser::HandleCollectionRenamed(const FCollectionNameType& OriginalCollection, const FCollectionNameType& NewCollection)
{
	return HandleCollectionRemoved(OriginalCollection);
}

void SCodeBrowser::HandleCollectionUpdated(const FCollectionNameType& Collection)
{
	const FSourcesData& SourcesData = AssetViewPtr->GetSourcesData();

	// If we're currently viewing the dynamic collection that was updated, make sure our active filter text is up-to-date
	if (SourcesData.IsDynamicCollection() && SourcesData.Collections[0] == Collection)
	{
		FCollectionManagerModule& CollectionManagerModule = FCollectionManagerModule::GetModule();

		const FCollectionNameType& DynamicCollection = SourcesData.Collections[0];

		FString DynamicQueryString;
		CollectionManagerModule.Get().GetDynamicQueryText(DynamicCollection.Name, DynamicCollection.Type, DynamicQueryString);

		const FText DynamicQueryText = FText::FromString(DynamicQueryString);
		SetSearchBoxText(DynamicQueryText);
		SearchBoxPtr->SetText(DynamicQueryText);
	}
}

void SCodeBrowser::HandlePathRemoved(const FString& Path)
{
	const FName PathName(*Path);

	auto RemoveHistoryDelegate = [&](const FHistoryData& HistoryData)
	{
		return (HistoryData.SourcesData.PackagePaths.Num() == 1 &&
				HistoryData.SourcesData.Collections.Num() == 0 &&
				HistoryData.SourcesData.PackagePaths.Contains(PathName));
	};

	HistoryManager.RemoveHistoryData(RemoveHistoryDelegate);
}

FText SCodeBrowser::GetSearchAssetsHintText() const
{
	if (PathViewPtr.IsValid())
	{
		TArray<FString> Paths = PathViewPtr->GetSelectedPaths();
		if (Paths.Num() != 0)
		{
			FString SearchHint = NSLOCTEXT( "CodeBrowser", "SearchBoxPartialHint", "Search" ).ToString();
			SearchHint += TEXT(" ");
			for(int32 i = 0; i < Paths.Num(); i++)
			{
				const FString& Path = Paths[i];
				if (CodeBrowserUtils::IsRootDir(Path))
				{
					SearchHint += CodeBrowserUtils::GetRootDirDisplayName(Path).ToString();
				}
				else
				{
					SearchHint += FPaths::GetCleanFilename(Path);
				}

				if (i + 1 < Paths.Num())
				{
					SearchHint += ", ";
				}
			}

			return FText::FromString(SearchHint);
		}
	}
	
	return NSLOCTEXT( "CodeBrowser", "SearchBoxHint", "Search Assets" );
}

TArray<FString> SCodeBrowser::GetAssetSearchSuggestions() const
{
	TArray<FString> AllSuggestions;

	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
	TArray< TWeakPtr<IAssetTypeActions> > AssetTypeActionsList;
	AssetToolsModule.Get().GetAssetTypeActionsList(AssetTypeActionsList);

	for ( auto TypeActionsIt = AssetTypeActionsList.CreateConstIterator(); TypeActionsIt; ++TypeActionsIt )
	{
		if ( (*TypeActionsIt).IsValid() )
		{
			const TSharedPtr<IAssetTypeActions> TypeActions = (*TypeActionsIt).Pin();
			if ( TypeActions->GetSupportedClass() )
			{
				AllSuggestions.Add( TypeActions->GetSupportedClass()->GetName() );
			}
		}
	}

	return AllSuggestions;
}

TSharedPtr<SWidget> SCodeBrowser::GetFolderContextMenu(const TArray<FString>& SelectedPaths, FCodeBrowserMenuExtender_SelectedPaths InMenuExtender, FOnCreateNewFolder InOnCreateNewFolder, bool bPathView)
{
	// Clear any selection in the asset view, as it'll conflict with other view info
	// This is important for determining which context menu may be open based on the asset selection for rename/delete operations
	if (bPathView)
	{
		AssetViewPtr->ClearSelection();
	}
	
	// Ensure the path context menu has the up-to-date list of paths being worked on
	PathContextMenu->SetSelectedPaths(SelectedPaths);

	TSharedPtr<FExtender> Extender;
	if(InMenuExtender.IsBound())
	{
		Extender = InMenuExtender.Execute(SelectedPaths);
	}

	const bool bInShouldCloseWindowAfterSelection = true;
	FMenuBuilder MenuBuilder(bInShouldCloseWindowAfterSelection, Commands, Extender, true);
	
	// We can only create folders when we have a single path selected
	const bool bCanCreateNewFolder = SelectedPaths.Num() == 1 && CodeBrowserUtils::IsValidPathToCreateNewFolder(SelectedPaths[0]);

	FText NewFolderToolTip;
	if(SelectedPaths.Num() == 1)
	{
		if(bCanCreateNewFolder)
		{
			NewFolderToolTip = FText::Format(LOCTEXT("NewFolderTooltip_CreateIn", "Create a new folder in {0}."), FText::FromString(SelectedPaths[0]));
		}
		else
		{
			NewFolderToolTip = FText::Format(LOCTEXT("NewFolderTooltip_InvalidPath", "Cannot create new folders in {0}."), FText::FromString(SelectedPaths[0]));
		}
	}
	else
	{
		NewFolderToolTip = LOCTEXT("NewFolderTooltip_InvalidNumberOfPaths", "Can only create folders when there is a single path selected.");
	}
#pragma region // 文件夹右键菜单 - 
	// New Folder
	MenuBuilder.AddMenuEntry(
		LOCTEXT("NewFolder", "New Folder"),
		NewFolderToolTip,
		FSlateIcon(FEditorStyle::GetStyleSetName(), "CodeBrowser.NewFolderIcon"),
		FUIAction(
			FExecuteAction::CreateSP( this, &SCodeBrowser::CreateNewFolder, SelectedPaths.Num() > 0 ? SelectedPaths[0] : FString(), InOnCreateNewFolder ),
			FCanExecuteAction::CreateLambda( [bCanCreateNewFolder] { return bCanCreateNewFolder; } )
			)
		);

	MenuBuilder.AddMenuEntry(
		LOCTEXT("ShowInNewCodeBrowser", "Show in New Content Browser"),
		LOCTEXT("ShowInNewCodeBrowserTooltip", "Opens a new Content Browser at this folder location (at least 1 Content Browser window needs to be locked)"),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateSP(this, &SCodeBrowser::OpenNewCodeBrowser)),
		"FolderContext"
	);

	return MenuBuilder.MakeWidget();
}

void SCodeBrowser::CreateNewFolder(FString FolderPath, FOnCreateNewFolder InOnCreateNewFolder)
{
	// Create a valid base name for this folder
	FText DefaultFolderBaseName = LOCTEXT("DefaultFolderName", "NewFolder");
	FText DefaultFolderName = DefaultFolderBaseName;
	int32 NewFolderPostfix = 1;
	while(CodeBrowserUtils::DoesFolderExist(FolderPath / DefaultFolderName.ToString()))
	{
		DefaultFolderName = FText::Format(LOCTEXT("DefaultFolderNamePattern", "{0}{1}"), DefaultFolderBaseName, FText::AsNumber(NewFolderPostfix));
		NewFolderPostfix++;
	}

	InOnCreateNewFolder.ExecuteIfBound(DefaultFolderName.ToString(), FolderPath);
}

void SCodeBrowser::OpenNewCodeBrowser()
{
	FCodeBrowserSingleton::Get().SyncBrowserToFolders(PathContextMenu->GetSelectedPaths(), false, true, NAME_None, true);
}

#undef LOCTEXT_NAMESPACE
