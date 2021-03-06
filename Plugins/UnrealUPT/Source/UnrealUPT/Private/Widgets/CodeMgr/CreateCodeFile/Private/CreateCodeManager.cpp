// Fill out your copyright notice in the Description page of Project Settings.
#include "CreateCodeManager.h"
#include "GeneralProjectSettings.h"
#include "FileHelper.h"
#include "UPTDefine.h"
#include <SNotificationList.h>
#include <UPTDelegateCenter.h>

#define LOCTEXT_NAMESPACE "SEngineProjects"

FCreateCodeManager::FCreateCodeManager()
{
}

void FCreateCodeManager::SetTemplateDirectory(const FString InTemplateDirectory)
{
	if (IFileManager::Get().DirectoryExists(*InTemplateDirectory))
		TemplateDirectory = InTemplateDirectory;
}

FCreateCodeManager::~FCreateCodeManager()
{
}

TSharedPtr<FCreateCodeManager> FCreateCodeManager::Get()
{
	if (Singleton == nullptr)
	{
		Singleton = MakeShareable(new FCreateCodeManager);
		FString Dir = Get_UPT_Resource() / TEXT("Templates");
		Singleton->SetTemplateDirectory(Dir);
	}
	return Singleton;
}

const bool FCreateCodeManager::CreateFile(const FString Parent, ECodeTemplateType TemplateType, const FString HPath, const FString CppPath)
{
	if (TemplateType == ECodeTemplateType::None)
		return false;

	if (!HPath.IsEmpty())
	{
		const FString FileName = FPaths::GetBaseFilename(HPath);
		CreateClassHeaderFile(HPath, TemplateType, ECodeFileType::H, Parent, FileName, TArray<FString>(), TEXT(""), TEXT(""));
	}
	if (!CppPath.IsEmpty())
	{
		FString UnusedSyncLocation;
		const FString FileName = FPaths::GetBaseFilename(CppPath);
		CreateClassCPPFile(CppPath, TemplateType, ECodeFileType::Cpp, FileName, TArray<FString>(), TArray<FString>(), TArray<FString>(), TEXT(""), UnusedSyncLocation);
	}
	return false;
}

FString FCreateCodeManager::GetCodeFileByEnum(ECodeFileType Type)
{
	UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("ECodeFileType"), true);
	if (EnumPtr)
		return EnumPtr->GetNameStringByValue((uint8)Type);
	return FString();
}

FString FCreateCodeManager::GetCppDirectoryByEnum(ECppDirectory Type)
{
	UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("ECppDirectory"), true);
	if (EnumPtr)
		return EnumPtr->GetNameStringByValue((uint8)Type);
	return FString();
}

FString FCreateCodeManager::GetCodeTemplateByEnum(ECodeTemplateType Type)
{
	UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("ECodeTemplateType"), true);
	if (EnumPtr)
		return EnumPtr->GetNameStringByValue((uint8)Type);
	return FString();
}

FString FCreateCodeManager::GetEnumValueStringByUint8(FString EnumName, uint8 Index)
{
	UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, *EnumName, true);
	if (EnumPtr)
		return EnumPtr->GetNameStringByValue(Index);
	return FString();
}

FString FCreateCodeManager::GetClassPrefixCPP(ECodeTemplateType ClassType)
{
	switch (ClassType)
	{
		case ECodeTemplateType::ActorClass:
		case ECodeTemplateType::CharacterClass:
		case ECodeTemplateType::PawnClass:
			return TEXT("A");

		case ECodeTemplateType::ActorComponentClass:
		case ECodeTemplateType::InterfaceClass:
		case ECodeTemplateType::UObjectClass:
			return TEXT("U");

		case ECodeTemplateType::Command:
		case ECodeTemplateType::EmptyClass:
		case ECodeTemplateType::SlateWidgetStyle:
			return TEXT("F");

		case ECodeTemplateType::SlateWidget:
			return TEXT("S");

		default:
			break;
	}
	return TEXT("");
}
FString FCreateCodeManager::GetClassNameCPP(ECodeTemplateType ClassType)
{
	switch (ClassType)
	{
		case ECodeTemplateType::UObjectClass:
			return TEXT("Object");

		case ECodeTemplateType::EmptyClass:
			return TEXT("");

		case ECodeTemplateType::SlateWidget:
			return TEXT("CompoundWidget");

		case ECodeTemplateType::SlateWidgetStyle:
			return TEXT("SlateWidgetStyle");

		case ECodeTemplateType::InterfaceClass:
			return TEXT("Interface");

		default:
			break;
	}
	return TEXT("");
}
FString FCreateCodeManager::MakeCopyrightLine()
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

FString FCreateCodeManager::MakeCommaDelimitedList(const TArray<FString>& InList, bool bPlaceQuotesAroundEveryElement)
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
void FCreateCodeManager::HarvestCursorSyncLocation(FString& FinalOutput, FString& OutSyncLocation)
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
FString FCreateCodeManager::ReplaceWildcard(const FString& Input, const FString& From, const FString& To, bool bLeadingTab, bool bTrailingNewLine)
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
bool FCreateCodeManager::GenerateConstructorDeclaration(FString& Out, const FString& PrefixedClassName, FText& OutFailReason)
{
	FString TemplateFile = CODE_TEMPLATE_FILE(TemplateDirectory, ECodeTemplateType.UObjectClassConstructorDeclaration);
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
bool FCreateCodeManager::GenerateConstructorDefinition(FString& Out, const FString& PrefixedClassName, const FString& PropertyOverridesStr, FText& OutFailReason)
{
	FString TemplateFile = CODE_TEMPLATE_FILE(TemplateDirectory, ECodeTemplateType.UObjectClassConstructorDefinition);
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
FString FCreateCodeManager::MakeIncludeList(const TArray<FString>& InList)
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

const bool FCreateCodeManager::CreateClassHeaderFile(FString NewHeaderFileName, ECodeTemplateType TemplateType, ECodeFileType FileType, const FString ParentClassName, const FString UnPrefixedClassName, TArray<FString> ClassSpecifierList, const FString& ClassProperties, const FString& ClassFunctionDeclarations)
{
	FString TemplateFile = GetTemplateFile(TemplateType, FileType);
	const FString ClassPrefix = GetClassPrefixCPP(TemplateType);
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
		if (!GenerateConstructorDeclaration(EventualConstructorDeclaration, PrefixedClassName, OutFailReason))
		{
			return false;
		}
	}

	// Not all of these will exist in every class template
	FString FinalOutput = TemplateFile.Replace(TEXT("%COPYRIGHT_LINE%"), *MakeCopyrightLine(), ESearchCase::CaseSensitive);
	FinalOutput = FinalOutput.Replace(TEXT("%UNPREFIXED_CLASS_NAME%"), *UnPrefixedClassName, ESearchCase::CaseSensitive);
	FinalOutput = FinalOutput.Replace(TEXT("%CLASS_MODULE_API_MACRO%"), *ModuleAPIMacro, ESearchCase::CaseSensitive);
	FinalOutput = FinalOutput.Replace(TEXT("%UCLASS_SPECIFIER_LIST%"), *MakeCommaDelimitedList(ClassSpecifierList, false), ESearchCase::CaseSensitive);
	FinalOutput = FinalOutput.Replace(TEXT("%PREFIXED_CLASS_NAME%"), *PrefixedClassName, ESearchCase::CaseSensitive);
	FinalOutput = FinalOutput.Replace(TEXT("%PREFIXED_BASE_CLASS_NAME%"), *PrefixedBaseClassName, ESearchCase::CaseSensitive);
	FinalOutput = FinalOutput.Replace(TEXT("%MODULE_NAME%"), *ModuleName, ESearchCase::CaseSensitive);

	// Special case where where the wildcard starts with a tab and ends with a new line
	const bool bLeadingTab = true;
	const bool bTrailingNewLine = true;
	FinalOutput = ReplaceWildcard(FinalOutput, TEXT("%EVENTUAL_CONSTRUCTOR_DECLARATION%"), *EventualConstructorDeclaration, bLeadingTab, bTrailingNewLine);
	FinalOutput = ReplaceWildcard(FinalOutput, TEXT("%CLASS_PROPERTIES%"), *ClassProperties, bLeadingTab, bTrailingNewLine);
	FinalOutput = ReplaceWildcard(FinalOutput, TEXT("%CLASS_FUNCTION_DECLARATIONS%"), *ClassFunctionDeclarations, bLeadingTab, bTrailingNewLine);
	if (BaseClassIncludeDirective.Len() == 0)
	{
		FinalOutput = FinalOutput.Replace(TEXT("%BASE_CLASS_INCLUDE_DIRECTIVE%") LINE_TERMINATOR, TEXT(""), ESearchCase::CaseSensitive);
	}
	FinalOutput = FinalOutput.Replace(TEXT("%BASE_CLASS_INCLUDE_DIRECTIVE%"), *BaseClassIncludeDirective, ESearchCase::CaseSensitive);
	FString OutSyncLocation;
	HarvestCursorSyncLocation(FinalOutput, OutSyncLocation);

	if (FFileHelper::SaveStringToFile(FinalOutput, *NewHeaderFileName, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
	{
		FNotificationInfo Info(FText::Format(LOCTEXT("CreateHederFileNotification", "Create file successfully: {0}"), FText::FromString(FPaths::GetCleanFilename(NewHeaderFileName))));
		Info.ExpireDuration = 3;
		Info.bUseLargeFont = false;
		FUPTDelegateCenter::OnRequestAddNotification.Execute(Info);
		return true;
	}

	return false;
}
const bool FCreateCodeManager::CreateClassCPPFile(FString NewCppFileName, ECodeTemplateType Type, ECodeFileType FileType, const FString UnPrefixedClassName, TArray<FString> ClassSpecifierList, const TArray<FString>& AdditionalIncludes, const TArray<FString>& PropertyOverrides, const FString& AdditionalMemberDefinitions, FString& OutSyncLocation)
{
	FString TemplateFile = GetTemplateFile(Type, FileType);
	const FString ClassPrefix = GetClassPrefixCPP(Type);
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
		if (!GenerateConstructorDefinition(EventualConstructorDefinition, PrefixedClassName, PropertyOverridesStr, OutFailReason))
		{
			return false;
		}
	}

	// Not all of these will exist in every class template
	FString FinalOutput = TemplateFile.Replace(TEXT("%COPYRIGHT_LINE%"), *MakeCopyrightLine(), ESearchCase::CaseSensitive);
	FinalOutput = FinalOutput.Replace(TEXT("%UNPREFIXED_CLASS_NAME%"), *UnPrefixedClassName, ESearchCase::CaseSensitive);
	FinalOutput = FinalOutput.Replace(TEXT("%PREFIXED_CLASS_NAME%"), *PrefixedClassName, ESearchCase::CaseSensitive);
	FinalOutput = FinalOutput.Replace(TEXT("%MODULE_NAME%"), *ModuleName, ESearchCase::CaseSensitive);
	FinalOutput = FinalOutput.Replace(TEXT("%PCH_INCLUDE_DIRECTIVE%"), TEXT(""), ESearchCase::CaseSensitive);
	FinalOutput = FinalOutput.Replace(TEXT("%MY_HEADER_INCLUDE_DIRECTIVE%"), TEXT(""), ESearchCase::CaseSensitive);
	FinalOutput = FinalOutput.Replace(TEXT("%LOCTEXT_NAMESPACE%"), *PrefixedClassName, ESearchCase::CaseSensitive);

	// Special case where where the wildcard ends with a new line
	const bool bLeadingTab = false;
	const bool bTrailingNewLine = true;
	FinalOutput = ReplaceWildcard(FinalOutput, TEXT("%ADDITIONAL_INCLUDE_DIRECTIVES%"), *MakeIncludeList(AdditionalIncludes), bLeadingTab, bTrailingNewLine);
	FinalOutput = ReplaceWildcard(FinalOutput, TEXT("%EVENTUAL_CONSTRUCTOR_DEFINITION%"), *EventualConstructorDefinition, bLeadingTab, bTrailingNewLine);
	FinalOutput = ReplaceWildcard(FinalOutput, TEXT("%ADDITIONAL_MEMBER_DEFINITIONS%"), *AdditionalMemberDefinitions, bLeadingTab, bTrailingNewLine);

	HarvestCursorSyncLocation(FinalOutput, OutSyncLocation);

	if (FFileHelper::SaveStringToFile(FinalOutput, *NewCppFileName, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
	{
		FNotificationInfo Info(FText::Format(LOCTEXT("CreateCppFileNotification", "Create file successfully: {0}"), FText::FromString(FPaths::GetCleanFilename(NewCppFileName))));
		Info.ExpireDuration = 3;
		Info.bUseLargeFont = false;
		FUPTDelegateCenter::OnRequestAddNotification.Execute(Info);
		return true;
	}

	return false;
}

const bool FCreateCodeManager::CreateBasicSourceCode(const FString& NewProjectSourcePath, const FString& NewProjectName, const FString& NewProjectRoot, TArray<FString>& OutGeneratedStartupModuleNames, TArray<FString>& OutCreatedFiles, FText& OutFailReason)
{
	//const FString GameModulePath = NewProjectSourcePath / NewProjectName;
	//const FString EditorName = NewProjectName + TEXT("Editor");

	//// MyGame.Build.cs
	//{
	//	const FString NewBuildFilename = GameModulePath / NewProjectName + TEXT(".Build.cs");
	//	TArray<FString> PublicDependencyModuleNames;
	//	PublicDependencyModuleNames.Add(TEXT("Core"));
	//	PublicDependencyModuleNames.Add(TEXT("CoreUObject"));
	//	PublicDependencyModuleNames.Add(TEXT("Engine"));
	//	PublicDependencyModuleNames.Add(TEXT("InputCore"));
	//	TArray<FString> PrivateDependencyModuleNames;
	//	if (GenerateGameModuleBuildFile(NewBuildFilename, NewProjectName, PublicDependencyModuleNames, PrivateDependencyModuleNames, OutFailReason))
	//	{
	//		OutGeneratedStartupModuleNames.Add(NewProjectName);
	//		OutCreatedFiles.Add(NewBuildFilename);
	//	}
	//	else
	//	{
	//		return false;
	//	}
	//}

	//// MyGame.Target.cs
	//{
	//	const FString NewTargetFilename = NewProjectSourcePath / NewProjectName + TEXT(".Target.cs");
	//	TArray<FString> ExtraModuleNames;
	//	ExtraModuleNames.Add(NewProjectName);
	//	if (GenerateGameModuleTargetFile(NewTargetFilename, NewProjectName, ExtraModuleNames, OutFailReason))
	//	{
	//		OutCreatedFiles.Add(NewTargetFilename);
	//	}
	//	else
	//	{
	//		return false;
	//	}
	//}

	//// MyGameEditor.Target.cs
	//{
	//	const FString NewTargetFilename = NewProjectSourcePath / EditorName + TEXT(".Target.cs");
	//	// Include the MyGame module...
	//	TArray<FString> ExtraModuleNames;
	//	ExtraModuleNames.Add(NewProjectName);
	//	if (GenerateEditorModuleTargetFile(NewTargetFilename, EditorName, ExtraModuleNames, OutFailReason))
	//	{
	//		OutCreatedFiles.Add(NewTargetFilename);
	//	}
	//	else
	//	{
	//		return false;
	//	}
	//}

	//// MyGame.h
	//{
	//	const FString NewHeaderFilename = GameModulePath / NewProjectName + TEXT(".h");
	//	TArray<FString> PublicHeaderIncludes;
	//	if (GenerateGameModuleHeaderFile(NewHeaderFilename, PublicHeaderIncludes, OutFailReason))
	//	{
	//		OutCreatedFiles.Add(NewHeaderFilename);
	//	}
	//	else
	//	{
	//		return false;
	//	}
	//}

	//// MyGame.cpp
	//{
	//	const FString NewCPPFilename = GameModulePath / NewProjectName + TEXT(".cpp");
	//	if (GenerateGameModuleCPPFile(NewCPPFilename, NewProjectName, NewProjectName, OutFailReason))
	//	{
	//		OutCreatedFiles.Add(NewCPPFilename);
	//	}
	//	else
	//	{
	//		return false;
	//	}
	//}

	return true;
}

FString FCreateCodeManager::GetTemplateFile(ECodeTemplateType CodeType, ECodeFileType FileType)
{
	FString OutFileContents;
	FString CodeTypeEnumValueName = GetEnumValueStringByUint8(TEXT("ECodeTemplateType"), uint8(CodeType));
	FString FileTypeEnumValueName = GetEnumValueStringByUint8(TEXT("ECodeFileType"), uint8(FileType));
	FString FileTypeName = FileTypeEnumValueName.ToLower().Replace(TEXT("_"), TEXT(".")) + TEXT(".template");

	FString TemplateFile = TemplateDirectory / CodeTypeEnumValueName + TEXT(".") + FileTypeName;
	if (IFileManager::Get().FileExists(*TemplateFile))
		if (FFileHelper::LoadFileToString(OutFileContents, *TemplateFile))
			return OutFileContents;

	return FString();
}

TSharedPtr<FCreateCodeManager> FCreateCodeManager::Singleton = nullptr;
#undef LOCTEXT_NAMESPACE