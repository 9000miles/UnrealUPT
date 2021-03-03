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

	FString OutRootDir;
	FDesktopPlatformModule::Get()->GetEngineRootDirFromIdentifier(Identifier, OutRootDir);
	const bool bSourceDistribution = FDesktopPlatformModule::Get()->IsSourceDistribution(OutRootDir);

	const FString Vers = Version.ToString(EVersionComponent::Patch);
	//EngineVersion = Vers;
	EngineVersion = (bSourceDistribution ? TEXT("S_") : TEXT("B_")) + Vers;
	//EngineVersion = Identifier;

	Thumbnail = InThumbnail;
}