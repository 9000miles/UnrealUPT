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

	Style->Set("UPT.AppIcon", new IMAGE_BRUSH(TEXT("AppIcon"), Icon32x32));

	Style->Set("UPT.Tab.Source", new IMAGE_BRUSH(TEXT("Source"), Icon16x16));
	Style->Set("UPT.Tab.Binary", new IMAGE_BRUSH(TEXT("Binary"), Icon16x16));
	Style->Set("UPT.Tab.NotFound", new IMAGE_BRUSH(TEXT("NotFound"), Icon16x16));

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