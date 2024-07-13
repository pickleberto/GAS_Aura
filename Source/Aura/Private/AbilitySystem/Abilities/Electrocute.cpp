// Copyright pickleberto


#include "AbilitySystem/Abilities/Electrocute.h"

FString UElectrocute::GetLevelBasedDescription(int32 Level)
{
	const int32 dmg = Damage.GetValueAtLevel(Level);
	FString target = Level > 1 ? L"targets" : L"target";
	return FString::Printf(
		TEXT(
			"Emits a beam of lightning, "
			"connecting with the target "
			"and <Level>%d</> additional %s, "
			"repeatedly causing <Damage>%d</> " 
			"lightning damage "
			"with a chance to stun."
		)
		, FMath::Min(Level - 1, MaxNumShockTargets), *target, dmg);
}

FString UElectrocute::GetSpellName()
{
    return L"Electrocute";
}
