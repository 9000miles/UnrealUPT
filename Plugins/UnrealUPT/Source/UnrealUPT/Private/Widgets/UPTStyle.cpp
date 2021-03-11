// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "UPTStyle.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyleRegistry.h"
#include "Interfaces/IPluginManager.h"
#include "IPluginManager.h"
#include "SlateStyle.h"
#include "UPTDefine.h"

TSharedPtr< FSlateStyleSet > FUPTStyle::StyleInstance = NULL;

void FUPTStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FUPTStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FUPTStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("UPTStyle"));
	return StyleSetName;
}

const FSlateBrush* FUPTStyle::GetBrush(FName PropertyName, const ANSICHAR* Specifier /*= NULL*/)
{
	return nullptr;
}

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BORDER_BRUSH( RelativePath, ... ) FSlateBorderBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define TTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".ttf") ), __VA_ARGS__ )
#define OTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".otf") ), __VA_ARGS__ )

#define DEFAULT_FONT(...) FCoreStyle::GetDefaultFontStyle(__VA_ARGS__)

const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);
const FVector2D Icon32x32(32.0f, 32.0f);
const FVector2D Icon40x40(40.0f, 40.0f);
const FVector2D Icon64x64(64.0f, 64.0f);

TSharedRef< FSlateStyleSet > FUPTStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("UPTStyle"));
	Style->SetContentRoot(UPT_RESOURCE_PATH);

	Style->Set("UPT.ToolBar.Refresh", new IMAGE_BRUSH(TEXT("Refresh"), Icon32x32));
	Style->Set("UPT.ToolBar.FoldAllArea", new IMAGE_BRUSH(TEXT("FoldAllArea"), Icon32x32));
	Style->Set("UPT.ToolBar.ExpandAllArea", new IMAGE_BRUSH(TEXT("ExpandAllArea"), Icon32x32));

	Style->Set("UPT.AppIcon", new IMAGE_BRUSH(TEXT("AppIcon"), Icon20x20));

	Style->Set("UPT.Tab.Source", new IMAGE_BRUSH(TEXT("Source"), Icon16x16));
	Style->Set("UPT.Tab.Binary", new IMAGE_BRUSH(TEXT("Binary"), Icon16x16));
	Style->Set("UPT.Tab.NotFound", new IMAGE_BRUSH(TEXT("NotFound"), Icon16x16));

	FLinearColor DimBackground = FLinearColor(FColor(64, 64, 64));
	FLinearColor DimBackgroundHover = FLinearColor(FColor(50, 50, 50));
	FLinearColor DarkBackground = FLinearColor(FColor(42, 42, 42));
	Style->Set("UPT.PlacementBrowser.Tab", FCheckBoxStyle()
		.SetCheckBoxType(ESlateCheckBoxType::ToggleButton)
		.SetUncheckedImage(BOX_BRUSH("Tab/Selection", 8.0f / 32.0f, DimBackground))
		.SetUncheckedPressedImage(BOX_BRUSH("Tab/TabActive", 8.0f / 32.0f))
		.SetUncheckedHoveredImage(BOX_BRUSH("Tab/Selection", 8.0f / 32.0f, DimBackgroundHover))
		.SetCheckedImage(BOX_BRUSH("Tab/TabActive", 8.0f / 32.0f))
		.SetCheckedHoveredImage(BOX_BRUSH("Tab/TabActive", 8.0f / 32.0f))
		.SetCheckedPressedImage(BOX_BRUSH("Tab/TabActive", 8.0f / 32.0f))
		.SetPadding(8));

	const FSlateColor SelectionColor = MakeShareable(new FLinearColor(1.f, 1.f, 0.f));
	//const FSlateColor SelectionColor = MakeShareable(new FLinearColor(0.701f, 0.225f, 0.003f));
	Style->Set("UPT.PlacementBrowser.ActiveTabBar", new IMAGE_BRUSH("Tab/Selection", FVector2D(2.0f, 2.0f), SelectionColor));

	//Style->Set("PlacementBrowser.Tab.Text", FTextBlockStyle(NormalText)
	//	.SetFont(DEFAULT_FONT("Bold", 10))
	//	.SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 0.9f))
	//	.SetShadowOffset(FVector2D(1, 1))
	//	.SetShadowColorAndOpacity(FLinearColor(0, 0, 0, 0.9f)));

	//	Style->Set("Window", FWindowStyle()
	//#if !PLATFORM_MAC
	//		.SetMinimizeButtonStyle(MinimizeButtonStyle)
	//		.SetMaximizeButtonStyle(MaximizeButtonStyle)
	//		.SetRestoreButtonStyle(RestoreButtonStyle)
	//		.SetCloseButtonStyle(CloseButtonStyle)
	//#endif
	//		.SetTitleTextStyle(TitleTextStyle)
	//		.SetActiveTitleBrush(IMAGE_BRUSH("Common/Window/WindowTitle", Icon32x32, FLinearColor(1.0f, 1.0f, 1.0f, 1.0f), ESlateBrushTileType::Horizontal))
	//		.SetInactiveTitleBrush(IMAGE_BRUSH("Common/Window/WindowTitle_Inactive", Icon32x32, FLinearColor(1.0f, 1.0f, 1.0f, 1.0f), ESlateBrushTileType::Horizontal))
	//		.SetFlashTitleBrush(IMAGE_BRUSH("Common/Window/WindowTitle_Flashing", Icon24x24, FLinearColor(1.0f, 1.0f, 1.0f, 1.0f), ESlateBrushTileType::Horizontal))
	//		.SetOutlineBrush(BORDER_BRUSH("Common/Window/WindowOutline", FMargin(3.0f / 32.0f)))
	//		.SetOutlineColor(FLinearColor(0.1f, 0.1f, 0.1f, 1.0f))
	//		.SetBorderBrush(BOX_BRUSH("Common/Window/WindowBorder", 0.48f))
	//		.SetBackgroundBrush(IMAGE_BRUSH("Common/Window/WindowBackground", FVector2D(74.0f, 74.0f), FLinearColor::White, ESlateBrushTileType::Both))
	//		.SetChildBackgroundBrush(IMAGE_BRUSH("Common/NoiseBackground", FVector2D(64.0f, 64.0f), FLinearColor::White, ESlateBrushTileType::Both))
	//	);

	return Style;
}

#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
#undef TTF_FONT
#undef OTF_FONT

void FUPTStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FUPTStyle::Get()
{
	return *StyleInstance;
}