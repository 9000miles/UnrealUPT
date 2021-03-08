// Fill out your copyright notice in the Description page of Project Settings.

#include "SEngineContextMenu.h"
#include "SlateOptMacros.h"
#include "EditorStyleSet.h"
#include "PrintHelper.h"
#include "Widgets/UPTStyle.h"

#define LOCTEXT_NAMESPACE "SSEngineContextMenu"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SSEngineContextMenu::Construct(const FArguments& InArgs, const FString EnginePath)
{
	TSharedPtr<FExtender> Extender;
	TSharedPtr< const FUICommandList > Commands;

	const bool bInShouldCloseWindowAfterSelection = true;
	FMenuBuilder MenuBuilder(bInShouldCloseWindowAfterSelection, Commands, Extender, true);

	MenuBuilder.AddMenuEntry(
		LOCTEXT("OpenEngine", "Open Engine"),
		LOCTEXT("OpenEngineTip", "Open Engine"),
		FSlateIcon(FUPTStyle::GetStyleSetName(), "CodeBrowser.NewFolderIcon"),
		FUIAction(
			FExecuteAction::CreateLambda([EnginePath]() { PRINT_LOG(EnginePath); }),
			FCanExecuteAction()
		));

	ChildSlot
		[
			MenuBuilder.MakeWidget()
		];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE