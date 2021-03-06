// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CreateCodeDefine.generated.h"

#define CODE_TEMPLATE_FILE(Directory, Type) FString(Directory) / FString(#Type).Replace(TEXT("ECodeTemplateType."), TEXT("")).Replace(TEXT("_"), TEXT(".")) + TEXT(".template");
#define TEMPLATE_FILE(Directory, Type) FString(Directory) / FString(Type).Replace(TEXT("_"), TEXT(".")) + TEXT(".template")
#define CODE_FILE_TYPE_NAME(FileType) FString(Directory) / FString(FileType).Replace(TEXT("_"), TEXT(".")) + TEXT(".template")
//#define ENUM_Parameter(Type) FString(#Type).
UENUM(BlueprintType)
enum class ECodeFileType :uint8
{
	None,
	H,
	Cpp,
	Build_cs,
	Target_cs,
};

UENUM(BlueprintType)
enum class ECppDirectory :uint8
{
	Only_H,
	SameDirectory,
	PrivateDirectory
};

UENUM(BlueprintType)
enum class ECodeTemplateType :uint8
{
	None,
	ActorClass,
	ActorComponentClass,
	CharacterClass,
	Command,
	EditorModule,
	EmptyClass,
	GameModule,
	InterfaceClass,
	PawnClass,
	PluginModule,
	SlateWidget,
	SlateWidgetStyle,
	SoundEffectSourceClass,
	SoundEffectSubmixClass,
	Stub,
	SynthComponentClass,
	UObjectClass,
	UObjectClassConstructorDeclaration,
	UObjectClassConstructorDefinition,
};
