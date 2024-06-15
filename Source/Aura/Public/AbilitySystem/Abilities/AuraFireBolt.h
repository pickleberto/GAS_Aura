// Copyright pickleberto

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraProjectileSpell.h"
#include "AuraFireBolt.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraFireBolt : public UAuraProjectileSpell
{
	GENERATED_BODY()

protected:
	virtual FString GetLevelBasedDescription(int32 Level) override;
	virtual FString GetSpellName() override;
};
