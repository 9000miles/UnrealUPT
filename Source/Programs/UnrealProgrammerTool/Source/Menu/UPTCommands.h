// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "UPTStyle.h"

/**
 *
 */
class FUPTCommands: public TCommands<FUPTCommands>
{
public:

	FUPTCommands()
		: TCommands<FUPTCommands>(TEXT("UPTCommands"), NSLOCTEXT("Contexts", "UPTCommands", "UPTCommands"), NAME_None, FUPTStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > Refresh;
	TSharedPtr< FUICommandInfo > FoldAllArea;
	TSharedPtr< FUICommandInfo > ExpandAllArea;
};