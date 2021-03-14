#include "UPTCommands.h"

#define LOCTEXT_NAMESPACE "FUPTCommands"

void FUPTCommands::RegisterCommands()
{
	UI_COMMAND(Refresh, "Refresh", "Refresh", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(FoldAllArea, "FoldAllArea", "FoldAllArea", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(ExpandAllArea, "ExpandAllArea", "ExpandAllArea", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(OpenCodeMgr, "OpenCodeMgr", "OpenCodeMgr", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE