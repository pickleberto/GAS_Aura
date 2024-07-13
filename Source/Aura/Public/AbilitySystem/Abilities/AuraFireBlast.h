// Copyright pickleberto

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AuraFireBlast.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraFireBlast : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()
	
protected:
	virtual FString GetLevelBasedDescription(int32 Level) override;
	virtual FString GetSpellName() override;

	int32 GetNumFireBalls(int32 Level) const;

	UPROPERTY(EditDefaultsOnly, Category = "FireBlast")
	int32 NumFireBalls = 12;
};
