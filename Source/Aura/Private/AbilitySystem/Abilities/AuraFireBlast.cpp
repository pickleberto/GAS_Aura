// Copyright pickleberto


#include "AbilitySystem/Abilities/AuraFireBlast.h"

TArray<AAuraFireBall*> UAuraFireBlast::SpawnFireBalls()
{
	return TArray<AAuraFireBall*>();
}

FString UAuraFireBlast::GetLevelBasedDescription(int32 Level)
{
	const int32 dmg = Damage.GetValueAtLevel(Level);
	return FString::Printf(
		TEXT(
			"Lauches <Level>%d</> fire balls in all directions, "
			"each coming back and exploding upon return, causing "
			"<Damage>%d</> radial fire damage "
			"with a chance to burn."
		)
		, GetNumFireBalls(Level), dmg);
}

FString UAuraFireBlast::GetSpellName()
{
	return L"Fire Blast";
}

int32 UAuraFireBlast::GetNumFireBalls(int32 Level) const
{
	return FMath::Min(Level + 7, NumFireBalls);
}
