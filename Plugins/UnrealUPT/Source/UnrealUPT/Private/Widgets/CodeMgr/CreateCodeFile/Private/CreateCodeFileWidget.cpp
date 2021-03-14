// Fill out your copyright notice in the Description page of Project Settings.

#include "CreateCodeFileWidget.h"
#include "SlateOptMacros.h"
#include "DesktopPlatformModule.h"
//#include "GameProjectGeneration/Public/GameProjectUtils.h"
#include "IDesktopPlatform.h"
#include "Widgets/Layout/SGridPanel.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/Workflow/SWizard.h"
#include "CreateCodeDefine.h"
#include "CreateCodeManager.h"
#include "UPTStyle.h"

#define LOCTEXT_NAMESPACE "SCreateCodeFileWidget"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SCreateCodeFileWidget::Construct(const FArguments& InArgs)
{
	static TArray<TSharedPtr<ECodeTemplateType>> ALL_MODULE_TYPES = []()
	{
		TArray<TSharedPtr<ECodeTemplateType>> result;
		for (size_t i = (uint8)ECodeTemplateType::None; i < (uint8)ECodeTemplateType::UObjectClassConstructorDefinition; ++i)
		{
			TSharedPtr<ECodeTemplateType> value = MakeShareable(new ECodeTemplateType);
			*value = static_cast<ECodeTemplateType>(i);
			result.Add(value);
		}
		return result;
	}();
	static TArray<TSharedPtr<ECodeFileType>> ALL_LOADING_PHASES = []()
	{
		TArray<TSharedPtr<ECodeFileType>> result;
		for (size_t i = (uint8)ECodeFileType::None; i < (uint8)ECodeFileType::Target_cs; ++i)
		{
			TSharedPtr<ECodeFileType> value = MakeShareable(new ECodeFileType);
			*value = static_cast<ECodeFileType>(i);
			result.Add(value);
		}
		return result;
	}();
	static TSharedPtr<ECodeTemplateType> INITIALLY_SELECTED_HOST_TYPE =
		*ALL_MODULE_TYPES.FindByPredicate([](auto e) { return *e.Get() == ECodeTemplateType::ActorClass; });
	static TSharedPtr<ECodeFileType> INITIALL_SELECTED_LOADING_PHASE =
		*ALL_LOADING_PHASES.FindByPredicate([](auto e) { return *e.Get() == ECodeFileType::H; });

	PopulateAvailableModules();

	OnClickFinished = InArgs._OnClickFinished;

	H_OutputDirectory = FindSuitableModulePath();
	NewModuleName = "NewModule";
	SelectedHostType = ECodeTemplateType::ActorClass;
	SelectedLoadingPhase = ECodeFileType::H;

	CppLocation =
	{
		//MakeShareable(new ECppDirectory)
		MakeShareable(new ECppDirectory(ECppDirectory::Only_H)),
		MakeShareable(new ECppDirectory(ECppDirectory::SameDirectory)),
		MakeShareable(new ECppDirectory(ECppDirectory::PrivateDirectory)),
	};

	ChildSlot
		[
			SNew(SBorder)
			.Padding(18)
			.BorderImage(FUPTStyle::GetBrush("Docking.Tab.ContentAreaBrush"))
			[
				SNew(SVerticalBox)
				
				+ SVerticalBox::Slot()
				[
					SAssignNew(MainWizard, SWizard)
					.ShowPageList(false)
					
					.ButtonStyle(FUPTStyle::Get(), "FlatButton.Default")
					.CancelButtonStyle(FUPTStyle::Get(), "FlatButton.Default")
					.FinishButtonStyle(FUPTStyle::Get(), "FlatButton.Success")
					.ButtonTextStyle(FUPTStyle::Get(), "LargeText")
					.ForegroundColor(FUPTStyle::Get().GetSlateColor("WhiteBrush"))
					
					.CanFinish(this, &SCreateCodeFileWidget::CanFinishButtonBeClicked)
					.FinishButtonText(LOCTEXT("NewModule_FinishButtonText", "Create File"))
					.FinishButtonToolTip(
					LOCTEXT("NewModule_FinishButtonToolTip", "Creates the code files to add your new module.")
					)
					.OnCanceled(this, &SCreateCodeFileWidget::OnClickCancel)
					.OnFinished(this, &SCreateCodeFileWidget::OnClickFinish)
					.InitialPageIndex(0)
					
		// Error message at bottom
					.PageFooter()
					[
						SNew(SBorder)
						.Visibility(this, &SCreateCodeFileWidget::GetErrorLabelVisibility)
						.BorderImage(FUPTStyle::GetBrush("NewClassDialog.ErrorLabelBorder"))
						.Padding(FMargin(0, 5))
						.Content()
						[
							SNew(SHorizontalBox)
							
							+ SHorizontalBox::Slot()
							.VAlign(VAlign_Center)
							.Padding(2.f)
							.AutoWidth()
							[
								SNew(SImage)
								.Image(FUPTStyle::GetBrush("MessageLog.Warning"))
							]
							
							+ SHorizontalBox::Slot()
							.VAlign(VAlign_Center)
							[
								SNew(STextBlock)
								.Text(this, &SCreateCodeFileWidget::GetErrorLabelText)
								.TextStyle(FUPTStyle::Get(), "NewClassDialog.ErrorLabelFont")
							]
						]
					]
					
					+ SWizard::Page()
					[
						SNew(SVerticalBox)
						
			// Title
						+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(0)
						[
							SNew(STextBlock)
							.TextStyle(FUPTStyle::Get(), "NewClassDialog.PageTitle")
							.Text(LOCTEXT("NewModule_Title", "New File"))
						]
						
	// Title spacer
						+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(0, 2, 0, 8)
						[
							SNew(SSeparator)
						]
						
	// Page description and view options
						+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(0, 10)
						[
							SNew(SHorizontalBox)
							
							+ SHorizontalBox::Slot()
							.FillWidth(1.f)
							.VAlign(VAlign_Center)
							[
								SNew(STextBlock)
								.Text(
								LOCTEXT("NewModule_PageDescription", "This will add a new file to your game project and update the .uproject or .uplugin file, respectively.")
								)
							]
						]
						
	// First page: name module, module type, and loading phase
						+ SVerticalBox::Slot()
						.Padding(2, 2)
						.AutoHeight()
						.HAlign(EHorizontalAlignment::HAlign_Fill)//
						[
							SNew(SHorizontalBox)
							+ SHorizontalBox::Slot()
							.HAlign(EHorizontalAlignment::HAlign_Fill)//
							.FillWidth(1.f)//
							[
								SNew(SVerticalBox)
								
								+ SVerticalBox::Slot()
								.AutoHeight()
								.VAlign(VAlign_Center)
								[
			// Gray details panel
									SNew(SBorder)
									.BorderImage(FUPTStyle::GetBrush("DetailsView.CategoryTop"))
									.BorderBackgroundColor(FLinearColor(0.6f, 0.6f, 0.6f, 1.0f))
									.Padding(FMargin(6.0f, 4.0f, 7.0f, 4.0f))
									[
										SNew(SVerticalBox)
										
										+ SVerticalBox::Slot()
										.AutoHeight()
										.Padding(0)
										[
											SNew(SGridPanel)
											.FillColumn(1, 1.0f)
											
		// Name label
											+ SGridPanel::Slot(0, 1)
											.VAlign(VAlign_Center)
											.Padding(0, 0, 12, 0)
											[
												SNew(STextBlock)
												.TextStyle(FUPTStyle::Get(), "NewClassDialog.SelectedParentClassLabel")
												.Text(LOCTEXT("CreateModule_NameLabel", "Parent"))
											]
											
	// Name edit box
											+ SGridPanel::Slot(1, 1)
											.Padding(0.0f, 3.0f)
											.VAlign(VAlign_Center)
											[
												SNew(SBox)
												.HeightOverride(EditableTextHeight)
												[
													SAssignNew(ParentEditBox, SEditableTextBox)
												]
											]
	// Host type and loading phase
											+ SGridPanel::Slot(2, 1)
											.Padding(0.0f, 3.0f)
											.VAlign(VAlign_Center)
											[
												SNew(SBox)
												.HeightOverride(EditableTextHeight)
												[
													SNew(SHorizontalBox)
			// Host type
													+ SHorizontalBox::Slot()
													.AutoWidth()
													.Padding(6.0f, 0.0f, 0.0f, 0.0f)
													[
														SAssignNew(SelectableHostTypesComboBox, SComboBox<TSharedPtr<ECodeTemplateType>>)
														.Visibility(EVisibility::Visible)
														.ToolTipText(LOCTEXT("CreateModule_ModuleHostTypeTip", "Choose your module's loading context"))
														.OptionsSource(&ALL_MODULE_TYPES)
														.InitiallySelectedItem(INITIALLY_SELECTED_HOST_TYPE)
														.OnSelectionChanged(this, &SCreateCodeFileWidget::OnSelectedHostTypeChanged)
														.OnGenerateWidget(this, &SCreateCodeFileWidget::MakeWidgetForSelectedHostType)
														[
															SNew(STextBlock)
															.Text(this, &SCreateCodeFileWidget::GetSelectedHostTypeText)
														]
													]
													
	// Cpp Directory
													+ SHorizontalBox::Slot()
													.AutoWidth()
													.Padding(6.0f, 0.0f, 0.0f, 0.0f)
													[
														SAssignNew(CppDirectoryComboBox, SComboBox<TSharedPtr<ECppDirectory>>)
														.Visibility(EVisibility::Visible)
														.ToolTipText(LOCTEXT("CreateModule_ModuleLoadingPhaseTip", "Choose your module's loading phase"))
														.OptionsSource(&CppLocation)
														.OnSelectionChanged(this, &SCreateCodeFileWidget::OnSelectedCppDirectoryChanged)
														.OnGenerateWidget(this, &SCreateCodeFileWidget::MakeWidgetForSelectedLoadingPhase)
														[
															SNew(STextBlock)
															.Text(this, &SCreateCodeFileWidget::GetSelectedCppDirectoryText)
														]
													]
												]
												
											]
											
	// H Path label
											+ SGridPanel::Slot(0, 2)
											.VAlign(VAlign_Center)
											.Padding(0, 0, 12, 0)
											[
												SNew(STextBlock)
												.TextStyle(FUPTStyle::Get(), "NewClassDialog.SelectedParentClassLabel")
												.Text(LOCTEXT("CreateModule_PathLabel", "H Path"))
											]
	// Path edit box
											+ SGridPanel::Slot(1, 2)
											.VAlign(VAlign_Center)
											[
												SNew(SVerticalBox)
												+ SVerticalBox::Slot()
												.Padding(0)
												.AutoHeight()
												[
													SNew(SBox)
													.HeightOverride(EditableTextHeight)
													[
														SNew(SHorizontalBox)
														
														+ SHorizontalBox::Slot()
														.FillWidth(1.0f)
														[
															SAssignNew(HPathEditBox, SEditableTextBox)
														]
													]
												]
											]
	// Choose folder button
											+ SGridPanel::Slot(2, 2)
											.Padding(0.0f, 3.0f)
											.VAlign(VAlign_Center)
											[
												SNew(SBox)
												.HeightOverride(EditableTextHeight)
												[
													SNew(SHorizontalBox)
													+ SHorizontalBox::Slot()
													.FillWidth(1.f)
													.HAlign(HAlign_Fill)
													.Padding(6.0f, 0.0f, 0.0f, 0.0f)
													[
														SNew(SButton)
														.VAlign(VAlign_Center)
		//.OnClicked(this, &SCreateCodeFileWidget::HandleChooseFolderButtonClicked)
														.ToolTipText(LOCTEXT("CreateModule_ChooseFolderTooltip", "You can choose either the 'Source' folder in your project's root directory or of any plugin in the 'Plugins' folder."))
														.Text(LOCTEXT("BrowseButtonText", "Choose folder"))
													]
												]
											]
											
	//// CPP Path edit box
	//+ SGridPanel::Slot(0, 3)
	//	.VAlign(VAlign_Center)
	//	.Padding(0, 0, 12, 0)
	//	[
	//		SNew(STextBlock)
	//		.TextStyle(FUPTStyle::Get(), "NewClassDialog.SelectedParentClassLabel")
	//	.Text(LOCTEXT("CreateModule_PathLabel_Cpp", "Cpp Path"))
	//	]
	//+ SGridPanel::Slot(1, 3)
	//	.VAlign(VAlign_Center)
	//	[
	//		SNew(SVerticalBox)
	//		+ SVerticalBox::Slot()
	//	.Padding(0)
	//	.AutoHeight()
	//	[
	//		SNew(SBox)
	//		.HeightOverride(EditableTextHeight)
	//	[
	//		SNew(SHorizontalBox)
														
	//		+ SHorizontalBox::Slot()
	//	.FillWidth(1.0f)
	//	[
	//		SAssignNew(CppPathEditBox, SEditableTextBox)
	//	]
	//	]
	//	]
	//	]
	// Choose folder button
	//+SGridPanel::Slot(2, 3)
	//	.Padding(0.0f, 3.0f)
	//	.VAlign(VAlign_Center)
	//	[
	//		SNew(SBox)
	//		.HeightOverride(EditableTextHeight)
	//	[
	//		SNew(SHorizontalBox)
	//		+ SHorizontalBox::Slot()
	//	.FillWidth(1.f)
	//	.HAlign(HAlign_Fill)
	//	.Padding(6.0f, 0.0f, 0.0f, 0.0f)
	//	[
	//		SNew(SButton)
	//		.VAlign(VAlign_Center)
	//	//.OnClicked(this, &SCreateCodeFileWidget::HandleChooseFolderButtonClicked)
	//	.ToolTipText(LOCTEXT("CreateModule_ChooseFolderTooltip", "You can choose either the 'Source' folder in your project's root directory or of any plugin in the 'Plugins' folder."))
	//	.Text(LOCTEXT("BrowseButtonText", "Choose folder"))
	//	]
	//	]
	//	]
	//	]
									]
								]
							]
						]
					]
				]
			]
			
		]];

	CppDirectoryComboBox->SetSelectedItem(MakeShareable(new ECppDirectory(ECppDirectory::Only_H)));
}

void SCreateCodeFileWidget::PopulateAvailableModules()
{
	//TArray<FModuleContextInfo> CurrentModules = GameProjectUtils::GetCurrentProjectModules();
	//check(CurrentModules.Num());

	//TArray<FModuleContextInfo> CurrentPluginModules = GameProjectUtils::GetCurrentProjectPluginModules();

	//CurrentModules.Append(CurrentPluginModules);

	//AvailableModules.Reserve(CurrentModules.Num());
	//for (const FModuleContextInfo& ModuleInfo : CurrentModules)
	//{
	//	AvailableModules.Emplace(MakeShareable(new FModuleContextInfo(ModuleInfo)));
	//}

	//// TODO: discover engine module
}

FString SCreateCodeFileWidget::FindSuitableModulePath() const
{
	// Should not happen: if it does, then PopulateAvailableModules was not called before
	//check(AvailableModules.Num());

	//const FString ProjectName = FApp::GetProjectName();

	//// Find initially selected module based on simple fallback in this order..
	//// Main project module, a  runtime module
	//TSharedPtr<FModuleContextInfo> ProjectModule;
	//TSharedPtr<FModuleContextInfo> RuntimeModule;
	//for (const auto& AvailableModule : AvailableModules)
	//{
	//	if (AvailableModule->ModuleName == ProjectName)
	//	{
	//		ProjectModule = AvailableModule;
	//		break;
	//	}

	//	if (AvailableModule->ModuleType == EHostType::Runtime)
	//	{
	//		RuntimeModule = AvailableModule;
	//	}
	//}

	//TSharedPtr<FModuleContextInfo> SelectedModule;
	//if (ProjectModule.IsValid())
	//{
	//	SelectedModule = ProjectModule;
	//}
	//if (RuntimeModule.IsValid())
	//{
	//	SelectedModule = RuntimeModule;
	//}

	//if (SelectedModule != nullptr)
	//{
	//	FString foundPath = ProjectModule->ModuleSourcePath;
	//	// TODO: Use platform portable character instead of slash (this may only work on windows)
	//	foundPath.RemoveFromEnd(ProjectModule->ModuleName + FString("/"));
	//	return foundPath;
	//}
	return "";
}

bool SCreateCodeFileWidget::CanFinishButtonBeClicked() const
{
	return IsModuleNameAvailable() && !DoesModuleDirectoryAlreadyExist();
}

EVisibility SCreateCodeFileWidget::GetErrorLabelVisibility() const
{
	return GetErrorLabelText().IsEmpty() ? EVisibility::Collapsed : EVisibility::Visible;
}

FText SCreateCodeFileWidget::GetErrorLabelText() const
{
	if (!IsModuleNameAvailable())
	{
		return FText::Format(LOCTEXT("NewModule_ModuleUnavailable", "The module '{0}' is already in use."), FText::FromString(NewModuleName));
	}
	if (DoesModuleDirectoryAlreadyExist())
	{
		return FText::Format(LOCTEXT("NewModule_ModuleFolderAlreadyExists", "The target directory already contains a folder named '{0}'"), FText::FromString(NewModuleName));
	}
	return FText::GetEmpty();
}

bool SCreateCodeFileWidget::IsModuleNameAvailable() const
{
	const bool bDoesModuleNameExist = true;/* = !AvailableModules.ContainsByPredicate([this](auto e)
		{
			return e->ModuleName.Equals(NewModuleName);
		});*/

	return bDoesModuleNameExist;
}

bool SCreateCodeFileWidget::DoesModuleDirectoryAlreadyExist() const
{
	const bool bDoesModuleFolderExist = IFileManager::Get().DirectoryExists(*FPaths::Combine(H_OutputDirectory, NewModuleName));
	return bDoesModuleFolderExist;
}

void SCreateCodeFileWidget::OnClickCancel()
{
	CloseContainingWindow();
}

void SCreateCodeFileWidget::OnClickFinish()
{
	OnClickFinished.ExecuteIfBound
	(
		H_OutputDirectory,
		FString(),
		FString()
	);

	FString CppFile;
	TSharedPtr<ECppDirectory> CppDirectoryType = CppDirectoryComboBox->GetSelectedItem();
	if (!CppDirectoryType.IsValid()) return;

	FString HFile = HPathEditBox->GetText().ToString();
	switch (*CppDirectoryType.Get())
	{
		case ECppDirectory::Only_H:
			CppFile = FString();
			break;
		case  ECppDirectory::SameDirectory:
			CppFile = HFile.Replace(TEXT(".h"), TEXT(".cpp"));
			break;
		case ECppDirectory::PrivateDirectory:
			CppFile = HFile.Replace(TEXT("Public"), TEXT("Private")).Replace(TEXT(".h"), TEXT(".cpp"));
			break;

		default:
			break;
	}

	FString ParentName = ParentEditBox->GetText().ToString();
	FCreateCodeManager::Get()->CreateFile(ParentName, SelectedHostType, HFile, CppFile);

	CloseContainingWindow();
}

FText SCreateCodeFileWidget::OnGetModuleName() const
{
	return FText::FromString(NewModuleName);
}

void SCreateCodeFileWidget::OnModuleNameChanged(const FText& NewText)
{
	NewModuleName = NewText.ToString();
	UpdateInput();
}

void SCreateCodeFileWidget::OnModuleNameCommitted(const FText& NewText, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter)
	{
		if (CanFinishButtonBeClicked())
		{
			OnClickFinish();
		}
	}
}

void SCreateCodeFileWidget::OnSelectedHostTypeChanged(TSharedPtr<ECodeTemplateType> Value, ESelectInfo::Type SelectInfo)
{
	SelectedHostType = *Value.Get();
	UpdateInput();
}

TSharedRef<SWidget> SCreateCodeFileWidget::MakeWidgetForSelectedHostType(TSharedPtr<ECodeTemplateType> ForHostType) const
{
	const FText text = FText::Format(LOCTEXT("CreateModule_SelectHostTypeComboText", "{0}"), FText::FromString(FCreateCodeManager::Get()->GetCodeTemplateByEnum(*ForHostType.Get())));
	return SNew(STextBlock)
		.Text(text);
}

FText SCreateCodeFileWidget::GetSelectedHostTypeText() const
{
	return FText::Format(LOCTEXT("CreateModule_SelectedHostTypeComboText", "{0}"), FText::FromString(FCreateCodeManager::Get()->GetCodeTemplateByEnum(SelectedHostType)));
}

void SCreateCodeFileWidget::OnSelectedCppDirectoryChanged(TSharedPtr<ECppDirectory> Value, ESelectInfo::Type SelectInfo)
{
	SelectedCppDirectory = *Value.Get();
	UpdateInput();
}
void SCreateCodeFileWidget::OnSelectedLoadingPhaseChanged(TSharedPtr<ECodeFileType> Value, ESelectInfo::Type SelectInfo)
{
	SelectedLoadingPhase = *Value.Get();
	UpdateInput();
}

TSharedRef<SWidget> SCreateCodeFileWidget::MakeWidgetForSelectedLoadingPhase(TSharedPtr<ECppDirectory> CppDirectory) const
{
	const FText text = FText::Format(LOCTEXT("CreateModule_CppDirectoryComboText", "{0}"), FText::FromString(FCreateCodeManager::Get()->GetCppDirectoryByEnum(*CppDirectory.Get())));
	return SNew(STextBlock)
		.Text(text);
}

TSharedRef<SWidget> SCreateCodeFileWidget::MakeWidgetForSelectedLoadingPhase(TSharedPtr<ECodeFileType> ForLoadingPhase) const
{
	const FText text = FText::Format(LOCTEXT("CreateModule_SelectLoadingPhaseComboText", "{0}"), FText::FromString(FCreateCodeManager::Get()->GetCodeFileByEnum(*ForLoadingPhase.Get())));
	return SNew(STextBlock)
		.Text(text);
}

FText SCreateCodeFileWidget::GetSelectedCppDirectoryText() const
{
	return FText::Format(LOCTEXT("CreateModule_SelectedCppDirectoryComboText", "{0}"), FText::FromString(FCreateCodeManager::Get()->GetCppDirectoryByEnum(SelectedCppDirectory)));
}

FText SCreateCodeFileWidget::GetOutputPath() const
{
	return FText::FromString(H_OutputDirectory);
}

void SCreateCodeFileWidget::OnOutputPathChanged(const FText& NewText)
{
	H_OutputDirectory = NewText.ToString();
	UpdateInput();
}

FReply SCreateCodeFileWidget::HandleChooseFolderButtonClicked()
{
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	if (DesktopPlatform)
	{
		TSharedPtr<SWindow> ParentWindow = FSlateApplication::Get().FindWidgetWindow(AsShared());
		void* ParentWindowWindowHandle = (ParentWindow.IsValid()) ? ParentWindow->GetNativeWindow()->GetOSWindowHandle() : nullptr;

		FString FolderName;
		const FString Title = LOCTEXT("NewClassBrowseTitle", "Choose a source location").ToString();
		const bool bFolderSelected = DesktopPlatform->OpenDirectoryDialog(
			ParentWindowWindowHandle,
			Title,
			H_OutputDirectory,
			FolderName
		);

		if (bFolderSelected)
		{
			if (!FolderName.EndsWith(TEXT("/")))
			{
				FolderName += TEXT("/");
			}

			H_OutputDirectory = FolderName;
			UpdateInput();
		}
	}

	return FReply::Handled();
}

void SCreateCodeFileWidget::UpdateInput()
{
}

void SCreateCodeFileWidget::CloseContainingWindow()
{
	TSharedPtr<SWindow> ContainingWindow = FSlateApplication::Get().FindWidgetWindow(AsShared());

	if (ContainingWindow.IsValid())
	{
		ContainingWindow->RequestDestroyWindow();
	}
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE