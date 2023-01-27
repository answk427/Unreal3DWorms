// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectileExplosionEffect.generated.h"

UCLASS()
class AProjectileExplosionEffect : public AActor
{
	GENERATED_BODY()
	

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Effect, meta = (AllowPrivateAccess))
	class UParticleSystem* mExplosionFX;

	UPROPERTY(VisibleDefaultsOnly)
	class USceneComponent* mSceneComponent;
public:	
	// Sets default values for this actor's properties
	AProjectileExplosionEffect();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
