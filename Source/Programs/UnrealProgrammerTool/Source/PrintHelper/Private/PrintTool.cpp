// Fill out your copyright notice in the Description page of Project Settings.

#include "PrintTool.h"
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

void FPrintTool::PrintBlueprintCallStack()
{
	//	FString LogString;
	//	const TArray<const FFrame*> ScriptStacks
	//#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 26
	//		= FBlueprintContextTracker::Get().GetScriptStack();
	//#elif ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION <= 25
	//		= FBlueprintExceptionTracker::Get().ScriptStack;
	//#else
	//		;
	//#endif
	//
	//	int32 FrameCount = ScriptStacks.Num();
	//	FString ScriptStackStr = FString::Printf(TEXT("\n\nScript Stack (%d frames):\n"), FrameCount);
	//	for (int32 i = 0; i < FrameCount; ++i)
	//	{
	//		ScriptStackStr += ScriptStacks[i]->GetStackDescription() + TEXT("\n");
	//	}
	//
	//	LogString += FString(TEXT("\n")) + TEXT("  ↓↓↓ =======================  BLUEPRINT CALL STACK  ======================= ↓↓↓  ");
	//	LogString += (FString(TEXT("\n")) + ScriptStackStr);
	//	LogString += FString(TEXT("\n")) + TEXT("  ↑↑↑ =======================  BLUEPRINT CALL STACK  ======================= ↑↑↑  ");
	//	UE_LOG(FPrintHelper, Log, TEXT("%s"), *LogString);
}

void FPrintTool::PrintCppCallStack()
{
	return;

	//	FString LogString;
	//	LogString = FString(TEXT("\n")) + TEXT("  ↓↓↓ =======================  CPP CALL STACK  ======================= ↓↓↓  ");
	//	UE_LOG(FPrintHelper, Log, TEXT("%s"), *LogString);
	//
	//#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 26
	//	FDebug::DumpStackTraceToLog(ELogVerbosity::Log);
	//#elif ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION == 25
	//	//FDebug::DumpStackTraceToLog();
	//
	//	//const SIZE_T StackTraceSize = 65535;
	//	//ANSICHAR* StackTrace = (ANSICHAR*)FMemory::SystemMalloc(StackTraceSize);
	//	//FPlatformStackWalk::StackWalkAndDumpEx(StackTrace, StackTraceSize, 1, FGenericPlatformStackWalk::EStackWalkFlags::FlagsUsedWhenHandlingEnsure);
	//	//LogString += (FString(TEXT("\n")) + (ANSI_TO_TCHAR(StackTrace)));
	//	//FMemory::SystemFree(StackTrace);
	//	FDebug::DumpStackTraceToLog(TEXT("----------------"));
	//#else
	//	;
	//#endif
	//
	//	LogString = FString(TEXT("\n")) + TEXT("  ↑↑↑ =======================  CPP CALL STACK  ======================= ↑↑↑  ");
	//	UE_LOG(FPrintHelper, Log, TEXT("%s"), *LogString);
}

void FPrintTool::Output()
{
	if (bEnable == false || PrintType == EPrintType::None)
		return;

	FString FinalLogString = FString();
	FinalLogString = TargetObject.IsValid() ? (FString::Format(TEXT("[ {0} ] "), { TargetObject.Get()->GetName() }) + OutputString) : OutputString;

	switch (PrintType)
	{
	case EPrintType::PrintOnScreen:
		//if (GEngine)
		//	GEngine->AddOnScreenDebugMessage(-1, ShowTime, ShowColor, FinalLogString);
		//if (bPrintToLog)
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

#if !(UE_BUILD_SHIPPING || UE_BUILD_SHIPPING_WITH_EDITOR)
	if (bBlueprintCallStack)
		PrintBlueprintCallStack();

	if (bCppCallStack)
		PrintCppCallStack();
#endif

	Reset();
}

void FPrintTool::Reset()
{
	bPrintToLog = false;
	ShowTime = 2.0f;
	PrintType = EPrintType::None;
	OutputString.Empty();
	ShowColor = FColor::Cyan;

	//恢复默认值false，不影响其他输出语句
	bCppCallStack = false;
	bBlueprintCallStack = false;

	if (TargetObject.IsValid())
		TargetObject.Reset();
}