// Fill out your copyright notice in the Description page of Project Settings.


#include "DataTableStructures.h"
#include "Weapons/Projectile.h"
#include "Weapons/Weapon.h"

// Sets default values
ADataTableStructures::ADataTableStructures()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADataTableStructures::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADataTableStructures::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

