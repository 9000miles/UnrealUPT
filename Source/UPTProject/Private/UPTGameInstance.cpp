// Fill out your copyright notice in the Description page of Project Settings.

#include "UPTGameInstance.h"
#include "Launcher/UPTLaucher.h"
#include "SButton.h"
#include "Kismet/GameplayStatics.h"
#include "PrintHelper.h"
#include "Manager/UPTDelegateCenter.h"

void UUPTGameInstance::OnStartInitialize()
{
	UPTLaucher = MakeShared<FUPTLaucher>();
	UPTLaucher->Initialize();

	APlayerController* Player = UGameplayStatics::GetPlayerController(this, 0);
	Player->SetShowMouseCursor(true);
	FInputModeUIOnly InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	Player->SetInputMode(InputMode);

	FUPTDelegateCenter::OnExit.AddLambda([this, Player]()
		{
			UKismetSystemLibrary::QuitGame(GetWorld(), Player, EQuitPreference::Quit, false);
		});
}

void UUPTGameInstance::Shutdown()
{
	Super::Shutdown();
	if (UPTLaucher.IsValid())
		UPTLaucher->Shutdown();
}

#if WITH_EDITOR
FGameInstancePIEResult UUPTGameInstance::StartPlayInEditorGameInstance(ULocalPlayer* LocalPlayer, const FGameInstancePIEParameters& Params)
{
	FGameInstancePIEResult Result = Super::StartPlayInEditorGameInstance(LocalPlayer, Params);
	OnStartInitialize();
	return Result;
}
#endif

void UUPTGameInstance::StartGameInstance()
{
	Super::StartGameInstance();
	OnStartInitialize();
}