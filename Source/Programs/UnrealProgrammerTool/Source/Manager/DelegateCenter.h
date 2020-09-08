// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ProjectInfo.h"

class FDelegateCenter
{
public:
	DECLARE_DELEGATE_OneParam(FOnProjectOperation, TSharedRef<FProjectInfo> /*ProjectInfo*/);

	static FOnProjectOperation OnOpenProject;
	static FOnProjectOperation OnOpenIDE;
	static FOnProjectOperation OnGenerateSolution;
	static FOnProjectOperation OnShowInExplorer;

};