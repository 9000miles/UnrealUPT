#include "ProjectInfo.h"
#include "DesktopPlatform/Public/DesktopPlatformModule.h"
#include "Paths.h"
#include "EngineVersion.h"

FProjectInfo::FProjectInfo(const FString& InProjectPath, const TSharedPtr<FSlateBrush>& InThumbnail)
{
	ProjectName = FPaths::GetBaseFilename(InProjectPath);
	ProjectPath = InProjectPath;

	FString Identifier;
	FDesktopPlatformModule::Get()->GetEngineIdentifierForProject(ProjectPath, Identifier);
	FDesktopPlatformModule::Get()->GetEngineRootDirFromIdentifier(Identifier, EnginePath);

	FEngineVersion Version;
	FDesktopPlatformModule::Get()->TryGetEngineVersion(EnginePath, Version);
	EngineVersion = Version.ToString();

	Thumbnail = InThumbnail;
}