// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "UPTGameInstance.generated.h"

class FUPTLaucher;

/**
 *
 */
UCLASS()
class UPTPROJECT_API UUPTGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	void Shutdown() override;

#if WITH_EDITOR
	FGameInstancePIEResult StartPlayInEditorGameInstance(ULocalPlayer* LocalPlayer, const FGameInstancePIEParameters& Params) override;
#endif
	void StartGameInstance() override;

protected:
	void OnStartInitialize();

private:
	TSharedPtr<FUPTLaucher> UPTLaucher;
};
