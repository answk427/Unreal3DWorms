// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WarmsPortfolioProjectile.generated.h"


class USphereComponent;
class UProjectileMovementComponent;

UCLASS(config=Game)
class AWarmsPortfolioProjectile : public AActor
{
	GENERATED_BODY()

	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* mStaticMesh;

	UPROPERTY(EditDefaultsOnly, Category= Effect)
	TSubclassOf<class AProjectileExplosionEffect> mExplosionFX;

public:
	AWarmsPortfolioProjectile();

	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Returns CollisionComp subobject **/
	USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);
	virtual void BeginPlay();

	void Explode(const FHitResult& Impact);
	void ApplyDamage(const FHitResult& Impact);
};



