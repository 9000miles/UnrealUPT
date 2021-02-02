#include "UtilityTool.h"
#include "Async.h"
#include "PlatformFilemanager.h"
//
//UDeleteDirectoryAsyncAction* UDeleteDirectoryAsyncAction::AsyncDeleteDirectory(UObject* WorldContextObject, const FString& Directory, bool bMustExist /*= false*/, bool bDeleteAllSubDirectory /*= false*/)
//{
//	UDeleteDirectoryAsyncAction* AsyncAction = NewObject<UDeleteDirectoryAsyncAction>();
//	AsyncAction->StartDeleteDirectory(AsyncAction, Directory, bMustExist, bDeleteAllSubDirectory);
//	return AsyncAction;
//}
//
//void UDeleteDirectoryAsyncAction::StartDeleteDirectory(UDeleteDirectoryAsyncAction* AsyncAction, const FString& Directory, bool bMustExist /*= false*/, bool bDeleteAllSubDirectory /*= false*/)
//{
//	class FDeleteHelper
//	{
//	public:
//		const bool DeleteAllSubDirectory(const FString& Directory, bool bMustExist = false, bool bDeleteAllSubDirectory = false)
//		{
//			bool bSuccees = IFileManager::Get().DeleteDirectory(*Directory, bMustExist, bDeleteAllSubDirectory);
//			if (bSuccees)
//				return true;
//			else
//				return DeleteAllSubDirectory(Directory, bMustExist, bDeleteAllSubDirectory);
//		}
//	};
//
//	AsyncTask(ENamedThreads::GameThread, [AsyncAction, Directory, bMustExist, bDeleteAllSubDirectory]()
//		{
//			bool Result = false;
//			if (IFileManager::Get().DirectoryExists(*Directory))
//			{
//				if (bDeleteAllSubDirectory)
//				{
//					FDeleteHelper* DeleteHelper = new FDeleteHelper();
//					Result = DeleteHelper->DeleteAllSubDirectory(Directory, bMustExist, bDeleteAllSubDirectory);
//				}
//				else
//					Result = IFileManager::Get().DeleteDirectory(*Directory, bMustExist, bDeleteAllSubDirectory);
//			}
//
//			if (AsyncAction->OnActionFinished.IsBound())
//				AsyncAction->OnActionFinished.Broadcast(Result);
//		});
//}
//
//UCopyDirectoryAsyncAction* UCopyDirectoryAsyncAction::AsyncCopyDirectory(UObject* WorldContextObject, const FString& SourceDirectory, const FString& DestinationDirectory, bool bOverwriteAllExisting /*= true*/)
//{
//	UCopyDirectoryAsyncAction* AsyncAction = NewObject<UCopyDirectoryAsyncAction>();
//	AsyncAction->StartCopyDirectory(AsyncAction, SourceDirectory, DestinationDirectory, bOverwriteAllExisting);
//	return AsyncAction;
//}
//
//void UCopyDirectoryAsyncAction::StartCopyDirectory(UCopyDirectoryAsyncAction* AsyncAction, const FString& SourceDirectory, const FString& DestinationDirectory, bool bOverwriteAllExisting /*= true*/)
//{
//	AsyncTask(ENamedThreads::GameThread, [AsyncAction, DestinationDirectory, SourceDirectory, bOverwriteAllExisting]()
//		{
//			if (!IFileManager::Get().DirectoryExists(*DestinationDirectory))
//				IFileManager::Get().MakeDirectory(*DestinationDirectory, true);
//
//			bool Result = FPlatformFileManager::Get().GetPlatformFile().CopyDirectoryTree(*DestinationDirectory, *SourceDirectory, bOverwriteAllExisting);
//			if (AsyncAction->OnActionFinished.IsBound())
//				AsyncAction->OnActionFinished.Broadcast(Result);
//		});
//}
//
//UMoveDirectoryAsyncAction* UMoveDirectoryAsyncAction::AsyncMoveDirectory(UObject* WorldContextObject, const FString& SourceDirectory, const FString& DestinationDirectory)
//{
//	UMoveDirectoryAsyncAction* AsyncAction = NewObject<UMoveDirectoryAsyncAction>();
//	AsyncAction->StartMoveDirectory(AsyncAction, SourceDirectory, DestinationDirectory);
//	return AsyncAction;
//}
//
//void UMoveDirectoryAsyncAction::StartMoveDirectory(UMoveDirectoryAsyncAction* AsyncAction, const FString& SourceDirectory, const FString& DestinationDirectory)
//{
//	AsyncTask(ENamedThreads::GameThread, [AsyncAction, DestinationDirectory, SourceDirectory]()
//		{
//			if (!IFileManager::Get().DirectoryExists(*DestinationDirectory))
//				IFileManager::Get().MakeDirectory(*DestinationDirectory, true);
//
//			bool Result = FPlatformFileManager::Get().GetPlatformFile().CopyDirectoryTree(*DestinationDirectory, *SourceDirectory, true);
//			UDeleteDirectoryAsyncAction::AsyncDeleteDirectory(AsyncAction, *SourceDirectory, true, true);
//
//			if (AsyncAction->OnActionFinished.IsBound())
//				AsyncAction->OnActionFinished.Broadcast(Result);
//		});
//}
//
//UDeleteFileAsyncAction* UDeleteFileAsyncAction::AsyncDeleteFile(UObject* WorldContextObject, const FString& File)
//{
//	UDeleteFileAsyncAction* AsyncAction = NewObject<UDeleteFileAsyncAction>();
//	AsyncAction->StartDeleteFile(AsyncAction, File);
//	return AsyncAction;
//}
//
//void UDeleteFileAsyncAction::StartDeleteFile(UDeleteFileAsyncAction* AsyncAction, const FString& Filename)
//{
//	AsyncTask(ENamedThreads::GameThread, [AsyncAction, Filename]()
//		{
//			bool Result = false;
//			if (IFileManager::Get().FileExists(*Filename))
//				Result = IFileManager::Get().Delete(*Filename, true, false);
//
//			if (AsyncAction->OnActionFinished.IsBound())
//				AsyncAction->OnActionFinished.Broadcast(Result);
//		});
//}
//
//UCopyFileAsyncAction* UCopyFileAsyncAction::AsyncCopyFile(UObject* WorldContextObject, const FString& SourceFile, const FString& DestinationFile)
//{
//	UCopyFileAsyncAction* AsyncAction = NewObject<UCopyFileAsyncAction>();
//	AsyncAction->StartCopyFile(AsyncAction, SourceFile, DestinationFile);
//	return AsyncAction;
//}
//
//void UCopyFileAsyncAction::StartCopyFile(UCopyFileAsyncAction* AsyncAction, const FString& SourceFile, const FString& DestinationFile)
//{
//	AsyncTask(ENamedThreads::GameThread, [AsyncAction, SourceFile, DestinationFile]()
//		{
//			bool Result = false;
//			FString Directory = FPaths::GetPath(DestinationFile);
//			if (!IFileManager::Get().DirectoryExists(*Directory))
//				IFileManager::Get().MakeDirectory(*Directory, true);
//
//			if (IFileManager::Get().FileExists(*SourceFile))
//				Result = IFileManager::Get().Copy(*DestinationFile, *SourceFile, true, false) == COPY_OK;
//
//			if (AsyncAction->OnActionFinished.IsBound())
//				AsyncAction->OnActionFinished.Broadcast(Result);
//		});
//}
//
//UMoveFileAsyncAction* UMoveFileAsyncAction::AsyncMoveFile(UObject* WorldContextObject, const FString& SourceFile, const FString& DestinationFile)
//{
//	UMoveFileAsyncAction* AsyncAction = NewObject<UMoveFileAsyncAction>();
//	AsyncAction->StartMoveFile(AsyncAction, SourceFile, DestinationFile);
//	return AsyncAction;
//}
//
//void UMoveFileAsyncAction::StartMoveFile(UMoveFileAsyncAction* AsyncAction, const FString& SourceFile, const FString& DestinationFile)
//{
//	AsyncTask(ENamedThreads::GameThread, [AsyncAction, SourceFile, DestinationFile]()
//		{
//			bool Result = false;
//			FString Directory = FPaths::GetPath(DestinationFile);
//			if (!IFileManager::Get().DirectoryExists(*Directory))
//				IFileManager::Get().MakeDirectory(*Directory, true);
//
//			if (IFileManager::Get().FileExists(*SourceFile))
//				Result = IFileManager::Get().Move(*DestinationFile, *SourceFile, true, false);
//
//			if (AsyncAction->OnActionFinished.IsBound())
//				AsyncAction->OnActionFinished.Broadcast(Result);
//		});
//}
//
//UCollectFileDirectory* UCollectFileDirectory::AsyncCollectFileDirectory(UObject* WorldContextObject, const FString& Directory, const FString& Wildcard, const bool bFindFiles, const bool bFindDirectories)
//{
//	UCollectFileDirectory* AsyncAction = NewObject<UCollectFileDirectory>();
//	AsyncAction->StartCollectFileDirectory(AsyncAction, Directory, Wildcard, bFindFiles, bFindDirectories);
//	return AsyncAction;
//}
//
//void UCollectFileDirectory::StartCollectFileDirectory(UCollectFileDirectory* AsyncAction, const FString& Directory, const FString& Wildcard, const bool bFindFiles, const bool bFindDirectories)
//{
//	AsyncTask(ENamedThreads::GameThread, [AsyncAction, Directory, Wildcard, bFindFiles, bFindDirectories]()
//		{
//			FString InWidcard = Wildcard;
//			if (InWidcard.IsEmpty())
//				InWidcard = TEXT("*.*");
//
//			TArray<FString> FoundFilenames;
//			IFileManager::Get().FindFilesRecursive(FoundFilenames, *Directory, *InWidcard, bFindFiles, bFindDirectories, true);
//
//			if (AsyncAction->OnActionFinished.IsBound())
//				AsyncAction->OnActionFinished.Broadcast(FoundFilenames);
//		});
//}

void UPTUtility::AsyncDeleteFile(FOnFileDirectoryActionFinished OnFilished, const FString& Filename)
{
	AsyncTask(ENamedThreads::GameThread, [OnFilished, Filename]()
		{
			bool Result = false;
			if (IFileManager::Get().FileExists(*Filename))
				Result = IFileManager::Get().Delete(*Filename, true, false);

			if (OnFilished.IsBound())
				OnFilished.ExecuteIfBound(Result);
		});
}

void UPTUtility::AsyncDeleteDirectory(FOnFileDirectoryActionFinished OnFilished, const FString& Directory, bool bMustExist /*= false*/, bool bDeleteAllSubDirectory /*= false*/)
{
	class FDeleteHelper
	{
	public:
		const bool DeleteAllSubDirectory(const FString& Directory, bool bMustExist = false, bool bDeleteAllSubDirectory = false)
		{
			bool bSuccees = IFileManager::Get().DeleteDirectory(*Directory, bMustExist, bDeleteAllSubDirectory);
			if (bSuccees)
				return true;
			else
				return DeleteAllSubDirectory(Directory, bMustExist, bDeleteAllSubDirectory);
		}
	};

	AsyncTask(ENamedThreads::GameThread, [OnFilished, Directory, bMustExist, bDeleteAllSubDirectory]()
		{
			bool Result = false;
			if (IFileManager::Get().DirectoryExists(*Directory))
			{
				//if (bDeleteAllSubDirectory)
				//{
				//	FDeleteHelper* DeleteHelper = new FDeleteHelper();
				//	Result = DeleteHelper->DeleteAllSubDirectory(Directory, bMustExist, bDeleteAllSubDirectory);
				//}
				//else
				Result = IFileManager::Get().DeleteDirectory(*Directory, bMustExist, bDeleteAllSubDirectory);
			}

			if (OnFilished.IsBound())
				OnFilished.ExecuteIfBound(Result);
		});
}