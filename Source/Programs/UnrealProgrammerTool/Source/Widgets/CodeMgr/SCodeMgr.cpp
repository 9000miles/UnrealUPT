#include "SCodeMgr.h"
#include "SBoxPanel.h"
#include "SCodeMgrToolBar.h"
#include "TabManager.h"
#include "WorkspaceItem.h"
#include "EditorStyleSet.h"
#include "SDockTab.h"
#include "SDirectoryView.h"
#include "SFileViewport.h"
#include "SCodeDetails.h"
#include "SCodeMgrState.h"

#define LOCTEXT_NAMESPACE "SCodeMgr"

static const FName DirectoryTreeTabID("DirectoryTree");
static const FName FileViewportTabID("FileViewport");
static const FName CodeStateTabID("CodeState");
static const FName CodeDetailsTabID("CodeDetails");

void SCodeMgr::Construct(const FArguments& InArgs, const TSharedRef<SDockTab>& ConstructUnderMajorTab, const TSharedPtr<SWindow>& ConstructUnderWindow)
{


	TabManager = FGlobalTabmanager::Get()->NewTabManager(ConstructUnderMajorTab);
	TSharedRef<FWorkspaceItem> AppMenuGroup = TabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("UPTCodeMgrMenuGroupName", "UPTCodeMgr"));

	TabManager->RegisterTabSpawner(DirectoryTreeTabID, FOnSpawnTab::CreateRaw(this, &SCodeMgr::HandleTabManagerSpawnTab, DirectoryTreeTabID))
		.SetDisplayName(LOCTEXT("DirectoryTreeTabTitle", "Directory Tree"))
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "SessionFrontEnd.Tabs.Tools"))
		.SetGroup(AppMenuGroup);

	TabManager->RegisterTabSpawner(FileViewportTabID, FOnSpawnTab::CreateRaw(this, &SCodeMgr::HandleTabManagerSpawnTab, FileViewportTabID))
		.SetDisplayName(LOCTEXT("FileViewportTabTitle", "File Viewport"))
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "SessionFrontEnd.Tabs.Tools"))
		.SetGroup(AppMenuGroup);

	TabManager->RegisterTabSpawner(CodeDetailsTabID, FOnSpawnTab::CreateRaw(this, &SCodeMgr::HandleTabManagerSpawnTab, CodeDetailsTabID))
		.SetDisplayName(LOCTEXT("CodeDetailsTabTitle", "Code Details"))
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "SessionFrontEnd.Tabs.Tools"))
		.SetGroup(AppMenuGroup);

	// create tab layout
	const TSharedRef<FTabManager::FLayout> Layout = FTabManager::NewLayout("UPTCodeMgrLayout")
		->AddArea
		(
			FTabManager::NewPrimaryArea()
			->SetOrientation(Orient_Horizontal)
			->Split
			(
				// session browser
				FTabManager::NewStack()
				->AddTab(DirectoryTreeTabID, ETabState::OpenedTab)
				->SetSizeCoefficient(0.25f)
			)
			->Split
			(
				FTabManager::NewSplitter()
				->SetOrientation(Orient_Vertical)
				->SetSizeCoefficient(0.5f)
				->Split
				(
					FTabManager::NewStack()
					->AddTab(FileViewportTabID, ETabState::OpenedTab)
					->SetSizeCoefficient(0.8f)
				)
				->Split
				(
					FTabManager::NewStack()
					->AddTab(CodeStateTabID, ETabState::OpenedTab)
					->SetSizeCoefficient(0.2f)
				)
			)
			->Split
			(
				// applications
				FTabManager::NewStack()
				->AddTab(CodeDetailsTabID, ETabState::OpenedTab)
				->SetSizeCoefficient(0.25f)
			)
		);


	ChildSlot
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SCodeMgrToolBar)
			]
			+ SVerticalBox::Slot()
			.FillHeight(1.0f)
			[
				TabManager->RestoreFrom(Layout, ConstructUnderWindow).ToSharedRef()
			]
		];
}

TSharedRef<SDockTab> SCodeMgr::HandleTabManagerSpawnTab(const FSpawnTabArgs& Args, FName TabIdentifier)
{
	TSharedPtr<SWidget> TabWidget = SNullWidget::NullWidget;

	TSharedRef<SDockTab> DockTab = SNew(SDockTab)
		.TabRole(ETabRole::PanelTab);

	if (TabIdentifier == DirectoryTreeTabID)
	{
		SAssignNew(DirectoryTree, SDirectoryView);
		TabWidget = DirectoryTree;
	}
	else if (TabIdentifier == FileViewportTabID)
	{
		SAssignNew
		(
			FileViewport,
			SFileViewport,
		);

		TabWidget = FileViewport;
	}
	else if (TabIdentifier == CodeDetailsTabID)
	{
		SAssignNew(CodeDetails, SCodeDetails);
		TabWidget = CodeDetails;
	}
	else if (TabIdentifier == CodeStateTabID)
	{
		SAssignNew(CodeMgrState, SCodeMgrState);
		TabWidget = CodeMgrState;
	}

	DockTab->SetContent(TabWidget.ToSharedRef());

	return DockTab;
}

#undef LOCTEXT_NAMESPACE