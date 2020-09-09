// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "UnrealProgrammerTool.h"
#include "Launch/Public/RequiredProgramMainCPPInclude.h"
#include "Framework/Application/SlateApplication.h"
#include "Modules/ModuleManager.h"
#include "ISlateReflectorModule.h"
#include "Widgets/Docking/SDockTab.h"
#include "Slate/Public/Framework/Docking/WorkspaceItem.h"
#include "Slate/Public/Framework/Docking/TabManager.h"
#include "StandaloneRenderer/Public/StandaloneRenderer.h"
#include "ISourceCodeAccessModule.h"
#include "ProjectInfo.h"
#include "SUPTMainFrame.h"
#include "EPManager.h"
#include "MultiBoxBuilder.h"
#include "Editor/EditorStyle/Public/EditorStyleSet.h"
//#include "EPManager.h"
//#include "SUPTMainFrame.h"
//#include "MultiBoxBuilder.h"

DEFINE_LOG_CATEGORY_STATIC(LogUnrealProgrammerTool, Log, All);

IMPLEMENT_APPLICATION(UnrealProgrammerTool, "UnrealProgrammerTool");

#define LOCTEXT_NAMESPACE "UnrealProgrammerTool"


static TArray<TSharedPtr<FProjectInfo>> ProjectInfos;
static TSharedPtr<FTabManager> TestSuite1TabManager;

namespace UPTMeun
{
	TSharedRef<FWorkspaceItem> UPTGroup = FWorkspaceItem::NewGroup(LOCTEXT("UPTGroup", "UPTGroup"));
	TSharedRef<FWorkspaceItem> UPTMenuRoot = FWorkspaceItem::NewGroup(LOCTEXT("UPTMenuRoot", "UPTMenuRoot"));
}

void SetProjectDir()
{
	const FString ProjectDir = FString::Printf(TEXT("../../../Unreal_UPT/Programs/%s/"), FApp::GetProjectName());
	FPlatformMisc::SetOverrideProjectDir(ProjectDir);
}

TSharedRef<SDockTab> SpawnMainTab(const FSpawnTabArgs& Args, FName TabIdentifier)
{
	if (TabIdentifier == FName(TEXT("UPTWindowTab")))
	{
		return SNew(SDockTab)
			.Label(LOCTEXT("UPTWindowTab", "Layout Example"))
			.ToolTipText(LOCTEXT("UPTWindowTabToolTip", "Switches to the Layout Example tab, which shows off examples of various Slate layout primitives."))
			.Clipping(EWidgetClipping::ClipToBounds)
			[
				SNew(SUPTMainFrame, ProjectInfos)
			];
	}

	return SNew(SDockTab);
}

TSharedRef<SDockTab> SpawnMainWindown(const FSpawnTabArgs& Args)
{
	TSharedRef<FTabManager::FLayout> Layout = FTabManager::NewLayout("UPTMainWindow_Layout")
		->AddArea
		(
			// The primary area will be restored and returned as a widget.
			// Unlike other areas it will not get its own window.
			// This allows the caller to explicitly place the primary area somewhere in the widget hierarchy.
			FTabManager::NewPrimaryArea()
			->Split
			(
				//The first cell in the primary area will be occupied by a stack of tabs.
				// They are all opened.
				FTabManager::NewStack()
				->SetSizeCoefficient(0.35f)
				->AddTab("UPTWindowTab", ETabState::OpenedTab)
			)
		)
		;



	TSharedRef<SDockTab> TestSuite1Tab =
		SNew(SDockTab)
		.TabRole(ETabRole::MajorTab)
		.Label(LOCTEXT("TestSuite1TabLabel", "Test Suite 1"))
		.ToolTipText(LOCTEXT("TestSuite1TabToolTip", "The App for the first Test Suite."));


	TestSuite1TabManager = FGlobalTabmanager::Get()->NewTabManager(TestSuite1Tab);
	TestSuite1TabManager->RegisterTabSpawner("UPTWindowTab", FOnSpawnTab::CreateStatic(&SpawnMainTab, FName("UPTWindowTab")))
		.SetDisplayName(NSLOCTEXT("TestSuite1", "UPTWindowTab", "Layout Example"))
		.SetGroup(UPTMeun::UPTGroup);
	   
	FMenuBarBuilder MenuBarBuilder = FMenuBarBuilder(TSharedPtr<FUICommandList>());
	MenuBarBuilder.AddPullDownMenu(
		NSLOCTEXT("TestSuite", "WindowMenuLabel", "Window"),
		FText::GetEmpty(),
		FNewMenuDelegate::CreateSP(TestSuite1TabManager.ToSharedRef(), &FTabManager::PopulateTabSpawnerMenu, UPTMeun::UPTMenuRoot));

	TestSuite1Tab->SetContent
	(
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			MenuBarBuilder.MakeWidget()
		]
	+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		[
			TestSuite1TabManager->RestoreFrom(Layout, Args.GetOwnerWindow()).ToSharedRef()
		]
	);

	return TestSuite1Tab;

}

void CreateMainFrameWindow()
{
	// Need to load this module so we have the widget reflector tab available
	FModuleManager::LoadModuleChecked<ISlateReflectorModule>("SlateReflector");

	//FTestStyle::ResetToDefault();
	//TSharedPtr<SUPTMainFrame> MainFrame = SNew(SUPTMainFrame, ProjectInfos);

	FGlobalTabmanager::Get()->RegisterTabSpawner("UPTMainWindow", FOnSpawnTab::CreateStatic(&SpawnMainWindown));

	TSharedRef<FTabManager::FLayout> Layout = FTabManager::NewLayout("UPT_Layout")
		->AddArea
		(
			FTabManager::NewArea(1080, 600)
#if PLATFORM_MAC
			->SetWindow(FVector2D(420, 32), false)
#else
			->SetWindow(FVector2D(420, 10), false)
#endif
			->Split
			(
				FTabManager::NewStack()
				->AddTab("UPTMainWindow", ETabState::OpenedTab)
			)
		)
#if PLATFORM_SUPPORTS_MULTIPLE_NATIVE_WINDOWS
		->AddArea
		(
			// This area will get a 400x600 window at 10,10
			FTabManager::NewArea(400, 600)
#if PLATFORM_MAC
			->SetWindow(FVector2D(10, 32), false)
#else
			->SetWindow(FVector2D(10, 10), false)
#endif
			->Split
			(
				// The area contains a single tab with the widget reflector.
				FTabManager::NewStack()->AddTab("WidgetReflector", ETabState::OpenedTab)
			)
		)
#endif
		;


	FGlobalTabmanager::Get()->RestoreFrom(Layout, TSharedPtr<SWindow>());
}

void StartupMainFrame()
{
	FEPManager::Get()->Initialize();
	ProjectInfos = FEPManager::Get()->GetAllProjectInfos();
	CreateMainFrameWindow();
}

int WINAPI WinMain(_In_ HINSTANCE hInInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR, _In_ int nCmdShow)
{
	SetProjectDir();

	GEngineLoop.PreInit(GetCommandLineW());

	// Make sure all UObject classes are registered and default properties have been initialized
	ProcessNewlyLoadedUObjects();

	// Tell the module manager it may now process newly-loaded UObjects when new C++ modules are loaded
	FModuleManager::Get().StartProcessingNewlyLoadedObjects();

	// crank up a normal Slate application using the platform's standalone renderer
	FSlateApplication::InitializeAsStandaloneApplication(GetStandardStandaloneRenderer());

	// Load the source code access module
	ISourceCodeAccessModule& SourceCodeAccessModule = FModuleManager::LoadModuleChecked<ISourceCodeAccessModule>(FName("SourceCodeAccess"));

	// Manually load in the source code access plugins, as standalone programs don't currently support plugins.
#if PLATFORM_MAC
	IModuleInterface& XCodeSourceCodeAccessModule = FModuleManager::LoadModuleChecked<IModuleInterface>(FName("XCodeSourceCodeAccess"));
	SourceCodeAccessModule.SetAccessor(FName("XCodeSourceCodeAccess"));
#elif PLATFORM_WINDOWS
	IModuleInterface& VisualStudioSourceCodeAccessModule = FModuleManager::LoadModuleChecked<IModuleInterface>(FName("VisualStudioSourceCodeAccess"));
	SourceCodeAccessModule.SetAccessor(FName("VisualStudioSourceCodeAccess"));
#endif

	// set the application name
	FGlobalTabmanager::Get()->SetApplicationTitle(LOCTEXT("AppTitle", "Unreal Programmer Tool"));
	FModuleManager::LoadModuleChecked<ISlateReflectorModule>("SlateReflector").RegisterTabSpawner(UPTMeun::UPTGroup);

	FEditorStyle::ResetToDefault();

	StartupMainFrame();

	//FGlobalTabmanager::Get()->InvokeTab(FTabId("WidgetReflector"));

	// loop while the server does the rest
	while (!GIsRequestingExit)
	{
		FTaskGraphInterface::Get().ProcessThreadUntilIdle(ENamedThreads::GameThread);
		FStats::AdvanceFrame(false);
		FTicker::GetCoreTicker().Tick(FApp::GetDeltaTime());
		FSlateApplication::Get().PumpMessages();
		FSlateApplication::Get().Tick();
		FPlatformProcess::Sleep(0);
	}

	FCoreDelegates::OnExit.Broadcast();
	FSlateApplication::Shutdown();
	FModuleManager::Get().UnloadModulesAtShutdown();

	return 0;
}

#undef LOCTEXT_NAMESPACE