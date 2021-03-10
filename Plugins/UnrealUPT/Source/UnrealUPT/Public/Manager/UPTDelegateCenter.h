// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ProjectInfo.h"
#include "SNotificationList.h"

class UNREALUPT_API FUPTDelegateCenter
{
public:
	DECLARE_DELEGATE_OneParam(FOnProjectOperation, TSharedRef<FProjectInfo> /*ProjectInfo*/);

	static FOnProjectOperation OnLaunchGame;
	static FOnProjectOperation OnOpenProject;
	static FOnProjectOperation OnOpenIDE;
	static FOnProjectOperation OnGenerateSolution;
	static FOnProjectOperation OnShowInExplorer;
	static FOnProjectOperation OnClearProject;
	static FOnProjectOperation OnOpenCodeMgrWindow;
	static FOnProjectOperation OnAddNewCodeFile;

	DECLARE_DELEGATE_RetVal_OneParam(TSharedRef<SNotificationItem>, FOnRequestAddNotification, FNotificationInfo /*NotificationInfo*/);
	static FOnRequestAddNotification OnRequestAddNotification;

	static FSimpleDelegate OnRefresh;

	static FSimpleMulticastDelegate OnExit;
};