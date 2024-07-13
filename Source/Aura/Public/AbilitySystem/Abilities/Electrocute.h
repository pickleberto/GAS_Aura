// Copyright pickleberto

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraBeamSpell.h"
#include "Electrocute.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UElectrocute : public UAuraBeamSpell
{
	GENERATED_BODY()

protected:
	virtual FString GetLevelBasedDescription(int32 Level) override;
	virtual FString GetSpellName() override;
};
