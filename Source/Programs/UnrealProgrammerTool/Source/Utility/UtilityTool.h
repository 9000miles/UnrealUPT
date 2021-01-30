// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "Kismet/BlueprintFunctionLibrary.h"
//#include "Kismet/BlueprintAsyncActionBase.h"
//#include "UtilityTool.generated.h"

DECLARE_DELEGATE_OneParam(FOnFileDirectoryActionFinished, const bool);

namespace UPTUtility
{
	void AsyncDeleteFile(FOnFileDirectoryActionFinished OnFilished, const FString& Filename);

	void AsyncDeleteDirectory(FOnFileDirectoryActionFinished OnFilished, const FString& Directory, bool bMustExist = false, bool bDeleteAllSubDirectory = false);

	//UCLASS()
	//	class UCopyFileAsyncAction :public UFileDirectoryAsyncActionBase
	//{
	//	GENERATED_BODY()
	//public:
	//	UFUNCTION(BlueprintCallable, Category = "FileLibrary", meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"))
	//		static UCopyFileAsyncAction* AsyncCopyFile(UObject* WorldContextObject, const FString& SourceFile, const FString& DestinationFile);
	//private:
	//	void StartCopyFile(UCopyFileAsyncAction* AsyncAction, const FString& SourceFile, const FString& DestinationFile);
	//};

	//UCLASS()
	//	class UCopyDirectoryAsyncAction :public UFileDirectoryAsyncActionBase
	//{
	//	GENERATED_BODY()
	//public:
	//	UFUNCTION(BlueprintCallable, Category = "DirectoryLibrary", meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"))
	//		static UCopyDirectoryAsyncAction* AsyncCopyDirectory(UObject* WorldContextObject, const FString& SourceDirectory, const FString& DestinationDirectory, bool bOverwriteAllExisting = true);
	//private:
	//	void StartCopyDirectory(UCopyDirectoryAsyncAction* AsyncAction, const FString& SourceDirectory, const FString& DestinationDirectory, bool bOverwriteAllExisting = true);
	//};

	//UCLASS()
	//	class UMoveFileAsyncAction :public UFileDirectoryAsyncActionBase
	//{
	//	GENERATED_BODY()
	//public:
	//	UFUNCTION(BlueprintCallable, Category = "FileLibrary", meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"))
	//		static UMoveFileAsyncAction* AsyncMoveFile(UObject* WorldContextObject, const FString& SourceFile, const FString& DestinationFile);
	//private:
	//	void StartMoveFile(UMoveFileAsyncAction* AsyncAction, const FString& SourceFile, const FString& DestinationFile);
	//};

	//UCLASS()
	//	class UMoveDirectoryAsyncAction :public UFileDirectoryAsyncActionBase
	//{
	//	GENERATED_BODY()
	//public:
	//	UFUNCTION(BlueprintCallable, Category = "DirectoryLibrary", meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"))
	//		static UMoveDirectoryAsyncAction* AsyncMoveDirectory(UObject* WorldContextObject, const FString& SourceDirectory, const FString& DestinationDirectory);
	//private:
	//	void StartMoveDirectory(UMoveDirectoryAsyncAction* AsyncAction, const FString& SourceDirectory, const FString& DestinationDirectory);
	//};

	//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCollectFileDirectoryActionFinished, const TArray<FString>&, OutFiles);
	//UCLASS()
	//	class UCollectFileDirectory :public UObject
	//{
	//	GENERATED_BODY()
	//public:
	//	UFUNCTION(BlueprintCallable, Category = "DirectoryLibrary", meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"))
	//		static UCollectFileDirectory* AsyncCollectFileDirectory(UObject* WorldContextObject, const FString& Directory, const FString& Wildcard, const bool bFindFiles, const bool bFindDirectories);
	//	UPROPERTY(BlueprintAssignable)
	//		FOnCollectFileDirectoryActionFinished OnActionFinished;
	//private:
	//	void StartCollectFileDirectory(UCollectFileDirectory* AsyncAction, const FString& Directory, const FString& Wildcard, const bool bFindFiles, const bool bFindDirectories);
	//};
}