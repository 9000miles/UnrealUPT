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
	{	}

	static TSharedPtr<FUPTManager> Get();

	void Initialize();

	/*
	*	��ȡ���������Ŀ¼
	*/
	TArray<FString> GetAllEngineRootDir();

	void GetEngineVersion(TArray<FString>& EngineDirs, TArray<FString>& Versions);

	/*
	*	���������Ŀ¼��ȡ�ð汾��������й���·��
	*/
	TArray<FString> GetProjectPathsByEngineRootDir(const FString& RootDir);

	void GetProjectNames(TArray<FString>& ProjectPaths, TArray<FString>& ProjectNames);

	TArray<TSharedPtr<FProjectInfo>> GetAllProjectInfos();

	/*
	*	���ݹ���·����ȡ��Ŀ����ͼ
	*/
	TSharedPtr<FSlateBrush> GetProjectThumbnail(const FString& ProjectPath);

	/*
	*	�����Ƿ���Դ��棬����true
	*/
	const bool EngineIsDistribution(const FString& Identifer);

	bool OpenProject(TSharedRef<FProjectInfo> Info);
	bool OpenCodeIDE(TSharedRef<FProjectInfo> Info);
	void GenerateSolution(TSharedRef<FProjectInfo> Info);
	bool ShowInExplorer(TSharedRef<FProjectInfo> Info);
	//��������Binares��Immediate�ļ��д���
	void OpenClearSolutionWindow(TSharedRef<FProjectInfo> Info);

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