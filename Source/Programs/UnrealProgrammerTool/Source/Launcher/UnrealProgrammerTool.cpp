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
#include "UPTManager.h"
#include "MultiBoxBuilder.h"
#include "Editor/EditorStyle/Public/EditorStyleSet.h"
#include "UPTStyle.h"
#include "UPTMenuBar.h"
#include "UPTDelegateCenter.h"
#include "UPTDefine.h"
#include "UPTToolBar.h"
#include "UPTCommands.h"

IMPLEMENT_APPLICATION(UnrealUPT, "UnrealProgrammerTool");

#define LOCTEXT_NAMESPACE "UnrealProgrammerTool"

namespace UPTMeun
{
	TSharedRef<FWorkspaceItem> UPTGroup = FWorkspaceItem::NewGroup(LOCTEXT("UPTGroup", "UPTGroup"));
}

void SetProjectDir()
{
	const FString ProjectDir = FString::Printf(TEXT("../../../UnrealUPT/Programs/%s/"), FApp::GetProjectName());
	FPlatformMisc::SetOverrideProjectDir(ProjectDir);
}

TSharedRef<SDockTab> SpawnMainTab(const FSpawnTabArgs& Args, FName TabIdentifier)
{
	if (TabIdentifier == FName(TEXT("UPTWindowTab")))
	{
		return SNew(SDockTab)
			.Label(LOCTEXT("UPTWindowTab", "UPT Window"))
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
	TSharedRef<SDockTab> UPTTab =
		SNew(SDockTab)
		.TabRole(ETabRole::MajorTab)
		.Label(LOCTEXT("UPTLabel", "UPT"))
		.ToolTipText(LOCTEXT("UPTTabToolTip", "A tool that unreal programmers really want to use."));

	UPTTabManager = FGlobalTabmanager::Get()->NewTabManager(UPTTab);

	UPTTab->SetContent
	(
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(FMargin(2, 0))
		[
			FUPTMenuBar::MakeMenuBar(UPTTabManager.ToSharedRef())
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(FMargin(2, 2))
		[
			FUPTToolBar::MakeUPTToolBar()
		]
		+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		[
			SAssignNew(UPTMainFrame, SUPTMainFrame, ProjectInfos)
		]
	);

	return UPTTab;
}

void OnRefreshMainFrame()
{
	check(UPTMainFrame.IsValid());

	ProjectInfos.Empty();
	ProjectInfos = FUPTManager::Get()->GetAllProjectInfos();

	UPTMainFrame->RequestRefresh(ProjectInfos);
}

void CreateMainFrameWindow()
{
	
	FGlobalTabmanager::Get()->RegisterTabSpawner("UPTMainWindow", FOnSpawnTab::CreateStatic(&SpawnMainWindown));

	TSharedRef<FTabManager::FLayout> Layout = FTabManager::NewLayout("UPT_Layout")
		->AddArea
		(
			FTabManager::NewArea(1080, 600)
#if PLATFORM_MAC
			->SetWindow(FVector2D(420, 32), false)
#else
			->SetWindow(FVector2D(420, 200), false)
			->SetOrientation(Orient_Horizontal)
#endif
			->Split
			(
				FTabManager::NewStack()
				->AddTab("UPTMainWindow", ETabState::OpenedTab)
			)
		)
		;


	FGlobalTabmanager::Get()->RestoreFrom(Layout, TSharedPtr<SWindow>());
}

void StartupMainFrame()
{
	FUPTManager::Get()->Initialize();
	ProjectInfos = FUPTManager::Get()->GetAllProjectInfos();
	CreateMainFrameWindow();

	FUPTDelegateCenter::OnRefresh.BindStatic(&OnRefreshMainFrame);
	FUPTDelegateCenter::OnExit.AddStatic(&OnExit);
}

void SetAppIcon()
{
	FSlateApplication::Get().SetAppIcon(FUPTStyle::Get().GetBrush("UPT.AppIcon"));
}

void OnExit()
{
	GIsRequestingExit = true;
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
	FGlobalTabmanager::Get()->SetApplicationTitle(LOCTEXT("AppTitle", "UnrealUPT"));
	FModuleManager::LoadModuleChecked<ISlateReflectorModule>("SlateReflector").RegisterTabSpawner(UPTMeun::UPTGroup);

	FEditorStyle::ResetToDefault();
	FUPTStyle::Initialize();
	FUPTStyle::ReloadTextures();
	FUPTCommands::Register();

	SetAppIcon();
	StartupMainFrame();

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

	FUPTStyle::Shutdown();
	FCoreDelegates::OnExit.Broadcast();
	FSlateApplication::Shutdown();
	FModuleManager::Get().UnloadModulesAtShutdown();

	return 0;
}

#undef LOCTEXT_NAMESPACE