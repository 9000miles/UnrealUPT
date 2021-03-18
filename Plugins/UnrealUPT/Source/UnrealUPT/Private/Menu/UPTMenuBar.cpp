#include "UPTMenuBar.h"
#include "MultiBoxBuilder.h"
#include "UICommandList.h"
#include "TabManager.h"
#include "SlateApplication.h"
#include "UPTDelegateCenter.h"

#define LOCTEXT_NAMESPACE "FUPTMenuBar"

namespace UPTMeun
{
	TSharedRef<FWorkspaceItem> UPTMenuRoot = FWorkspaceItem::NewGroup(LOCTEXT("UPTMenuRoot", "UPTMenuRoot"));
}

TSharedRef<SWidget> FUPTMenuBar::MakeMenuBar(/*TSharedRef<FTabManager> TabManager*/)
{
	FMenuBarBuilder MenuBarBuilder = FMenuBarBuilder(TSharedPtr<FUICommandList>());

	MenuBarBuilder.AddPullDownMenu(LOCTEXT("FileLabel", "File"), FText::GetEmpty(), FNewMenuDelegate::CreateStatic(&FUPTMenuBar::FillFileMenuBarEntries));
	MenuBarBuilder.AddPullDownMenu(LOCTEXT("WindowsLabel", "Windows"), FText::GetEmpty(), FNewMenuDelegate::CreateStatic(&FUPTMenuBar::FillWindowsMenuBarEntries));
	MenuBarBuilder.AddPullDownMenu(LOCTEXT("HelpLabel", "Help"), FText::GetEmpty(), FNewMenuDelegate::CreateStatic(&FUPTMenuBar::FillHelpMenuBarEntries));

	return MenuBarBuilder.MakeWidget();
}

void FUPTMenuBar::FillFileMenuBarEntries(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddSubMenu(LOCTEXT("Recent Projects", "Recent Projects"), LOCTEXT("Recent Projects Tip", "Recently opened project"), FNewMenuDelegate::CreateLambda(&FUPTMenuBar::FillRecentProjectsSubMenu));
	MenuBuilder.AddMenuEntry(LOCTEXT("Refresh", "Refresh"), LOCTEXT("Refresh Tip", "Regather the engine and project"), FSlateIcon(), FUIAction(FExecuteAction::CreateStatic(&FUPTMenuBar::OnRefresh)));
	MenuBuilder.AddMenuEntry(LOCTEXT("Exit", "Exit"), LOCTEXT("Exit Tip", "Exit this program"), FSlateIcon(), FUIAction(FExecuteAction::CreateStatic(&FUPTMenuBar::OnExit)));
}

void FUPTMenuBar::FillWindowsMenuBarEntries(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuEntry(LOCTEXT("Widget Reflector", "Widget Reflector"), LOCTEXT("Widget Reflector Tip", "Widget Reflector"), FSlateIcon(), FUIAction(FExecuteAction::CreateLambda([]() { FGlobalTabmanager::Get()->TryInvokeTab(FTabId("WidgetReflector")); })));
	MenuBuilder.AddMenuEntry(LOCTEXT("UPT Tool", "UPT Tool"), LOCTEXT("UPT Tool Tip", "UPT Tool"), FSlateIcon(), FUIAction(FExecuteAction::CreateLambda([]() { FGlobalTabmanager::Get()->TryInvokeTab(FTabId("UPTTool")); })));
	MenuBuilder.AddMenuEntry(LOCTEXT("Settings", "Settings"), LOCTEXT("Settings Tip", "Settings"), FSlateIcon(), FUIAction(FExecuteAction::CreateLambda([]() { FGlobalTabmanager::Get()->TryInvokeTab(FTabId("UPTSettings")); })));
	MenuBuilder.AddMenuEntry(LOCTEXT("Engine information", "Engine information"), LOCTEXT("Engine information Tip", "Engine information"), FSlateIcon(), FUIAction(FExecuteAction::CreateLambda([]() { FGlobalTabmanager::Get()->TryInvokeTab(FTabId("EngineInfomation")); })));
	MenuBuilder.AddMenuEntry(LOCTEXT("Engine Projects", "Engine Projects"), LOCTEXT("Engine Projects Tip", "Engine Projects"), FSlateIcon(), FUIAction(FExecuteAction::CreateLambda([]() { FGlobalTabmanager::Get()->TryInvokeTab(FTabId("UPTWindowTab")); })));
	MenuBuilder.AddMenuEntry(LOCTEXT("Common Application", "Common Application"), LOCTEXT("Common Application Tip", "Common Application"), FSlateIcon(), FUIAction(FExecuteAction::CreateLambda([]() { FGlobalTabmanager::Get()->TryInvokeTab(FTabId("CommonApplication")); })));
	MenuBuilder.AddMenuEntry(LOCTEXT("Output Log", "Output Log"), LOCTEXT("Output Log Tip", "Output Log"), FSlateIcon(), FUIAction(FExecuteAction::CreateLambda([]() { FGlobalTabmanager::Get()->TryInvokeTab(FTabId("OutputLogPlus")); })));
}

void FUPTMenuBar::FillHelpMenuBarEntries(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuEntry(LOCTEXT("Documentation", "Documentation"), LOCTEXT("Documentation Tip", "Documentation"), FSlateIcon(), FUIAction(FExecuteAction::CreateLambda([]() { FGlobalTabmanager::Get()->TryInvokeTab(FTabId("Documentation")); })));
	MenuBuilder.AddMenuEntry(LOCTEXT("About UPT", "About UPT"), LOCTEXT("About UPT Tip", "About UPT"), FSlateIcon(), FUIAction(FExecuteAction::CreateLambda([]() { FGlobalTabmanager::Get()->TryInvokeTab(FTabId("AboutUPT")); })));
}

void FUPTMenuBar::FillRecentProjectsSubMenu(FMenuBuilder& MenuBuilder)
{
}

void FUPTMenuBar::OnRefresh()
{
	if (FUPTDelegateCenter::OnRefresh.IsBound())
	{
		FUPTDelegateCenter::OnRefresh.ExecuteIfBound();
	}
}

void FUPTMenuBar::OnExit()
{
	if (FUPTDelegateCenter::OnExit.IsBound())
	{
		FUPTDelegateCenter::OnExit.Broadcast();
	}
}

#undef LOCTEXT_NMACESPACE