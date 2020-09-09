// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

void SetProjectDir();
TSharedRef<SDockTab> SpawnMainTab(const FSpawnTabArgs& Args, FName TabIdentifier);
void StartupMainFrame();

void SetAppIcon();

void OnExit();