// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UtilityWeapon.h"
#include "NinjaRope.generated.h"

class UCableComponent;
class USphereComponent;
class UPhysicsConstraintComponent;

UCLASS()
class WARMSPORTFOLIO_API ANinjaRope : public AUtilityWeapon
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANinjaRope();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cable")
	UCableComponent* mCable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Cable")
	UStaticMeshComponent* mCableEnd;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* mRoot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USphereComponent* mCableEndCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPhysicsConstraintComponent* mPhysicsConstraint;

	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
};


