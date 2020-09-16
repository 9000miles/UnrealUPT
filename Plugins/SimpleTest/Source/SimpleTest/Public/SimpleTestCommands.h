// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "SimpleTestStyle.h"

class FSimpleTestCommands : public TCommands<FSimpleTestCommands>
{
public:

	FSimpleTestCommands()
		: TCommands<FSimpleTestCommands>(TEXT("SimpleTest"), NSLOCTEXT("Contexts", "SimpleTest", "SimpleTest Plugin"), "LevelViewport", FSimpleTestStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;
public:
	TSharedPtr< FUICommandInfo > OpenSimpleTestWindow;
};