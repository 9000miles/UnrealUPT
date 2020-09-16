// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class FCodeTest;

struct FSimpleTestHyperlinkData
{
public:
	FSimpleTestHyperlinkData(const FString& InDisplayText, const FString& InFileName, const int32& InLineNumber)
		:DisplayName(InDisplayText), FileName(InFileName), LineNumber(InLineNumber)
	{}

public:
	FString DisplayName;
	FString FileName;
	int32 LineNumber;
};

struct FCodeTestData :public FSimpleTestHyperlinkData
{
public:
	FCodeTestData(const FString& InDisplayText, const FString& InFileName, const int32& InLineNumber, FCodeTest* InCodeTest)
		:FSimpleTestHyperlinkData(InDisplayText, InFileName, InLineNumber)
		, CodeTest(InCodeTest)
	{}

public:
	FCodeTest* CodeTest;
};

struct FSlateTestData :public FSimpleTestHyperlinkData
{
public:
	FSlateTestData(const FString& InDisplayText, const FString& InFileName, const int32& InLineNumber, TSharedPtr<SWidget> InWidget)
		:FSimpleTestHyperlinkData(InDisplayText, InFileName, InLineNumber)
		, Widget(InWidget)
	{}

public:
	TSharedPtr<SWidget> Widget;
};

class FSimpleTestManager :public TSharedFromThis<FSimpleTestManager>
{
public:
	static TSharedPtr<FSimpleTestManager> Get();

	void RegisterTest(const FString& TestPath, TSharedPtr<FCodeTestData> Target);
	void RunTest(const FString& TestPath);
	FCodeTest* GetTest(const FString& TestPath);

	void RegisterSlate(const FString& Command, TSharedPtr<FSlateTestData> Target);
	TSharedPtr<SWidget> GetWidget(const FString& Command);

	void GetCommands(TArray<TSharedPtr<FSlateTestData>>& Commands);
	void GetTestPaths(TArray<TSharedPtr<FCodeTestData>>& TestPaths);


private:
	static TSharedPtr<FSimpleTestManager> Instance;

	TMap<FString, TSharedPtr<FCodeTestData>> TestMap;
	TMap<FString, TSharedPtr<FSlateTestData>> CommandMap;
};