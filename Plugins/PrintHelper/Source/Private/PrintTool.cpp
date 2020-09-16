// Fill out your copyright notice in the Description page of Project Settings.


#include "PrintTool.h"
#include "Engine/Engine.h"
#include "PrintHelperSettings.h"

DEFINE_LOG_CATEGORY_STATIC(FPrintHelper, Log, All);
TSharedPtr<FPrintTool> FPrintTool::Singleton = nullptr;

FPrintTool::FPrintTool()
{
	ShowTime = 2.0f;
	ShowColor = FColor::Cyan;
	TargetObject.Reset();
}

FPrintTool::~FPrintTool()
{
	OutputString.Empty();
	TargetObject.Reset();
	Singleton = nullptr;
}

TSharedPtr<FPrintTool> FPrintTool::Get()
{
	if (!Singleton.IsValid())
		Singleton = MakeShareable(new FPrintTool());

	return Singleton;
}

void FPrintTool::Initialize(EPrintType Type, UObject* InObject /*= nullptr*/, bool bInPrintToLog /*= false*/, float InTime /*= 2.0f*/, FColor InColor /*= FColor::Cyan*/)
{
	bPrintToLog = bInPrintToLog;
	PrintType = Type;
	TargetObject = MakeWeakObjectPtr<UObject>(InObject);
	ShowTime = InTime;
	ShowColor = InColor;
	OutputString = FString();
}

void FPrintTool::Output()
{
	if (bEnable == false || PrintType == EPrintType::None)
		return;

	FString FinalLogString = FString();
	FinalLogString = TargetObject.IsValid() ? (FString::Format(TEXT("[ {0} ] "), { TargetObject.Get()->GetName() }) + OutputString) : OutputString;

	if (bCallStack)
	{
		const SIZE_T StackTraceSize = 65535;
		ANSICHAR* StackTrace = (ANSICHAR*)FMemory::SystemMalloc(StackTraceSize);
		FPlatformStackWalk::StackWalkAndDumpEx(StackTrace, StackTraceSize, 1, FGenericPlatformStackWalk::EStackWalkFlags::FlagsUsedWhenHandlingEnsure);
		FinalLogString += (FString(TEXT("\n")) + (ANSI_TO_TCHAR(StackTrace)));
		FMemory::SystemFree(StackTrace);
	}

	switch (PrintType)
	{
	case EPrintType::PrintOnScreen:
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, ShowTime, ShowColor, FinalLogString);
		if (bPrintToLog)
			UE_LOG(FPrintHelper, Log, TEXT("%s"), *FinalLogString);
		break;

	case EPrintType::Log:
		UE_LOG(FPrintHelper, Log, TEXT("%s"), *FinalLogString);
		break;

	case EPrintType::Warning:
		UE_LOG(FPrintHelper, Warning, TEXT("%s"), *FinalLogString);
		break;

	case EPrintType::Error:
		UE_LOG(FPrintHelper, Error, TEXT("%s"), *FinalLogString);
		break;
	}

	Reset();
}

void FPrintTool::Reset()
{
	bPrintToLog = false;
	ShowTime = 2.0f;
	PrintType = EPrintType::None;
	OutputString.Empty();
	ShowColor = FColor::Cyan;
	if (TargetObject.IsValid())
		TargetObject.Reset();
}

