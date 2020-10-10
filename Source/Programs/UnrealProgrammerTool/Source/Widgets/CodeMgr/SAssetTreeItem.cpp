#include "SAssetTreeItem.h"
#include "Widgets/Images/SImage.h"
#include "EditorStyleSet.h"
#include "EditorFontGlyphs.h"

#include "Widgets/Text/SInlineEditableTextBlock.h"
#include "SBorder.h"
#include "SBoxPanel.h"
#include "FTreeItem.h"
#include "FAssetDragDropOp.h"

#define LOCTEXT_NAMESPACE "SAssetTreeItem"

//////////////////////////
// SAssetTreeItem
//////////////////////////

void SAssetTreeItem::Construct(const FArguments& InArgs)
{
	TreeItem = InArgs._TreeItem;
	OnNameChanged = InArgs._OnNameChanged;
	OnVerifyNameChanged = InArgs._OnVerifyNameChanged;
	OnAssetsOrPathsDragDropped = InArgs._OnAssetsOrPathsDragDropped;
	OnFilesDragDropped = InArgs._OnFilesDragDropped;
	IsItemExpanded = InArgs._IsItemExpanded;
	bDraggedOver = false;

	FolderOpenBrush = FEditorStyle::GetBrush("ContentBrowser.AssetTreeFolderOpen");
	FolderClosedBrush = FEditorStyle::GetBrush("ContentBrowser.AssetTreeFolderClosed");
	FolderOpenCodeBrush = FEditorStyle::GetBrush("ContentBrowser.AssetTreeFolderOpenCode");
	FolderClosedCodeBrush = FEditorStyle::GetBrush("ContentBrowser.AssetTreeFolderClosedCode");
	FolderDeveloperBrush = FEditorStyle::GetBrush("ContentBrowser.AssetTreeFolderDeveloper");

	FolderType = EFolderType::Normal;
	//if (ContentBrowserUtils::IsDevelopersFolder(InArgs._TreeItem->FolderPath))
	//{
	//	FolderType = EFolderType::Developer;
	//}
	//else if (ContentBrowserUtils::IsClassPath/*IsClassRootDir*/(InArgs._TreeItem->FolderPath))
	//{
	//	FolderType = EFolderType::Code;
	//}

	bool bIsRoot = !InArgs._TreeItem->Parent.IsValid();

	ChildSlot
		[
			SNew(SBorder)
			.BorderImage(this, &SAssetTreeItem::GetBorderImage)
			.Padding(FMargin(0, bIsRoot ? 3 : 0, 0, 0))	// For root items in the tree, give them a little breathing room on the top
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(0, 0, 2, 0)
				.VAlign(VAlign_Center)
				[
					// Folder Icon
					SNew(SImage)
					.Image(this, &SAssetTreeItem::GetFolderIcon)
					.ColorAndOpacity(this, &SAssetTreeItem::GetFolderColor)
				]

				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				[
					SAssignNew(InlineRenameWidget, SInlineEditableTextBlock)
					.Text(this, &SAssetTreeItem::GetNameText)
					.ToolTipText(this, &SAssetTreeItem::GetToolTipText)
					.Font(InArgs._FontOverride.IsSet() ? InArgs._FontOverride : FEditorStyle::GetFontStyle(bIsRoot ? "ContentBrowser.SourceTreeRootItemFont" : "ContentBrowser.SourceTreeItemFont"))
					.HighlightText(InArgs._HighlightText)
					.OnTextCommitted(this, &SAssetTreeItem::HandleNameCommitted)
					.OnVerifyTextChanged(this, &SAssetTreeItem::VerifyNameChanged)
					.IsSelected(InArgs._IsSelected)
					.IsReadOnly(this, &SAssetTreeItem::IsReadOnly)
				]
			]
		];

	if (InlineRenameWidget.IsValid())
	{
		EnterEditingModeDelegateHandle = TreeItem.Pin()->OnRenamedRequestEvent.AddSP(InlineRenameWidget.Get(), &SInlineEditableTextBlock::EnterEditingMode);
	}
}

SAssetTreeItem::~SAssetTreeItem()
{
	if (InlineRenameWidget.IsValid())
	{
		TreeItem.Pin()->OnRenamedRequestEvent.Remove(EnterEditingModeDelegateHandle);
	}
}

bool SAssetTreeItem::ValidateDragDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent, bool& OutIsKnownDragOperation) const
{
	OutIsKnownDragOperation = false;

	TSharedPtr<FTreeItem> TreeItemPinned = TreeItem.Pin();
	return TreeItemPinned.IsValid();// && DragDropHandler::ValidateDragDropOnAssetFolder(MyGeometry, DragDropEvent, TreeItemPinned->FolderPath, OutIsKnownDragOperation);
}

void SAssetTreeItem::OnDragEnter(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
	ValidateDragDrop(MyGeometry, DragDropEvent, bDraggedOver); // updates bDraggedOver
}

void SAssetTreeItem::OnDragLeave(const FDragDropEvent& DragDropEvent)
{
	TSharedPtr<FDragDropOperation> Operation = DragDropEvent.GetOperation();
	if (Operation.IsValid())
	{
		Operation->SetCursorOverride(TOptional<EMouseCursor::Type>());

		if (Operation->IsOfType<FAssetDragDropOp>())
		{
			TSharedPtr<FAssetDragDropOp> DragDropOp = StaticCastSharedPtr<FAssetDragDropOp>(Operation);
			DragDropOp->ResetToDefaultToolTip();
		}
	}

	bDraggedOver = false;
}

FReply SAssetTreeItem::OnDragOver(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
	ValidateDragDrop(MyGeometry, DragDropEvent, bDraggedOver); // updates bDraggedOver
	return (bDraggedOver) ? FReply::Handled() : FReply::Unhandled();
}

FReply SAssetTreeItem::OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
	if (ValidateDragDrop(MyGeometry, DragDropEvent, bDraggedOver)) // updates bDraggedOver
	{
		bDraggedOver = false;

		TSharedPtr<FDragDropOperation> Operation = DragDropEvent.GetOperation();
		if (!Operation.IsValid())
		{
			return FReply::Unhandled();
		}

		if (Operation->IsOfType<FAssetDragDropOp>())
		{
			TSharedPtr<FAssetDragDropOp> DragDropOp = StaticCastSharedPtr<FAssetDragDropOp>(DragDropEvent.GetOperation());
			OnAssetsOrPathsDragDropped.ExecuteIfBound(DragDropOp->GetAssets(), DragDropOp->GetAssetPaths(), TreeItem.Pin());
			return FReply::Handled();
		}

		if (Operation->IsOfType<FExternalDragOperation>())
		{
			TSharedPtr<FExternalDragOperation> DragDropOp = StaticCastSharedPtr<FExternalDragOperation>(DragDropEvent.GetOperation());
			OnFilesDragDropped.ExecuteIfBound(DragDropOp->GetFiles(), TreeItem.Pin());
			return FReply::Handled();
		}
	}

	if (bDraggedOver)
	{
		// We were able to handle this operation, but could not due to another error - still report this drop as handled so it doesn't fall through to other widgets
		bDraggedOver = false;
		return FReply::Handled();
	}

	return FReply::Unhandled();
}

void SAssetTreeItem::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	LastGeometry = AllottedGeometry;
}

bool SAssetTreeItem::VerifyNameChanged(const FText& InName, FText& OutError) const
{
	if (TreeItem.IsValid())
	{
		TSharedPtr<FTreeItem> TreeItemPtr = TreeItem.Pin();
		if (OnVerifyNameChanged.IsBound())
		{
			return OnVerifyNameChanged.Execute(InName.ToString(), OutError, TreeItemPtr->FolderPath);
		}
	}

	return true;
}

void SAssetTreeItem::HandleNameCommitted(const FText& NewText, ETextCommit::Type CommitInfo)
{
	if (TreeItem.IsValid())
	{
		TSharedPtr<FTreeItem> TreeItemPtr = TreeItem.Pin();

		if (TreeItemPtr->bNamingFolder)
		{
			TreeItemPtr->bNamingFolder = false;

			const FString OldPath = TreeItemPtr->FolderPath;
			FString Path;
			TreeItemPtr->FolderPath.Split(TEXT("/"), &Path, NULL, ESearchCase::CaseSensitive, ESearchDir::FromEnd);
			TreeItemPtr->DisplayName = NewText;
			TreeItemPtr->FolderName = NewText.ToString();
			TreeItemPtr->FolderPath = Path + TEXT("/") + NewText.ToString();

			FVector2D MessageLoc;
			MessageLoc.X = LastGeometry.AbsolutePosition.X;
			MessageLoc.Y = LastGeometry.AbsolutePosition.Y + LastGeometry.Size.Y * LastGeometry.Scale;

			OnNameChanged.ExecuteIfBound(TreeItemPtr, OldPath, MessageLoc, CommitInfo);
		}
	}
}

bool SAssetTreeItem::IsReadOnly() const
{
	if (TreeItem.IsValid())
	{
		return !TreeItem.Pin()->bNamingFolder;
	}
	else
	{
		return true;
	}
}

bool SAssetTreeItem::IsValidAssetPath() const
{
	if (TreeItem.IsValid())
	{
		// The classes folder is not a real path
		//return !ContentBrowserUtils::IsClassPath(TreeItem.Pin()->FolderPath);
		return true;
	}
	else
	{
		return false;
	}
}

const FSlateBrush* SAssetTreeItem::GetFolderIcon() const
{
	switch (FolderType)
	{
	case EFolderType::Code:
		return (IsItemExpanded.Get()) ? FolderOpenCodeBrush : FolderClosedCodeBrush;

	case EFolderType::Developer:
		return FolderDeveloperBrush;

	default:
		return (IsItemExpanded.Get()) ? FolderOpenBrush : FolderClosedBrush;
	}
}

FSlateColor SAssetTreeItem::GetFolderColor() const
{
	if (TreeItem.IsValid())
	{
		const TSharedPtr<FLinearColor> Color = MakeShareable(new FLinearColor( FLinearColor::Blue));
		//const TSharedPtr<FLinearColor> Color = ContentBrowserUtils::LoadColor(TreeItem.Pin()->FolderPath);
		if (Color.IsValid())
		{
			return *Color.Get();
		}
	}
	return FSlateColor(FLinearColor::Green);
}

FText SAssetTreeItem::GetNameText() const
{
	TSharedPtr<FTreeItem> TreeItemPin = TreeItem.Pin();
	if (TreeItemPin.IsValid())
	{
		return TreeItemPin->DisplayName;
	}
	else
	{
		return FText();
	}
}

FText SAssetTreeItem::GetToolTipText() const
{
	TSharedPtr<FTreeItem> TreeItemPin = TreeItem.Pin();
	if (TreeItemPin.IsValid())
	{
		return FText::FromString(TreeItemPin->FolderPath);
	}
	else
	{
		return FText();
	}
}

const FSlateBrush* SAssetTreeItem::GetBorderImage() const
{
	return bDraggedOver ? FEditorStyle::GetBrush("Menu.Background") : FEditorStyle::GetBrush("NoBorder");
}
