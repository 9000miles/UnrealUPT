// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "UPTLaucher.h"
#include "ProjectInfo.h"
#include "SUPTMainFrame.h"
#include "UPTManager.h"
#include "Menu/UPTMenuBar.h"
#include "Menu/UPTToolBar.h"

#define LOCTEXT_NAMESPACE "UnrealProgrammerTool"

namespace UPTMeun
{
	TSharedRef<FWorkspaceItem> UPTGroup = FWorkspaceItem::NewGroup(LOCTEXT("UPTGroup", "UPTGroup"));
}

void FUPTLaucher::Initialize()
{
	FUPTManager::Get()->Initialize();
	ProjectInfos = FUPTManager::Get()->GetAllProjectInfos();

	CreateMainFrameWindow();

	FUPTDelegateCenter::OnRefresh.BindRaw(this, &FUPTLaucher::OnRefreshMainFrame);
}

void FUPTLaucher::Shutdown()
{
	FUPTManager::Get()->Deinitialize();
}

TSharedPtr<SWidget> FUPTLaucher::CreateWidget()
{
	return
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(FMargin(2, 0))
		[
			FUPTMenuBar::MakeMenuBar()
		]
	+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(FMargin(2, 2))
		[
			FUPTToolBar::MakeUPTToolBar()
		]
	+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		[
			SAssignNew(UPTMainFrame, SUPTMainFrame, ProjectInfos)
		];
}

void FUPTLaucher::OnRefreshMainFrame()
{
	check(UPTMainFrame.IsValid());

	ProjectInfos.Empty();
	ProjectInfos = FUPTManager::Get()->GetAllProjectInfos();

	UPTMainFrame->RequestRefresh(ProjectInfos);
}

void FUPTLaucher::CreateMainFrameWindow()
{
	EAutoCenter AutoCenterType = EAutoCenter::PrimaryWorkArea;

	int32 ResX = 1080;
	int32 ResY = 600;

	TSharedRef<SWindow> Window = SNew(SWindow)
		.Type(EWindowType::GameWindow)
		//.Style(bUseBorderlessWindow ? &BorderlessStyle : &FCoreStyle::Get().GetWidgetStyle<FWindowStyle>("Window"))
		.ClientSize(FVector2D(ResX, ResY))
		.AdjustInitialSizeAndPositionForDPIScale(true)
		.Title(FText::FromString(TEXT("Unreal Programmer Tool")))
		.AutoCenter(AutoCenterType)
		.ScreenPosition(FVector2D(0, 0))
		.MinWidth(640)
		.MinHeight(480)
		.FocusWhenFirstShown(true)
		.SaneWindowPlacement(AutoCenterType == EAutoCenter::None)
		.UseOSWindowBorder(false)
		.CreateTitleBar(true)
		.ShouldPreserveAspectRatio(false)
		.LayoutBorder(FMargin(5))
		//.UserResizeBorder(FMargin(0))//无法调整窗口大小
		.SizingRule(true ? ESizingRule::UserSized : ESizingRule::FixedSize)
		.HasCloseButton(true)
		.SupportsMinimize(true)
		.SupportsMaximize(true)
		.Content()
		[
			CreateWidget().ToSharedRef()
		];

	Window->SetOnWindowClosed(FOnWindowClosed::CreateRaw(this, &FUPTLaucher::OnWindowClose));

	FSlateApplication::Get().AddWindow(Window, true);
}

void FUPTLaucher::OnExit()
{
	FUPTDelegateCenter::OnExit.Broadcast();
}

void FUPTLaucher::OnWindowClose(const TSharedRef<SWindow>& Window)
{
	OnExit();
}

#undef LOCTEXT_NAMESPACE