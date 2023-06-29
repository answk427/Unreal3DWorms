// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WorldData.generated.h"

UCLASS()
class WARMSPORTFOLIO_API AWorldData : public AActor
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category="MapInfo")
	FVector MapMin;

	UPROPERTY(EditAnywhere, Category = "MapInfo")
	FVector MapMax;

	//The Mximum Height a Character can Spawned 
	UPROPERTY(EditAnywhere, Category = "MapInfo")
	float SpawnMaxHeight;

	UPROPERTY(EditAnywhere, Category = "MapInfo")
	float SpawnMinHeight;
	
public:	
	// Sets default values for this actor's properties
	AWorldData();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
