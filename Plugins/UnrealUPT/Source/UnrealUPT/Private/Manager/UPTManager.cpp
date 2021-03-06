#include "UPTManager.h"
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
#include "UPTDelegateCenter.h"
#include "ProjectInfo.h"
#include "ProjectDescriptor.h"
#include "PlatformFilemanager.h"
#include "CommandLine.h"
#include "Programs/UnrealVersionSelector/Private/PlatformInstallation.h"
#include "JsonReader.h"
#include "JsonSerializer.h"
#include "IPluginManager.h"
#include "TabManager.h"
#include "SCodeMgr.h"
#include "SDockTab.h"
#include "CreateCodeFileWidget.h"
#include "UtilityTool.h"
#include "PrintHelper.h"
#include <Async.h>
#include "SNotificationList.h"
//#include "Engine.h"

#define LOCTEXT_NAMESPACE "FEPManager"

static const FName CodeMgrWindow("CodeMgrWindow");
static const FName AddNewCodeFileWindow("AddNewCodeFileWindow");

TSharedPtr<FUPTManager> FUPTManager::Get()
{
	static TSharedPtr<FUPTManager> Instance = MakeShareable(new FUPTManager());
	return Instance;
}

void FUPTManager::Initialize()
{
	FUPTDelegateCenter::OnLaunchGame.BindLambda([this](TSharedRef<FProjectInfo> Info) { LaunchGame(Info); });
	FUPTDelegateCenter::OnOpenProject.BindLambda([this](TSharedRef<FProjectInfo> Info) { OpenProject(Info); });
	FUPTDelegateCenter::OnOpenIDE.BindLambda([this](TSharedRef<FProjectInfo> Info) { OpenCodeIDE(Info); });
	FUPTDelegateCenter::OnGenerateSolution.BindLambda([this](TSharedRef<FProjectInfo> Info) { GenerateSolution(Info); });
	FUPTDelegateCenter::OnShowInExplorer.BindLambda([this](TSharedRef<FProjectInfo> Info) { ShowInExplorer(Info); });
	FUPTDelegateCenter::OnClearProject.BindLambda([this](TSharedRef<FProjectInfo> Info) { OpenClearProjectWindow(Info); });
	FUPTDelegateCenter::OnOpenCodeMgrWindow.BindLambda([this](TSharedRef<FProjectInfo> Info) { OpenManagedCodeWindow(Info); });
	FUPTDelegateCenter::OnAddNewCodeFile.BindLambda([this](TSharedRef<FProjectInfo> Info) { AddNewCodeFile(Info); });

	FGlobalTabmanager::Get()->RegisterTabSpawner(CodeMgrWindow, FOnSpawnTab::CreateRaw(this, &FUPTManager::SpawnCodeMgrWindow, CodeMgrWindow))
		.SetDisplayName((LOCTEXT("CodeMgrTabTile", "Code Mgr")));

	FGlobalTabmanager::Get()->RegisterTabSpawner(AddNewCodeFileWindow, FOnSpawnTab::CreateRaw(this, &FUPTManager::SpawnAddNewCodeFileWindow, AddNewCodeFileWindow))
		.SetDisplayName((LOCTEXT("AddNewCodeFileTabTile", "Add New Code File")));

	FUPTDelegateCenter::OnRequestAddNotification.BindRaw(this, &FUPTManager::AddNotification);

	const FString BasePath = TEXT("G:/UnrealProjects/UPTTemp/trunk/Source/Programs/UnrealProgrammerTool/Source");
	const FString DesBasePath = TEXT("G:/UnrealProjects/UnrealUPT/trunk/Plugins/UnrealUPT/Source/UnrealUPT");
	const FString DesPrivate = DesBasePath / TEXT("Private");
	const FString DesPublic = DesBasePath / TEXT("Public");

#if 0
	TArray<FString> OutFiles;
	IFileManager::Get().FindFilesRecursive(OutFiles, *BasePath, TEXT("*.h"), true, false);
	OutFiles.RemoveAll([](const FString File) { return File.Contains(TEXT("PrintHelper")); });
	for (const FString File : OutFiles)
	{
		const FString FileName = File.Replace(*BasePath, TEXT(""));
		const FString filename = DesPublic / FileName;
		const FString Directory = FPaths::GetPath(filename);
		if (!IFileManager::Get().DirectoryExists(*Directory))
			IFileManager::Get().MakeDirectory(*Directory);

		IFileManager::Get().Copy(*filename, *File);
	}

	IFileManager::Get().FindFilesRecursive(OutFiles, *BasePath, TEXT("*.cpp"), true, false);
	OutFiles.RemoveAll([](const FString File) { return File.Contains(TEXT("PrintHelper")); });
	for (const FString File : OutFiles)
	{
		const FString FileName = File.Replace(*BasePath, TEXT(""));
		const FString filename = DesPrivate / FileName;
		const FString Directory = FPaths::GetPath(filename);
		if (!IFileManager::Get().DirectoryExists(*Directory))
			IFileManager::Get().MakeDirectory(*Directory);

		IFileManager::Get().Copy(*filename, *File);
	}
#endif
}

void FUPTManager::Deinitialize()
{
	FGlobalTabmanager::Get()->UnregisterTabSpawner(CodeMgrWindow);
	FGlobalTabmanager::Get()->UnregisterTabSpawner(AddNewCodeFileWindow);
}

TArray<FString> FUPTManager::GetAllEngineRootDir()
{
	TMap<FString, FString> OutInstallations;
	FDesktopPlatformModule::Get()->EnumerateEngineInstallations(OutInstallations);

	TArray<FString> RootDirs;
	if (OutInstallations.Num() > 0)
		OutInstallations.GenerateValueArray(RootDirs);

	return RootDirs;
}

void FUPTManager::GetEngineVersion(TArray<FString>& EngineDirs, TArray<FString>& Versions)
{
	for (const FString RootDir : EngineDirs)
	{
		FEngineVersion Version;
		FDesktopPlatformModule::Get()->TryGetEngineVersion(RootDir, Version);
		Versions.Add(Version.ToString());
	}
}

TArray<FString> FUPTManager::GetProjectPathsByEngineRootDir(const FString& RootDir)
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

void FUPTManager::GetProjectNames(TArray<FString>& ProjectPaths, TArray<FString>& ProjectNames)
{
	for (const FString ProjectPath : ProjectPaths)
	{
		FString Name = FPaths::GetBaseFilename(ProjectPath);
		ProjectNames.Add(Name);
	}
}

TArray<TSharedPtr<FProjectInfo>> FUPTManager::GetAllProjectInfos()
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

TSharedPtr<FSlateBrush> FUPTManager::GetProjectThumbnail(const FString& ProjectPath)
{
	TSharedPtr<FSlateDynamicImageBrush> Brush = nullptr;
	bool bSucceeded = false;

	const FString ProjectDir = FPaths::GetPath(ProjectPath);
	const FString ProjectName = FPaths::GetBaseFilename(ProjectPath);

	FString Identifier;
	FDesktopPlatformModule::Get()->GetEngineIdentifierForProject(ProjectPath, Identifier);

	const FString ProjectThumbnail_TargetImagePath = ProjectDir / FString::Printf(TEXT("%s.png"), *ProjectName);
	const FString ProjectThumbnail_AutomaticImagePath = ProjectDir / TEXT("Saved/AutoScreenshot.png");
	const FString DefaultProjectThumbnailPath = UPT_RESOURCE_PATH / TEXT("default_game_thumbnail_192x.png");

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
				TArray<uint8> RawData;
				if (ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, RawData))
				{
					if (FSlateApplication::Get().GetRenderer()->GenerateDynamicImageResource(*ImagePath, ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), RawData))
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

const bool FUPTManager::EngineIsDistribution(const FString& Identifer)
{
	FString EngineDir;
	FDesktopPlatformModule::Get()->GetEngineRootDirFromIdentifier(Identifer, EngineDir);
	return FDesktopPlatformModule::Get()->IsSourceDistribution(EngineDir);
}

const bool FUPTManager::EngineIsDistributionByRootDir(const FString& EngineDir)
{
	return FDesktopPlatformModule::Get()->IsSourceDistribution(EngineDir);
}

bool FUPTManager::LaunchGame(TSharedRef<FProjectInfo> Info)
{
	FString EngineDir = Info->GetEnginePath();
	FString ProjectFile = Info->GetProjectPath();

	FNotificationInfo NInfo(FText::Format(LOCTEXT("LaunchGameNotification", "Launch Game {0} Succeed"), FText::FromString(FPaths::GetBaseFilename(ProjectFile))));
	NInfo.ExpireDuration = 5;
	NInfo.bUseLargeFont = false;

	//判断工程路径是不是.uproject文件
	if (ProjectFile.IsEmpty() || !ProjectFile.EndsWith(TEXT(".uproject")))
	{
		UE_LOG(UPTLog, Error, TEXT("Project path is error"));
		NInfo.Text = FText::Format(LOCTEXT("LaunchGameNotification", "Launch Game {0} Failed"), FText::FromString(FPaths::GetBaseFilename(ProjectFile)));
		FUPTDelegateCenter::OnRequestAddNotification.Execute(NInfo);
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

	FUPTDelegateCenter::OnRequestAddNotification.Execute(NInfo);

#if PLATFORM_WINDOWS
	//使用CMD打开工程
	FString Cmd = FString::Printf(TEXT("%s"), *(ProjectFile + TEXT(" -game")));
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

bool FUPTManager::OpenProject(TSharedRef<FProjectInfo> Info)
{
	FString EngineDir = Info->GetEnginePath();
	FString ProjectFile = Info->GetProjectPath();

	FNotificationInfo NInfo(FText::Format(LOCTEXT("OpenProjectNotification", "Open Project {0} Succeed"), FText::FromString(FPaths::GetBaseFilename(ProjectFile))));
	NInfo.ExpireDuration = 5;
	NInfo.bUseLargeFont = false;

	//判断工程路径是不是.uproject文件
	if (ProjectFile.IsEmpty() || !ProjectFile.EndsWith(TEXT(".uproject")))
	{
		UE_LOG(UPTLog, Error, TEXT("Project path is error"));
		NInfo.Text = FText::Format(LOCTEXT("OpenProjectNotification", "Open Project {0} Failed"), FText::FromString(FPaths::GetBaseFilename(ProjectFile)));
		FUPTDelegateCenter::OnRequestAddNotification.Execute(NInfo);
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

	FUPTDelegateCenter::OnRequestAddNotification.Execute(NInfo);

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

bool FUPTManager::OpenCodeIDE(TSharedRef<FProjectInfo> Info)
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

void FUPTManager::GenerateSolution(TSharedRef<FProjectInfo> Info)
{
	FString ProjectFileName = Info->GetProjectPath();

	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();

	// Check it's a code project
	FString SourceDir = FPaths::GetPath(ProjectFileName) / TEXT("Source");
	if (!IPlatformFile::GetPlatformPhysical().DirectoryExists(*SourceDir))
	{
		FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("This project does not have any source code. You need to add C++ source files to the project from the Editor before you can generate project files."), TEXT("Error"));
	}

	// Get the engine root directory
	FString RootDir = Info->GetEnginePath();

	AsyncTask(ENamedThreads::GameThread, [DesktopPlatform, RootDir, ProjectFileName]()
		{
			// Generate project files
			FFeedbackContext* Warn = DesktopPlatform->GetNativeFeedbackContext();
			const bool bResult = DesktopPlatform->GenerateProjectFiles(RootDir, ProjectFileName, Warn, FString::Printf(TEXT("%s/Saved/Logs/%s-%s.log"), *FPaths::GetPath(ProjectFileName), FPlatformProcess::ExecutableName(), *FDateTime::Now().ToString()));
			if (!bResult)
			{
				// Display an error dialog if we failed
				FPlatformMisc::MessageBoxExt(EAppMsgType::Ok, TEXT("Failed to generate project files."), TEXT("Error"));
			}
		});
}

bool FUPTManager::ShowInExplorer(TSharedRef<FProjectInfo> Info)
{
	if (!Info->GetProjectPath().IsEmpty())
	{
		FPlatformProcess::ExploreFolder(*(Info->GetProjectPath()));
		return true;
	}

	return false;
}

void FUPTManager::OpenClearProjectWindow(TSharedRef<FProjectInfo> Info)
{
	EAppReturnType::Type ReturnType = FPlatformMisc::MessageBoxExt(EAppMsgType::YesNoCancel, TEXT("Are you sure you want to clear the solution."), TEXT("Clear Solution"));
	if (ReturnType != EAppReturnType::Yes)
		return;

	TArray<FString> Directorys;
	const FString ProjectDirectory = FPaths::GetPath(Info->GetProjectPath());
	const FString PluginsDirectory = ProjectDirectory / TEXT("Plugins");

	const FString Binaries(TEXT("Binaries"));
	const FString Intermediate(TEXT("Intermediate"));

	Directorys.Add(ProjectDirectory / Binaries);
	Directorys.Add(ProjectDirectory / TEXT("Intermediate"));
	TArray<FString> Plugins;
	IFileManager::Get().FindFiles(Plugins, *(PluginsDirectory / TEXT("*")), false, true);
	for (const FString PluginName : Plugins)
	{
		Directorys.Add(PluginsDirectory / PluginName / Binaries);
		Directorys.Add(PluginsDirectory / PluginName / Intermediate);
	}

	for (const FString Directory : Directorys)
	{
		FOnFileDirectoryActionFinished OnFinished = FOnFileDirectoryActionFinished::CreateLambda([Directory](const bool bSuccess)
			{
				PRINT_LOG(FString::Printf(TEXT("delete directory %s is %s."), *Directory, *(bSuccess ? FCoreTexts::Get().True.ToString() : FCoreTexts::Get().False.ToString())));
				FNotificationInfo Info(FText::Format(LOCTEXT("ClearSolutionNotification", "Delecte {0} is {1}"), FText::FromString(FPaths::GetCleanFilename(Directory)), bSuccess ? FCoreTexts::Get().True : FCoreTexts::Get().False));
				Info.ExpireDuration = 5;
				Info.bUseLargeFont = false;
				Info.bUseSuccessFailIcons = bSuccess;
				FUPTDelegateCenter::OnRequestAddNotification.Execute(Info);
			});
		UPTUtility::AsyncDeleteDirectory(OnFinished, Directory, true, true);
	}
}

TSharedRef<SDockTab> FUPTManager::SpawnCodeMgrWindow(const FSpawnTabArgs& Args, FName TabIdentifier)
{
	if (TabIdentifier == CodeMgrWindow)
	{
		const TSharedRef<SDockTab> DockTab = SNew(SDockTab)
			.TabRole(ETabRole::PanelTab);

		DockTab->SetContent(SNew(SCodeMgr, DockTab, Args.GetOwnerWindow()));
		return DockTab;
	}

	return  SNew(SDockTab);
}

TSharedRef<SDockTab> FUPTManager::SpawnAddNewCodeFileWindow(const FSpawnTabArgs& Args, FName TabIdentifier)
{
	if (TabIdentifier == AddNewCodeFileWindow)
	{
		const TSharedRef<SDockTab> DockTab = SNew(SDockTab)
			.TabRole(ETabRole::PanelTab);

		DockTab->SetContent(SNew(SCreateCodeFileWidget));
		return DockTab;
	}

	return  SNew(SDockTab);
}

void FUPTManager::OpenManagedCodeWindow(TSharedRef<FProjectInfo> Info)
{
	FGlobalTabmanager::Get()->TryInvokeTab(CodeMgrWindow);
}

void FUPTManager::AddNewCodeFile(TSharedRef<FProjectInfo> Info)
{
	FGlobalTabmanager::Get()->TryInvokeTab(AddNewCodeFileWindow);
}

TSharedPtr<FJsonObject> FUPTManager::LoadProjectFile(const FString& FileName)
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

TSharedRef<SNotificationItem> FUPTManager::AddNotification(FNotificationInfo Info)
{
	return NotificationListPtr->AddNotification(Info);
}

#undef LOCTEXT_NAMESPACE