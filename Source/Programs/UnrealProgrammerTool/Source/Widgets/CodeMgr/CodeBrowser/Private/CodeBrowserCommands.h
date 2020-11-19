// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorStyleSet.h"
#include "Framework/Commands/Commands.h"

class FCodeBrowserCommands
	: public TCommands<FCodeBrowserCommands>
{
public:

	/** Default constructor. */
	FCodeBrowserCommands()
		: TCommands<FCodeBrowserCommands>(TEXT("CodeBrowser"), NSLOCTEXT( "CodeBrowser", "CodeBrowser", "Content Browser" ), NAME_None, FEditorStyle::GetStyleSetName() )
	{ }

public:

	//~ TCommands interface

	virtual void RegisterCommands() override;

public:

	TSharedPtr<FUICommandInfo> CreateNewFolder;
	TSharedPtr<FUICommandInfo> DirectoryUp;
	TSharedPtr<FUICommandInfo> OpenAssetsOrFolders;
	TSharedPtr<FUICommandInfo> PreviewAssets;
	TSharedPtr<FUICommandInfo> SaveSelectedAsset;
	TSharedPtr<FUICommandInfo> SaveAllCurrentFolder;
	TSharedPtr<FUICommandInfo> ResaveAllCurrentFolder;
};
