// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "SimpleTestModule.h"
#include "SimpleTestStyle.h"
#include "SimpleTestCommands.h"
#include "LevelEditor.h"
#include "UICommandList.h"
#include "SDockTab.h"
#include "MultiBoxBuilder.h"
#include "SSimpleTestTreeView.h"
#include "TreeViewItem.h"
#include "SHeaderRow.h"
#include "SCheckBox.h"
#include "SButton.h"
#include "SimpleTestManager.h"
#include "SimpleTestDefine.h"

#define LOCTEXT_NAMESPACE "FSimpleTestModule"
static const FName OpenSimpleTestWindowTabName("SimpleTestTab");

void FSimpleTestModule::StartupModule()
{
	FSimpleTestStyle::Initialize();
	FSimpleTestStyle::ReloadTextures();
	FSimpleTestCommands::Register();

	PluginCommands = MakeShareable(new FUICommandList());

	PluginCommands->MapAction(
		FSimpleTestCommands::Get().OpenSimpleTestWindow,
		FExecuteAction::CreateRaw(this, &FSimpleTestModule::OpenWindow),
		FCanExecuteAction());

	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

	{
		TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());

		MenuExtender->AddMenuExtension("WindowLayout", EExtensionHook::Before, PluginCommands, FMenuExtensionDelegate::CreateRaw(this, &FSimpleTestModule::AddMenuExtension));
		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);

		LevelEditorModule.GetGlobalLevelEditorActions()->Append(PluginCommands.ToSharedRef());
	}

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(OpenSimpleTestWindowTabName, FOnSpawnTab::CreateRaw(this, &FSimpleTestModule::OnSpawnSimpleTestPluginTab))
		.SetDisplayName(LOCTEXT("FSimpleTestTabTitle", "Unit Test"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FSimpleTestModule::ShutdownModule()
{
	FSimpleTestStyle::Shutdown();

	FSimpleTestCommands::Unregister();
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(OpenSimpleTestWindowTabName);
}

void FSimpleTestModule::AddMenuExtension(FMenuBuilder& Builder)
{
	Builder.BeginSection("Unit Test", LOCTEXT("SimpleTestText", "Unit Test"));
	{
		Builder.AddMenuEntry(FSimpleTestCommands::Get().OpenSimpleTestWindow);
	}
	Builder.EndSection();
}

TSharedRef<class SDockTab> FSimpleTestModule::OnSpawnSimpleTestPluginTab(const class FSpawnTabArgs& SpawnTabArgs)
{
	const TSharedRef<SDockTab> DockTab = SNew(SDockTab).TabRole(MajorTab);

	TArray<TSharedPtr<FSlateTestData>> Commands;
	TArray<TSharedPtr<FCodeTestData>> TestPaths;
	FSimpleTestManager::Get()->GetCommands(Commands);
	FSimpleTestManager::Get()->GetTestPaths(TestPaths);

	TSharedRef<SSimpleTestMainFrame> MainFrame = SNew(SSimpleTestMainFrame, DockTab, SpawnTabArgs.GetOwnerWindow(), Commands, TestPaths)
		.OnGetDetailObject_Raw(this, &FSimpleTestModule::OnRefreshDetailObject)
		.OnRunTestItemClicked_Raw(this, &FSimpleTestModule::OnWorkAreaRunTestButtonClick)
		.OnGetCommandWidget_Raw(this, &FSimpleTestModule::OnGetCommandWidget)
		;
	DockTab->SetContent(MainFrame);

	return DockTab;
}

void FSimpleTestModule::OpenWindow()
{
	FGlobalTabmanager::Get()->InvokeTab(OpenSimpleTestWindowTabName);
}

UObject* FSimpleTestModule::OnRefreshDetailObject(const FString& TestPath)
{
	FCodeTest* Test = FSimpleTestManager::Get()->GetTest(TestPath);
	if (Test)
	{
		return Test->GetObject();
	}

	return nullptr;
}

void FSimpleTestModule::OnWorkAreaRunTestButtonClick(const FString TestPath)
{
	FCodeTest* Test = FSimpleTestManager::Get()->GetTest(TestPath);
	if (Test)
	{
		Test->RunTest();
	}
}

TSharedPtr<SWidget> FSimpleTestModule::OnGetCommandWidget(const FString& Command)
{
	return FSimpleTestManager::Get()->GetWidget(Command);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FSimpleTestModule, SimpleTest)