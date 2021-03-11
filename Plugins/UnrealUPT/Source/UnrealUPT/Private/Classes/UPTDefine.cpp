#include "Classes/UPTDefine.h"
#include "App.h"
#include "IPluginManager.h"

const FString Get_UPT_Resource()
{
	FString LaunchDir = FPaths::ConvertRelativePathToFull(FPaths::LaunchDir());
	LaunchDir = FPaths::ConvertRelativePathToFull(FPaths::RootDir());
	LaunchDir = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
	LaunchDir = FPaths::ConvertRelativePathToFull(FPaths::ProjectModsDir());
	LaunchDir = FPaths::ConvertRelativePathToFull(FPaths::SourceConfigDir());
	LaunchDir = FPaths::ConvertRelativePathToFull(FPaths::GameSourceDir());

	//FString ProjectDir = FPaths::ProjectDir();
	//FString ProjectPath = FString::Printf(TEXT("Programs/%s/"), FApp::GetProjectName());
	//FString ResourcePath = FString::Printf(TEXT("Source/Programs/%s/Resources"), FApp::GetProjectName());
	//FString ResourceDir = ProjectDir.Replace(*ProjectPath, *ResourcePath);

	FString ResourceDir = IPluginManager::Get().FindPlugin(TEXT("UnrealUPT"))->GetBaseDir() / TEXT("Resources");
	return FPaths::ConvertRelativePathToFull(ResourceDir);
}