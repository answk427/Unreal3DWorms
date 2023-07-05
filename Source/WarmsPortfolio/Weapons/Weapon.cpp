// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "DataTableStructures.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Voxel/Public/VoxelWorld.h"
#include "Voxel/Public/VoxelTools/Gen/VoxelSphereTools.h"


// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	//mMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
		

}
//
//// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	SetObjectType();
	

	UE_LOG(LogTemp, Warning, TEXT("%s AWeapon::BeginPlay"), *GetName());
}

void AWeapon::SetWeaponData(const FWeaponData* WeaponData)
{
	InitMesh();
	
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

bool AWeapon::SetMesh(UStaticMesh* StaticMesh)
{
	if (StaticMesh == nullptr)
		return false;

	if(mMeshComponent == nullptr)
	{
		//MeshComponent�� Nullptr�� ��� UStaticMeshComponent�� ����
		mMeshComponent = NewObject<UStaticMeshComponent>(this);
		mMeshComponent->SetupAttachment(GetRootComponent());
		mMeshComponent->RegisterComponent();
	}
	else if(!Cast<UStaticMeshComponent>(mMeshComponent))
	{
		//MeshComponent�� Nullptr�� �ƴϰ� UStaticMeshComponent�� �ƴҰ�� false ����
		return false;
	}

	((UStaticMeshComponent*)mMeshComponent)->SetStaticMesh(StaticMesh);

	return true;
}

bool AWeapon::SetMesh(USkeletalMesh* SkeletalMesh)
{
	if (SkeletalMesh == nullptr)
		return false;

	if(mMeshComponent == nullptr)
	{
		//MeshComponent�� Nullptr�� ��� USkeletalMeshComponent�� ����
		mMeshComponent = NewObject<USkeletalMeshComponent>(this);
		mMeshComponent->SetupAttachment(GetRootComponent());
		mMeshComponent->RegisterComponent();
	}
	else if (!Cast<USkeletalMeshComponent>(mMeshComponent))
	{
		//MeshComponent�� Nullptr�� �ƴϰ� USkeletalMeshComponent�� �ƴҰ�� false ����
		return false;
	}
	((USkeletalMeshComponent*)mMeshComponent)->SetSkeletalMesh(SkeletalMesh);

	return true;
}

UMeshComponent* AWeapon::GetMesh()
{
	return mMeshComponent;
}

UMeshComponent* AWeapon::GetMesh() const
{
	return mMeshComponent;
}

void AWeapon::RemoveVoxelSphere(const FVector& Pos, float Radius)
{
	TArray<AActor*> Results;

	//Find Actors in Sphere
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
	

	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), Pos, Radius, ObjectTypes,
		AVoxelWorld::StaticClass(), TArray<AActor*>(), Results);

	for(auto Actor : Results)
	{
		AVoxelWorld* HitVoxelWorld = Cast<AVoxelWorld>(Actor);

		if (HitVoxelWorld)
		{
			UE_LOG(LogTemp, Warning, TEXT("HitVoxelWorld"));
			UVoxelSphereTools::RemoveSphere(HitVoxelWorld, Pos, Radius);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No Hit VoxelWorld"));
		}
	}
}

void AWeapon::InitMesh()
{
	SetMesh(mStaticMesh);
	SetMesh(mSkeletalMesh);
}

void AWeapon::PreInitializeComponents()
{
	InitMesh();
	Super::PreInitializeComponents();
}

void AWeapon::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	InitMesh();
	UE_LOG(LogTemp, Warning, TEXT("%s AWeapon OnConstruction"), *GetName());
}

void AWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (mMeshComponent != nullptr)
		mMeshComponent->SetRelativeScale3D(FVector(MeshScale, MeshScale, MeshScale));
}

AWeapon* AWeapon::GetFiredWeapon()
{
	return this;
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

