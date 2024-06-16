// Copyright pickleberto


#include "AbilitySystem/Abilities/AuraFireBolt.h"

#include "AuraGameplayTags.h"

FString UAuraFireBolt::GetLevelBasedDescription(int32 Level)
{
	const int32 dmg = Damage.GetValueAtLevel(Level);
	FString bolt = Level > 1 ? L"bolts" : L"bolt";
	return FString::Printf(
		TEXT(
			"Lauches <Level>%d</> %s of fire, "
			"exploding on impact and dealing "
			"<Damage>%d</> fire damage "
			"with a chance to burn."
		)
		, FMath::Min(Level, NumProjectiles), *bolt, dmg);
}

FString UAuraFireBolt::GetSpellName()
{
	return L"Fire Bolt";
}
