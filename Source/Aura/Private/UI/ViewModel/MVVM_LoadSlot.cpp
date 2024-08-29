// Copyright pickleberto


#include "UI/ViewModel/MVVM_LoadSlot.h"

void UMVVM_LoadSlot::InitializeSlot()
{
	//TODO: Check slot status based on loaded data
	SetWidgetSwitcherIndex.Broadcast(2);
}

void UMVVM_LoadSlot::SetLoadSlotName(FString InValue)
{
	UE_MVVM_SET_PROPERTY_VALUE(LoadSlotName, InValue);
}
