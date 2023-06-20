// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldData.h"

// Sets default values
AWorldData::AWorldData()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetHidden(true);
}

// Called when the game starts or when spawned
void AWorldData::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWorldData::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

