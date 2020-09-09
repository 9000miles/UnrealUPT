// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class FUPTMenuBar
{
public:
	static TSharedRef<SWidget> MakeMenuBar(TSharedRef<FTabManager> TabManager);

protected:
	static void FillFileMenuBarEntries(FMenuBuilder& MenuBuilder);
	static void FillWindowsMenuBarEntries(FMenuBuilder& MenuBuilder);
	static void FillHelpMenuBarEntries(FMenuBuilder& MenuBuilder);

	static void FillRecentProjectsSubMenu(FMenuBuilder& MenuBuilder);
	static void OnRefresh();
	static void OnExit();
private:
};