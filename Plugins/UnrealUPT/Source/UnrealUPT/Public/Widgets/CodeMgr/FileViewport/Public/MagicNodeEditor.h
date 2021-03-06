//// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
//
//#pragma once
//
//#include "CoreMinimal.h"
//
//class SMGC_TextEditorWidget;
//class SScrollBox;
////class SListView;
//
//class SFileView :public SCompoundWidget
//{
//public:
//	SLATE_BEGIN_ARGS(SFileView) {}
//	SLATE_ATTRIBUTE(FText, Text)
//	SLATE_END_ARGS()
//
//		void Construct(const FArguments& InArgs);
//
//public:
//
//	TSharedRef<ITableRow> OnGenerateLineCounter(TSharedPtr<FString>Item, const TSharedRef<STableViewBase>& OwnerTable);
//	void OnSelectedLineCounterItem(TSharedPtr<FString>Item, ESelectInfo::Type SelectInfo);
//	TSharedRef<SDockTab> OnSpawnPTMToolTab(const FSpawnTabArgs& SpawnTabArgs);
//public:
//	TArray<TSharedPtr<FString>>LineCount;
//	FText ScriptText;
//	TSharedPtr<SMGC_TextEditorWidget>SCRIPT_EDITOR;
//
//private:
//	TSharedPtr<SScrollBox>VS_SCROLL_BOX;
//	TSharedPtr<SListView<TSharedPtr<FString>>>LINE_COUNTER;
//};
