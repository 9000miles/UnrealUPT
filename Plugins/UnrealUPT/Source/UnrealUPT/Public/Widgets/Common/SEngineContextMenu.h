// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 *
 */
class SSEngineContextMenu : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SSEngineContextMenu)
	{}
	SLATE_END_ARGS()

		/** Constructs this widget with InArgs */
		void Construct(const FArguments& InArgs, const FString EnginePath);
};
