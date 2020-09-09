// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SlateBrush.h"

struct FProjectInfo;

class FEPManager :public TSharedFromThis<FEPManager>
{
public:
	FEPManager()
	{	}

	static TSharedPtr<FEPManager> Get();

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

	bool OpenProject(TSharedRef<FProjectInfo> Info);
	bool OpenCodeIDE(TSharedRef<FProjectInfo> Info);
	bool GenerateSolution(TSharedRef<FProjectInfo> Info);
	bool ShowInExplorer(TSharedRef<FProjectInfo> Info);
	//��������Binares��Immediate�ļ��д���
	void OpenClearSolutionWindow(TSharedRef<FProjectInfo> Info);

	void OpenManagedCodeWindow(TSharedRef<FProjectInfo> Info);

private:
	TSharedPtr<FJsonObject> LoadProjectFile(const FString &FileName);
};