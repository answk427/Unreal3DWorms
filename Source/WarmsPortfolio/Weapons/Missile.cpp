// Fill out your copyright notice in the Description page of Project Settings.


#include "Missile.h"

#include "WPAmmo.h"
#include "Components/SphereComponent.h"

AMissile::AMissile()
{
	//mCollisionComp->OnComponentHit.AddDynamic(this, &AMissile::OnHit);

	AmmoSocket = CreateDefaultSubobject<USceneComponent>(TEXT("AmmoSocket"));
	AmmoSocket->SetRelativeLocation(FVector::ZeroVector);

	mCollisionComp->SetSimulatePhysics(false);
}

void AMissile::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("AMissile:BeginPlay()"));
}

//void AMissile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
//	FVector NormalImpulse, const FHitResult& Hit)
//{
//	//UE_LOG(LogTemp, Warning, TEXT("Called OnHit"));
//
//	// Only add impulse and destroy projectile if we hit a physics
//	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr)) //&& OtherComp->IsSimulatingPhysics())
//	{
//		Explode(Hit);
//		//UE_LOG(LogTemp, Warning, TEXT("OnHit otherActor"));
//		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
//		Destroy();
//	}
//}

inline void AMissile::PostInitializeComponents()
{
	Super::PostInitializeComponents();
		
}

void AMissile::Fire()
{
	GetCollider()->IgnoreActorWhenMoving(GetInstigator(), true);
		
	//FVector FirePos = mGunPos->GetComponentLocation();
	FVector FirePos = GetActorLocation();
	
	const FTransform SpawnTr(GetActorRotation(), FirePos);
		
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		
	//GetRootComponent()->SetWorldTransform(SpawnTr);
	
	FinishSpawning(SpawnTr, true);
	   
	Ammo->Fire();

	Destroy();
}

void AMissile::Explode(const FHitResult& Impact)
{
	
}

void AMissile::ApplyDamage(const FHitResult& Impact)
{
	
}

void AMissile::SetWeaponData(const FWeaponData* WeaponData)
{
	Super::SetWeaponData(WeaponData);

	if (GetMesh()->DoesSocketExist(AmmoSocketName))
	{
		UE_LOG(LogTemp, Warning, TEXT("AmmoSocket Exist"));
		AmmoSocket->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, AmmoSocketName);

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AmmoSocket Not Exist"));
	}

	if (AmmoClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnActorDeffered Ammo"));
		Ammo = GetWorld()->SpawnActorDeferred<AWPAmmo>(AmmoClass, FTransform(), this, (APawn*)GetOwner());
		Ammo->SetActorRelativeLocation(FVector::ZeroVector);
		Ammo->AttachToComponent(AmmoSocket, FAttachmentTransformRules::KeepRelativeTransform);
		Ammo->SetWeaponData(&mProjectileInfo);
	}

}

void AMissile::DrawTrajectory()
{
	//Super::DrawTrajectory();
	Ammo->DrawTrajectory();
}

void AMissile::Clicking(float DeltaTime)
{
	

	bAiming = true;

	Ammo->Clicking(DeltaTime);
}

AWeapon* AMissile::GetFiredWeapon()
{
	return Ammo;
}









