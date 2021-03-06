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

private:
	TSharedPtr<FUPTLaucher> UPTLaucher;
public:
	void Shutdown() override;

protected:
	void OnStart() override;
};
