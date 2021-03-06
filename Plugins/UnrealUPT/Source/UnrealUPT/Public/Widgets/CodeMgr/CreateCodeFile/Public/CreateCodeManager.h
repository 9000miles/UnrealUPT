// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CreateCodeDefine.h"

/**
 *
 */
class FCreateCodeManager
{
public:
	FCreateCodeManager();
	~FCreateCodeManager();
	static TSharedPtr<FCreateCodeManager> Get();
	void SetTemplateDirectory(const FString InTemplateDirectory);

	const bool CreateHeaderFile();
	const bool CreateCppFile();

	const bool CreateFile(const FString Parent, ECodeTemplateType TemplateType, const FString HPath, const FString CppPath);

	FString GetCodeFileByEnum(ECodeFileType Type);
	FString GetCppDirectoryByEnum(ECppDirectory Type);
	FString GetCodeTemplateByEnum(ECodeTemplateType Type);

	FString GetEnumValueStringByUint8(FString EnumName, uint8 Index);
	const bool CreateClassHeaderFile(FString NewHeaderFileName, ECodeTemplateType TemplateType, ECodeFileType FileType, const FString ParentClassName, const FString UnPrefixedClassName, TArray<FString> ClassSpecifierList, const FString& ClassProperties, const FString& ClassFunctionDeclarations);
	const bool CreateClassCPPFile(FString NewCppFileName, ECodeTemplateType Type, ECodeFileType FileType, const FString UnPrefixedClassName, TArray<FString> ClassSpecifierList, const TArray<FString>& AdditionalIncludes, const TArray<FString>& PropertyOverrides, const FString& AdditionalMemberDefinitions, FString& OutSyncLocation);
	const bool CreateBasicSourceCode(const FString& NewProjectSourcePath, const FString& NewProjectName, const FString& NewProjectRoot, TArray<FString>& OutGeneratedStartupModuleNames, TArray<FString>& OutCreatedFiles, FText& OutFailReason);
protected:
	FString GetTemplateFile(ECodeTemplateType CodeType, ECodeFileType FileType);

private:
	FString GetClassPrefixCPP(ECodeTemplateType ClassType);
	FString GetClassNameCPP(ECodeTemplateType ClassType);
	FString MakeCopyrightLine();
	FString MakeCommaDelimitedList(const TArray<FString>& InList, bool bPlaceQuotesAroundEveryElement);
	void HarvestCursorSyncLocation(FString& FinalOutput, FString& OutSyncLocation);
	FString ReplaceWildcard(const FString& Input, const FString& From, const FString& To, bool bLeadingTab, bool bTrailingNewLine);
	bool GenerateConstructorDeclaration(FString& Out, const FString& PrefixedClassName, FText& OutFailReason);
	bool GenerateConstructorDefinition(FString& Out, const FString& PrefixedClassName, const FString& PropertyOverridesStr, FText& OutFailReason);
	FString MakeIncludeList(const TArray<FString>& InList);

private:
	FString TemplateDirectory;
	static TSharedPtr<FCreateCodeManager> Singleton;
};
