#include "SSlateTestArea.h"

void SSlateTestArea::Construct(const FArguments& InArgs)
{
}

void SSlateTestArea::SetTestWidget(TSharedRef<SWidget> Widget)
{
	ChildSlot
		[
			Widget
		];
}