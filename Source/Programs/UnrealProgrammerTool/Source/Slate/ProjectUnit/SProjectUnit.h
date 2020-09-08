// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SImage.h"

class SProjectThumbnail :public SImage
{
public:
	SLATE_BEGIN_ARGS(SProjectThumbnail)
	{	}
	SLATE_EVENT(FSimpleDelegate, OnDoubleClick)
		SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs, const FSlateBrush* Brush);
	FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	void OnMouseLeave(const FPointerEvent& MouseEvent) override;
	FReply OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) override;

private:
	FSimpleDelegate OnDoubleClick;
};

DECLARE_DELEGATE_OneParam(FOnRightClick, const FString& /*ProjectPath*/);

struct FProjectInfo;
struct FSlateBrush;

class SProjectUnit :public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SProjectUnit)
	{	}
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs, const TSharedRef<FProjectInfo> Info);
	TSharedRef<FProjectInfo> GetProjectInfo() { return ProjectInfo.ToSharedRef(); }

private:
	void OnDoubleClicked();
	FSlateBrush* GetThumbnail();

private:
	TSharedPtr<FProjectInfo> ProjectInfo;
};