#include "UPTDefine.h"
#include "App.h"

const FString Get_UPT_Resource()
{
	FString ProjectDir = FPaths::ProjectDir();
	FString ProjectPath = FString::Printf(TEXT("Programs/%s/"), FApp::GetProjectName());
	FString ResourcePath = FString::Printf(TEXT("Source/Programs/%s/Resources"), FApp::GetProjectName());
	FString ResourceDir = ProjectDir.Replace(*ProjectPath, *ResourcePath);
	return FPaths::ConvertRelativePathToFull(ResourceDir);
}
