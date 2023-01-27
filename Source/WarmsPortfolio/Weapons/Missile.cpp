// Fill out your copyright notice in the Description page of Project Settings.


#include "Missile.h"

#include "Components/SphereComponent.h"

AMissile::AMissile()
{
	mCollisionComp->OnComponentHit.AddDynamic(this, &AMissile::OnHit);
}

void AMissile::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("AMissile:BeginPlay()"));
}

void AMissile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	//UE_LOG(LogTemp, Warning, TEXT("Called OnHit"));

	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr)) //&& OtherComp->IsSimulatingPhysics())
	{
		Explode(Hit);
		//UE_LOG(LogTemp, Warning, TEXT("OnHit otherActor"));
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
		Destroy();
	}
}
