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
	void SetAppIcon();
	void Shutdown();

	TSharedPtr<SWidget> CreateWidget();

	void OnRefreshMainFrame();
	void CreateMainFrameWindow();

	void OnExit();

private:
	void OnWindowClose(const TSharedRef<SWindow>& Window);

private:
	TArray<TSharedPtr<FProjectInfo>> ProjectInfos;
	TSharedPtr<SUPTMainFrame> UPTMainFrame;
};