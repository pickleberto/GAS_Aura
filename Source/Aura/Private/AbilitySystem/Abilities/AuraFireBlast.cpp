// Copyright pickleberto


#include "AbilitySystem/Abilities/AuraFireBlast.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Actor/AuraFireBall.h"

TArray<AAuraFireBall*> UAuraFireBlast::SpawnFireBalls()
{
	TArray<AAuraFireBall*> FireBalls;

	const int32 CurNumFireBalls = GetNumFireBalls(GetAbilityLevel());
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	TArray<FRotator> Rotators = UAuraAbilitySystemLibrary::EvenlySpacedRotators(Forward, FVector::UpVector, 360.f, CurNumFireBalls);

	for (const FRotator& Rotator : Rotators)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(Location);
		SpawnTransform.SetRotation(Rotator.Quaternion());

		AAuraFireBall* FireBall = GetWorld()->SpawnActorDeferred<AAuraFireBall>(
			FireBallClass, 
			SpawnTransform, 
			GetOwningActorFromActorInfo(), 
			CurrentActorInfo->PlayerController->GetPawn(),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		FireBall->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();
		FireBall->ReturnToActor = GetAvatarActorFromActorInfo();

		FireBall->ExplosionDamageParams = MakeDamageEffectParamsFromClassDefaults();
		FireBall->SetOwner(GetAvatarActorFromActorInfo());

		FireBalls.Add(FireBall);

		FireBall->FinishSpawning(SpawnTransform);
	}

	return FireBalls;
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
