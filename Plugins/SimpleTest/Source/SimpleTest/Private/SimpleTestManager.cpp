#include "SimpleTestManager.h"
#include "SimpleTestDefine.h"
#include "SNullWidget.h"
#include "ModuleManager.h"

DEFINE_LOG_CATEGORY_STATIC(TestManagerLog, Log, All);

TSharedPtr<FSimpleTestManager> FSimpleTestManager::Instance;

TSharedPtr<FSimpleTestManager> FSimpleTestManager::Get()
{
	if (!Instance.IsValid())
	{
		Instance = MakeShareable(new FSimpleTestManager());
	}
	return Instance;
}

void FSimpleTestManager::RegisterTest(const FString& TestPath, TSharedPtr<FCodeTestData> Target)
{
	check(Target.IsValid());
	check(Target->CodeTest);

	bool bAlreadyContains = TestMap.Contains(TestPath);
	checkf(!bAlreadyContains, TEXT("%s multiple registration"), *TestPath);

	if (!bAlreadyContains)
	{
		TestMap.Add(TestPath, Target);
	}
}

void FSimpleTestManager::RunTest(const FString& TestPath)
{
	if (!TestMap.Contains(TestPath))
	{
		UE_LOG(TestManagerLog, Log, TEXT("TestPath not found"));
		return;
	}

	TSharedPtr<FCodeTestData>* Target = TestMap.Find(TestPath);
	if (Target)
		(*Target)->CodeTest->RunTest();
}

FCodeTest* FSimpleTestManager::GetTest(const FString& TestPath)
{
	if (!TestMap.Contains(TestPath))
	{
		UE_LOG(TestManagerLog, Log, TEXT("TestPath not found"));
		return nullptr;
	}

	TSharedPtr<FCodeTestData>* Target = TestMap.Find(TestPath);
	if (Target)
		return (*Target)->CodeTest;

	return nullptr;
}

void FSimpleTestManager::RegisterSlate(const FString& Command, TSharedPtr<FSlateTestData> Target)
{
	check(Target.IsValid());

	if (Command.IsEmpty())
	{
		UE_LOG(TestManagerLog, Log, TEXT("Command is Empty"));
		return;
	}

	bool bAlreadyContains = TestMap.Contains(Command);
	checkf(!bAlreadyContains, TEXT("%s multiple registration"), *Command);

	if (!bAlreadyContains)
	{
		CommandMap.Add(Command, Target);
	}
}

TSharedPtr<SWidget> FSimpleTestManager::GetWidget(const FString& Command)
{
	if (!CommandMap.Contains(Command))
	{
		UE_LOG(TestManagerLog, Log, TEXT("Command not found"));
		return SNullWidget::NullWidget;
	}

	TSharedPtr<FSlateTestData>* Widget = CommandMap.Find(Command);
	if (Widget)
		return (*Widget)->Widget;

	return SNullWidget::NullWidget;
}

void FSimpleTestManager::GetCommands(TArray<TSharedPtr<FSlateTestData>>& Commands)
{
	CommandMap.GenerateValueArray(Commands);
}

void FSimpleTestManager::GetTestPaths(TArray<TSharedPtr<FCodeTestData>>& TestPaths)
{
	TestMap.GenerateValueArray(TestPaths);
}