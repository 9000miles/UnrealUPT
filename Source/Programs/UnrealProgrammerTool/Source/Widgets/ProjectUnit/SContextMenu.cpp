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
		LOCTEXT("OpenProject", "Open Project"),
		LOCTEXT("OpenProjectTip", "Open Project"),
		FSlateIcon(FEditorStyle::GetStyleSetName(), "ContentBrowser.NewFolderIcon"),
		FUIAction(
			FExecuteAction::CreateLambda([this]() { FUPTDelegateCenter::OnOpenProject.ExecuteIfBound(ProjectInfo.ToSharedRef()); }),
			FCanExecuteAction()
		));

	MenuBuilder.AddMenuEntry(
		LOCTEXT("GenerateSolution", "Generate Solution"),
		LOCTEXT("Generate SolutionTip", "Generate Solution Tip"),
		FSlateIcon(FEditorStyle::GetStyleSetName(), "ContentBrowser.NewFolderIcon"),
		FUIAction(
			FExecuteAction::CreateLambda([this]() { FUPTDelegateCenter::OnGenerateSolution.ExecuteIfBound(ProjectInfo.ToSharedRef()); }),
			FCanExecuteAction()
		));

	MenuBuilder.AddMenuEntry(
		LOCTEXT("OpenIDE", "Open IDE"),
		LOCTEXT("OpenIdeTip", "Open IDE"),
		FSlateIcon(FEditorStyle::GetStyleSetName(), "ContentBrowser.NewFolderIcon"),
		FUIAction(
			FExecuteAction::CreateLambda([this]() { FUPTDelegateCenter::OnOpenIDE.ExecuteIfBound(ProjectInfo.ToSharedRef()); }),
			FCanExecuteAction()
		));

	MenuBuilder.AddMenuEntry(
		LOCTEXT("Clear Solution", "Clear Solution"),
		LOCTEXT("Clear SolutionTip", "Clear Solution"),
		FSlateIcon(FEditorStyle::GetStyleSetName(), "ContentBrowser.NewFolderIcon"),
		FUIAction(
			FExecuteAction::CreateLambda([this]() { FUPTDelegateCenter::OnClearSolution.ExecuteIfBound(ProjectInfo.ToSharedRef()); }),
			FCanExecuteAction()
		));

	MenuBuilder.AddMenuEntry(
		LOCTEXT("Show In Explorer", "Show In Explorer"),
		LOCTEXT("Show In ExplorerTip", "Show In Explorer Tip"),
		FSlateIcon(FEditorStyle::GetStyleSetName(), "ContentBrowser.NewFolderIcon"),
		FUIAction(
			FExecuteAction::CreateLambda([this]() { FUPTDelegateCenter::OnShowInExplorer.ExecuteIfBound(ProjectInfo.ToSharedRef()); }),
			FCanExecuteAction()
		));

	MenuBuilder.AddMenuEntry(
		LOCTEXT("Managed Code", "Managed Code"),
		LOCTEXT("Managed Code Tip", "Managed Code Tip"),
		FSlateIcon(FEditorStyle::GetStyleSetName(), "ContentBrowser.NewFolderIcon"),
		FUIAction(
			FExecuteAction::CreateLambda([this]() { FUPTDelegateCenter::OnOpenCodeMgrWindow.ExecuteIfBound(ProjectInfo.ToSharedRef()); }),
			FCanExecuteAction()
		));

	ChildSlot
		[
			MenuBuilder.MakeWidget()
		];
}

#undef LOCTEXT_NAMESPACE