#include "UPTDelegateCenter.h"

FUPTDelegateCenter::FOnProjectOperation FUPTDelegateCenter::OnOpenProject;

FUPTDelegateCenter::FOnProjectOperation FUPTDelegateCenter::OnOpenIDE;

FUPTDelegateCenter::FOnProjectOperation FUPTDelegateCenter::OnGenerateSolution;

FUPTDelegateCenter::FOnProjectOperation FUPTDelegateCenter::OnShowInExplorer;

FUPTDelegateCenter::FOnProjectOperation FUPTDelegateCenter::OnClearSolution;

FUPTDelegateCenter::FOnProjectOperation FUPTDelegateCenter::OnManagedCode;

FSimpleDelegate FUPTDelegateCenter::OnRefresh;

FSimpleMulticastDelegate FUPTDelegateCenter::OnExit;

