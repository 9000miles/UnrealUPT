#include "SSimpleTestTreeView.h"

template<typename ItemType>
void SSimpleTestTreeView<ItemType>::Construct(const FArguments& InArgs)
{
	this->OnGenerateRow = InArgs._OnGenerateRow;
	this->OnItemScrolledIntoView = InArgs._OnItemScrolledIntoView;
	this->OnGetChildren = InArgs._OnGetChildren;
	this->OnSetExpansionRecursive = InArgs._OnSetExpansionRecursive;
	this->TreeItemsSource = InArgs._TreeItemsSource;

	this->OnContextMenuOpening = InArgs._OnContextMenuOpening;
	this->OnDoubleClick = InArgs._OnMouseButtonDoubleClick;
	this->OnSelectionChanged = InArgs._OnSelectionChanged;
	this->OnExpansionChanged = InArgs._OnExpansionChanged;
	this->SelectionMode = InArgs._SelectionMode;

	this->bClearSelectionOnClick = InArgs._ClearSelectionOnClick;

	// Check for any parameters that the coder forgot to specify.
	FString ErrorString;
	{
		if (!this->OnGenerateRow.IsBound())
		{
			ErrorString += TEXT("Please specify an OnGenerateRow. \n");
		}

		if (this->TreeItemsSource == NULL)
		{
			ErrorString += TEXT("Please specify a TreeItemsSource. \n");
		}

		if (!this->OnGetChildren.IsBound())
		{
			ErrorString += TEXT("Please specify an OnGetChildren. \n");
		}
	}

	if (ErrorString.Len() > 0)
	{
		// Let the coder know what they forgot
		this->ChildSlot
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(FText::FromString(ErrorString))
			];
	}
	else
	{
		// Make the TableView
		this->ConstructChildren(0, InArgs._ItemHeight, EListItemAlignment::LeftAligned, InArgs._HeaderRow, InArgs._ExternalScrollbar, InArgs._OnTreeViewScrolled);
	}
}