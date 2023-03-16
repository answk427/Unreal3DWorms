// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../GiveGameObjectType.h"
#include "Weapon.generated.h"


UCLASS()
class WARMSPORTFOLIO_API AWeapon : public AActor, public IGiveGameObjectType
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* mStaticMesh;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual void SetObjectType() override; 

	// Called every frame
	//virtual void Tick(float DeltaTime) override;
};
