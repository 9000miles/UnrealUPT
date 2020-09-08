#include "SContextMenu.h"
#include "ProjectInfo.h"
#include "UPTDefine.h"
#include "MultiBoxBuilder.h"
#include "MultiBoxExtender.h"
#include "Editor/EditorStyle/Public/EditorStyleSet.h"
#include "UIAction.h"
#include "UPTDefine.h"
#include "DelegateCenter.h"
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
			FExecuteAction::CreateLambda([this]() { FDelegateCenter::OnOpenProject.ExecuteIfBound(ProjectInfo.ToSharedRef()); }),
			FCanExecuteAction()
		));

	MenuBuilder.AddMenuEntry(
		LOCTEXT("GenerateSolution", "Generate Solution"),
		LOCTEXT("Generate SolutionTip", "Generate Solution Tip"),
		FSlateIcon(FEditorStyle::GetStyleSetName(), "ContentBrowser.NewFolderIcon"),
		FUIAction(
			FExecuteAction::CreateLambda([this]() { FDelegateCenter::OnGenerateSolution.ExecuteIfBound(ProjectInfo.ToSharedRef()); }),
			FCanExecuteAction()
		));

	MenuBuilder.AddMenuEntry(
		LOCTEXT("OpenIDE", "Open IDE"),
		LOCTEXT("OpenIdeTip", "Open IDE"),
		FSlateIcon(FEditorStyle::GetStyleSetName(), "ContentBrowser.NewFolderIcon"),
		FUIAction(
			FExecuteAction::CreateLambda([this]() { FDelegateCenter::OnOpenIDE.ExecuteIfBound(ProjectInfo.ToSharedRef()); }),
			FCanExecuteAction()
		));

	MenuBuilder.AddMenuEntry(
		LOCTEXT("Show In Explorer", "Show In Explorer"),
		LOCTEXT("Show In ExplorerTip", "Show In Explorer Tip"),
		FSlateIcon(FEditorStyle::GetStyleSetName(), "ContentBrowser.NewFolderIcon"),
		FUIAction(
			FExecuteAction::CreateLambda([this]() { FDelegateCenter::OnShowInExplorer.ExecuteIfBound(ProjectInfo.ToSharedRef()); }),
			FCanExecuteAction()
		));

	ChildSlot
		[
			MenuBuilder.MakeWidget()
		];
}

#undef LOCTEXT_NAMESPACE