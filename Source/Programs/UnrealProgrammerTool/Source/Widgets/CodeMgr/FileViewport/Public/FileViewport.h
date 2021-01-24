//// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
//
//#pragma once
//
//#include "CoreMinimal.h"
//#include "Modules/ModuleManager.h"
//#include "FileViewport.generated.h"
//
//class SMGC_TextEditorWidget;
//class SScrollBox;
//class SFileView;
//
//class FFileViewportModule : public IModuleInterface
//{
//public:
//
//	/** IModuleInterface implementation */
//	virtual void StartupModule() override;
//	virtual void ShutdownModule() override;
//	TSharedRef<SDockTab>OnSpawnPTMToolTab(const FSpawnTabArgs& SpawnTabArgs);
//	TSharedRef<ITableRow> OnGenerateLineCounter(TSharedPtr<FString>Item, const TSharedRef<STableViewBase>& OwnerTable);
//	void OnSelectedLineCounterItem(TSharedPtr<FString>Item, ESelectInfo::Type SelectInfo);
//public:
//	FString Script;
//	TArray<TSharedPtr<FString>>LineCount;
//	TSharedPtr<SFileView> FileView;
//
//private:
//	TSharedPtr<SMGC_TextEditorWidget>SCRIPT_EDITOR;
//	TSharedPtr<SScrollBox>VS_SCROLL_BOX;
//	TSharedPtr<SListView<TSharedPtr<FString>>>LINE_COUNTER;
//};
//
//UCLASS()
//class UFileViewportObject :public UObject
//{
//	GENERATED_BODY()
//public:
//	UPROPERTY(EditAnywhere)
//		FString FilePath;
//};