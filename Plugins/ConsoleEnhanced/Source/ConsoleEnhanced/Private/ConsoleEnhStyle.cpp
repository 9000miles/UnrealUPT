#include "ConsoleEnhStyle.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyleRegistry.h"
#include "Interfaces/IPluginManager.h"

void FConsoleEnhStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FConsoleEnhStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

void FConsoleEnhStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FConsoleEnhStyle::Get()
{
	return *StyleInstance;
}

FName FConsoleEnhStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("ConsoleEnhStyle"));
	return StyleSetName;
}

const FSlateBrush* FConsoleEnhStyle::GetBrush(FName PropertyName, const ANSICHAR* Specifier /*= NULL*/)
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

TSharedRef< class FSlateStyleSet > FConsoleEnhStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("ConsoleEnhStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("ConsoleEnhanced")->GetBaseDir() / TEXT("Resources"));

	Style->Set("Log.TabIcon", new IMAGE_BRUSH(TEXT("icon_tab_OutputLog_16x"), Icon16x16));
	Style->Set("Menu.Background", new BOX_BRUSH(TEXT("Menu_Background"), FMargin(8.0f / 64.0f)));
	Style->Set("ToolPanel.GroupBorder", new BOX_BRUSH(TEXT("Common/GroupBorder"), FMargin(8.0f / 64.0f)));

	NormalText = FTextBlockStyle()
		.SetFont(DEFAULT_FONT("Regular", FCoreStyle::RegularTextSize))
		.SetColorAndOpacity(FSlateColor::UseForeground())
		.SetShadowOffset(FVector2D::ZeroVector)
		.SetShadowColorAndOpacity(FLinearColor::Black)
		.SetHighlightColor(FLinearColor(0.02f, 0.3f, 0.0f))
		.SetHighlightShape(BOX_BRUSH("TextBlockHighlightShape", FMargin(3.f / 8.f)));

	Style->Set("MessageLog", FTextBlockStyle(NormalText)
		.SetFont(DEFAULT_FONT("Regular", 8))
		.SetShadowOffset(FVector2D::ZeroVector)
	);

	Style->Set("Log.Warning", FTextBlockStyle(NormalText)
		.SetFont(DEFAULT_FONT("Regular", 8))
		.SetShadowOffset(FVector2D::ZeroVector)
	);

	// EditableTextBox
	NormalEditableTextBoxStyle = FEditableTextBoxStyle()
		.SetBackgroundImageNormal(BOX_BRUSH("Common/TextBox", FMargin(4.0f / 16.0f)))
		.SetBackgroundImageHovered(BOX_BRUSH("Common/TextBox_Hovered", FMargin(4.0f / 16.0f)))
		.SetBackgroundImageFocused(BOX_BRUSH("Common/TextBox_Hovered", FMargin(4.0f / 16.0f)))
		.SetBackgroundImageReadOnly(BOX_BRUSH("Common/TextBox_ReadOnly", FMargin(4.0f / 16.0f)));
	//.SetScrollBarStyle(ScrollBar);
	{
		Style->Set("EditableTextBox.Background.Normal", new BOX_BRUSH("Common/TextBox", FMargin(4.0f / 16.0f)));
		Style->Set("EditableTextBox.Background.Hovered", new BOX_BRUSH("Common/TextBox_Hovered", FMargin(4.0f / 16.0f)));
		Style->Set("EditableTextBox.Background.Focused", new BOX_BRUSH("Common/TextBox_Hovered", FMargin(4.0f / 16.0f)));
		Style->Set("EditableTextBox.Background.ReadOnly", new BOX_BRUSH("Common/TextBox_ReadOnly", FMargin(4.0f / 16.0f)));
		Style->Set("EditableTextBox.BorderPadding", FMargin(4.0f, 2.0f));
	}
	Style->Set("Log.TextBox", FEditableTextBoxStyle(NormalEditableTextBoxStyle)
		.SetBackgroundImageNormal(BOX_BRUSH("Common/WhiteGroupBorder", FMargin(4.0f / 16.0f)))
		.SetBackgroundImageHovered(BOX_BRUSH("Common/WhiteGroupBorder", FMargin(4.0f / 16.0f)))
		.SetBackgroundImageFocused(BOX_BRUSH("Common/WhiteGroupBorder", FMargin(4.0f / 16.0f)))
		.SetBackgroundImageReadOnly(BOX_BRUSH("Common/WhiteGroupBorder", FMargin(4.0f / 16.0f)))
		//.SetBackgroundColor(LogColor_Background)
	);

	Style->Set("GenericFilters.TextStyle", FTextBlockStyle(NormalText)
		.SetFont(DEFAULT_FONT("Bold", 9))
		.SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 0.9f))
		.SetShadowOffset(FVector2D(1, 1))
		.SetShadowColorAndOpacity(FLinearColor(0, 0, 0, 0.9f)));

	const int32 LogFontSize = 9;

	//const FTextBlockStyle NormalLogText = FTextBlockStyle(NormalText)
	//	.SetFont(DEFAULT_FONT("Mono", LogFontSize))
	//	.SetColorAndOpacity(LogColor_Normal)
	//	.SetSelectedBackgroundColor(LogColor_SelectionBackground);

	//Style->Set("Log.Normal", NormalLogText);

	//Style->Set("Log.Command", FTextBlockStyle(NormalLogText)
	//	.SetColorAndOpacity(LogColor_Command)
	//);

	//Style->Set("Log.Warning", FTextBlockStyle(NormalLogText)
	//	.SetColorAndOpacity(LogColor_Warning)
	//);

	//Style->Set("Log.Error", FTextBlockStyle(NormalLogText)
	//	.SetColorAndOpacity(LogColor_Error)
	//);

	//Style->Set("Log.TabIcon", new IMAGE_BRUSH("Icons/icon_tab_OutputLog_16x", Icon16x16));

	//Style->Set("Log.TextBox", FEditableTextBoxStyle(NormalEditableTextBoxStyle)
	//	.SetBackgroundImageNormal(BOX_BRUSH("Common/WhiteGroupBorder", FMargin(4.0f / 16.0f)))
	//	.SetBackgroundImageHovered(BOX_BRUSH("Common/WhiteGroupBorder", FMargin(4.0f / 16.0f)))
	//	.SetBackgroundImageFocused(BOX_BRUSH("Common/WhiteGroupBorder", FMargin(4.0f / 16.0f)))
	//	.SetBackgroundImageReadOnly(BOX_BRUSH("Common/WhiteGroupBorder", FMargin(4.0f / 16.0f)))
	//	.SetBackgroundColor(LogColor_Background)
	//);

	return Style;
}

TSharedPtr< class FSlateStyleSet > FConsoleEnhStyle::StyleInstance;

FTextBlockStyle FConsoleEnhStyle::NormalText;

FEditableTextBoxStyle FConsoleEnhStyle::NormalEditableTextBoxStyle;