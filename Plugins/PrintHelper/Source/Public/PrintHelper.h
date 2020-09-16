// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "PrintTool.h"

class FPrintHelperModule : public IModuleInterface
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	bool HandleSettingsSaved();
};

#define PRINT_LOG(...) 	PRINT_MESSAGE(Log, nullptr, __VA_ARGS__)
#define PRINT_WARNING(...) PRINT_MESSAGE(Warning, nullptr, __VA_ARGS__)
#define PRINT_ERROR(...) PRINT_MESSAGE(Error, nullptr, __VA_ARGS__)

#define PRINT_MESSAGE(PrintType, Object, ...) \
{\
	bool bPrintFile;\
	bool bPrintLine;\
	FPrintTool::Get()->GetSettings(bPrintFile, bPrintLine);\
	if (bPrintFile)\
	{\
		if (!bPrintLine)\
			FPrint::##PrintType(Object) << __VA_ARGS__ << " " << __FILE__ << FPrint::End();\
		else\
			FPrint::##PrintType(Object) << __VA_ARGS__ << " " << __FILE__ << "(" << __LINE__ << ")" << FPrint::End();\
	}\
	else\
	{\
		FPrint::##PrintType##(Object) << __VA_ARGS__ << FPrint::End();\
	}\
}

/*
* 打印助手
* <<操作符可连接的参数类型见 PrintTool.h << 操作符重载，可以自由添加
* 使用示例：FPrint::Log() << "使用示例" << FPrint::End();
*					FPrint::Log(Object) << "Object为UObject子类对象实例" << FString(TEXT("FString字符串")) << FPrint::End();
*					FPrint::PrintOnScreen(true, Object) << "Object为UObject子类对象实例" << LOCTEXT("FText", "FText文本") << FVector(1, 2, 3) << FPrint::End();
*/
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
