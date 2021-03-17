#include "SContextMenu.h"
#include "ProjectInfo.h"
#include "UPTDefine.h"
#include "MultiBoxBuilder.h"
#include "MultiBoxExtender.h"
#include "Editor/EditorStyle/Public/EditorStyleSet.h"
#include "UIAction.h"
#include "UPTDefine.h"
#include "UPTDelegateCenter.h"
#include "SBorder.h"
#include "SButton.h"
#include "Widgets/UPTStyle.h"
#include "PrintHelper/Public/PrintHelper.h"
#include "Settings/ProjectSettings.h"
#include "Settings/UPTSettings.h"
#include "MonitoredProcess.h"
#include "TabManager.h"
#include "Manager/UPTManager.h"

#define LOCTEXT_NAMESPACE "SContextMenu"

void SContextMenu::Construct(const FArguments& InArgs, TSharedPtr<FProjectInfo> Info)
{
	ProjectInfo = Info;

	TSharedPtr<FExtender> Extender;
	TSharedPtr< const FUICommandList > Commands;

	const bool bInShouldCloseWindowAfterSelection = true;
	FMenuBuilder MenuBuilder(bInShouldCloseWindowAfterSelection, Commands, Extender, true);

	MenuBuilder.AddMenuEntry(
		LOCTEXT("LaunchGame", "Launch Game"),
		LOCTEXT("LaunchGameTip", "Launch Game"),
		FSlateIcon(FUPTStyle::GetStyleSetName(), "CodeBrowser.NewFolderIcon"),
		FUIAction(
			FExecuteAction::CreateSP(this, &SContextMenu::LaunchGame, ProjectInfo),
			FCanExecuteAction()
		));

	MenuBuilder.AddMenuEntry(
		LOCTEXT("OpenProject", "Open Project"),
		LOCTEXT("OpenProjectTip", "Open Project"),
		FSlateIcon(FUPTStyle::GetStyleSetName(), "CodeBrowser.NewFolderIcon"),
		FUIAction(
			FExecuteAction::CreateLambda([this]() { FUPTDelegateCenter::OnOpenProject.ExecuteIfBound(ProjectInfo.ToSharedRef()); }),
			FCanExecuteAction()
		));

	MenuBuilder.AddMenuEntry(
		LOCTEXT("GenerateSolution", "Generate Solution"),
		LOCTEXT("Generate SolutionTip", "Generate Solution Tip"),
		FSlateIcon(FUPTStyle::GetStyleSetName(), "CodeBrowser.NewFolderIcon"),
		FUIAction(
			FExecuteAction::CreateLambda([this]() { FUPTDelegateCenter::OnGenerateSolution.ExecuteIfBound(ProjectInfo.ToSharedRef()); }),
			FCanExecuteAction()
		));

	MenuBuilder.AddMenuEntry(
		LOCTEXT("OpenIDE", "Open IDE"),
		LOCTEXT("OpenIdeTip", "Open IDE"),
		FSlateIcon(FUPTStyle::GetStyleSetName(), "CodeBrowser.NewFolderIcon"),
		FUIAction(
			FExecuteAction::CreateLambda([this]() { FUPTDelegateCenter::OnOpenIDE.ExecuteIfBound(ProjectInfo.ToSharedRef()); }),
			FCanExecuteAction()
		));

	MenuBuilder.AddMenuEntry(
		LOCTEXT("Clear Project", "Clear Project"),
		LOCTEXT("Clear ProjectTip", "Clear Project"),
		FSlateIcon(FUPTStyle::GetStyleSetName(), "CodeBrowser.NewFolderIcon"),
		FUIAction(
			FExecuteAction::CreateLambda([this]() { FUPTDelegateCenter::OnClearProject.ExecuteIfBound(ProjectInfo.ToSharedRef()); }),
			FCanExecuteAction()
		));

	MenuBuilder.AddMenuEntry(
		LOCTEXT("Package", "Package"),
		LOCTEXT("PackageTip", "Package"),
		FSlateIcon(FUPTStyle::GetStyleSetName(), "CodeBrowser.NewFolderIcon"),
		FUIAction(
			FExecuteAction::CreateSP(this, &SContextMenu::PackageProject, ProjectInfo),
			FCanExecuteAction()
		));

	MenuBuilder.AddMenuEntry(
		LOCTEXT("CopyPackageFile", "Copy Package File"),
		LOCTEXT("CopyPackageFileTip", "Copy Package File"),
		FSlateIcon(FUPTStyle::GetStyleSetName(), "CodeBrowser.NewFolderIcon"),
		FUIAction(
			FExecuteAction::CreateSP(this, &SContextMenu::CopyPackageFiles, ProjectInfo),
			FCanExecuteAction()
		));

	MenuBuilder.AddMenuEntry(
		LOCTEXT("Show In Explorer", "Show In Explorer"),
		LOCTEXT("Show In ExplorerTip", "Show In Explorer Tip"),
		FSlateIcon(FUPTStyle::GetStyleSetName(), "CodeBrowser.NewFolderIcon"),
		FUIAction(
			FExecuteAction::CreateLambda([this]() { FUPTDelegateCenter::OnShowInExplorer.ExecuteIfBound(ProjectInfo.ToSharedRef()); }),
			FCanExecuteAction()
		));

	MenuBuilder.AddMenuEntry(
		LOCTEXT("Managed Code", "Managed Code"),
		LOCTEXT("Managed Code Tip", "Managed Code Tip"),
		FSlateIcon(FUPTStyle::GetStyleSetName(), "CodeBrowser.NewFolderIcon"),
		FUIAction(
			FExecuteAction::CreateLambda([this]() { FUPTDelegateCenter::OnOpenCodeMgrWindow.ExecuteIfBound(ProjectInfo.ToSharedRef()); }),
			FCanExecuteAction()
		));

	MenuBuilder.AddMenuEntry(
		LOCTEXT("Add Code File", "Add Code File"),
		LOCTEXT("Add Code File Tip", "Add Code File Tip"),
		FSlateIcon(FUPTStyle::GetStyleSetName(), "CodeBrowser.NewFolderIcon"),
		FUIAction(
			FExecuteAction::CreateLambda([this]() { FUPTDelegateCenter::OnAddNewCodeFile.ExecuteIfBound(ProjectInfo.ToSharedRef()); }),
			FCanExecuteAction()
		));

	ChildSlot
		[
			MenuBuilder.MakeWidget()
		];
}

void SContextMenu::LaunchGame(TSharedPtr<FProjectInfo> Info)
{
	if (Info.IsValid() == false)
		return;

	FString EngineDir = Info->GetEnginePath();
	FString ProjectFile = Info->GetProjectPath();

	FNotificationInfo NInfo(FText::Format(LOCTEXT("LaunchGameNotification", "Launch Game {0} Succeed"), FText::FromString(FPaths::GetBaseFilename(ProjectFile))));
	NInfo.ExpireDuration = 5;
	NInfo.bUseLargeFont = false;

	//判断工程路径是不是.uproject文件
	if (ProjectFile.IsEmpty() || !ProjectFile.EndsWith(TEXT(".uproject")))
	{
		PRINT_ERROR("Project path is error");
		NInfo.Text = FText::Format(LOCTEXT("LaunchGameNotification", "Launch Game {0} Failed"), FText::FromString(FPaths::GetBaseFilename(ProjectFile)));
		FUPTDelegateCenter::OnRequestAddNotification.Execute(NInfo);
		return;
	}

	FString ExeFilename;
	ExeFilename = EngineDir + "/Engine/Binaries/Win64/UE4Editor.exe";
	PRINT_LOG(ExeFilename);

	//判断UE4Editor.exe是否存在
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*ExeFilename))
	{
		PRINT_ERROR("Engine path is error");
		return;
	}

	FUPTDelegateCenter::OnRequestAddNotification.Execute(NInfo);

#if PLATFORM_WINDOWS
	//使用CMD打开工程
	FString Cmd = FString::Printf(TEXT("%s"), *(ProjectFile + TEXT(" -game")));
	FProcHandle Handle = FPlatformProcess::CreateProc(*ExeFilename, *Cmd, true, false, false, NULL, 0, NULL, NULL);
	if (!Handle.IsValid())
	{
		PRINT_ERROR("Failed to create process");
		return;
	}
	FUPTDelegateCenter::OnOpenProject.ExecuteIfBound(ProjectInfo.ToSharedRef());
	FPlatformProcess::CloseProc(Handle);
#endif
}

void SContextMenu::PackageProject(TSharedPtr<FProjectInfo> Info)
{
	if (Info.IsValid() == false)
		return;

	FString EngineDir = Info->GetEnginePath();

	FString RunUAT;
	RunUAT = EngineDir + "/Engine/Build/BatchFiles/RunUAT.bat";

	//判断RunUAT.bat是否存在
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*RunUAT))
	{
		PRINT_ERROR(FString::Printf(TEXT("RunUAT.bat no exists  %s"), *RunUAT));
		return;
	}

	FProjectSettings ProjectSettings = UUPTSettings::GetProjectSetting(Info->GetProjectPath());
	if (FPaths::FileExists(ProjectSettings.ProjectPath))
	{
		FString CmdExe = TEXT("cmd.exe");
		FString FullCommandLine = FString::Printf(TEXT("/c \"\"%s\" %s\""), *RunUAT, *ProjectSettings.PackingParameters);
		TSharedPtr<FMonitoredProcess> UatProcess = MakeShareable(new FMonitoredProcess(CmdExe, FullCommandLine, true));
		UatProcess->OnCanceled().BindRaw(this, &SContextMenu::HandleUatProcessCanceled);
		UatProcess->OnCompleted().BindSP(this, &SContextMenu::HandleUatProcessCompleted);
		UatProcess->OnOutput().BindLambda([](FString Log) {FUPTManager::Get()->PrintLog(Log); });
		//UatProcess->OnOutput().BindStatic(&SContextMenu::HandleUatProcessOutput);

		if (UatProcess->Launch())
		{
			//FGlobalTabmanager::Get()->InvokeTab(FName(TEXT("OutputLog")));
			PRINT_LOG("UatProcess->Launch()");
		}
	}

#if PLATFORM_WINDOWS && 0
	//使用CMD打开工程
	FProjectSettings ProjectSettings = UUPTSettings::GetProjectSetting(Info->GetProjectPath());
	if (FPaths::FileExists(ProjectSettings.ProjectPath))
	{
		FString CommandLine = FString::Printf(TEXT(" %s %s"), *RunUAT, *ProjectSettings.PackingParameters);
		//FString CommandLine = FString::Printf(TEXT("%s"), *ProjectSettings.PackingParameters);
		FProcHandle Handle = FPlatformProcess::CreateProc(TEXT("C:/Windows/system32/cmd.exe"), *CommandLine, true, false, false, NULL, 0, NULL, NULL);
		if (!Handle.IsValid())
		{
			PRINT_ERROR("Failed to create process");
			return;
		}
		FPlatformProcess::CloseProc(Handle);
	}
#endif
}

void SContextMenu::CopyPackageFiles(TSharedPtr<FProjectInfo> Info)
{
	if (Info.IsValid() == false)
		return;

	FString ProjectPath = Info->GetProjectPath();
	FPaths::NormalizeFilename(ProjectPath);
	if (FPaths::FileExists(ProjectPath) == false)
		return;

	const FString ProjectDir = FPaths::GetPath(ProjectPath);
	FString DefaultEditorIni = ProjectDir + TEXT("/Config/DefaultEditor.ini");
	if (FPaths::FileExists(DefaultEditorIni))
	{
		PRINT_LOG(FString::Printf(TEXT("DefaultEditorIni: %s"), *DefaultEditorIni));
		TArray<FString> FileDirectorys;
		GConfig->GetArray(TEXT("/PackageCompletedCopyResource"), TEXT("+FileDirectorys"), FileDirectorys, DefaultEditorIni);

		if (const UUPTSettings* UPTSettings = GetDefault<UUPTSettings>())
		{
			FProjectSettings ProjectSettings = UUPTSettings::GetProjectSetting(ProjectPath);
			FString PackagePath = ProjectSettings.PackageDir;
			FPaths::NormalizeDirectoryName(PackagePath);

			if (FPaths::DirectoryExists(PackagePath))
			{
				for (FString FileDir : FileDirectorys)
				{
					const FString Src = ProjectDir / FileDir;
					const FString Dest = PackagePath / FileDir;
					if (IFileManager::Get().FileExists(*Src))
					{
						bool Result = IFileManager::Get().Copy(*Dest, *Src) == COPY_OK;
						PRINT_LOG(FString::Printf(TEXT("%s Copy File To >> %s : %s"), *Src, *Dest, *(Result ? FCoreTexts::Get().True : FCoreTexts::Get().False).ToString()));
						continue;
					}

					if (IFileManager::Get().DirectoryExists(*Src))
					{
						FString Directory = FPaths::GetPath(Dest);
						if (!IFileManager::Get().DirectoryExists(*Directory))
							IFileManager::Get().MakeDirectory(*Directory, true);

						bool Result = FPlatformFileManager::Get().GetPlatformFile().CopyDirectoryTree(*Dest, *Src, true);
						PRINT_LOG(FString::Printf(TEXT("%s Copy Directory To >> %s : %s"), *Src, *Dest, *(Result ? FCoreTexts::Get().True : FCoreTexts::Get().False).ToString()));
						continue;
					}

					PRINT_LOG(FString::Printf(TEXT("File or directory does not exist : %s"), *FileDir));
				}
			}
			else
			{
				PRINT_ERROR(FString::Printf(TEXT("%s not exists"), *PackagePath));
			}
		}
	}
	else
	{
		PRINT_ERROR(FString::Printf(TEXT("%s not exists : "), *DefaultEditorIni));
	}
}

void SContextMenu::HandleUatProcessCanceled()
{
}

void SContextMenu::HandleUatProcessCompleted(int32 Result)
{
}

void SContextMenu::HandleUatProcessOutput(FString Message)
{
	UE_LOG(LogTemp, Log, TEXT("%s"), *Message);
	//PRINT_LOG(Message);
}

#undef LOCTEXT_NAMESPACE