// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "SimpleTestCommands.h"
#include "ApplicationCore/Public/GenericPlatform/GenericApplication.h"

#define LOCTEXT_NAMESPACE "FSimpleTestCommands"

void FSimpleTestCommands::RegisterCommands()
{
	UI_COMMAND(OpenSimpleTestWindow, "Unit Test", "Open Unit Test Window", EUserInterfaceActionType::Button, FInputChord(EKeys::T, true, true, true, false));
}

#undef LOCTEXT_NAMESPACE