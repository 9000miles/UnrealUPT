// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.


#include "CodeBrowserModule.h"
#include "CodeBrowserLog.h"
#include "CodeBrowserSingleton.h"
#include "MRUFavoritesList.h"
#include "Settings/CodeBrowserSettings.h"

IMPLEMENT_MODULE( FCodeBrowserModule, CodeBrowser );
DEFINE_LOG_CATEGORY(LogCodeBrowser);
const FName FCodeBrowserModule::NumberOfRecentAssetsName(TEXT("NumObjectsInRecentList"));

void FCodeBrowserModule::StartupModule()
{
	CodeBrowserSingleton = new FCodeBrowserSingleton();
	
	RecentlyOpenedAssets = MakeUnique<FMainMRUFavoritesList>(TEXT("CodeBrowserRecent"), GetDefault<UCodeBrowserSettings>()->NumObjectsInRecentList);
	RecentlyOpenedAssets->ReadFromINI();

	UCodeBrowserSettings::OnSettingChanged().AddRaw(this, &FCodeBrowserModule::ResizeRecentAssetList);
}

void FCodeBrowserModule::ShutdownModule()
{	
	if ( CodeBrowserSingleton )
	{
		delete CodeBrowserSingleton;
		CodeBrowserSingleton = NULL;
	}
	UCodeBrowserSettings::OnSettingChanged().RemoveAll(this);
	RecentlyOpenedAssets.Reset();
}

ICodeBrowserSingleton& FCodeBrowserModule::Get() const
{
	check(CodeBrowserSingleton);
	return *CodeBrowserSingleton;
}

void FCodeBrowserModule::ResizeRecentAssetList(FName InName)
{
	if (InName == NumberOfRecentAssetsName)
	{
		RecentlyOpenedAssets->WriteToINI();
		RecentlyOpenedAssets = MakeUnique<FMainMRUFavoritesList>(TEXT("CodeBrowserRecent"), GetDefault<UCodeBrowserSettings>()->NumObjectsInRecentList);
		RecentlyOpenedAssets->ReadFromINI();
	}
}