// Copyright pickleberto


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Interaction/CombatInterface.h"
#include "AuraAbilityTypes.h"

struct AuraDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);

	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance);

	AuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false);
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

	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitResistanceDef);
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
	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();

	FAggregatorEvaluateParameters EvalParams;
	EvalParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvalParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	// Get Damage Set by Caller Magnitude
	float Damage = 0.f;
	for (FGameplayTag DamageTypeTag : FAuraGameplayTags::Get().DamageTypes)
	{
		const float DamageTypeValue = Spec.GetSetByCallerMagnitude(DamageTypeTag);
		Damage += DamageTypeValue;
	}
	
	float TargetBlockChance = GetAttributeValue(DamageStatics().BlockChanceDef, EvalParams, ExecutionParams);
	const bool bBlocked = FMath::RandRange(0, 100) < TargetBlockChance;

	UAuraAbilitySystemLibrary::SetIsBlockedHit(EffectContextHandle, bBlocked);

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

	float SourceCritChance = GetAttributeValue(DamageStatics().CriticalHitChanceDef, EvalParams, ExecutionParams);
	float TargetCritResistance = GetAttributeValue(DamageStatics().CriticalHitResistanceDef, EvalParams, ExecutionParams);
	float SourceCritDmgBonus = GetAttributeValue(DamageStatics().CriticalHitDamageDef, EvalParams, ExecutionParams);

	const float CritResistanceCoefficient = CharacterClassInfo->DamageCalculationCoefficients
		->FindCurve(FName("CriticalHitResistance"), FString())->Eval(TargetCombatInterface->GetPlayerLevel());

	// Critical Hit Resistance reduces Critical Hit Chance by a certain percentage
	const float EffectiveCritChance = SourceCritChance - TargetCritResistance * CritResistanceCoefficient;
	const bool bCriticalHit = FMath::RandRange(0, 100) < EffectiveCritChance;

	UAuraAbilitySystemLibrary::SetIsCriticalHit(EffectContextHandle, bCriticalHit);

	// Double damage plus a bonus if critical hit
	if(bCriticalHit) Damage = Damage * 2.f + SourceCritDmgBonus;

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
