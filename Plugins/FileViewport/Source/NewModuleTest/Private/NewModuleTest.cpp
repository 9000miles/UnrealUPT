// Fill out your copyright notice in the Description page of Project Settings.

#include "NewModuleTest.h"
//#include "Logging.h"

#include "Modules/ModuleManager.h"
#include "GameProjectGeneration/Public/GameProjectUtils.h"
#include "GeneralProjectSettings.h"

#define TEMPLATE_FILE(Directory, Type) FString(Directory) / FString(#Type).Replace(TEXT("ECodeTemplateType."),TEXT("")).Replace(TEXT("_"),TEXT("."))+TEXT(".template");

#define LOCTEXT_NAMESPACE "FNewModuleTest"

enum class ECodeFileType :uint8
{
	None,
	H,
	Cpp,
	Build_cs,
	Target_cs,
};

enum class ECodeTemplateType :uint8
{
	None,
	ActorClass_cpp,
	ActorClass_h,
	ActorComponentClass_cpp,
	ActorComponentClass_h,
	CharacterClass_cpp,
	CharacterClass_h,
	EditorModule_Build_cs,
	EditorModule_Target_cs,
	EmptyClass_cpp,
	EmptyClass_h,
	GameModule_Build_cs,
	GameModule_Target_cs,
	GameModule_cpp,
	GameModule_h,
	InterfaceClass_cpp,
	InterfaceClass_h,
	PawnClass_cpp,
	PawnClass_h,
	PluginModule_Build_cs,
	PluginModule_cpp,
	PluginModule_h,
	SlateWidget_cpp,
	SlateWidget_h,
	SlateWidgetStyle_cpp,
	SlateWidgetStyle_h,
	SoundEffectSourceClass_cpp,
	SoundEffectSourceClass_h,
	SoundEffectSubmixClass_cpp,
	SoundEffectSubmixClass_h,
	Stub_Target_cs,
	SynthComponentClass_cpp,
	SynthComponentClass_h,
	UObjectClass_cpp,
	UObjectClass_h,
	UObjectClassConstructorDeclaration,
	UObjectClassConstructorDefinition,
};

FString MyGetClassPrefixCPP(ECodeTemplateType ClassType)
{
	switch (ClassType)
	{
	case ECodeTemplateType::ActorClass_cpp:
	case ECodeTemplateType::ActorClass_h:
	case ECodeTemplateType::CharacterClass_cpp:
	case ECodeTemplateType::CharacterClass_h:
	case ECodeTemplateType::PawnClass_cpp:
	case ECodeTemplateType::PawnClass_h:
		return TEXT("A");

	case ECodeTemplateType::ActorComponentClass_cpp:
	case ECodeTemplateType::ActorComponentClass_h:
	case ECodeTemplateType::InterfaceClass_cpp:
	case ECodeTemplateType::InterfaceClass_h:
	case ECodeTemplateType::UObjectClass_cpp:
	case ECodeTemplateType::UObjectClass_h:
		return TEXT("U");

	case ECodeTemplateType::EmptyClass_cpp:
	case ECodeTemplateType::EmptyClass_h:
		return TEXT("F");

	case ECodeTemplateType::SlateWidget_cpp:
	case ECodeTemplateType::SlateWidget_h:
		return TEXT("S");

	case ECodeTemplateType::SlateWidgetStyle_cpp:
	case ECodeTemplateType::SlateWidgetStyle_h:
		return TEXT("F");

	default:
		break;
	}
	return TEXT("");
}
FString MyGetClassNameCPP(ECodeTemplateType ClassType)
{
	switch (ClassType)
	{
	case ECodeTemplateType::UObjectClass_cpp:
	case ECodeTemplateType::EmptyClass_cpp:
		return TEXT("");

	case ECodeTemplateType::SlateWidget_cpp:
		return TEXT("CompoundWidget");

	case ECodeTemplateType::SlateWidgetStyle_cpp:
		return TEXT("SlateWidgetStyle");

	case ECodeTemplateType::InterfaceClass_cpp:
		return TEXT("Interface");

	default:
		break;
	}
	return TEXT("");
}
FString MyMakeCopyrightLine()
{
	const FString CopyrightNotice = GetDefault<UGeneralProjectSettings>()->CopyrightNotice;
	if (!CopyrightNotice.IsEmpty())
	{
		return FString(TEXT("// ")) + CopyrightNotice;
	}
	else
	{
		return FString();
	}
}

FString MyMakeCommaDelimitedList(const TArray<FString>& InList, bool bPlaceQuotesAroundEveryElement)
{
	FString ReturnString;

	for (auto ListIt = InList.CreateConstIterator(); ListIt; ++ListIt)
	{
		FString ElementStr;
		if (bPlaceQuotesAroundEveryElement)
		{
			ElementStr = FString::Printf(TEXT("\"%s\""), **ListIt);
		}
		else
		{
			ElementStr = *ListIt;
		}

		if (ReturnString.Len() > 0)
		{
			// If this is not the first item in the list, prepend with a comma
			ElementStr = FString::Printf(TEXT(", %s"), *ElementStr);
		}

		ReturnString += ElementStr;
	}

	return ReturnString;
}
void MyHarvestCursorSyncLocation(FString& FinalOutput, FString& OutSyncLocation)
{
	OutSyncLocation.Empty();

	// Determine the cursor focus location if this file will by synced after creation
	TArray<FString> Lines;
	FinalOutput.ParseIntoArray(Lines, TEXT("\n"), false);
	for (int32 LineIdx = 0; LineIdx < Lines.Num(); ++LineIdx)
	{
		const FString& Line = Lines[LineIdx];
		int32 CharLoc = Line.Find(TEXT("%CURSORFOCUSLOCATION%"));
		if (CharLoc != INDEX_NONE)
		{
			// Found the sync marker
			OutSyncLocation = FString::Printf(TEXT("%d:%d"), LineIdx + 1, CharLoc + 1);
			break;
		}
	}

	// If we did not find the sync location, just sync to the top of the file
	if (OutSyncLocation.IsEmpty())
	{
		OutSyncLocation = TEXT("1:1");
	}

	// Now remove the cursor focus marker
	FinalOutput = FinalOutput.Replace(TEXT("%CURSORFOCUSLOCATION%"), TEXT(""), ESearchCase::CaseSensitive);
}
FString MyReplaceWildcard(const FString& Input, const FString& From, const FString& To, bool bLeadingTab, bool bTrailingNewLine)
{
	FString Result = Input;
	FString WildCard = bLeadingTab ? TEXT("\t") : TEXT("");

	WildCard.Append(From);

	if (bTrailingNewLine)
	{
		WildCard.Append(LINE_TERMINATOR);
	}

	int32 NumReplacements = Result.ReplaceInline(*WildCard, *To, ESearchCase::CaseSensitive);

	// if replacement fails, try again using just the plain wildcard without tab and/or new line
	if (NumReplacements == 0)
	{
		Result = Result.Replace(*From, *To, ESearchCase::CaseSensitive);
	}

	return Result;
}
bool MyGenerateConstructorDeclaration(FString& Out, const FString& PrefixedClassName, FText& OutFailReason)
{
	FString Template;
	FString TemplateDirectory = FString(TEXT("M:/UnrealProjects/UnrealUPT/Plugins/FileViewport/Resources/Templates"));
	FString TemplateFile = TEMPLATE_FILE(TemplateDirectory, ECodeTemplateType.UObjectClassConstructorDeclaration);
	if (IFileManager::Get().FileExists(*TemplateFile))
	{
		FString OutFileContents;
		if (FFileHelper::LoadFileToString(OutFileContents, *TemplateFile))
		{
			Out = OutFileContents.Replace(TEXT("%PREFIXED_CLASS_NAME%"), *PrefixedClassName, ESearchCase::CaseSensitive);
			return true;
		}
	}
	return false;
}
bool CreateClassHeaderFile(FString NewHeaderFileName, ECodeTemplateType Type, const FString ParentClassName, const FString UnPrefixedClassName, FString& TemplateFile, TArray<FString> ClassSpecifierList, const FString& ClassProperties, const FString& ClassFunctionDeclarations)
{
	const FString ClassPrefix = MyGetClassPrefixCPP(Type);
	const FString PrefixedClassName = ClassPrefix + UnPrefixedClassName;
	const FString PrefixedBaseClassName = ParentClassName;

	FString BaseClassIncludeDirective;
	FString BaseClassIncludePath;

	FString ModuleName;
	FString ModuleAPIMacro;
	FString EventualConstructorDeclaration;

	if (true)
	{
		FText OutFailReason;
		if (!MyGenerateConstructorDeclaration(EventualConstructorDeclaration, PrefixedClassName, OutFailReason))
		{
			return false;
		}
	}

	// Not all of these will exist in every class template
	FString FinalOutput = TemplateFile.Replace(TEXT("%COPYRIGHT_LINE%"), *MyMakeCopyrightLine(), ESearchCase::CaseSensitive);
	FinalOutput = FinalOutput.Replace(TEXT("%UNPREFIXED_CLASS_NAME%"), *UnPrefixedClassName, ESearchCase::CaseSensitive);
	FinalOutput = FinalOutput.Replace(TEXT("%CLASS_MODULE_API_MACRO%"), *ModuleAPIMacro, ESearchCase::CaseSensitive);
	FinalOutput = FinalOutput.Replace(TEXT("%UCLASS_SPECIFIER_LIST%"), *MyMakeCommaDelimitedList(ClassSpecifierList, false), ESearchCase::CaseSensitive);
	FinalOutput = FinalOutput.Replace(TEXT("%PREFIXED_CLASS_NAME%"), *PrefixedClassName, ESearchCase::CaseSensitive);
	FinalOutput = FinalOutput.Replace(TEXT("%PREFIXED_BASE_CLASS_NAME%"), *PrefixedBaseClassName, ESearchCase::CaseSensitive);
	FinalOutput = FinalOutput.Replace(TEXT("%MODULE_NAME%"), *ModuleName, ESearchCase::CaseSensitive);

	// Special case where where the wildcard starts with a tab and ends with a new line
	const bool bLeadingTab = true;
	const bool bTrailingNewLine = true;
	FinalOutput = MyReplaceWildcard(FinalOutput, TEXT("%EVENTUAL_CONSTRUCTOR_DECLARATION%"), *EventualConstructorDeclaration, bLeadingTab, bTrailingNewLine);
	FinalOutput = MyReplaceWildcard(FinalOutput, TEXT("%CLASS_PROPERTIES%"), *ClassProperties, bLeadingTab, bTrailingNewLine);
	FinalOutput = MyReplaceWildcard(FinalOutput, TEXT("%CLASS_FUNCTION_DECLARATIONS%"), *ClassFunctionDeclarations, bLeadingTab, bTrailingNewLine);
	if (BaseClassIncludeDirective.Len() == 0)
	{
		FinalOutput = FinalOutput.Replace(TEXT("%BASE_CLASS_INCLUDE_DIRECTIVE%") LINE_TERMINATOR, TEXT(""), ESearchCase::CaseSensitive);
	}
	FinalOutput = FinalOutput.Replace(TEXT("%BASE_CLASS_INCLUDE_DIRECTIVE%"), *BaseClassIncludeDirective, ESearchCase::CaseSensitive);
	FString OutSyncLocation;
	MyHarvestCursorSyncLocation(FinalOutput, OutSyncLocation);

	if (FFileHelper::SaveStringToFile(FinalOutput, *NewHeaderFileName))
	{
		return true;
	}

	return false;
}
bool MyGenerateConstructorDefinition(FString& Out, const FString& PrefixedClassName, const FString& PropertyOverridesStr, FText& OutFailReason)
{
	FString TemplateDirectory = FString(TEXT("M:/UnrealProjects/UnrealUPT/Plugins/FileViewport/Resources/Templates"));
	FString TemplateFile = TEMPLATE_FILE(TemplateDirectory, ECodeTemplateType.UObjectClassConstructorDefinition);
	if (IFileManager::Get().FileExists(*TemplateFile))
	{
		FString OutFileContents;
		if (FFileHelper::LoadFileToString(OutFileContents, *TemplateFile))
		{
			Out = OutFileContents.Replace(TEXT("%PREFIXED_CLASS_NAME%"), *PrefixedClassName, ESearchCase::CaseSensitive);
			Out = Out.Replace(TEXT("%PROPERTY_OVERRIDES%"), *PropertyOverridesStr, ESearchCase::CaseSensitive);
			return true;
		}
	}

	return true;
}
FString MyMakeIncludeList(const TArray<FString>& InList)
{
	FString ReturnString;

	const TCHAR IncludePathFormatString[] = TEXT("#include \"%s\"");
	for (auto ListIt = InList.CreateConstIterator(); ListIt; ++ListIt)
	{
		ReturnString += FString::Printf(IncludePathFormatString, **ListIt);
		ReturnString += LINE_TERMINATOR;
	}

	return ReturnString;
}
bool CreateClassCPPFile(FString NewHeaderFileName, ECodeTemplateType Type, const FString UnPrefixedClassName, FString& TemplateFile, TArray<FString> ClassSpecifierList, const TArray<FString>& AdditionalIncludes, const TArray<FString>& PropertyOverrides, const FString& AdditionalMemberDefinitions, FString& OutSyncLocation)
{
	const FString ClassPrefix = MyGetClassPrefixCPP(Type);
	const FString PrefixedClassName = ClassPrefix + UnPrefixedClassName;

	FString PropertyOverridesStr;
	for (int32 OverrideIdx = 0; OverrideIdx < PropertyOverrides.Num(); ++OverrideIdx)
	{
		if (OverrideIdx > 0)
		{
			PropertyOverridesStr += LINE_TERMINATOR;
		}

		PropertyOverridesStr += TEXT("\t");
		PropertyOverridesStr += *PropertyOverrides[OverrideIdx];
	}

	FString BaseClassIncludeDirective;
	FString BaseClassIncludePath;

	FString ModuleName;
	FString ModuleAPIMacro;
	FString EventualConstructorDefinition;
	if (PropertyOverrides.Num() != 0)
	{
		FText OutFailReason;
		if (!MyGenerateConstructorDefinition(EventualConstructorDefinition, PrefixedClassName, PropertyOverridesStr, OutFailReason))
		{
			return false;
		}
	}

	// Not all of these will exist in every class template
	FString FinalOutput = TemplateFile.Replace(TEXT("%COPYRIGHT_LINE%"), *MyMakeCopyrightLine(), ESearchCase::CaseSensitive);
	FinalOutput = FinalOutput.Replace(TEXT("%UNPREFIXED_CLASS_NAME%"), *UnPrefixedClassName, ESearchCase::CaseSensitive);
	FinalOutput = FinalOutput.Replace(TEXT("%PREFIXED_CLASS_NAME%"), *PrefixedClassName, ESearchCase::CaseSensitive);
	FinalOutput = FinalOutput.Replace(TEXT("%MODULE_NAME%"), *ModuleName, ESearchCase::CaseSensitive);
	FinalOutput = FinalOutput.Replace(TEXT("%PCH_INCLUDE_DIRECTIVE%"), TEXT(""), ESearchCase::CaseSensitive);
	FinalOutput = FinalOutput.Replace(TEXT("%MY_HEADER_INCLUDE_DIRECTIVE%"), TEXT(""), ESearchCase::CaseSensitive);
	FinalOutput = FinalOutput.Replace(TEXT("%LOCTEXT_NAMESPACE%"), *PrefixedClassName, ESearchCase::CaseSensitive);

	// Special case where where the wildcard ends with a new line
	const bool bLeadingTab = false;
	const bool bTrailingNewLine = true;
	FinalOutput = MyReplaceWildcard(FinalOutput, TEXT("%ADDITIONAL_INCLUDE_DIRECTIVES%"), *MyMakeIncludeList(AdditionalIncludes), bLeadingTab, bTrailingNewLine);
	FinalOutput = MyReplaceWildcard(FinalOutput, TEXT("%EVENTUAL_CONSTRUCTOR_DEFINITION%"), *EventualConstructorDefinition, bLeadingTab, bTrailingNewLine);
	FinalOutput = MyReplaceWildcard(FinalOutput, TEXT("%ADDITIONAL_MEMBER_DEFINITIONS%"), *AdditionalMemberDefinitions, bLeadingTab, bTrailingNewLine);

	MyHarvestCursorSyncLocation(FinalOutput, OutSyncLocation);

	if (FFileHelper::SaveStringToFile(FinalOutput, *NewHeaderFileName))
	{
		return true;
	}

	return false;
}
void FNewModuleTest::StartupModule()
{
	FString TemplateDirectory = FString(TEXT("M:/UnrealProjects/UnrealUPT/Plugins/FileViewport/Resources/Templates"));
	FString TemplateFile = TEMPLATE_FILE(TemplateDirectory, ECodeTemplateType.ActorClass_h);
	if (IFileManager::Get().FileExists(*TemplateFile))
	{
		FString OutFileContents;
		if (FFileHelper::LoadFileToString(OutFileContents, *TemplateFile))
		{
			CreateClassHeaderFile
			(
				TEXT("M:/UnrealProjects/UnrealUPT/Source/CreateCodeTest/Public/Actor/MyTemplateNewActorClass.h"),
				ECodeTemplateType::ActorClass_h,
				TEXT("AActor"),
				TEXT("MyTemplateNewActorClass"),
				OutFileContents,
				{ FString(TEXT("BlueprintType")),FString(TEXT("Blueprintable")) },
				TEXT(""),
				TEXT("")
			);
		}
	}

	TemplateFile = TEMPLATE_FILE(TemplateDirectory, ECodeTemplateType.EmptyClass_h);
	if (IFileManager::Get().FileExists(*TemplateFile))
	{
		FString OutFileContents;
		if (FFileHelper::LoadFileToString(OutFileContents, *TemplateFile))
		{
			CreateClassHeaderFile
			(
				TEXT("M:/UnrealProjects/UnrealUPT/Source/CreateCodeTest/Public/Classes/MyTemplateEmptyClass.h"),
				ECodeTemplateType::EmptyClass_h,
				TEXT("GameClass"),
				TEXT("MyTemplateEmptyClass"),
				OutFileContents,
				TArray<FString>(),
				TEXT(""),
				TEXT("")
			);
		}
	}

	TemplateFile = TEMPLATE_FILE(TemplateDirectory, ECodeTemplateType.SlateWidget_h);
	if (IFileManager::Get().FileExists(*TemplateFile))
	{
		FString OutFileContents;
		if (FFileHelper::LoadFileToString(OutFileContents, *TemplateFile))
		{
			CreateClassHeaderFile
			(
				TEXT("M:/UnrealProjects/UnrealUPT/Source/CreateCodeTest/Public/Slate/MyTemplateSlateWidget.h"),
				ECodeTemplateType::SlateWidget_h,
				TEXT(""),
				TEXT("MyTemplateSlateWidget"),
				OutFileContents,
				TArray<FString>(),
				TEXT(""),
				TEXT("")
			);
		}
	}
	/// <summary>
	/// ///////////////////////////////////////////////////////
	/// </summary>
	TemplateFile = TEMPLATE_FILE(TemplateDirectory, ECodeTemplateType.ActorClass_cpp);
	if (IFileManager::Get().FileExists(*TemplateFile))
	{
		FString OutFileContents;
		if (FFileHelper::LoadFileToString(OutFileContents, *TemplateFile))
		{
			FString UnusedSyncLocation;
			CreateClassCPPFile
			(
				TEXT("M:/UnrealProjects/UnrealUPT/Source/CreateCodeTest/Private/Actor/MyTemplateNewActorClass.cpp"),
				ECodeTemplateType::ActorClass_cpp,
				TEXT("MyTemplateNewActorClass"),
				OutFileContents,
				TArray<FString>(),
				TArray<FString>(),
				TArray<FString>(),
				TEXT(""),
				UnusedSyncLocation
			);
		}
	}
	TemplateFile = TEMPLATE_FILE(TemplateDirectory, ECodeTemplateType.EmptyClass_cpp);
	if (IFileManager::Get().FileExists(*TemplateFile))
	{
		FString OutFileContents;
		if (FFileHelper::LoadFileToString(OutFileContents, *TemplateFile))
		{
			FString UnusedSyncLocation;
			CreateClassCPPFile
			(
				TEXT("M:/UnrealProjects/UnrealUPT/Source/CreateCodeTest/Private/Classes/MyTemplateEmptyClass.cpp"),
				ECodeTemplateType::EmptyClass_cpp,
				TEXT("MyTemplateEmptyClass"),
				OutFileContents,
				TArray<FString>(),
				TArray<FString>(),
				TArray<FString>(),
				TEXT(""),
				UnusedSyncLocation
			);
		}
	}
	TemplateFile = TEMPLATE_FILE(TemplateDirectory, ECodeTemplateType.SlateWidget_cpp);
	if (IFileManager::Get().FileExists(*TemplateFile))
	{
		FString OutFileContents;
		if (FFileHelper::LoadFileToString(OutFileContents, *TemplateFile))
		{
			FString UnusedSyncLocation;
			CreateClassCPPFile
			(
				TEXT("M:/UnrealProjects/UnrealUPT/Source/CreateCodeTest/Private/Slate/MyTemplateSlateWidget.cpp"),
				ECodeTemplateType::SlateWidget_cpp,
				TEXT("MyTemplateSlateWidget"),
				OutFileContents,
				TArray<FString>(),
				TArray<FString>(),
				TArray<FString>(),
				TEXT(""),
				UnusedSyncLocation
			);
		}
	}
	UCreateCodeFilesBPLirary::CreateCodeFile();
}
void FNewModuleTest::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FNewModuleTest, NewModuleTest);
/// <summary>
///
/// </summary>
/// <param name="Type"></param>
/// <returns></returns>
FString GetTemplateFile(ECodeTemplateType Type)
{
	FString TemplateDirectory = FString(TEXT("M:/UnrealProjects/UnrealUPT/Plugins/FileViewport/Resources/Templates"));
	FString TemplateFile = TEMPLATE_FILE(TemplateDirectory, ECodeTemplateType.ActorClass_cpp);
	return TemplateFile;
}

void UCreateCodeFilesBPLirary::CreateCodeFile()
{
	TArray<FString> PD;
	TArray<FString> PriD;
	FText OuF;
	//GameProjectUtils::GenerateGameModuleBuildFile(FString(TEXT("ModuleTest")), TEXT("ModuleNameTest"), PD, PriD, OuF);
	//GameProjectUtils::GenerateClassCPPFile(FString(TEXT("ModuleTest")), TEXT("ModuleNameTest"), PD, PriD, OuF);

	FString ActorClassTemplate = FString(TEXT("M:/UnrealEngine-4.26.0/Engine/Content/Editor/Templates/ActorClass.h.template"));
	FString OutFileContents;
	if (FFileHelper::LoadFileToString(OutFileContents, *ActorClassTemplate))
	{
	}

	FString UnusedSyncLocation;
	FText OutFailReason;

	FModuleContextInfo NewModuleInfo;
	NewModuleInfo.ModuleName = FString(TEXT("NewModule"));
	NewModuleInfo.ModuleType = EHostType::Runtime;
	NewModuleInfo.ModuleSourcePath = FString(TEXT("M:/UnrealProjects/UnrealUPT/Source/CreateCodeTest")); // Ensure trailing /
	{
		GameProjectUtils::GenerateClassHeaderFile
		(
			TEXT("M:/UnrealProjects/UnrealUPT/Source/CreateCodeTest/NewActor.h"),
			TEXT("NewObject"),
			FNewClassInfo(AActor::StaticClass()),
			{ FString(TEXT("BlueprintType")),FString(TEXT("Blueprintable")) },
			TEXT(""),
			TEXT(""),
			UnusedSyncLocation,
			NewModuleInfo,
			true,
			OutFailReason
		);
	}
	{
		GameProjectUtils::GenerateClassCPPFile
		(
			TEXT("M:/UnrealProjects/UnrealUPT/Source/CreateCodeTest/NewActor.cpp"),
			TEXT("NewObject"),
			FNewClassInfo(AActor::StaticClass()),
			TArray<FString>(),
			TArray<FString>(),
			TEXT(""),
			UnusedSyncLocation,
			NewModuleInfo,
			OutFailReason
		);
	}

	//////////////////////////////////////
	{
		GameProjectUtils::GenerateClassHeaderFile
		(
			TEXT("M:/UnrealProjects/UnrealUPT/Source/CreateCodeTest/Private/NewClass.h"),
			TEXT("NewClass"),
			FNewClassInfo(FNewClassInfo::EClassType::EmptyCpp),
			TArray<FString>(),
			TEXT(""),
			TEXT(""),
			UnusedSyncLocation,
			NewModuleInfo,
			false,
			OutFailReason
		);
	}
	{
		GameProjectUtils::GenerateClassCPPFile
		(
			TEXT("M:/UnrealProjects/UnrealUPT/Source/CreateCodeTest/Public/NewObject.cpp"),
			TEXT("NewClass"),
			FNewClassInfo(FNewClassInfo::EClassType::EmptyCpp),
			TArray<FString>(),
			TArray<FString>(),
			TEXT(""),
			UnusedSyncLocation,
			NewModuleInfo,
			OutFailReason
		);
	}

	//////////////////////////////////////
	{
		GameProjectUtils::GenerateClassHeaderFile
		(
			TEXT("M:/UnrealProjects/UnrealUPT/Source/CreateCodeTest/Private/Widget/NewWidget.h"),
			TEXT("NewWidget"),
			FNewClassInfo(FNewClassInfo::EClassType::SlateWidget),
			TArray<FString>(),
			TEXT(""),
			TEXT(""),
			UnusedSyncLocation,
			NewModuleInfo,
			false,
			OutFailReason
		);
	}
	{
		GameProjectUtils::GenerateClassCPPFile
		(
			TEXT("M:/UnrealProjects/UnrealUPT/Source/CreateCodeTest/Public/Widget/NewWidget.cpp"),
			TEXT("NewWidget"),
			FNewClassInfo(FNewClassInfo::EClassType::SlateWidget),
			TArray<FString>(),
			TArray<FString>(),
			TEXT(""),
			UnusedSyncLocation,
			NewModuleInfo,
			OutFailReason
		);
	}
}