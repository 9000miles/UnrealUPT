// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "UMG/Public/Components/Widget.h"
#include "UMG/Public/Components/CanvasPanelSlot.h"
#include "UMG/Public/Components/PanelWidget.h"
#include "PrintHelper/Public/PrintHelper.h"
#include "Engine/UserInterfaceSettings.h"
#include "UPTGameInstance.generated.h"

class FUPTLaucher;

USTRUCT(BlueprintType)
struct FWidgetRect
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
		FVector2D LeftUp;
	UPROPERTY(BlueprintReadOnly)
		FVector2D LeftDown;
	UPROPERTY(BlueprintReadOnly)
		FVector2D RightUp;
	UPROPERTY(BlueprintReadOnly)
		FVector2D RightDown;

	FWidgetRect() {}
	FWidgetRect(FVector2D InLeftUp, FVector2D InLeftDown, FVector2D InRightUp, FVector2D InRightDown)
		:LeftUp(InLeftUp)
		, LeftDown(InLeftDown)
		, RightUp(InRightUp)
		, RightDown(InRightDown)
	{
	}

	FWidgetRect operator+(const FWidgetRect& Other)
	{
		FVector2D InLeftUp = LeftUp + Other.LeftUp;
		FVector2D InLeftDown = LeftDown + Other.LeftDown;
		FVector2D InRightUp = RightUp + Other.RightUp;
		FVector2D InRightDown = RightDown + Other.RightDown;

		return FWidgetRect(InLeftUp, InLeftDown, InRightUp, InRightDown);
	}

	FString ToString()
	{
		return FString::Printf(TEXT("LeftUp:%s, LeftDown:%s, RightUp:%s, RightDown:%s"), *LeftUp.ToString(), *LeftDown.ToString(), *RightUp.ToString(), *RightDown.ToString());
	}
};

/**
 *
 */
UCLASS()
class UPTPROJECT_API UUPTGameInstance : public UGameInstance
{
	GENERATED_BODY()

private:
	TSharedPtr<FUPTLaucher> UPTLaucher;
public:
	void Shutdown() override;

	FGameInstancePIEResult StartPlayInEditorGameInstance(ULocalPlayer* LocalPlayer, const FGameInstancePIEParameters& Params) override;
	void StartGameInstance() override;

	static UPanelWidget* GetParentPanel(UWidget* Widget)
	{
		if (Widget == nullptr) return nullptr;

		UPanelWidget* Parent = Widget->GetParent();
		if (Parent != nullptr)
			return GetParentPanel(Parent);

		return Cast<UPanelWidget>(Widget);
	}

	UFUNCTION(BlueprintCallable)
		static void GetUIRectPos_Pos(UWidget* Widget, FVector2D& LeftUp, FVector2D& LeftDown, FVector2D& RightUp, FVector2D& RightDown)
	{
		UPanelWidget* Parent = GetParentPanel(Widget);
		if (Parent == nullptr) return;

		FGeometry WidgetGeometry = Widget->GetCachedGeometry();
		FPaintGeometry WidgetPaintGeometry = WidgetGeometry.ToPaintGeometry();

		FGeometry ParentGeometry = Parent->GetCachedGeometry();

		FVector2D MousePosition = ParentGeometry.AbsoluteToLocal(FSlateApplication::Get().GetCursorPos());
		FVector2D WidgetPosition = ParentGeometry.AbsoluteToLocal(WidgetPaintGeometry.DrawPosition);
		GLog->Log(FString::Printf(TEXT("MousePos: %s"), *MousePosition.ToString()));
		GLog->Log(FString::Printf(TEXT("Widget: %s"), *WidgetPosition.ToString()));

		FVector2D Widget_Draw_Pos = WidgetPaintGeometry.DrawPosition;

		float Up = Widget_Draw_Pos.Y;
		float Down = Widget_Draw_Pos.Y + WidgetPaintGeometry.GetLocalSize().Y * WidgetPaintGeometry.DrawScale;
		float Left = Widget_Draw_Pos.X;
		float Right = Widget_Draw_Pos.X + WidgetPaintGeometry.GetLocalSize().X * WidgetPaintGeometry.DrawScale;

		LeftUp = ParentGeometry.AbsoluteToLocal(Widget_Draw_Pos);//左上
		LeftDown = ParentGeometry.AbsoluteToLocal(FVector2D(Widget_Draw_Pos.X, Down));//左下
		RightUp = ParentGeometry.AbsoluteToLocal(FVector2D(Right, Widget_Draw_Pos.Y));//右上
		RightDown = ParentGeometry.AbsoluteToLocal(FVector2D(Right, Down));//右下

		GLog->Log(FString::Printf(TEXT("LeftUp:%s, LeftDown:%s, RightUp:%s, RightDown:%s"), *LeftUp.ToString(), *LeftDown.ToString(), *RightUp.ToString(), *RightDown.ToString()));
	}

	UFUNCTION(BlueprintCallable, meta = (bInternalUse = "false", AdvancedDisplay = "bInternalUse"))
		static void GetUIRectPos(UWidget* Widget, FVector2D& LeftUp, FVector2D& LeftDown, FVector2D& RightUp, FVector2D& RightDown, bool bInternalUse = false)
	{
		if (Widget == nullptr) return;

		UCanvasPanelSlot* CanvasPanel = Cast<UCanvasPanelSlot>(Widget->Slot);
		if (CanvasPanel == nullptr) return;

		FAnchors Anchor = CanvasPanel->GetAnchors();
		FVector2D Position = CanvasPanel->GetPosition();
		FVector2D Alignment = CanvasPanel->GetAlignment();
		FVector2D Size = CanvasPanel->GetSize();

		UPanelWidget* Parent = Widget->GetParent();
		if (Parent != nullptr)
			GetUIRectPos(Cast<UWidget>(Parent), LeftUp, LeftDown, RightUp, RightDown, true);

		if (bInternalUse)
			Size = FVector2D::ZeroVector;

		FVector2D ParentSize = FVector2D::ZeroVector;
		if (Parent)
		{
			if (UCanvasPanelSlot* ParentSlot = Cast<UCanvasPanelSlot>(Parent->Slot))
				ParentSize = ParentSlot->GetAutoSize() ? Size : ParentSlot->GetSize();
		}

		float Up = Position.Y - Size.Y * (Alignment.Y) + ParentSize.Y * Anchor.Minimum.Y;// - (1.f - Anchor.Minimum.Y) * 720.f;
		float Down = Position.Y + Size.Y * (1.f - Alignment.Y) + ParentSize.Y * Anchor.Maximum.Y;// - (1.f - Anchor.Minimum.Y) * 720.f;
		float Left = Position.X - Size.X * (Alignment.X) + ParentSize.X * Anchor.Minimum.X;// - (1.f - Anchor.Minimum.X) * 1280.f;
		float Right = Position.X + Size.X * (1.f - Alignment.X) + ParentSize.X * Anchor.Maximum.X;//- (1.f - Anchor.Minimum.X) * 1280.f;

		LeftUp = LeftUp + FVector2D(Left, Up);
		LeftDown = LeftDown + FVector2D(Left, Down);
		RightUp = RightUp + FVector2D(Right, Up);
		RightDown = RightDown + FVector2D(Right, Down);

		GLog->Log(FString::Printf(TEXT("LeftUp:%s, LeftDown:%s, RightUp:%s, RightDown:%s"), *LeftUp.ToString(), *LeftDown.ToString(), *RightUp.ToString(), *RightDown.ToString()));
		PRINT_WARNING("GetPosition: " << CanvasPanel->GetPosition());

		FAnchorData AnchorData = CanvasPanel->GetLayout();
		PRINT_WARNING("GetLayout - Offsets: " << FString::Printf(TEXT("Top: %f, Bottom: %f, Left: %f, Right: %f"),
			AnchorData.Offsets.Top, AnchorData.Offsets.Bottom, AnchorData.Offsets.Left, AnchorData.Offsets.Right));

		PRINT_WARNING("GetLayout - Anchors: " << FString::Printf(TEXT("Minimum: %s, Maximum: %s"),
			*AnchorData.Anchors.Minimum.ToString(), *AnchorData.Anchors.Maximum.ToString()));

		PRINT_WARNING("GetLayout - Alignment: " << FString::Printf(TEXT("Alignment: %s"),
			*AnchorData.Alignment.ToString()));
	}

protected:
	void OnStartInitialize();
};
