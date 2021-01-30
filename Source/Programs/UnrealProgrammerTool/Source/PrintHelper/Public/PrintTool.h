// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM()
enum class EPrintType : uint8
{
	None,

	PrintOnScreen,
	Log,
	Warning,
	Error,
};

/**
 *打印助手
 */
class FPrintTool :public TSharedFromThis<FPrintTool>
{
public:
	FPrintTool();
	~FPrintTool();

	static TSharedPtr<FPrintTool> Get();

	//InObject用于显示对象名字
	void Initialize(EPrintType Type, UObject* InObject = nullptr, bool bInPrintToLog = false, float InTime = 2.0f, FColor InColor = FColor::Cyan);;

	//打印输出
	void Output();

	void SetEnable(const bool bInEnable) { bEnable = bInEnable; }
	const bool GetEnable() const { return bEnable; }

	void SetCanPrintFileLine(const bool bFileLine) { bPrintFileLine = bFileLine; }
	void GetCanPrintFileLine(bool& bFileLine) { bFileLine = bPrintFileLine; }

	//是否可以打印蓝图或CPP调用堆栈
	FPrintTool& SetCanPrintCallStack(const bool _bBlueprintCallStack = false, const bool _bCppCallStack = false) { bCppCallStack = _bCppCallStack; bBlueprintCallStack = _bBlueprintCallStack; return *this; }

	inline FPrintTool& operator<<(FString info) { OutputString.Append(info); return *this; }
	inline FPrintTool& operator<<(FName info) { OutputString.Append(info.ToString()); return *this; }
	inline FPrintTool& operator<<(FText info) { OutputString.Append(info.ToString()); return *this; }
	inline FPrintTool& operator<<(char* info) { OutputString += info; return *this; }
	inline FPrintTool& operator<<(const char* info) { OutputString += info; return *this; }
	inline FPrintTool& operator<<(char info) { OutputString.AppendChar(info); return *this; }
	inline FPrintTool& operator<<(TCHAR* info) { OutputString.Append(FString(info)); return *this; }
	inline FPrintTool& operator<<(const TCHAR* info) { OutputString.Append(FString(info)); return *this; }
	inline FPrintTool& operator<<(bool info) { OutputString.Append(info ? FString("true") : FString("false")); return *this; }
	inline FPrintTool& operator<<(uint8 info) { OutputString.Append(FString::Printf(TEXT("%d"), info)); return *this; }
	inline FPrintTool& operator<<(int32 info) { OutputString.Append(FString::FromInt(info)); return *this; }
	inline FPrintTool& operator<<(int64 info) { OutputString.Append(FString::FromInt(info)); return *this; }
	inline FPrintTool& operator<<(float info) { OutputString.Append(FString::SanitizeFloat(info)); return *this; }
	inline FPrintTool& operator<<(double info) { OutputString.Append(FString::SanitizeFloat(info)); return *this; }
	inline FPrintTool& operator<<(FVector2D info) { OutputString.Append(info.ToString()); return *this; }
	inline FPrintTool& operator<<(FVector info) { OutputString.Append(info.ToString()); return *this; }
	inline FPrintTool& operator<<(FRotator info) { OutputString.Append(info.ToString()); return *this; }
	inline FPrintTool& operator<<(FQuat info) { OutputString.Append(info.ToString()); return *this; }
	inline FPrintTool& operator<<(FTransform info) { OutputString.Append(info.ToString()); return *this; }
	inline FPrintTool& operator<<(FMatrix info) { OutputString.Append(info.ToString()); return *this; }
	inline FPrintTool& operator<<(FColor info) { OutputString.Append(info.ToString()); return *this; }
	inline FPrintTool& operator<<(FLinearColor info) { OutputString.Append(info.ToString()); return *this; }

	inline void operator<<(FPrintTool& Helper) { Helper.Output(); }

private:
	void Reset();

	void PrintBlueprintCallStack();

	void PrintCppCallStack();

private:
	static TSharedPtr<FPrintTool> Singleton;
	bool bEnable = true;

	//是否打印输出语句所在文件名和行号，在项目设置中设置，由DefaultGame初始化
	bool bPrintFileLine = true;

	//是否打印C++调用堆栈，每条语句独自设置是否打印调用堆栈，打印结束后恢复默认值false
	bool bCppCallStack = false;
	//是否打印蓝图调用堆栈，每条语句独自设置是否打印调用堆栈，打印结束后恢复默认值false
	bool bBlueprintCallStack = false;

	bool bPrintToLog = true;
	float ShowTime;
	EPrintType PrintType;
	FString OutputString;
	FColor ShowColor;

	//输出名称的目标对象
	TWeakObjectPtr<UObject> TargetObject;
};
