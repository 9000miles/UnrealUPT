#include "SContextMenu.h"
#include "ProjectInfo.h"
#include "UPTDefine.h"
#include "MultiBoxBuilder.h"
#include "MultiBoxExtender.h"
#include "Editor/EditorStyle/Public/EditorStyleSet.h"
#include "UIAction.h"
#include "UPTDefine.h"
#include "UPTDelegateCenter.h"
#include "SBorder.h"
#include "SButton.h"

#define LOCTEXT_NAMESPACE "SContextMenu"

void SContextMenu::Construct(const FArguments& InArgs, TSharedPtr<FProjectInfo> Info)
{
	ProjectInfo = Info;

	TSharedPtr<FExtender> Extender;
	TSharedPtr< const FUICommandList > Commands;

	const bool bInShouldCloseWindowAfterSelection = true;
	FMenuBuilder MenuBuilder(bInShouldCloseWindowAfterSelection, Commands, Extender, true);

	MenuBuilder.AddMenuEntry(
		LOCTEXT("LaunchGame", "Launch Game"),
		LOCTEXT("LaunchGameTip", "Launch Game"),
		FSlateIcon(FEditorStyle::GetStyleSetName(), "CodeBrowser.NewFolderIcon"),
		FUIAction(
			FExecuteAction::CreateLambda([this]() { FUPTDelegateCenter::OnLaunchGame.ExecuteIfBound(ProjectInfo.ToSharedRef()); }),
			FCanExecuteAction()
		));

	MenuBuilder.AddMenuEntry(
		LOCTEXT("OpenProject", "Open Project"),
		LOCTEXT("OpenProjectTip", "Open Project"),
		FSlateIcon(FEditorStyle::GetStyleSetName(), "CodeBrowser.NewFolderIcon"),
		FUIAction(
			FExecuteAction::CreateLambda([this]() { FUPTDelegateCenter::OnOpenProject.ExecuteIfBound(ProjectInfo.ToSharedRef()); }),
			FCanExecuteAction()
		));

	MenuBuilder.AddMenuEntry(
		LOCTEXT("GenerateSolution", "Generate Solution"),
		LOCTEXT("Generate SolutionTip", "Generate Solution Tip"),
		FSlateIcon(FEditorStyle::GetStyleSetName(), "CodeBrowser.NewFolderIcon"),
		FUIAction(
			FExecuteAction::CreateLambda([this]() { FUPTDelegateCenter::OnGenerateSolution.ExecuteIfBound(ProjectInfo.ToSharedRef()); }),
			FCanExecuteAction()
		));

	MenuBuilder.AddMenuEntry(
		LOCTEXT("OpenIDE", "Open IDE"),
		LOCTEXT("OpenIdeTip", "Open IDE"),
		FSlateIcon(FEditorStyle::GetStyleSetName(), "CodeBrowser.NewFolderIcon"),
		FUIAction(
			FExecuteAction::CreateLambda([this]() { FUPTDelegateCenter::OnOpenIDE.ExecuteIfBound(ProjectInfo.ToSharedRef()); }),
			FCanExecuteAction()
		));

	MenuBuilder.AddMenuEntry(
		LOCTEXT("Clear Project", "Clear Project"),
		LOCTEXT("Clear ProjectTip", "Clear Project"),
		FSlateIcon(FEditorStyle::GetStyleSetName(), "CodeBrowser.NewFolderIcon"),
		FUIAction(
			FExecuteAction::CreateLambda([this]() { FUPTDelegateCenter::OnClearProject.ExecuteIfBound(ProjectInfo.ToSharedRef()); }),
			FCanExecuteAction()
		));

	MenuBuilder.AddMenuEntry(
		LOCTEXT("Show In Explorer", "Show In Explorer"),
		LOCTEXT("Show In ExplorerTip", "Show In Explorer Tip"),
		FSlateIcon(FEditorStyle::GetStyleSetName(), "CodeBrowser.NewFolderIcon"),
		FUIAction(
			FExecuteAction::CreateLambda([this]() { FUPTDelegateCenter::OnShowInExplorer.ExecuteIfBound(ProjectInfo.ToSharedRef()); }),
			FCanExecuteAction()
		));

	MenuBuilder.AddMenuEntry(
		LOCTEXT("Managed Code", "Managed Code"),
		LOCTEXT("Managed Code Tip", "Managed Code Tip"),
		FSlateIcon(FEditorStyle::GetStyleSetName(), "CodeBrowser.NewFolderIcon"),
		FUIAction(
			FExecuteAction::CreateLambda([this]() { FUPTDelegateCenter::OnOpenCodeMgrWindow.ExecuteIfBound(ProjectInfo.ToSharedRef()); }),
			FCanExecuteAction()
		));

	MenuBuilder.AddMenuEntry(
		LOCTEXT("Add Code File", "Add Code File"),
		LOCTEXT("Add Code File Tip", "Add Code File Tip"),
		FSlateIcon(FEditorStyle::GetStyleSetName(), "CodeBrowser.NewFolderIcon"),
		FUIAction(
			FExecuteAction::CreateLambda([this]() { FUPTDelegateCenter::OnAddNewCodeFile.ExecuteIfBound(ProjectInfo.ToSharedRef()); }),
			FCanExecuteAction()
		));

	ChildSlot
		[
			MenuBuilder.MakeWidget()
		];
}

#undef LOCTEXT_NAMESPACE