// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "../DataTableStructures.h"


// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	mMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));

	
}
//
//// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	SetObjectType();
	UE_LOG(LogTemp, Warning, TEXT("AWeapon::BeginPlay"));
}

void AWeapon::SetWeaponData(const FWeaponData* WeaponData)
{
}

void AWeapon::DrawTrajectory()
{
}

void AWeapon::SetObjectType()
{
	GameObjectType = EObjectTypeName::Weapon;
}

void AWeapon::GetAdjustedStartLocation()
{
	APawn* OwnerPawn = GetInstigator();
	
	if(OwnerPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("Instigator : %s"), *OwnerPawn->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Instigator is NULL"));
	}
	
	//AShooterPlayerController* PC = MyPawn ? Cast<AShooterPlayerController>(MyPawn->Controller) : NULL;
	//AShooterAIController* AIPC = MyPawn ? Cast<AShooterAIController>(MyPawn->Controller) : NULL;
	//FVector OutStartTrace = FVector::ZeroVector;

	//if (PC)
	//{
	//	// use player's camera
	//	FRotator UnusedRot;
	//	PC->GetPlayerViewPoint(OutStartTrace, UnusedRot);

	//	// Adjust trace so there is nothing blocking the ray between the camera and the pawn, and calculate distance from adjusted start
	//	OutStartTrace = OutStartTrace + AimDir * ((GetInstigator()->GetActorLocation() - OutStartTrace) | AimDir);
	//}
	//else if (AIPC)
	//{
	//	OutStartTrace = GetMuzzleLocation();
	//}

	//return OutStartTrace;
}

void AWeapon::Fire()
{
	UE_LOG(LogTemp, Warning, TEXT("AWeapon Fire"));
}

void AWeapon::Clicking(float DeltaTime)
{
}


//
//// Called every frame
//void AWeapon::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}

