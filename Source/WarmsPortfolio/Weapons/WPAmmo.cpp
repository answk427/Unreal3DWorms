// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/WPAmmo.h"

#include "Components/SphereComponent.h"

AWPAmmo::AWPAmmo()
{
	mCollisionComp->OnComponentHit.AddDynamic(this, &AWPAmmo::OnHit);
	
	//AmmoSocket = CreateDefaultSubobject<USceneComponent>(TEXT("AmmoSocket"));
	
}

void AWPAmmo::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("AWPAmmo:BeginPlay()"));
}

void AWPAmmo::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	//UE_LOG(LogTemp, Warning, TEXT("Called OnHit"));

	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr)) //&& OtherComp->IsSimulatingPhysics())
	{
		Explode(Hit);
		//UE_LOG(LogTemp, Warning, TEXT("OnHit otherActor"));
		//OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
		Destroy();
	}
}

inline void AWPAmmo::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	//AmmoSocket->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, AmmoSocketName);
}