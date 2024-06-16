// Copyright pickleberto


#include "AbilitySystem/Abilities/AuraGameplayAbility.h"

#include "AbilitySystem/AuraAttributeSet.h"

FString UAuraGameplayAbility::GetDescription(int32 Level)
{
    return GetFullDescription(GetSpellName(), Level);
}

FString UAuraGameplayAbility::GetNextLevelDescription(int32 Level)
{
    return GetFullDescription(L"Next Level:", Level);
}

FString UAuraGameplayAbility::GetLockedDescription(int32 Level)
{
    return FString::Printf(TEXT("<Default>Spell Locked Until Level: </><Level>%d</>"), Level);
}

FString UAuraGameplayAbility::GetLevelBasedDescription(int32 Level)
{
    return L"Description";
}

FString UAuraGameplayAbility::GetSpellName()
{
    return L"Spell Name";
}

float UAuraGameplayAbility::GetManaCost(float InLevel) const
{
    float ManaCost = 0.f;
    if (const UGameplayEffect* CostEffect = GetCostGameplayEffect())
    {
        for (FGameplayModifierInfo Mod : CostEffect->Modifiers)
        {
            if (Mod.Attribute == UAuraAttributeSet::GetManaAttribute())
            {
                Mod.ModifierMagnitude.GetStaticMagnitudeIfPossible(InLevel, ManaCost);
                break;
            }
        }
    }

    return ManaCost;
}

float UAuraGameplayAbility::GetCooldown(float InLevel) const
{
    float Cooldown = 0.f;

    if (const UGameplayEffect* CooldownEffect = GetCooldownGameplayEffect())
    {
        CooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(InLevel, Cooldown);
    }
    
    return Cooldown;
}

FString UAuraGameplayAbility::GetFullDescription(FString Title, int32 Level)
{
    return FString::Printf(
        TEXT(
            "<Title>%s</>\n\n"
            "<Small>Level:</> <Level>%d</>\n"
            "<Small>Mana Cost:</> <ManaCost>%.1f</>\n"
            "<Small>Cooldown:</> <Cooldown>%.1f</>\n"
            "\n"
            "%s\n\n"
        ),
        *Title, Level, FMath::Abs(GetManaCost(Level)), GetCooldown(Level), *GetLevelBasedDescription(Level));
}
