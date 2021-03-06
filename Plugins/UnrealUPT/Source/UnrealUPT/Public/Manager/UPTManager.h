// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SlateBrush.h"
#include "UPTDelegateCenter.h"
#include "SNotificationList.h"

struct FProjectInfo;

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

private:
	TSharedPtr<FJsonObject> LoadProjectFile(const FString& FileName);
	TSharedRef<SNotificationItem> AddNotification(FNotificationInfo Info);

private:
	TWeakPtr<FProjectInfo> CurrentSelectedProject;
	TSharedPtr<SNotificationList> NotificationListPtr;
};