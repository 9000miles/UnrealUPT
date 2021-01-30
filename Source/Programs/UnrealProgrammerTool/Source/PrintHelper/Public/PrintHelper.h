// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "PrintTool.h"

#if 0
《打印助手示例》
<< 操作符可连接的参数类型见 PrintTool.h << 操作符重载，可以自由添加
FPrint::Log() << "使用示例" << FPrint::End();
FPrint::Log(UObject) << "UObject子类对象实例" << FString(TEXT("FString字符串")) << FPrint::End();
FPrint::PrintOnScreen(true, UObject) << "UObject子类对象实例" << LOCTEXT("FText", "FText文本") << FVector(1, 2, 3) << FPrint::End();

FPrint::Log() << "FPrint::Log ==>> Use the sample" << FPrint::End();
PRINT_LOG(true << 464 << 6.16464f << " Multiple types of parameter output ");
PRINT_WARNING(false << " ============= PRINT_WARNING =============");

PRINT_WARNING_Call_Stack(true, true, true << 464 << 6.16464f << " PRINT_WARNING_Call_Stack ");
PRINT_ERROR_Call_Stack(false, true, false << true << 464 << 6.16464f << " PRINT_ERROR_Call_Stack");

FPrint::Log().SetCanPrintCallStack(true, true) << "FPrint::Log == FPrint::Log().SetCanPrintCallStack(true, true)" << FPrint::End();
#endif

class FPrintHelperModule : public IModuleInterface
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	bool HandleSettingsSaved();
};

#define PRINT_LOG_Call_Stack(bBlueprintCallStack, bCppCallStack, ...)\
FPrintTool::Get()->SetCanPrintCallStack(bBlueprintCallStack, bCppCallStack);\
FPrint::Log(nullptr) << __VA_ARGS__ << FPrint::End();

#define PRINT_WARNING_Call_Stack(bBlueprintCallStack, bCppCallStack, ...)\
FPrintTool::Get()->SetCanPrintCallStack(bBlueprintCallStack, bCppCallStack);\
FPrint::Warning(nullptr) << __VA_ARGS__ << FPrint::End();

#define PRINT_ERROR_Call_Stack(bBlueprintCallStack, bCppCallStack, ...)\
FPrintTool::Get()->SetCanPrintCallStack(bBlueprintCallStack, bCppCallStack);\
FPrint::Error(nullptr) << __VA_ARGS__ << FPrint::End();

#define PRINT_LOG(...)\
{\
	bool bPrintFileLine;\
	FPrintTool::Get()->GetCanPrintFileLine(bPrintFileLine);\
	if(!bPrintFileLine)\
		FPrint::Log(nullptr) << __VA_ARGS__  << " " << FPrint::End();\
	else\
		FPrint::Log(nullptr) << __VA_ARGS__  << " " << FPaths::GetCleanFilename(__FILE__) << "(" << __LINE__ << ")"<< FPrint::End();\
}

#define PRINT_WARNING(...)\
{\
	bool bPrintFileLine;\
	FPrintTool::Get()->GetCanPrintFileLine(bPrintFileLine);\
	if (!bPrintFileLine)\
		FPrint::Warning(nullptr) << __VA_ARGS__ << " " << FPrint::End();\
	else\
		FPrint::Warning(nullptr) << __VA_ARGS__ << " " << FPaths::GetCleanFilename(__FILE__) << "(" << __LINE__ << ")" << FPrint::End();\
}

#define PRINT_ERROR(...)\
{\
	bool bPrintFileLine;\
	FPrintTool::Get()->GetCanPrintFileLine(bPrintFileLine);\
	if (!bPrintFileLine)\
		FPrint::Error(nullptr) << __VA_ARGS__ << " " << FPrint::End();\
	else\
		FPrint::Error(nullptr) << __VA_ARGS__ << " " << FPaths::GetCleanFilename(__FILE__) << "(" << __LINE__ << ")" << FPrint::End();\
}


namespace FPrint
{
	/**
	 * 打印消息到屏幕上
	 *
	 * @param bPrintToLog	同时打印到Log
	 * @param InObject			目标对象不为空时在消息前面加上该对象的名称
	 * @param InColor			屏幕消息的颜色
	 * @param InTime			消息显示时长
	 * @return						返回PrintTool可通过<<操作符扩展支持的打印类型
	 */
	FORCEINLINE FPrintTool& PrintOnScreen(bool bPrintToLog = true, UObject* InObject = nullptr, FColor InColor = FColor::Cyan, float InTime = 2.0f)
	{
		FPrintTool::Get()->Initialize(EPrintType::PrintOnScreen, InObject, bPrintToLog, InTime, InColor);
		return *FPrintTool::Get();
	}

	/**
	 * 在Output Log窗口打印Log级别的消息
	 *
	 * @param InObject			目标对象不为空时在消息前面加上该对象的名称
	 * @return						返回PrintTool可通过<<操作符扩展支持的打印类型
	 */
	FORCEINLINE FPrintTool& Log(UObject* InObject = nullptr)
	{
		FPrintTool::Get()->Initialize(EPrintType::Log, InObject);
		return *FPrintTool::Get();
	}

	/**
	 * 在Output Log窗口打印Warning级别的消息
	 *
	 * @param InObject			目标对象不为空时在消息前面加上该对象的名称
	 * @return						返回PrintTool可通过<<操作符扩展支持的打印类型
	 */
	FORCEINLINE FPrintTool& Warning(UObject* InObject = nullptr)
	{
		FPrintTool::Get()->Initialize(EPrintType::Warning, InObject);
		return *FPrintTool::Get();
	}

	/**
	 * 在Output Log窗口打印Error级别的消息
	 *
	 * @param InObject			目标对象不为空时在消息前面加上该对象的名称
	 * @return						返回PrintTool可通过<<操作符扩展支持的打印类型
	 */
	FORCEINLINE FPrintTool& Error(UObject* InObject = nullptr)
	{
		FPrintTool::Get()->Initialize(EPrintType::Error, InObject);
		return *FPrintTool::Get();
	}

	/**
	 * 输出消息
	 *
	 * @param InObject			目标对象不为空时在消息前面加上该对象的名称
	 * @return						返回PrintTool可通过<<操作符扩展支持的打印类型
	 */
	FORCEINLINE FPrintTool& End()
	{
		return *FPrintTool::Get();
	}
}

//UCLASS()
//class UPrintHelperBlueprintLibrary :public UBlueprintFunctionLibrary
//{
//	GENERATED_BODY()
//public:
//	UFUNCTION(BlueprintCallable, Category = "Utilities|String", meta = (WorldContext = "WorldContextObject", CallableWithoutWorldContext))
//		static void PrintCallStack(UObject* WorldContextObject, bool bBlueprintCallStack, bool bCppCallStack, FString Message, EPrintType PrintType = EPrintType::Log)
//	{
//		switch (PrintType)
//		{
//		case EPrintType::Log:
//			PRINT_LOG_Call_Stack(bBlueprintCallStack, bCppCallStack, Message);
//			break;
//		case EPrintType::Warning:
//			PRINT_WARNING_Call_Stack(bBlueprintCallStack, bCppCallStack, Message);
//			break;
//		case EPrintType::Error:
//			PRINT_ERROR_Call_Stack(bBlueprintCallStack, bCppCallStack, Message);
//			break;
//		default:
//			PRINT_WARNING("The output type is not suitable for call stack output, Please Set PrintType to Log, Warning, Error");
//			break;
//		}
//	}
//};
