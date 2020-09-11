// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

struct FProjectInfo;
class FTabManager;
class SUPTMainFrame;

void SetProjectDir();
TSharedRef<SDockTab> SpawnMainTab(const FSpawnTabArgs& Args, FName TabIdentifier);
void OnRefreshMainFrame();
void StartupMainFrame();

void SetAppIcon();

void OnExit();

static TArray<TSharedPtr<FProjectInfo>> ProjectInfos;
static TSharedPtr<FTabManager> UPTTabManager;
static TSharedPtr<SUPTMainFrame> UPTMainFrame;