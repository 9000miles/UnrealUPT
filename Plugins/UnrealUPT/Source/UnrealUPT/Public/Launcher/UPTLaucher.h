// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Classes/ProjectInfo.h"

struct FProjectInfo;
class FTabManager;
class SUPTMainFrame;

class UNREALUPT_API FUPTLaucher
{
public:
	void Initialize();
	void Shutdown();

	void SetProjectDir();
	TSharedRef<SDockTab> SpawnMainTab(const FSpawnTabArgs& Args, FName TabIdentifier);

	TSharedPtr<SWidget> CreateWidget();

	void OnRefreshMainFrame();
	void CreateMainFrameWindow();
	void StartupMainFrame();

	void SetAppIcon();

	void OnExit();

private:
	TArray<TSharedPtr<FProjectInfo>> ProjectInfos;
	TSharedPtr<FTabManager> UPTTabManager;
	TSharedPtr<SUPTMainFrame> UPTMainFrame;
};