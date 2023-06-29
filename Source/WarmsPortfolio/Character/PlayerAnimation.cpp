// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimation.h"
#include "PlayerCharacter.h"
#include "Effects/ProjectileExplosionEffect.h"
#include "Kismet/GameplayStatics.h"
#include "Voxel/Public/VoxelWorld.h"
#include "Voxel/Public/VoxelTools/Gen/VoxelSphereTools.h"
#include <GameFramework/PawnMovementComponent.h>

UPlayerAnimation::UPlayerAnimation()
{ 
	
	static ConstructorHelpers::FObjectFinder<UAnimMontage> DieMontage
	(TEXT("AnimMontage'/Game/BluePrints/Animation/DieMontage.DieMontage'"));

	if (DieMontage.Succeeded())
	{
		UE_LOG(LogTemp, Warning, TEXT("DieMotionMontage ObjectFind Success"));
		DieMotionMontage = DieMontage.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> AimingMtg
	(TEXT("AnimMontage'/Game/BluePrints/Animation/RifleAimingMontage.RifleAimingMontage'"));

	if (AimingMtg.Succeeded())
	{
		UE_LOG(LogTemp, Warning, TEXT("AimingMontage ObjectFind Success"));
		AimingMontage = AimingMtg.Object;
	}
		
}


void UPlayerAnimation::PlayDieMotion()
{
	Montage_Play(DieMotionMontage, 1.f);
}

void UPlayerAnimation::PlayAimingMotion()
{
	if(!Montage_IsPlaying(AimingMontage))
	{
		Montage_Play(AimingMontage, 1.f);
	}
}

void UPlayerAnimation::AnimNotify_DieExplosion()
{
	APawn* Pawn = TryGetPawnOwner();

	if (!IsValid(Pawn))
		return;

	Pawn->SetHidden(true);

	//폭발 이펙트 생성
	if (mExplosionFX)
	{
		const FTransform SpawnTransform(Pawn->GetActorRotation(), Pawn->GetActorLocation());
		AProjectileExplosionEffect* const EffectActor = GetWorld()->SpawnActor<AProjectileExplosionEffect>(mExplosionFX, SpawnTransform);
	}


	TArray<AActor*> Ignores;
	UGameplayStatics::ApplyRadialDamage(GetWorld(), DieExplosionDamage, Pawn->GetActorLocation(),
		DieExplosionRange, nullptr, Ignores, Pawn);


	//폭발범위만큼 지형파괴
	TArray<AActor*> Results;

	//Find Actors in Sphere
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));


	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), Pawn->GetActorLocation(), 
		DieExplosionRange, ObjectTypes,
		AVoxelWorld::StaticClass(), TArray<AActor*>(), Results);

	for (auto Actor : Results)
	{
		AVoxelWorld* HitVoxelWorld = Cast<AVoxelWorld>(Actor);

		if (HitVoxelWorld)
		{
			UE_LOG(LogTemp, Warning, TEXT("HitVoxelWorld"));
			UVoxelSphereTools::RemoveSphere(HitVoxelWorld, Pawn->GetActorLocation(), DieExplosionRange);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No Hit VoxelWorld"));
		}
	}
	APlayerCharacter* CurrCharacter = Cast<APlayerCharacter>(Pawn);
	CurrCharacter->OnDieDelegate.Broadcast(CurrCharacter);
}


void UPlayerAnimation::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	
	auto Pawn = TryGetPawnOwner();

	if (IsValid(Pawn))
	{
		Speed = Pawn->GetVelocity().Size();
	}

	auto Character = Cast<APlayerCharacter>(Pawn);

	if (Character)
	{
		IsFalling = Character->GetMovementComponent()->IsFalling();
		Hanging = Character->Hanging;
		Vertical = Character->mVertical;
		Horizontal = Character->mHorizontal;
		TakingDamage = Character->bTakingDamage;
		Aiming = Character->bFireHoldDown;
		//UE_LOG(LogTemp, Error, TEXT("UPlayerAnimation IsFalling : %d, Vertical : %f, Horizontal : %f"), IsFalling, Vertical, Horizontal);
	}

}
