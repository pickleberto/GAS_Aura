// Copyright pickleberto


#include "AbilitySystem/Abilities/ArcaneShards.h"

FString UArcaneShards::GetLevelBasedDescription(int32 Level)
{
	const int32 dmg = Damage.GetValueAtLevel(Level);
	FString points = Level > 1 ? L"shards" : L"shard";
	return FString::Printf(
		TEXT(
			"Summon <Level>%d</> %s of  "
			"arcane energy, causing "
			"radial arcane damage of "
			"<Damage>%d</> at the shard origin."
			//"and knocking back enemies."
		)
		, FMath::Min(Level, MaxNumShards), *points, dmg);
}

FString UArcaneShards::GetSpellName()
{
    return L"Arcane Shards";
}
