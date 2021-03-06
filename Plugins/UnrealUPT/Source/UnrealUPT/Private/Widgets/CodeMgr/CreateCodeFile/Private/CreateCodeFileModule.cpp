//// Fill out your copyright notice in the Description page of Project Settings.
//
//#include "CreateCodeFileModule.h"
////#include "Logging.h"
//
//#include "Modules/ModuleManager.h"
//#include "GameProjectGeneration/Public/GameProjectUtils.h"
//#include "GeneralProjectSettings.h"
//#include "CreateCodeDefine.h"
//#include "CreateCodeManager.h"
//#include "FileHelper.h"
//#include "CreateCodeFileCommand.h"
//#include "ToolMenus/Public/ToolMenus.h"
//#include "CreateCodeFileWidget.h"
//
//#define LOCTEXT_NAMESPACE "FNewModuleTest"
//void FCreateCodeFileModule::StartupModule()
//{
//	//	FCreateCodeFileCommand::Register();
//	//
//	//	PluginCommands = MakeShareable(new FUICommandList);
//	//	PluginCommands->MapAction(
//	//		FCreateCodeFileCommand::Get().NewFile,
//	//		FExecuteAction::CreateRaw(this, &FCreateCodeFileModule::OpenNewFile),
//	//		FCanExecuteAction());
//	//
//	//	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateLambda(
//	//		[this]()
//	//		{
//	//			UToolMenu* FileMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.File");
//	//			FToolMenuSection& Section = FileMenu->FindOrAddSection("FileProject");
//	//			Section.AddMenuEntryWithCommandList(FCreateCodeFileCommand::Get().NewFile, PluginCommands);
//	//		}
//	//	));
//	//
//	//	FCreateCodeManager::Get()->SetTemplateDirectory(TEXT("G:/UnrealProjects/UnrealUPT/Plugins/FileViewport/Resources/Templates"));
//	//
//	//#if 0
//	//	FString UnPrefixedClassName;
//	//	FString FileName;
//	//	FString UnusedSyncLocation;
//	//
//	//	UnPrefixedClassName = TEXT("CreateCodeFileTest");
//	//	FileName = FString::Printf(TEXT("G:/UnrealProjects/UnrealUPT/Plugins/FileViewport/Source/CreateCodeFile/Public/%s.h"), *UnPrefixedClassName);
//	//	FCreateCodeManager::Get()->CreateClassHeaderFile
//	//	(
//	//		FileName,
//	//		ECodeTemplateType::SlateWidget,
//	//		ECodeFileType::H,
//	//		TEXT(""),
//	//		UnPrefixedClassName,
//	//		TArray<FString>(),
//	//		TEXT(""),
//	//		TEXT("")
//	//	);
//	//
//	//	FileName = FString::Printf(TEXT("G:/UnrealProjects/UnrealUPT/Plugins/FileViewport/Source/CreateCodeFile/Private/%s.cpp"), *UnPrefixedClassName);
//	//	FCreateCodeManager::Get()->CreateClassCPPFile
//	//	(
//	//		FileName,
//	//		ECodeTemplateType::SlateWidget,
//	//		ECodeFileType::Cpp,
//	//		UnPrefixedClassName,
//	//		TArray<FString>(),
//	//		TArray<FString>(),
//	//		TArray<FString>(),
//	//		TEXT(""),
//	//		UnusedSyncLocation
//	//	);
//	//#endif
//	//	UCreateCodeFilesBPLirary::CreateCodeFile();
//	//
//	{
//		//FString EnumTes = FCreateCodeManager::Get()->GetCodeFileByEnum(ECodeFileType::Build_cs);
//		//EnumTes = FCreateCodeManager::Get()->GetEnumValueStringByUint8(TEXT("ECodeFileType"), (uint8)ECodeFileType::Target_cs);
//		//EnumTes = FCreateCodeManager::Get()->GetEnumValueStringByUint8(TEXT("ECodeTemplateType"), (uint8)ECodeTemplateType::InterfaceClass_h);
//		//EnumTes = FCreateCodeManager::Get()->GetEnumValueStringByUint8(TEXT("ECodeTemplateType"), (uint8)ECodeTemplateType::InterfaceClass_h);
//
//		//FString TemplateDirectory = FString(TEXT("M:/UnrealProjects/UnrealUPT/Plugins/FileViewport/Resources/Templates"));
//		//FString TemplateFile = TEMPLATE_FILE(TemplateDirectory, ECodeTemplateType.ActorClass_h);
//		//if (IFileManager::Get().FileExists(*TemplateFile))
//		//{
//		//	FString OutFileContents;
//		//	if (FFileHelper::LoadFileToString(OutFileContents, *TemplateFile))
//		//	{
//		//		CreateClassHeaderFile
//		//		(
//		//			TEXT("M:/UnrealProjects/UnrealUPT/Source/CreateCodeTest/Public/Actor/MyTemplateNewActorClass.h"),
//		//			ECodeTemplateType::ActorClass_h,
//		//			TEXT("AActor"),
//		//			TEXT("MyTemplateNewActorClass"),
//		//			OutFileContents,
//		//			{ FString(TEXT("BlueprintType")),FString(TEXT("Blueprintable")) },
//		//			TEXT(""),
//		//			TEXT("")
//		//		);
//		//	}
//		//}
//
//		//TemplateFile = TEMPLATE_FILE(TemplateDirectory, ECodeTemplateType.EmptyClass_h);
//		//if (IFileManager::Get().FileExists(*TemplateFile))
//		//{
//		//	FString OutFileContents;
//		//	if (FFileHelper::LoadFileToString(OutFileContents, *TemplateFile))
//		//	{
//		//		CreateClassHeaderFile
//		//		(
//		//			TEXT("M:/UnrealProjects/UnrealUPT/Source/CreateCodeTest/Public/Classes/MyTemplateEmptyClass.h"),
//		//			ECodeTemplateType::EmptyClass_h,
//		//			TEXT("GameClass"),
//		//			TEXT("MyTemplateEmptyClass"),
//		//			OutFileContents,
//		//			TArray<FString>(),
//		//			TEXT(""),
//		//			TEXT("")
//		//		);
//		//	}
//		//}
//
//		//TemplateFile = TEMPLATE_FILE(TemplateDirectory, ECodeTemplateType.SlateWidget_h);
//		//if (IFileManager::Get().FileExists(*TemplateFile))
//		//{
//		//	FString OutFileContents;
//		//	if (FFileHelper::LoadFileToString(OutFileContents, *TemplateFile))
//		//	{
//		//		CreateClassHeaderFile
//		//		(
//		//			TEXT("M:/UnrealProjects/UnrealUPT/Source/CreateCodeTest/Public/Slate/MyTemplateSlateWidget.h"),
//		//			ECodeTemplateType::SlateWidget_h,
//		//			TEXT(""),
//		//			TEXT("MyTemplateSlateWidget"),
//		//			OutFileContents,
//		//			TArray<FString>(),
//		//			TEXT(""),
//		//			TEXT("")
//		//		);
//		//	}
//		//}
//
//		//TemplateFile = TEMPLATE_FILE(TemplateDirectory, ECodeTemplateType.EmptyClass_h);
//		//if (IFileManager::Get().FileExists(*TemplateFile))
//		//{
//		//	FString OutFileContents;
//		//	if (FFileHelper::LoadFileToString(OutFileContents, *TemplateFile))
//		//	{
//		//		CreateClassHeaderFile
//		//		(
//		//			TEXT("M:/UnrealProjects/UnrealUPT/Plugins/FileViewport/Source/NewModuleTest/Public/CreateCodeHelper.h"),
//		//			ECodeTemplateType::EmptyClass_h,
//		//			TEXT(""),
//		//			TEXT("CreateCodeHelper"),
//		//			OutFileContents,
//		//			TArray<FString>(),
//		//			TEXT(""),
//		//			TEXT("")
//		//		);
//		//	}
//		//}
//		///// <summary>
//		///// ///////////////////////////////////////////////////////
//		///// </summary>
//		//TemplateFile = TEMPLATE_FILE(TemplateDirectory, ECodeTemplateType.ActorClass_cpp);
//		//if (IFileManager::Get().FileExists(*TemplateFile))
//		//{
//		//	FString OutFileContents;
//		//	if (FFileHelper::LoadFileToString(OutFileContents, *TemplateFile))
//		//	{
//		//		FString UnusedSyncLocation;
//		//		CreateClassCPPFile
//		//		(
//		//			TEXT("M:/UnrealProjects/UnrealUPT/Source/CreateCodeTest/Private/Actor/MyTemplateNewActorClass.cpp"),
//		//			ECodeTemplateType::ActorClass_cpp,
//		//			TEXT("MyTemplateNewActorClass"),
//		//			OutFileContents,
//		//			TArray<FString>(),
//		//			TArray<FString>(),
//		//			TArray<FString>(),
//		//			TEXT(""),
//		//			UnusedSyncLocation
//		//		);
//		//	}
//		//}
//		//TemplateFile = TEMPLATE_FILE(TemplateDirectory, ECodeTemplateType.EmptyClass_cpp);
//		//if (IFileManager::Get().FileExists(*TemplateFile))
//		//{
//		//	FString OutFileContents;
//		//	if (FFileHelper::LoadFileToString(OutFileContents, *TemplateFile))
//		//	{
//		//		FString UnusedSyncLocation;
//		//		CreateClassCPPFile
//		//		(
//		//			TEXT("M:/UnrealProjects/UnrealUPT/Source/CreateCodeTest/Private/Classes/MyTemplateEmptyClass.cpp"),
//		//			ECodeTemplateType::EmptyClass_cpp,
//		//			TEXT("MyTemplateEmptyClass"),
//		//			OutFileContents,
//		//			TArray<FString>(),
//		//			TArray<FString>(),
//		//			TArray<FString>(),
//		//			TEXT(""),
//		//			UnusedSyncLocation
//		//		);
//		//	}
//		//}
//		//TemplateFile = TEMPLATE_FILE(TemplateDirectory, ECodeTemplateType.SlateWidget_cpp);
//		//if (IFileManager::Get().FileExists(*TemplateFile))
//		//{
//		//	FString OutFileContents;
//		//	if (FFileHelper::LoadFileToString(OutFileContents, *TemplateFile))
//		//	{
//		//		FString UnusedSyncLocation;
//		//		CreateClassCPPFile
//		//		(
//		//			TEXT("M:/UnrealProjects/UnrealUPT/Source/CreateCodeTest/Private/Slate/MyTemplateSlateWidget.cpp"),
//		//			ECodeTemplateType::SlateWidget_cpp,
//		//			TEXT("MyTemplateSlateWidget"),
//		//			OutFileContents,
//		//			TArray<FString>(),
//		//			TArray<FString>(),
//		//			TArray<FString>(),
//		//			TEXT(""),
//		//			UnusedSyncLocation
//		//		);
//		//	}
//		//}
//		//TemplateFile = TEMPLATE_FILE(TemplateDirectory, ECodeTemplateType.EmptyClass_cpp);
//		//if (IFileManager::Get().FileExists(*TemplateFile))
//		//{
//		//	FString OutFileContents;
//		//	if (FFileHelper::LoadFileToString(OutFileContents, *TemplateFile))
//		//	{
//		//		FString UnusedSyncLocation;
//		//		CreateClassCPPFile
//		//		(
//		//			TEXT("M:/UnrealProjects/UnrealUPT/Plugins/FileViewport/Source/NewModuleTest/Private/CreateCodeHelper.cpp"),
//		//			ECodeTemplateType::EmptyClass_cpp,
//		//			TEXT("CreateCodeHelper"),
//		//			OutFileContents,
//		//			TArray<FString>(),
//		//			TArray<FString>(),
//		//			TArray<FString>(),
//		//			TEXT(""),
//		//			UnusedSyncLocation
//		//		);
//		//	}
//		//}
//	}
//}
//void FCreateCodeFileModule::ShutdownModule()
//{
//}
//
//void FCreateCodeFileModule::OpenNewFile()
//{
//	const FVector2D WindowSize(940, 380); // 480
//	const FText WindowTitle = LOCTEXT("NewFile_Title", "New Code File");
//
//	TSharedRef<SWindow> AddCodeWindow =
//		SNew(SWindow)
//		.Title(WindowTitle)
//		.ClientSize(WindowSize)
//		.SizingRule(ESizingRule::FixedSize)
//		.SupportsMinimize(false).SupportsMaximize(false);
//
//	auto NewModuleDialog =
//		SNew(SCreateCodeFileWidget)
//		.ParentWindow(AddCodeWindow)
//		.OnClickFinished(FOnRequestNewFile::CreateLambda([](auto Directory, auto ModuleDescriptor, auto ErrorCallback) {}));
//	AddCodeWindow->SetContent(NewModuleDialog);
//	FSlateApplication::Get().AddWindow(AddCodeWindow);
//}
//
//#undef LOCTEXT_NAMESPACE
//
//IMPLEMENT_MODULE(FCreateCodeFileModule, CreateCodeFile);
///// <summary>
/////
///// </summary>
///// <param name="Type"></param>
///// <returns></returns>