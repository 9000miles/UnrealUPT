#include "SProjectTileView.h"
#include "SProjectUnit.h"
#include "STileView.h"
#include "SContextMenu.h"
#include "ProjectInfo.h"
#include "UPTDefine.h"

#define LOCTEXT_NAMESPACE "SProjectTileView"

void SProjectTileView::Construct(const FArguments& InArgs, TArray<TSharedPtr<FProjectInfo>>& Projects)
{
	ChildSlot
	[
		SNew(SBorder)
		.Padding(FMargin(10))
		[
			SAssignNew(TileView, STileView<TSharedPtr<SProjectUnit>>)
			.ItemWidth(PROJECT_UNIT_WIDTH)
			.ItemHeight(PROJECT_UNIT_HEIGHT)
			.ItemAlignment(EListItemAlignment::EvenlyDistributed)
			.ScrollbarVisibility(EVisibility::Hidden)
			.AllowOverscroll(EAllowOverscroll::No)
			.ConsumeMouseWheel(EConsumeMouseWheel::Never)
			.ListItemsSource(&ProjectUnits)
			.OnGenerateTile(this, &SProjectTileView::OnGenerateProjectTile)
			.OnContextMenuOpening(this, &SProjectTileView::MakeProjectContextMenu)
			.OnSelectionChanged(this, &SProjectTileView::OnSelectionChanged)
		]
	];

	Refresh(Projects);
}

void SProjectTileView::Refresh(TArray<TSharedPtr<FProjectInfo>> Infos)
{
	ProjectUnits.Empty();
	CreateUnits(Infos);

	TileView->RequestListRefresh();
}

void SProjectTileView::CreateUnits(TArray<TSharedPtr<FProjectInfo>> Infos)
{
	//工程根据名字排序
	Infos.Sort([](const TSharedPtr<FProjectInfo>& A, const TSharedPtr<FProjectInfo>& B) -> bool { return A->GetProjectName() < B->GetProjectName(); });

	for (TSharedPtr<FProjectInfo> Info : Infos)
	{
		check(Info.IsValid());

		TSharedRef<SProjectUnit> Unit = SNew(SProjectUnit, Info.ToSharedRef());
		ProjectUnits.Add(Unit);
	}
}

TSharedRef<ITableRow> SProjectTileView::OnGenerateProjectTile(TSharedPtr<SProjectUnit> ContentSource, const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(STableRow<TSharedPtr<FString>>, OwnerTable)
		.Padding(FMargin(2.f, 0.f, 2.f, 25.f))
		[
			ContentSource->AsShared()
		];
}

TSharedPtr<SWidget> SProjectTileView::MakeProjectContextMenu()
{
	if (CurSelectedUnit.IsValid())
		return SNew(SContextMenu, CurSelectedUnit->GetProjectInfo());

	return SNullWidget::NullWidget;
}


void SProjectTileView::OnSelectionChanged(TSharedPtr<SProjectUnit> Unit, ESelectInfo::Type Type)
{
	CurSelectedUnit = Unit;
}

#undef LOCTEXT_NAMESPCE