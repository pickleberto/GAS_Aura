// Copyright pickleberto


#include "UI/ViewModel/MVVM_LoadSlot.h"

void UMVVM_LoadSlot::InitializeSlot()
{
	const int32 WidgetSwitcherIndex = SlotStatus.GetValue();
	SetWidgetSwitcherIndex.Broadcast(WidgetSwitcherIndex);
}

void UMVVM_LoadSlot::SetLoadSlotName(FString InValue)
{
	UE_MVVM_SET_PROPERTY_VALUE(LoadSlotName, InValue);
}

void UMVVM_LoadSlot::SetPlayerName(FString InValue)
{
	UE_MVVM_SET_PROPERTY_VALUE(PlayerName, InValue);
}

void UMVVM_LoadSlot::SetMapName(FString InValue)
{
	UE_MVVM_SET_PROPERTY_VALUE(MapName, InValue);
}
