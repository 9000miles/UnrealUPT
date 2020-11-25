// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "FileViewport.h"
#include "SimpleTestDefine.h"
#include "TabManager.h"
#include "SDockTab.h"
#include "MGC_TextEditorWidget.h"
#include "MagicNodeEditorStyle.h"
#include "MGC_CodeEditorStyle.h"
#include "MGC_TextSyntaxHighlighter.h"
#include "FileHelper.h"
#include "SOverlay.h"
#include "SBoxPanel.h"
#include "SBox.h"
#include "EditorStyleSet.h"
#include "SScrollBox.h"
#include "SListView.h"
#include "SComboBox.h"
#include "SlateEnums.h"
#include "SlateApplication.h"
#include "MagicNodeEditor.h"

#define LOCTEXT_NAMESPACE "FFileView"


TSharedRef<SDockTab> SFileView::OnSpawnPTMToolTab(const FSpawnTabArgs& SpawnTabArgs)
{
	TSharedRef<SDockTab>TAB = SNew(SDockTab).TabRole(ETabRole::PanelTab);


	//
	//
	return TAB;
}


void SFileView::Construct(const FArguments& InArgs)
{
			TSharedPtr<SScrollBar> VS_SCROLL;
	TSharedPtr<SScrollBar>	HS_SCROLL;

	TSharedPtr<FMGC_TextSyntaxHighlighter>MARSHALL = FMGC_TextSyntaxHighlighter::Create(
		FMGC_TextSyntaxHighlighter::FSyntaxTextStyle(/*KeywordDB, ClassDB, FunctionDB, SemanticDB*/)
	);///

	VS_SCROLL = SNew(SScrollBar)
		//.OnUserScrolled_Lambda([](float Offset) {v})
		.Thickness(FVector2D(8.f, 8.f)).AlwaysShowScrollbar(false)
		.Orientation(Orient_Vertical);
	//
	HS_SCROLL = SNew(SScrollBar)
		//.OnUserScrolled(this, &SMGC_CodeEditorCore::OnScriptHorizontalScroll)
		.Orientation(Orient_Horizontal).AlwaysShowScrollbar(false)
		.Thickness(FVector2D(8.f, 8.f));

	TSharedPtr<SOverlay>OverlayWidget;
	ChildSlot
		[

	//TSharedRef<SMGC_TextEditorWidget> Wi = SNew(SMGC_TextEditorWidget)
	//	//.OnTextChanged(this, &SKMGC_MagicNodeWidget::OnScriptTextChanged, ETextCommit::Default)
	//	//.OnTextCommitted(this, &SKMGC_MagicNodeWidget::OnScriptTextComitted)
	//	//.Visibility(this, &SKMGC_MagicNodeWidget::GetScriptPanelVisibility)
	//	//.OnInvokeSearch(this, &SKMGC_MagicNodeWidget::OnInvokedSearch)
	//	//.OnAutoComplete(this, &SKMGC_MagicNodeWidget::OnAutoComplete)
	//	//.IsEnabled(this, &SKMGC_MagicNodeWidget::HasScript)
	//	.Text(FText::FromString(Script))
	//	.VScrollBar(VS_SCROLL).HScrollBar(HSS_SCROLL)
	//	.Marshaller(MARSHALL.ToSharedRef())
	//	.CanKeyboardFocus(true)
	//	.IsReadOnly(false);


	SAssignNew(OverlayWidget, SOverlay)
		+ SOverlay::Slot()
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
		[
			SNew(SBox)
			.VAlign(VAlign_Fill).HAlign(HAlign_Fill)
		.MinDesiredWidth(500.f).MinDesiredHeight(300.f)
		[
			SNew(SBorder)
			.VAlign(VAlign_Fill).HAlign(HAlign_Fill)
		.BorderImage(FEditorStyle::GetBrush("ToolPanel.DarkGroupBorder"))
		[
			SAssignNew(VS_SCROLL_BOX, SScrollBox)
			//.OnUserScrolled(this, &SMGC_CodeEditorCore::OnVerticalScroll)
		.Orientation(EOrientation::Orient_Vertical)
		.ScrollBarThickness(FVector2D(8.f, 8.f))
		+ SScrollBox::Slot()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		.VAlign(VAlign_Fill).HAlign(HAlign_Left).AutoWidth()
		[
			SNew(SBorder)
			.VAlign(VAlign_Fill).HAlign(HAlign_Fill)
		.BorderImage(FEditorStyle::GetBrush("Graph.Node.Body"))
		[
			SAssignNew(LINE_COUNTER, SListView<TSharedPtr<FString>>)
			.OnSelectionChanged(this, &SFileView::OnSelectedLineCounterItem)
		.OnGenerateRow(this, &SFileView::OnGenerateLineCounter)
		.ScrollbarVisibility(EVisibility::Collapsed)
		.ListItemsSource(&LineCount).ItemHeight(14)
		.SelectionMode(ESelectionMode::Single)
		]
		]
	+ SHorizontalBox::Slot()
		.VAlign(VAlign_Fill).HAlign(HAlign_Fill)
		[
			SAssignNew(SCRIPT_EDITOR, SMGC_TextEditorWidget)
			//.OnTextChanged(this, &SMGC_CodeEditorCore::OnScriptTextChanged, ETextCommit::Default)
			//.OnTextCommitted(this, &SMGC_CodeEditorCore::OnScriptTextComitted)
			//.IsEnabled(this, &SMGC_CodeEditorCore::IsScriptEditable)
		.Text(ScriptText)
		.VScrollBar(VS_SCROLL).HScrollBar(HS_SCROLL)
		.Marshaller(MARSHALL.ToSharedRef())
		.CanKeyboardFocus(true)
		.IsReadOnly(false)
		]
		]
		]
		]
		]
		]

		];
}


TSharedRef<ITableRow> SFileView::OnGenerateLineCounter(TSharedPtr<FString>Item, const TSharedRef<STableViewBase>& OwnerTable) {
	return
		SNew(SComboRow<TSharedRef<FString>>, OwnerTable)
		[
		//	SNew(SBorder)
		//	.BorderImage(FEditorStyle::GetBrush("Graph.Node.Body"))
		//.ColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 1.f))
		//.Padding(FMargin(5.f, 0.f, 5.f, 0.f))
		//[
			SNew(STextBlock)
			.Text(FText::FromString(*Item.Get()))
		.ColorAndOpacity(FSlateColor(FLinearColor(FColor(75, 185, 245, 225))))
		.Font(FMGC_CodeEditorStyle::Get().Get()->GetWidgetStyle<FTextBlockStyle>("MGC.CodeBlockStyle").Font)
		//]
		];//
}

void SFileView::OnSelectedLineCounterItem(TSharedPtr<FString>Item, ESelectInfo::Type SelectInfo) {
	if (!Item.IsValid()) { return; }
	//
	const int32 LineID = FCString::Atoi(**Item.Get());
	//
	FSlateApplication::Get().SetKeyboardFocus(SCRIPT_EDITOR.ToSharedRef());
	SCRIPT_EDITOR->GoToLineColumn(LineID - 1, 0);
	SCRIPT_EDITOR->SelectLine();
	//
	LINE_COUNTER->SetItemSelection(Item, false);
}
