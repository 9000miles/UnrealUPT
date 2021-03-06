#include "UPTToolBar.h"
#include "MultiBoxBuilder.h"
#include "UICommandInfo.h"
#include "UICommandList.h"
#include "UPTCommands.h"
#include "UPTDelegateCenter.h"
#include "UPTManager.h"

TSharedPtr<FUICommandList> FUPTToolBar::UPTCommandList;

TSharedRef<SWidget> FUPTToolBar::MakeUPTToolBar()
{
	UPTCommandList = MakeShareable(new FUICommandList);
	static const FName LevelEditorToolBarName = "UPTToolBar";

	UPTCommandList->MapAction(FUPTCommands::Get().Refresh, FExecuteAction::CreateLambda([]() { FUPTDelegateCenter::OnRefresh.ExecuteIfBound(); }));
	UPTCommandList->MapAction(FUPTCommands::Get().OpenCodeMgr, FExecuteAction::CreateStatic(&FUPTToolBar::OnOpenCodeMgrWindow));

	FToolBarBuilder ToolbarBuilder(UPTCommandList, FMultiBoxCustomization::AllowCustomization(LevelEditorToolBarName));
	ToolbarBuilder.AddToolBarButton(FUPTCommands::Get().Refresh, NAME_None, TAttribute<FText>(), TAttribute<FText>(), FSlateIcon(FUPTStyle::GetStyleSetName(), TEXT("UPT.ToolBar.Refresh")));
	//ToolbarBuilder.AddToolBarButton(FUPTCommands::Get().FoldAllArea, NAME_None, TAttribute<FText>(), TAttribute<FText>(), FSlateIcon(FUPTStyle::GetStyleSetName(), TEXT("UPT.ToolBar.FoldAllArea")));
	//ToolbarBuilder.AddToolBarButton(FUPTCommands::Get().ExpandAllArea, NAME_None, TAttribute<FText>(), TAttribute<FText>(), FSlateIcon(FUPTStyle::GetStyleSetName(), TEXT("UPT.ToolBar.ExpandAllArea")));

	ToolbarBuilder.AddToolBarButton(FUPTCommands::Get().OpenCodeMgr, NAME_None, TAttribute<FText>(), TAttribute<FText>(), FSlateIcon(FUPTStyle::GetStyleSetName(), TEXT("UPT.ToolBar.ExpandAllArea")));

	return ToolbarBuilder.MakeWidget();
}

void FUPTToolBar::OnOpenCodeMgrWindow()
{
	TSharedPtr<FProjectInfo> SelectedProject = FUPTManager::Get()->GetCurrentSelectedProject();
	if (SelectedProject.IsValid())
	{
		FUPTDelegateCenter::OnOpenCodeMgrWindow.ExecuteIfBound(SelectedProject.ToSharedRef());
	}
}

