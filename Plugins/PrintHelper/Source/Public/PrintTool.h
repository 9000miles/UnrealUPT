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
class PRINTHELPER_API FPrintTool :public TSharedFromThis<FPrintTool>
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
	void SetSettings(const bool bFile, const bool bLine, const bool _bCallStack) { bPrintFile = bFile; bPrintLine = bLine; bCallStack = _bCallStack; }
	void GetSettings(bool& bFile, bool& bLine) { bFile = bPrintFile; bLine = bPrintFile ? bPrintLine : false; }

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

private:
	static TSharedPtr<FPrintTool> Singleton;
	bool bEnable = true;
	bool bPrintFile = false;
	bool bPrintLine = false;
	bool bCallStack = false;

	bool bPrintToLog = true;
	float ShowTime;
	EPrintType PrintType;
	FString OutputString;
	FColor ShowColor;
	//输出名称的目标对象
	TWeakObjectPtr<UObject> TargetObject;
};
