// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "STileView.h"

struct FProjectInfo;
class SProjectUnit;

class SProjectTileView :public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SProjectTileView)
	{ }
	SLATE_END_ARGS()

		void Construct(const FArguments& InArgs, TArray<TSharedPtr<FProjectInfo>>& Projects);
	void Refresh(TArray<TSharedPtr<FProjectInfo>> Infos);

private:
	void CreateUnits(TArray<TSharedPtr<FProjectInfo>> Infos);
	TSharedRef<ITableRow> OnGenerateProjectTile(TSharedPtr<SProjectUnit> ContentSource, const TSharedRef<STableViewBase>& OwnerTable);
	TSharedPtr<SWidget> MakeProjectContextMenu();
	void OnSelectionChanged(TSharedPtr<SProjectUnit> Unit, ESelectInfo::Type Type);

private:
	TSharedPtr<SProjectUnit> CurSelectedUnit;

	TArray<TSharedPtr<SProjectUnit>> ProjectUnits;
	TSharedPtr<STileView<TSharedPtr<SProjectUnit>>> TileView;
};