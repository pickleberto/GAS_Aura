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
		const FVector ChosenLocation = Location + Direction * FMath::FRandRange(MinSpawnDistance, MaxSpawnDistance);

		SpawnLocations.Add(ChosenLocation);

		//Debug

		UKismetSystemLibrary::DrawDebugArrow(GetAvatarActorFromActorInfo(), Location,
			Location + Direction * MaxSpawnDistance, 4.f, FLinearColor::Green, 3.f);

		DrawDebugSphere(GetWorld(), Location + Direction * MinSpawnDistance, 5.f, 12, FColor::Red, false, 3.f);
		DrawDebugSphere(GetWorld(), ChosenLocation, 18.f, 12, FColor::Cyan, false, 3.f);
		DrawDebugSphere(GetWorld(), Location + Direction * MaxSpawnDistance, 5.f, 12, FColor::Red, false, 3.f);
	}

	return SpawnLocations;
}