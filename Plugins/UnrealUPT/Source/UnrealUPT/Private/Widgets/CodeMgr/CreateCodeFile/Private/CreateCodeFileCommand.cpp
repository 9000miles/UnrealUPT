// Fill out your copyright notice in the Description page of Project Settings.
#include "CreateCodeFileCommand.h"

#define LOCTEXT_NAMESPACE "FCreateCodeFileCommand"

void FCreateCodeFileCommand::RegisterCommands()
{
	UI_COMMAND(NewFile, "New Code File...", "Creates a new command description", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE