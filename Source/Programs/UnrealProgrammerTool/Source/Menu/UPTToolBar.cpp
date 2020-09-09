#include "UPTToolBar.h"
#include "MultiBoxBuilder.h"
#include "UICommandInfo.h"
#include "UICommandList.h"
#include "UPTCommands.h"

TSharedPtr<FUICommandList> FUPTToolBar::UPTCommandList;

TSharedRef<SWidget> FUPTToolBar::MakeUPTToolBar()
{
	UPTCommandList = MakeShareable(new FUICommandList);
	static const FName LevelEditorToolBarName = "UPTToolBar";

	FToolBarBuilder ToolbarBuilder(UPTCommandList, FMultiBoxCustomization::AllowCustomization(LevelEditorToolBarName));
	ToolbarBuilder.AddToolBarButton(FUPTCommands::Get().Refresh, NAME_None, TAttribute<FText>(), TAttribute<FText>(), FSlateIcon(FUPTStyle::GetStyleSetName(), TEXT("UPT.Refresh")));
	ToolbarBuilder.AddToolBarButton(FUPTCommands::Get().FoldAllArea, NAME_None, TAttribute<FText>(), TAttribute<FText>(), FSlateIcon(FUPTStyle::GetStyleSetName(), TEXT("UPT.FoldAllArea")));
	ToolbarBuilder.AddToolBarButton(FUPTCommands::Get().ExpandAllArea, NAME_None, TAttribute<FText>(), TAttribute<FText>(), FSlateIcon(FUPTStyle::GetStyleSetName(), TEXT("UPT.ExpandAllArea")));

	return ToolbarBuilder.MakeWidget();
}


