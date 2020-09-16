// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SimpleTestManager.h"

class SIMPLETEST_API FTestBase
{
public:
	FTestBase(const FString InTestName, const FString InFileName, const int32 InLineNumber)
		:TestName(InTestName), FileName(InFileName), LineNumber(InLineNumber)
	{	}

	virtual ~FTestBase() {}

protected:
	FString TestName;
	FString FileName;
	int32 LineNumber;
};

/*
* 不要将 IMPLEMENT_UNIT_TEST_OBJECT 写在 UObject子类的 .h 文件里
*/
class SIMPLETEST_API FCodeTest :public FTestBase
{
public:
	FCodeTest(const FString InTestPath, const FString InTestClassName, const FString InFileName, const int32 InLineNumber)
		:FTestBase(InTestPath, InFileName, InLineNumber), TestClassName(InTestClassName)
	{
		TSharedPtr<FCodeTestData> Data = MakeShareable(new FCodeTestData(TestName, FileName, LineNumber, this));
		FSimpleTestManager::Get()->RegisterTest(TestName, Data);
	}

	~FCodeTest()
	{
		Object = nullptr;
	}

	virtual void RunTest() = 0;

	const FString GetTestClassName() const { return TestClassName; }
	const FString GetTestPath() const { return TestName; }
	const FString GetSourceFileName() const { return FileName; }
	const int32 GetSourceFileLine() const { return LineNumber; }

	UObject* GetObject() { return Object; }

	void Reset()
	{
		if (Object)
		{
			Object->RemoveFromRoot();
			Object = nullptr;
		}
	};

protected:
	UObject* Object;

	template<typename TObjectClass>
	TObjectClass* GetObject();

private:
	FString TestClassName;
};

class SIMPLETEST_API FSlateTest :public FTestBase
{
public:
	FSlateTest(const FString CommandName, TSharedPtr<SWidget> InWidget, const FString InFileName, const int32 InLineNumber)
		:FTestBase(CommandName, InFileName, InLineNumber)
	{
		Widget = InWidget;
		TSharedPtr<FSlateTestData> Data = MakeShareable(new FSlateTestData(CommandName, FileName, LineNumber, InWidget));
		FSimpleTestManager::Get()->RegisterSlate(CommandName, Data);
	}

	TSharedPtr<SWidget> GetWidget() { return Widget; };

private:
	TSharedPtr<SWidget> Widget;
};

template<typename TObjectClass>
TObjectClass* FCodeTest::GetObject()
{
	if (Object != nullptr)
	{
		return Cast<TObjectClass>(Object);
	}

	return nullptr;
}

#define IMPLEMENT_SLATE_TEST(TClass, WidgetIns)\
IMPLEMENT_SLATE_TEST_PRIVATE(TClass, WidgetIns,  __FILE__, __LINE__)\
namespace\
{\
	TClass TClass##SimpleTestInstance(TEXT(#TClass), WidgetIns); \
}

#define IMPLEMENT_SLATE_TEST_PRIVATE(TClass, WidgetIns, FileName, LineNumber)\
class TClass : public FSlateTest\
{\
public:\
	TClass(const FString& InName,TSharedRef<SWidget> Widget) : FSlateTest(TEXT(#TClass), Widget, FileName, LineNumber)\
	{  }\
};

#define IMPLEMENT_CODE_TEST(TClass, TestPath)\
IMPLEMENT_CODE_TEST_OBJECT(TClass, TestPath, UObject)

#define IMPLEMENT_CODE_TEST_OBJECT(TClass, TestPath, TObjClass)\
IMPLEMENT_CODE_TEST_PRIVATE(TClass, TestPath, TObjClass, __FILE__, __LINE__)\
namespace\
{\
	TClass TClass##SimpleTestInstance(TEXT(#TClass)); \
}

#define IMPLEMENT_CODE_TEST_PRIVATE(TClass, TestPath, TObjClass, FileName, LineNumber)\
class TClass : public FCodeTest\
{\
public:\
	TClass(const FString& InName) : FCodeTest(TEXT(TestPath), InName, FileName, LineNumber)\
	{\
		if (TObjClass##::StaticClass() != UObject::StaticClass())\
		{\
			Object = NewObject<TObjClass>();\
			Object->AddToRoot();\
		}\
	}\
	virtual void RunTest() override;\
};
