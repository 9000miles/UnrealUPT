#include "Settings/UPTSettings.h"

FProjectSettings UUPTSettings::GetProjectSetting(const FString Path)
{
	if (FPaths::FileExists(Path))
	{
		const UUPTSettings* UPTSettings = GetDefault<UUPTSettings>();
		if (UPTSettings)
		{
			if (const FProjectSettings* FoundSettings = UPTSettings->ProjectSettings.FindByPredicate([Path](FProjectSettings Settings) {return Settings.ProjectPath == Path; }))
				return *FoundSettings;
		}
	}
	return FProjectSettings();
}

FApplicationSettings UUPTSettings::GetApplicationSetting(const FString Path)
{
	if (FPaths::FileExists(Path))
	{
		const UUPTSettings* UPTSettings = GetDefault<UUPTSettings>();
		if (UPTSettings)
		{
			if (const FApplicationSettings* FoundSettings = UPTSettings->ApplicationSettings.FindByPredicate([Path](FApplicationSettings Settings) {return Settings.ExePath == Path; }))
				return *FoundSettings;
		}
	}
	return FApplicationSettings();
}