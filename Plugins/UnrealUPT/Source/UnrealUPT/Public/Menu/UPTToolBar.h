// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UICommandList.h"
#include "Commands.h"

class FUPTToolBar:public TCommands<FUPTToolBar>
{
public:
	static TSharedRef<SWidget> MakeUPTToolBar();
	static void OnOpenCodeMgrWindow();

private:
	static TSharedPtr<FUICommandList> UPTCommandList;
};