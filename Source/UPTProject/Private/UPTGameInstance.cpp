// Fill out your copyright notice in the Description page of Project Settings.

#include "UPTGameInstance.h"
#include "Launcher/UPTLaucher.h"
#include "SButton.h"
#include "Kismet/GameplayStatics.h"

void UUPTGameInstance::Shutdown()
{
	Super::Shutdown();
	if (UPTLaucher.IsValid())
		UPTLaucher->Shutdown();
}

void UUPTGameInstance::OnStart()
{
	Super::OnStart();
	UPTLaucher = MakeShared<FUPTLaucher>();
	UPTLaucher->Initialize();

	FSpawnTabArgs Arg(nullptr, FTabId());

	TSharedPtr<SWidget> Content = UPTLaucher->CreateWidget();

	APlayerController* Player = UGameplayStatics::GetPlayerController(this, 0);
	Player->SetShowMouseCursor(true);
	FInputModeUIOnly InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	Player->SetInputMode(InputMode);

	//Content = SNew(SButton);
	GEngine->GameViewport->AddViewportWidgetContent(Content.ToSharedRef());
}