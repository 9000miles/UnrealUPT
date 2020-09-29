#include "SimpleTestDemo.h"
#include "SimpleTestDefine.h"
#include "PrintHelper.h"
#include "DesktopPlatform/Public/DesktopPlatformModule.h"
#include "GameProjectUtils.h"
#include "GenericPlatformFile.h"
#include "Paths.h"
#include "PlatformFilemanager.h"
#include "Private/ProjectManager.h"
#include "App.h"
#include "FileHelper.h"

#define LOCTEXT_NAMESPACE "TestDemo"
#pragma optimize("",off)
IMPLEMENT_CODE_TEST_OBJECT(FTeDemo, "FTeDemo.Test1", UTestDemo);
void FTeDemo::RunTest()
{
	GLog->Log("void FTeDemo::RunTest()");
	UTestDemo* SuObject = GetObject<UTestDemo>();
	PRINT_LOG(SuObject->HelloI);
}

IMPLEMENT_CODE_TEST_OBJECT(FTeDemo_New, "FTeDemo.Test1_New", UTestDemo_New);
void FTeDemo_New::RunTest()
{
	GLog->Log("void FTeDemo_New::RunTest()");
	UTestDemo_New* SuObject = GetObject<UTestDemo_New>();
	PRINT_LOG(SuObject->Texture->GetPathName());
}

IMPLEMENT_CODE_TEST(FOWEeiowe, "fjojewo");
void FOWEeiowe::RunTest()
{
	PRINT_LOG("void FOWEeiowe::RunTest()");
}

IMPLEMENT_CODE_TEST(FEngineProjectTest, "Engine Project Test");
void FEngineProjectTest::RunTest()
{
	//FString ProjectFileName = FString(TEXT("E:\\UnrealProject\\UnrealEngine_GitHub\\Projects\\AboutEngine\\AboutEngine.uproject"));
	FString ProjectFileName = FString(TEXT("E:\\UnrealProject\\Plugin425\\Plugin425.uproject"));
	FString OutIdentifer;
	FString OutRootDir;
	FDesktopPlatformModule::Get()->GetEngineIdentifierForProject(ProjectFileName, OutIdentifer);
	FDesktopPlatformModule::Get()->GetEngineRootDirFromIdentifier(OutIdentifer, OutRootDir);
	PRINT_LOG(OutIdentifer);
	PRINT_LOG(OutRootDir);
	PRINT_LOG("========================================");

	//TArray<FString> OutProjectFileNames;
	//FDesktopPlatformModule::Get()->EnumerateProjectsKnownByEngine(OutIdentifer, true, OutProjectFileNames);
	//for (FString FileName : OutProjectFileNames)
	//{
	//	PRINT_LOG(FileName);
	//}
	PRINT_LOG("========================================");

	TMap<FString, FString> OutInstallations;
	FDesktopPlatformModule::Get()->EnumerateEngineInstallations(OutInstallations);
	int32 i = 0;
	for (auto Installation = OutInstallations.CreateConstIterator(); Installation; ++Installation)
	{
		i++;
		PRINT_LOG("Num Start:: " << i);

		PRINT_LOG("版本号： " << Installation.Key());
		PRINT_LOG("RootDir： " << Installation.Value());

		FString RootDir = Installation.Value();
		FString Indentifer;
		FDesktopPlatformModule::Get()->GetEngineIdentifierFromRootDir(RootDir, Indentifer);

		TArray<FString> OutProjectFileNames;
		FDesktopPlatformModule::Get()->EnumerateProjectsKnownByEngine(Indentifer, true, OutProjectFileNames);
		for (FString _FileName : OutProjectFileNames)
		{
			PRINT_LOG(_FileName);
		}
		PRINT_LOG("Num End:: " << i);
	}
	/*
		FPrintHelper: 4.22
		FPrintHelper: D:/Program Files/Epic Games/UE_4.22
		FPrintHelper: 4.25
		FPrintHelper: D:/Program Files/Epic Games/UE_4.25
		FPrintHelper: 4.24
		FPrintHelper: D:/Program Files/Epic Games/UE_4.24
		FPrintHelper: {9D2EAB45-4985-E5BB-6331-CC8DB3778F0A}
		FPrintHelper: E:/UnrealProject/UnrealEngine_GitHub
	*/
}

IMPLEMENT_SLATE_TEST
(
	FSlate_Test,
	SNew(SButton)
	.Text(LOCTEXT("IMPLEMENT_SLATE_TEST", "OJFOWOEI"))
);

void FindCodeFiles(const TCHAR* BaseDirectory, TArray<FString>& FileNames, int32 MaxNumFileNames)
{
	struct FDirectoryVisitor : public IPlatformFile::FDirectoryVisitor
	{
		TArray<FString>& FileNames;
		int32 MaxNumFileNames;

		FDirectoryVisitor(TArray<FString>& InFileNames, int32 InMaxNumFileNames)
			: FileNames(InFileNames)
			, MaxNumFileNames(InMaxNumFileNames)
		{
		}

		virtual bool Visit(const TCHAR* FilenameOrDirectory, bool bIsDirectory) override
		{
			if (bIsDirectory)
			{
				FString CleanDirectoryName(FPaths::GetCleanFilename(FilenameOrDirectory));
				if (!CleanDirectoryName.StartsWith(TEXT(".")))
				{
					FindCodeFiles(FilenameOrDirectory, FileNames, MaxNumFileNames);
				}
			}
			else
			{
				FString FileName(FilenameOrDirectory);
				if (FileName.EndsWith(TEXT(".h")) || FileName.EndsWith(".cpp"))
				{
					FileNames.Add(FileName);
				}
			}
			return (FileNames.Num() < MaxNumFileNames);
		}
	};

	// Enumerate the contents of the current directory
	FDirectoryVisitor Visitor(FileNames, MaxNumFileNames);
	FPlatformFileManager::Get().GetPlatformFile().IterateDirectory(BaseDirectory, Visitor);
}

IMPLEMENT_CODE_TEST_OBJECT(FProjectTest, "FProjectTest", UProjectTestSettings);
void FProjectTest::RunTest()
{
	//FPlatformMisc::SetOverrideProjectDir(GetObject<UProjectTestSettings>()->ProjectPath);

	TArray<FString> FileNames;
	UProjectTestSettings* Settings = GetObject<UProjectTestSettings>();
	FPlatformMisc::SetOverrideProjectDir(Settings->ProjectPath);
	FindCodeFiles(*(Settings->ProjectPath), FileNames, INT_MAX);
	for (FString File : FileNames)
	{
		PRINT_LOG(File);//拿到了，所有以.h .cpp扩展的文件，包括Intermediate文件夹下
	}

	PRINT_LOG("GameProjectUtils::ProjectHasCodeFiles()" << GameProjectUtils::ProjectHasCodeFiles());
}

IMPLEMENT_CODE_TEST_OBJECT(FModuleSourceFileTest, "FModuleSourceFileTest", UProjectTestSettings);
void FModuleSourceFileTest::RunTest()
{
	UProjectTestSettings* Settings = GetObject<UProjectTestSettings>();
	FString ProjectPath = Settings->ProjectPath;
	if (!ProjectPath.IsEmpty())
	{
		FPlatformMisc::SetOverrideProjectDir(ProjectPath);
		FProjectManager::Get().LoadProjectFile(ProjectPath);
	}

	TArray<FModuleContextInfo> ProjectModules = GameProjectUtils::GetCurrentProjectModules();
	for (FModuleContextInfo Module : ProjectModules)
	{
		PRINT_LOG("ProjectModules : " << Module.ModuleName << "   " << Module.ModuleSourcePath << "   " << Module.ModuleType);
	}

	TArray<FModuleContextInfo> PluginsModules = GameProjectUtils::GetCurrentProjectPluginModules();
	for (FModuleContextInfo Module : PluginsModules)
	{
		PRINT_LOG("PluginsModules : " << Module.ModuleName << "   " << Module.ModuleSourcePath << "   " << Module.ModuleType);
	}
}

IMPLEMENT_CODE_TEST_OBJECT(FFileManagerTest, "FFileManagerTest", UFileManagerTestSettings);
void FFileManagerTest::RunTest()
{
	UFileManagerTestSettings* Settings = GetObject<UFileManagerTestSettings>();
	FString ProjectPath = Settings->ProjectPath;
	FString FileExtension = Settings->FileExtension;
	if (ProjectPath.IsEmpty())
	{
		return;
	}

	PRINT_LOG("Start Time : " << FDateTime::Now().ToString());
	TArray<FString> OutFiles;
	FPlatformFileManager::Get().GetPlatformFile().FindFilesRecursively(OutFiles, *ProjectPath, *FileExtension);
	PRINT_LOG("FindFilesRecursively Time : " << FDateTime::Now().ToString());

	int32 Count = 0;
	int64 TotalLineCount = 0;
	//for (FString FileName : OutFiles)
	//{
	//	TArray<FString> LineArray;
	//	FFileHelper::LoadFileToStringArray(LineArray, *FileName);
	//	int32 LineCount = LineArray.Num();
	//	TotalLineCount += LineCount;
	//	//PRINT_LOG(++Count << "  " << "FileCount : " << LineCount << "   " << FileName);
	//}
	PRINT_LOG(" ====  TotalLineCount : " << TotalLineCount);
	PRINT_LOG("PRINT_LOG Time : " << FDateTime::Now().ToString());
}
#pragma optimize("",on)
#undef LOCTEXT_NAMESPACE