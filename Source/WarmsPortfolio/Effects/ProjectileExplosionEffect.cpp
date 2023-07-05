// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileExplosionEffect.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Components/PointLightComponent.h"
#include "WarmsGameModeBase.h"
#include "SoundManager.h"
#include "Engine/LevelStreaming.h"

// Sets default values
AProjectileExplosionEffect::AProjectileExplosionEffect()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//static ConstructorHelpers::FObjectFinder<UParticleSystem> bomb(TEXT("ParticleSystem'/Game/M5VFXVOL2/Particles/Explosion/Fire_Exp_00.Fire_Exp_00'"));

	mSceneComponent=CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	RootComponent = mSceneComponent;

	//if (bomb.Succeeded())
		//mExplosionFX = bomb.Object;
		
	SetActorEnableCollision(false);

	UE_LOG(LogTemp, Warning, TEXT("Explosion Constructor"));
}

// Called when the game starts or when spawned
void AProjectileExplosionEffect::BeginPlay()
{
	Super::BeginPlay();
	//FVector trLocation = GetTransform().GetLocation();
	//FVector location = GetActorLocation();
	
	//UE_LOG(LogTemp, Warning, TEXT("Effect TRLocation X:%f Y:%f Z:%f"), trLocation.X, trLocation.Y, trLocation.Z);
	//UE_LOG(LogTemp, Warning, TEXT("Effect Location X:%f Y:%f Z:%f"), location.X, location.Y, location.Z);
	if (mExplosionFX)
		UGameplayStatics::SpawnEmitterAtLocation(this, mExplosionFX, GetActorLocation(), GetActorRotation());

	auto GM = Cast<AWarmsGameModeBase>(GetWorld()->GetAuthGameMode());
	auto Sound = GM->SoundManager->GetSound(TEXT("Explosion"));
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), Sound, GetActorLocation(), 0.3f);

	Destroy();
}

// Called every frame
void AProjectileExplosionEffect::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

