#include "SDetailArea.h"
#include "SSimpleTestMainFrame.h"
#include "ModuleManager.h"
#include "PropertyEditorModule.h"
#include "SBoxPanel.h"
#include "SBox.h"
#include "SBorder.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "STestDetails"
void SDetailArea::Construct(const FArguments& InArgs)
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked< FPropertyEditorModule >("PropertyEditor");
	FDetailsViewArgs DetailsViewArgs(false, false, true, FDetailsViewArgs::HideNameArea, true);
	DetailView = PropertyModule.CreateDetailView(DetailsViewArgs);

	ChildSlot
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.VAlign(VAlign_Fill)
			[
				SNew(SBox)
				.WidthOverride(300)
				.MinDesiredWidth(300)
				.MaxDesiredWidth(300)
				[
					SNew(SBorder)
					.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
					[
						DetailView.ToSharedRef()
					]
				]
			]
		];
}

void SDetailArea::SetObject(UObject* Object)
{
	if (DetailView.IsValid())
	{
		DetailView->SetObject(Object);
	}
}

#undef LOCTEXT_NAMESPACE