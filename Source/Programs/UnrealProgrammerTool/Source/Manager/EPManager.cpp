#include "EPManager.h"
#include "DesktopPlatform/Public/DesktopPlatformModule.h"
#include "Paths.h"
#include "FileHelper.h"
#include "IImageWrapperModule.h"
#include "ModuleManager.h"
#include "IImageWrapper.h"
#include "SlateApplication.h"
#include "EngineVersion.h"
#include "UProjectInfo.h"
#include "ISourceCodeAccessModule.h"
#include "ISourceCodeAccessor.h"
#include "UPTDefine.h"
#include "DelegateCenter.h"
#include "ProjectInfo.h"
#include "ProjectDescriptor.h"
#include "PlatformFilemanager.h"
#include "CommandLine.h"
#include "Programs/UnrealVersionSelector/Private/PlatformInstallation.h"
#include "JsonReader.h"
#include "JsonSerializer.h"

#define LOCTEXT_NAMESPACE "FEPManager"

TSharedPtr<FEPManager> FEPManager::Get()
{
	static TSharedPtr<FEPManager> Instance = MakeShareable(new FEPManager());
	return Instance;
}

void FEPManager::Initialize()
{
	FUPTDelegateCenter::OnOpenProject.BindLambda([this](TSharedRef<FProjectInfo> Info) { OpenProject(Info); });
	FUPTDelegateCenter::OnOpenIDE.BindLambda([this](TSharedRef<FProjectInfo> Info) { OpenCodeIDE(Info); });
	FUPTDelegateCenter::OnGenerateSolution.BindLambda([this](TSharedRef<FProjectInfo> Info) { GenerateSolution(Info); });
	FUPTDelegateCenter::OnShowInExplorer.BindLambda([this](TSharedRef<FProjectInfo> Info) { ShowInExplorer(Info); });
	FUPTDelegateCenter::OnClearSolution.BindLambda([this](TSharedRef<FProjectInfo> Info) { OpenClearSolutionWindow(Info); });
	FUPTDelegateCenter::OnManagedCode.BindLambda([this](TSharedRef<FProjectInfo> Info) { OpenManagedCodeWindow(Info); });

}

TArray<FString> FEPManager::GetAllEngineRootDir()
{
	TMap<FString, FString> OutInstallations;
	FDesktopPlatformModule::Get()->EnumerateEngineInstallations(OutInstallations);

	TArray<FString> RootDirs;
	if (OutInstallations.Num() > 0)
		OutInstallations.GenerateValueArray(RootDirs);

	return RootDirs;
}

void FEPManager::GetEngineVersion(TArray<FString>& EngineDirs, TArray<FString>& Versions)
{
	for (const FString RootDir : EngineDirs)
	{
		FEngineVersion Version;
		FDesktopPlatformModule::Get()->TryGetEngineVersion(RootDir, Version);
		Versions.Add(Version.ToString());
	}
}

TArray<FString> FEPManager::GetProjectPathsByEngineRootDir(const FString& RootDir)
{
	FString Identifer;
	FDesktopPlatformModule::Get()->GetEngineIdentifierFromRootDir(RootDir, Identifer);

	TArray<FString> OutProjectFileNames, TempProjectFiles;

	FDesktopPlatformModule::Get()->EnumerateProjectsKnownByEngine(Identifer, false, OutProjectFileNames);

	OutProjectFileNames.RemoveAll([](const FString& ProjectFileName)
	{
		return !FPlatformFileManager::Get().GetPlatformFile().FileExists(*ProjectFileName);
	});

	return OutProjectFileNames;
}

void FEPManager::GetProjectNames(TArray<FString>& ProjectPaths, TArray<FString>& ProjectNames)
{
	for (const FString ProjectPath : ProjectPaths)
	{
		FString Name = FPaths::GetBaseFilename(ProjectPath);
		ProjectNames.Add(Name);
	}
}

TArray<TSharedPtr<FProjectInfo>> FEPManager::GetAllProjectInfos()
{
	TArray<TSharedPtr<FProjectInfo>> Result;
	TArray<FString> AllEngineRootDir = GetAllEngineRootDir();

	TArray<FString> TempProjects;
	for (const FString RootDir : AllEngineRootDir)
	{
		TArray<FString> Projects = GetProjectPathsByEngineRootDir(RootDir);
		for (const FString Project : Projects)
		{
			TempProjects.Add(Project);
		}
	}

	//数组去重
	TSet<FString> ArrayToTeavy(TempProjects);

	for (const FString Project : ArrayToTeavy)
	{
		TSharedPtr<FSlateBrush> Thumbnail = GetProjectThumbnail(Project);
		TSharedPtr<FProjectInfo> Info = MakeShareable(new FProjectInfo(Project, Thumbnail));
		Result.Add(Info);
	}

	return Result;
}

TSharedPtr<FSlateBrush> FEPManager::GetProjectThumbnail(const FString& ProjectPath)
{
	TSharedPtr<FSlateDynamicImageBrush> Brush = nullptr;
	bool bSucceeded = false;

	const FString ProjectDir = FPaths::GetPath(ProjectPath);
	const FString ProjectName = FPaths::GetBaseFilename(ProjectPath);

	FString Identifier, EngineRootDir;
	FDesktopPlatformModule::Get()->GetEngineIdentifierForProject(ProjectPath, Identifier);
	FDesktopPlatformModule::Get()->GetEngineRootDirFromIdentifier(Identifier, EngineRootDir);

	const FString ProjectThumbnail_TargetImagePath = ProjectDir / FString::Printf(TEXT("%s.png"), *ProjectName);
	const FString ProjectThumbnail_AutomaticImagePath = ProjectDir / TEXT("Saved/AutoScreenshot.png");
	const FString DefaultProjectThumbnailPath = EngineRootDir / TEXT("Content") / TEXT("Editor") / TEXT("Slate") / TEXT("GameProjectDialog") / TEXT("default_game_thumbnail_192x.png");

	FString ImagePath;
	if (FPaths::FileExists(ProjectThumbnail_TargetImagePath))
		ImagePath = ProjectThumbnail_TargetImagePath;
	else	if (FPaths::FileExists(ProjectThumbnail_AutomaticImagePath))
		ImagePath = ProjectThumbnail_AutomaticImagePath;
	else
		ImagePath = DefaultProjectThumbnailPath;

	TArray<uint8> RawFileData;
	if (FFileHelper::LoadFileToArray(RawFileData, *ImagePath))
	{
		IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
		TSharedPtr<IImageWrapper> ImageWrappers[4] =
		{
			ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG),
			ImageWrapperModule.CreateImageWrapper(EImageFormat::BMP),
			ImageWrapperModule.CreateImageWrapper(EImageFormat::ICO),
			ImageWrapperModule.CreateImageWrapper(EImageFormat::ICNS),
		};

		for (auto ImageWrapper : ImageWrappers)
		{
			if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(RawFileData.GetData(), RawFileData.Num()))
			{
				const TArray<uint8>* RawData = NULL;
				if (ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, RawData))
				{
					if (FSlateApplication::Get().GetRenderer()->GenerateDynamicImageResource(*ImagePath, ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), *RawData))
					{
						Brush = MakeShareable(new FSlateDynamicImageBrush(*ImagePath, FVector2D(ImageWrapper->GetWidth(), ImageWrapper->GetHeight())));
						bSucceeded = true;
						break;
					}
				}
			}
		}

		if (!bSucceeded)
		{
			UE_LOG(UPTLog, Log, TEXT("Only BGRA pngs, bmps or icos are supported in by External Image Picker"));
		}
	}
	else
	{
		UE_LOG(UPTLog, Log, TEXT("Could not find file for image: %s"), *ImagePath);
	}

	return Brush;
}

bool FEPManager::OpenProject(TSharedRef<FProjectInfo> Info)
{
	FString EngineDir = Info->GetEnginePath();
	FString ProjectFile = Info->GetProjectPath();

	//判断工程路径是不是.uproject文件
	if (ProjectFile.IsEmpty() || !ProjectFile.EndsWith(TEXT(".uproject")))
	{
		UE_LOG(UPTLog, Error, TEXT("Project path is error"));
		return 0;
	}

	FString ExeFilename;
	ExeFilename = EngineDir + "/Engine/Binaries/Win64/UE4Editor.exe";
	GLog->Log(ExeFilename);

	//判断UE4Editor.exe是否存在
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*ExeFilename))
	{
		UE_LOG(UPTLog, Error, TEXT("Engine path is error"));
		return 0;
	}

#if PLATFORM_WINDOWS
	//使用CMD打开工程
	FString PendingProjName = FString::Printf(TEXT("\"%s\""), *ProjectFile);
	FString Cmd = FString::Printf(TEXT("%s %s"), *PendingProjName, FCommandLine::Get());
	FProcHandle Handle = FPlatformProcess::CreateProc(*ExeFilename, *Cmd, true, false, false, NULL, 0, NULL, NULL);
	if (!Handle.IsValid())
	{
		UE_LOG(UPTLog, Error, TEXT("Failed to create process"));
		return 0;
	}
	FPlatformProcess::CloseProc(Handle);
#endif
	return true;
}

bool FEPManager::OpenCodeIDE(TSharedRef<FProjectInfo> Info)
{
	FString ProjectFile = Info->GetProjectPath();
	FText OutFailReason;

	if (ProjectFile.IsEmpty())
	{
		OutFailReason = LOCTEXT("NoProjectFileSpecified", "You must specify a project file.");
		UE_LOG(UPTLog, Log, TEXT("Could not find file for image: %s"), *(OutFailReason.ToString()));
		return false;
	}

	// Check whether this project is a foreign project. Don't use the cached project dictionary; we may have just created a new project.
	FString SolutionFolder;
	FString SolutionFilenameWithoutExtension;
	if (FUProjectDictionary(FPaths::RootDir()).IsForeignProject(ProjectFile))
	{
		SolutionFolder = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*FPaths::GetPath(ProjectFile));
		SolutionFilenameWithoutExtension = FPaths::GetBaseFilename(ProjectFile);
	}
	else
	{
		SolutionFolder = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*FPaths::RootDir());
		SolutionFilenameWithoutExtension = TEXT("UE4");
	}

	ISourceCodeAccessModule& SourceCodeAccessModule = FModuleManager::LoadModuleChecked<ISourceCodeAccessModule>("SourceCodeAccess");
	bool bCanOpenProjectSolution = SourceCodeAccessModule.GetAccessor().OpenSolutionAtPath(FPaths::Combine(SolutionFolder, SolutionFilenameWithoutExtension));

	if (!bCanOpenProjectSolution)
	{
		FFormatNamedArguments Args;
		Args.Add(TEXT("AccessorName"), SourceCodeAccessModule.GetAccessor().GetNameText());
		OutFailReason = FText::Format(LOCTEXT("OpenCodeIDE_FailedToOpen", "Failed to open selected source code accessor '{AccessorName}'"), Args);
		UE_LOG(UPTLog, Log, TEXT("Could not find file for image: %s"), *(OutFailReason.ToString()));
		return false;
	}

	return true;
}

bool FEPManager::GenerateSolution(TSharedRef<FProjectInfo> Info)
{
	FString ProjectFileName = Info->GetProjectPath();

	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();

	// Check it's a code project
	FString SourceDir = FPaths::GetPath(ProjectFileName) / TEXT("Source");
	if (!IPlatformFile::GetPlatformPhysical().DirectoryExists(*SourceDir))
	{
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("This project does not have any source code. You need to add C++ source files to the project from the Editor before you can generate project files."), TEXT("Error"));
		return false;
	}

	// Get the engine root directory
	FString RootDir = Info->GetEnginePath();

	// Start capturing the log output
	FStringOutputDevice LogCapture;
	LogCapture.SetAutoEmitLineTerminator(true);
	GLog->AddOutputDevice(&LogCapture);

	// Generate project files
	FFeedbackContext* Warn = DesktopPlatform->GetNativeFeedbackContext();
	bool bResult = DesktopPlatform->GenerateProjectFiles(RootDir, ProjectFileName, Warn, FString::Printf(TEXT("%s/Saved/Logs/%s-%s.log"), *FPaths::GetPath(ProjectFileName), FPlatformProcess::ExecutableName(), *FDateTime::Now().ToString()));
	GLog->RemoveOutputDevice(&LogCapture);

	// Display an error dialog if we failed
	if (!bResult)
	{
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("Failed to generate project files."), TEXT("Error"));
		return false;
	}

	return true;
}

bool FEPManager::ShowInExplorer(TSharedRef<FProjectInfo> Info)
{
	if (!Info->GetProjectPath().IsEmpty())
	{
		FPlatformProcess::ExploreFolder(*(Info->GetProjectPath()));
		return true;
	}

	return false;
}

void FEPManager::OpenClearSolutionWindow(TSharedRef<FProjectInfo> Info)
{

}

void FEPManager::OpenManagedCodeWindow(TSharedRef<FProjectInfo> Info)
{

}

TSharedPtr<FJsonObject> FEPManager::LoadProjectFile(const FString &FileName)
{
	FString FileContents;

	if (!FFileHelper::LoadFileToString(FileContents, *FileName))
	{
		return TSharedPtr<FJsonObject>(NULL);
	}

	TSharedPtr< FJsonObject > JsonObject;
	TSharedRef< TJsonReader<> > Reader = TJsonReaderFactory<>::Create(FileContents);
	if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
	{
		return TSharedPtr<FJsonObject>(NULL);
	}

	return JsonObject;
}

#undef LOCTEXT_NAMESPACE