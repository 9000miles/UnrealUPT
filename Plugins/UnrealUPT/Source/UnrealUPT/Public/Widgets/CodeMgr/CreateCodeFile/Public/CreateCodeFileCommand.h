// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "EditorStyleSet.h"

/**
 *
 */
class FCreateCodeFileCommand : public TCommands<FCreateCodeFileCommand>
{
public:
	FCreateCodeFileCommand()
		: TCommands<FCreateCodeFileCommand>(TEXT("CreateCodeFile"), NSLOCTEXT("Contexts", "FCreateCodeFileCommand", "FCreateCodeFileCommandCommands"), NAME_None, FUPTStyle::GetStyleSetName())
	{}
	void RegisterCommands() override;

public:
	TSharedPtr<FUICommandInfo> NewFile;
};
