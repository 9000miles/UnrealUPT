// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AppFramework/Public/Widgets/Workflow/SWizard.h"
#include "CreateCodeDefine.h"
DECLARE_DELEGATE_ThreeParams(FOnRequestNewFile, const FString& /*OutputDirectory*/, const FString& /*ClassPath*/, const FString& /* ErrorCallback */);

/**
 *
 */
class SCreateCodeFileWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SCreateCodeFileWidget)
	{}
	/** A reference to the parent window */
	SLATE_ARGUMENT(TSharedPtr<SWindow>, ParentWindow)

		SLATE_EVENT(FOnRequestNewFile, OnClickFinished)
		SLATE_END_ARGS()

		/** Constructs this widget with InArgs */
		void Construct(const FArguments& InArgs);
private:

	void PopulateAvailableModules();
	FString FindSuitableModulePath() const;

	bool CanFinishButtonBeClicked() const;
	EVisibility GetErrorLabelVisibility() const;
	FText GetErrorLabelText() const;
	bool IsModuleNameAvailable() const;
	bool DoesModuleDirectoryAlreadyExist() const;

	// Button events
	void OnClickCancel();
	void OnClickFinish();

	// Edit box: Module name
	FText OnGetModuleName() const;
	void OnModuleNameChanged(const FText& NewText);
	void OnModuleNameCommitted(const FText& NewText, ETextCommit::Type CommitType);

	// Combo box: Module type
	void OnSelectedHostTypeChanged(TSharedPtr<ECodeTemplateType> Value, ESelectInfo::Type SelectInfo);
	TSharedRef<SWidget> MakeWidgetForSelectedHostType(TSharedPtr<ECodeTemplateType> ForHostType) const;
	FText GetSelectedHostTypeText() const;

	void OnSelectedCppDirectoryChanged(TSharedPtr<ECppDirectory> Value, ESelectInfo::Type SelectInfo);
	// Combo box: Loading phase
	void OnSelectedLoadingPhaseChanged(TSharedPtr<ECodeFileType> Value, ESelectInfo::Type SelectInfo);
	TSharedRef<SWidget> MakeWidgetForSelectedLoadingPhase(TSharedPtr<ECodeFileType> ForLoadingPhase) const;
	TSharedRef<SWidget> MakeWidgetForSelectedLoadingPhase(TSharedPtr<ECppDirectory> CppDirectory) const;
	FText GetSelectedCppDirectoryText() const;

	// Edit box: Path
	FText GetOutputPath() const;
	void OnOutputPathChanged(const FText& NewText);
	FReply HandleChooseFolderButtonClicked();

	void UpdateInput();
	void CloseContainingWindow();

private:

	// Widget references
	TSharedPtr<SWizard> MainWizard;
	TSharedPtr<SEditableTextBox> ParentEditBox;
	TSharedPtr<SEditableTextBox> HPathEditBox;
	TSharedPtr<SEditableTextBox> CppPathEditBox;

	TArray<TSharedPtr<ECppDirectory>> CppLocation;

	TSharedPtr<SComboBox<TSharedPtr<ECodeTemplateType>>> SelectableHostTypesComboBox;
	TSharedPtr<SComboBox<TSharedPtr<ECodeFileType>>> SelectableLoadingPhasesComboBox;
	TSharedPtr<SComboBox<TSharedPtr<ECppDirectory>>> CppDirectoryComboBox;

	// Other data
	TArray<TSharedPtr<FModuleContextInfo>> AvailableModules;

	// Input data
	FString H_OutputDirectory;
	FString Cpp_OutputDirectory;
	FString NewModuleName;
	ECodeTemplateType SelectedHostType;
	ECodeFileType SelectedLoadingPhase;
	ECppDirectory SelectedCppDirectory;

	// Style constants
	const float EditableTextHeight = 26.f;

	FOnRequestNewFile OnClickFinished;
};
