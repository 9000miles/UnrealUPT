// Fill out your copyright notice in the Description page of Project Settings.

#include "UPTGameModeBase.h"
#include "PrintHelper/Public/PrintHelper.h"

void AUPTGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	FString Text;
	if (FFileHelper::LoadFileToString(Text, TEXT("C:/Users/Administrator/Desktop/新建文本文档.txt")))
	{
		//FRegexPattern ProjectDigitpattern(TEXT("\\[(\\s|.)+?]"));
		FRegexPattern ProjectDigitpattern(TEXT("\\[[^[\\]]*(((?'Open'\\[)|(?'-Open'\\])|[^[\\]]*)+)*(?(Open)(?!))\\]"));
		FRegexMatcher Matcher(ProjectDigitpattern, Text);
		if (Matcher.FindNext())
		{
			int32 Beginning = Matcher.GetMatchBeginning();
			int32 Ending = Matcher.GetMatchEnding();
			FString OutString = Text.Mid(Beginning, Ending - Beginning);
			PRINT_LOG(OutString);
		}
	}
}

void AUPTGameModeBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}