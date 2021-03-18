// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SlateBrush.h"
#include "UPTDelegateCenter.h"
#include "SNotificationList.h"
#include "PrintHelper/Public/PrintHelper.h"
#include "MonitoredProcess.h"

struct FProjectInfo;

class FUPTNotificationTask
{
public:

	FUPTNotificationTask(TWeakPtr<SNotificationItem> InNotificationItemPtr, SNotificationItem::ECompletionState InCompletionState, const FText& InText, const FText& InLinkText = FText(), bool InExpireAndFadeout = true)
		: CompletionState(InCompletionState)
		, NotificationItemPtr(InNotificationItemPtr)
		, Text(InText)
		, LinkText(InLinkText)
		, bExpireAndFadeout(InExpireAndFadeout)

	{
	}

	static void HandleHyperlinkNavigate()
	{
		FMessageLog("PackagingResults").Open(EMessageSeverity::Error, true);
	}

	static void HandleDismissButtonClicked()
	{
		TSharedPtr<SNotificationItem> NotificationItem = ExpireNotificationItemPtr.Pin();
		if (NotificationItem.IsValid())
		{
			NotificationItem->SetExpireDuration(0.0f);
			NotificationItem->SetFadeOutDuration(0.0f);
			NotificationItem->SetCompletionState(SNotificationItem::CS_Fail);
			NotificationItem->ExpireAndFadeout();
			ExpireNotificationItemPtr.Reset();
		}
	}

	void DoTask(ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent)
	{
		if (NotificationItemPtr.IsValid())
		{
			//if (CompletionState == SNotificationItem::CS_Fail)
			//{
			//	GEditor->PlayEditorSound(TEXT("/Engine/EditorSounds/Notifications/CompileFailed_Cue.CompileFailed_Cue"));
			//}
			//else
			//{
			//	GEditor->PlayEditorSound(TEXT("/Engine/EditorSounds/Notifications/CompileSuccess_Cue.CompileSuccess_Cue"));
			//}

			TSharedPtr<SNotificationItem> NotificationItem = NotificationItemPtr.Pin();
			NotificationItem->SetText(Text);

			if (!LinkText.IsEmpty())
			{
				FText VLinkText(LinkText);
				const TAttribute<FText> Message = TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateLambda([VLinkText]()
					{
						return VLinkText;
					}));

				NotificationItem->SetHyperlink(FSimpleDelegate::CreateStatic(&HandleHyperlinkNavigate), Message);
			}

			if (bExpireAndFadeout)
			{
				ExpireNotificationItemPtr.Reset();
				NotificationItem->SetExpireDuration(6.0f);
				NotificationItem->SetFadeOutDuration(0.5f);
				NotificationItem->SetCompletionState(CompletionState);
				NotificationItem->ExpireAndFadeout();
			}
			else
			{
				// Handling the notification expiration in callback
				ExpireNotificationItemPtr = NotificationItem;
				NotificationItem->SetCompletionState(CompletionState);
			}
		}
	}

	static ESubsequentsMode::Type GetSubsequentsMode() { return ESubsequentsMode::TrackSubsequents; }
	ENamedThreads::Type GetDesiredThread() { return ENamedThreads::GameThread; }
	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FUPTNotificationTask, STATGROUP_TaskGraphTasks);
	}

private:

	static TWeakPtr<SNotificationItem> ExpireNotificationItemPtr;

	SNotificationItem::ECompletionState CompletionState;
	TWeakPtr<SNotificationItem> NotificationItemPtr;
	FText Text;
	FText LinkText;
	bool bExpireAndFadeout;
};

class FUPTManager :public TSharedFromThis<FUPTManager>
{
public:
	FUPTManager()
	{
	}

	static TSharedPtr<FUPTManager> Get();

	void Initialize();
	void Deinitialize();

	/*
	*	获取所有引擎根目录
	*/
	TArray<FString> GetAllEngineRootDir();

	void GetEngineVersion(TArray<FString>& EngineDirs, TArray<FString>& Versions);

	/*
	*	根据引擎根目录获取该版本引擎的所有工程路径
	*/
	TArray<FString> GetProjectPathsByEngineRootDir(const FString& RootDir);

	void GetProjectNames(TArray<FString>& ProjectPaths, TArray<FString>& ProjectNames);

	TArray<TSharedPtr<FProjectInfo>> GetAllProjectInfos();

	/*
	*	根据工程路径获取项目缩略图
	*/
	TSharedPtr<FSlateBrush> GetProjectThumbnail(const FString& ProjectPath);

	/*
	*	引擎是否是源码版，返回true
	*/
	const bool EngineIsDistribution(const FString& Identifer);
	const bool EngineIsDistributionByRootDir(const FString& EngineDir);

	bool LaunchGame(TSharedRef<FProjectInfo> Info);
	bool OpenProject(TSharedRef<FProjectInfo> Info);
	void PackageProject(TSharedRef<FProjectInfo> Info);
	void HandleUatProcessCompleted(int32 Result);
	bool OpenCodeIDE(TSharedRef<FProjectInfo> Info);
	void GenerateSolution(TSharedRef<FProjectInfo> Info);
	bool ShowInExplorer(TSharedRef<FProjectInfo> Info);
	//弹出清理Binares和Immediate文件夹窗口
	void OpenClearProjectWindow(TSharedRef<FProjectInfo> Info);

	void SetCurrentSelectedProject(TSharedPtr<FProjectInfo> Info)
	{
		CurrentSelectedProject = Info;
	}

	TSharedPtr<FProjectInfo> GetCurrentSelectedProject()
	{
		return CurrentSelectedProject.Pin();
	}

	TSharedRef<SDockTab> SpawnCodeMgrWindow(const FSpawnTabArgs& Args, FName TabIdentifier);
	TSharedRef<SDockTab> SpawnAddNewCodeFileWindow(const FSpawnTabArgs& Args, FName TabIdentifier);

	void OpenManagedCodeWindow(TSharedRef<FProjectInfo> Info);
	void AddNewCodeFile(TSharedRef<FProjectInfo> Info);

	void SetNotificationList(TSharedRef<SNotificationList> NotificationList)
	{
		NotificationListPtr = NotificationList;
	}

	void PrintLog(FString log)
	{
		GLog->Log(log);
	}
private:
	TSharedPtr<FJsonObject> LoadProjectFile(const FString& FileName);
	TSharedRef<SNotificationItem> AddNotification(FNotificationInfo Info);

private:
	TWeakPtr<FProjectInfo> CurrentSelectedProject;
	TSharedPtr<SNotificationList> NotificationListPtr;
	TWeakPtr<SNotificationItem> NotificationItemPtr;
	TSharedPtr<FMonitoredProcess> UatProcess;
};