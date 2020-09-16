#include "SSimpleTestMainFrame.h"
#include "SBoxPanel.h"
#include "SCodeTestArea.h"
#include "SSlateTestArea.h"
#include "EditorStyleSet.h"
#include "UICommandList.h"
#include "MultiBoxBuilder.h"
#include "SCommandArea.h"
#include "SWorkArea.h"
#include "SDetailArea.h"
#include "SButton.h"
#include "SHyperlink.h"
#include "SlateApplication.h"
#include "SimpleTestManager.h"
#include "SCommandItem.h"

#define LOCTEXT_NAMESPACE "SMainFrame"

static const FName CommandTabId("Command");
static const FName WorkAreaTabId("WorkArea");
static const FName TestDetialTabId("TestDetial");

void SSimpleTestMainFrame::Construct(const FArguments& InArgs, const TSharedRef<SDockTab>& ConstructUnderMajorTab, const TSharedPtr<SWindow>& ConstructUnderWindow, TArray<TSharedPtr<FSlateTestData>> Commands, TArray<TSharedPtr<FCodeTestData>> TestPaths)
{
	OnCommandClick = InArgs._OnCommandClick;
	OnGetDetailObject = InArgs._OnGetDetailObject;
	OnGetCommandWidget = InArgs._OnGetCommandWidget;
	OnRunTestItemClicked = InArgs._OnRunTestItemClicked;

	TabManager = FGlobalTabmanager::Get()->NewTabManager(ConstructUnderMajorTab);
	TSharedRef<FWorkspaceItem> AppMenuGroup = TabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("SimpleTestMainFrameMenuGroupName", "Unit Test"));

	TabManager->RegisterTabSpawner(CommandTabId, FOnSpawnTab::CreateRaw(this, &SSimpleTestMainFrame::HandleTabManagerSpawnTab, CommandTabId))
		.SetDisplayName(LOCTEXT("CommandTabTitle", "Command"))
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "SessionFrontEnd.Tabs.Tools"))
		.SetGroup(AppMenuGroup);

	TabManager->RegisterTabSpawner(WorkAreaTabId, FOnSpawnTab::CreateRaw(this, &SSimpleTestMainFrame::HandleTabManagerSpawnTab, WorkAreaTabId))
		.SetDisplayName(LOCTEXT("WorkAreaTabTitle", "Work Area"))
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "SessionFrontEnd.Tabs.Tools"))
		.SetGroup(AppMenuGroup);

	TabManager->RegisterTabSpawner(TestDetialTabId, FOnSpawnTab::CreateRaw(this, &SSimpleTestMainFrame::HandleTabManagerSpawnTab, TestDetialTabId))
		.SetDisplayName(LOCTEXT("TestDetialTabTitle", "Detial"))
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "SessionFrontEnd.Tabs.Tools"))
		.SetGroup(AppMenuGroup);

	// create tab layout
	const TSharedRef<FTabManager::FLayout> Layout = FTabManager::NewLayout("SimpleTestLayout")
		->AddArea
		(
			FTabManager::NewPrimaryArea()
			->SetOrientation(Orient_Horizontal)
			->Split
			(
				// session browser
				FTabManager::NewStack()
				->AddTab(CommandTabId, ETabState::OpenedTab)
				->SetSizeCoefficient(0.25f)
			)
			->Split
			(
				// applications
				FTabManager::NewStack()
				->AddTab(WorkAreaTabId, ETabState::OpenedTab)
				->SetSizeCoefficient(0.5f)
			)
			->Split
			(
				// applications
				FTabManager::NewStack()
				->AddTab(TestDetialTabId, ETabState::OpenedTab)
				->SetSizeCoefficient(0.25f)
			)
		);

	// create & initialize main menu
	FMenuBarBuilder MenuBarBuilder = FMenuBarBuilder(TSharedPtr<FUICommandList>());

	MenuBarBuilder.AddPullDownMenu
	(
		LOCTEXT("WindowMenuLabel", "Window"),
		FText::GetEmpty(),
		FNewMenuDelegate::CreateSP(this, &SSimpleTestMainFrame::FillWindowMenu, TabManager),
		"Window"
	);

	ChildSlot
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				MenuBarBuilder.MakeWidget()
			]
			+ SVerticalBox::Slot()
			.FillHeight(1.0f)
			[
				TabManager->RestoreFrom(Layout, ConstructUnderWindow).ToSharedRef()
			]
		];

	// Tell tab-manager about the multi-box for platforms with a global menu bar
	TabManager->SetMenuMultiBox(MenuBarBuilder.GetMultiBox());

	InitCommandArea(Commands);
	InitWorkArea(TestPaths);
}

void SSimpleTestMainFrame::InitCommandArea(TArray<TSharedPtr<FSlateTestData>> CommandNames)
{
	for (TSharedPtr<FSlateTestData> Data : CommandNames)
	{
		check(Data.IsValid());

		TSharedRef<SWidget> Command = SNew(SCommandItem, Data->DisplayName, Data->FileName, Data->LineNumber)
			.OnCommandClick_Lambda([this](const FString InDisplayName)
		{
			if (WorkArea.IsValid())
				WorkArea->SwitchArea(false);

			if (OnCommandClick.IsBound())
				OnCommandClick.ExecuteIfBound(InDisplayName);

			if (OnGetCommandWidget.IsBound())
			{
				TSharedPtr<SWidget> Widget = OnGetCommandWidget.Execute(InDisplayName);
				WorkArea->SetSlateTestWidget(Widget);
			}
		});

		CommandArea->AddCommandWidget(Command);
	}
}

void SSimpleTestMainFrame::InitWorkArea(TArray<TSharedPtr<FCodeTestData>> TestPaths)
{
	WorkArea->SetCodeTestPaths(TestPaths);
}

TSharedRef<SDockTab> SSimpleTestMainFrame::HandleTabManagerSpawnTab(const FSpawnTabArgs& Args, FName TabIdentifier)
{
	TSharedPtr<SWidget> TabWidget = SNullWidget::NullWidget;

	TSharedRef<SDockTab> DockTab = SNew(SDockTab)
		.TabRole(ETabRole::PanelTab);

	if (TabIdentifier == CommandTabId)
	{
		SAssignNew(CommandArea, SCommandArea)
			.OnSwitchedToCodeTestClicked_Lambda([this]()
		{
			if (WorkArea.IsValid())
				WorkArea->SwitchArea(true);
		})
			;
		TabWidget = CommandArea;
	}
	else if (TabIdentifier == WorkAreaTabId)
	{
		SAssignNew
		(
			WorkArea,
			SWorkArea,
			FOnTestRowClick::CreateSP(this, &SSimpleTestMainFrame::OnRefreshDetail),
			FOnRunTestItemClicked::CreateSP(this, &SSimpleTestMainFrame::OnRunTestButtonClick)
		);

		TabWidget = WorkArea;
	}
	else if (TabIdentifier == TestDetialTabId)
	{
		SAssignNew(DetailArea, SDetailArea);
		TabWidget = DetailArea;
	}

	DockTab->SetContent(TabWidget.ToSharedRef());

	return DockTab;
}

void SSimpleTestMainFrame::FillWindowMenu(FMenuBuilder& MenuBuilder, const TSharedPtr<class FTabManager> InTabManager)
{
	if (!InTabManager.IsValid())
		return;

	InTabManager->PopulateLocalTabSpawnerMenu(MenuBuilder);
}

void SSimpleTestMainFrame::OnRefreshDetail(const FString TestPath)
{
	if (OnGetDetailObject.IsBound())
	{
		UObject* Object = OnGetDetailObject.Execute(TestPath);
		DetailArea->SetObject(Object);
	}
}

void SSimpleTestMainFrame::OnRunTestButtonClick(const FString TestPath)
{
	if (OnRunTestItemClicked.IsBound())
	{
		OnRunTestItemClicked.ExecuteIfBound(TestPath);
	}
}

#undef LOCTEXT_NAMESPACE