// Copyright pickleberto


#include "AbilitySystem/Abilities/AuraSummonAbility.h"

#include "Kismet/KismetSystemLibrary.h"

TArray<FVector> UAuraSummonAbility::GetSpawnLocations()
{
	const FVector Foward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	const FVector LeftOfSpread = Foward.RotateAngleAxis(-SpawnSpread / 2.f, FVector::UpVector);
	const float DeltaSpread = SpawnSpread / NumMinions;

	TArray<FVector> SpawnLocations;

	for (int32 i = 0; i < NumMinions; i++)
	{
		const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
		FVector ChosenSpawnLocation = Location + Direction * FMath::FRandRange(MinSpawnDistance, MaxSpawnDistance);

		FHitResult Hit;
		GetWorld()->LineTraceSingleByChannel(Hit, ChosenSpawnLocation + FVector(0, 0, 400), ChosenSpawnLocation - FVector(0, 0, 400), ECollisionChannel::ECC_Visibility);
		
		if (Hit.bBlockingHit)
		{
			ChosenSpawnLocation = Hit.ImpactPoint;
		}

		SpawnLocations.Add(ChosenSpawnLocation);
	}

	return SpawnLocations;
}

TSubclassOf<APawn> UAuraSummonAbility::GetRandomMinionClass()
{
	const int32 SelectionIndex = FMath::RandRange(0, MinionClasses.Num() - 1);
	return MinionClasses[SelectionIndex];
}