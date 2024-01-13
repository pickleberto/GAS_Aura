// Copyright pickleberto


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Interaction/CombatInterface.h"

struct AuraDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);

	AuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);
	}
};

static const AuraDamageStatics& DamageStatics()
{
	static AuraDamageStatics DStatics;
	return DStatics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);

}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetSourceAbilitySystemComponent();

	AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;
	ICombatInterface* SourceCombatInterface = Cast<ICombatInterface>(SourceAvatar);
	ICombatInterface* TargetCombatInterface = Cast<ICombatInterface>(TargetAvatar);

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	FAggregatorEvaluateParameters EvalParams;
	EvalParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvalParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	// Get Damage Set by Caller Magnitude
	float Damage = Spec.GetSetByCallerMagnitude(FAuraGameplayTags::Get().Damage);
	
	float TargetBlockChance = GetAttributeValue(DamageStatics().BlockChanceDef, EvalParams, ExecutionParams);
	const bool bBlocked = FMath::RandRange(0, 100) < TargetBlockChance;

	// If block, halve the damage
	if (bBlocked) Damage /= 2.f;

	float TargetArmor = GetAttributeValue(DamageStatics().ArmorDef, EvalParams, ExecutionParams);
	float SourceArmorPenetration = GetAttributeValue(DamageStatics().ArmorPenetrationDef, EvalParams, ExecutionParams);

	const UCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(SourceAvatar);

	const float ArmorPenetrationCoefficient = CharacterClassInfo->DamageCalculationCoefficients
		->FindCurve(FName("ArmorPenetration"), FString())->Eval(SourceCombatInterface->GetPlayerLevel());

	// Armor Penetration ignores a percentage of the Target's Armor
	float EffectiveArmor = TargetArmor * ((100 - SourceArmorPenetration * ArmorPenetrationCoefficient) / 100.f);
	
	const float EffectiveArmorCoefficient = CharacterClassInfo->DamageCalculationCoefficients
		->FindCurve(FName("EffectiveArmor"), FString())->Eval(TargetCombatInterface->GetPlayerLevel());

	// Armor ignores a percentage of incoming damage
	Damage *= (100 - EffectiveArmor * EffectiveArmorCoefficient) / 100.f;

	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}

float UExecCalc_Damage::GetAttributeValue(const FGameplayEffectAttributeCaptureDefinition& AttrbuteDef, 
	const FAggregatorEvaluateParameters& EvaluationParams, const FGameplayEffectCustomExecutionParameters& ExecutionParams) const
{
	float Value = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(AttrbuteDef, EvaluationParams, Value);
	Value = FMath::Max<float>(0.f, Value);
	return Value;
}
