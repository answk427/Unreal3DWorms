// Copyright Epic Games, Inc. All Rights Reserved.

#include "WarmsPortfolioProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Effects/ProjectileExplosionEffect.h"

AWarmsPortfolioProjectile::AWarmsPortfolioProjectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AWarmsPortfolioProjectile::OnHit);		// set up a notification for when this component hits something blocking
	

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;
	

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
	mStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));

	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> staticMesh(
		TEXT("StaticMesh'/Game/FirstPerson/Meshes/FirstPersonProjectileMesh.FirstPersonProjectileMesh'")
	);
	if (staticMesh.Succeeded())
	{
		mStaticMesh->SetStaticMesh(staticMesh.Object);
		UE_LOG(LogTemp, Warning, TEXT("Projectile Mesh Succeeded"));
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("Projectile Mesh Failed"));
		
	mStaticMesh->SetupAttachment(RootComponent);
	mStaticMesh->SetWorldScale3D(FVector(0.1f, 0.1f, 0.1f));
	//SetActorScale3D(FVector(0.1f, 0.1f, 0.1f));

	AProjectileExplosionEffect* bombEffect = CreateDefaultSubobject<AProjectileExplosionEffect>(TEXT("ProjectileExplosionEffect"));
	UClass* uBombEffect = bombEffect->GetClass();
	mExplosionFX = uBombEffect;

	CollisionComp->BodyInstance.bNotifyRigidBodyCollision = false;

}

void AWarmsPortfolioProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//UE_LOG(LogTemp, Warning, TEXT("AWarmsPortfolioProjectile OnHit"));
	
	// Only add impulse and destroy projectile if we hit a physics
	//if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))// && OtherComp->IsSimulatingPhysics())
	//{
	//	Explode(Hit);
	//	OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
	//	Destroy();
	//}
}

void AWarmsPortfolioProjectile::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	UE_LOG(LogTemp, Warning, TEXT("Projectile EndPlay"));
}

void AWarmsPortfolioProjectile::BeginPlay()
{
	Super::BeginPlay();
	//UE_LOG(LogTemp, Warning, TEXT("Projectile BeginPlay"));
	FVector trLocation = GetTransform().GetLocation();
	FVector location = GetActorLocation();

	//UE_LOG(LogTemp, Warning, TEXT("Projectile TRLocation X:%f Y:%f Z:%f"), trLocation.X, trLocation.Y, trLocation.Z);
	//UE_LOG(LogTemp, Warning, TEXT("Projectile Location X:%f Y:%f Z:%f"), location.X, location.Y, location.Z);
}

void AWarmsPortfolioProjectile::Explode(const FHitResult& Impact)
{
	// effects and damage origin shouldn't be placed inside mesh at impact point
	const FVector NudgedImpactLocation = Impact.ImpactPoint + Impact.ImpactNormal * 10.0f;

	/*if (WeaponConfig.ExplosionDamage > 0 && WeaponConfig.ExplosionRadius > 0 && WeaponConfig.DamageType)
	{
		UGameplayStatics::ApplyRadialDamage(this, WeaponConfig.ExplosionDamage, NudgedImpactLocation, WeaponConfig.ExplosionRadius, WeaponConfig.DamageType, TArray<AActor*>(), this, MyController.Get());
	}*/
	DrawDebugCapsule(GetWorld(), Impact.ImpactPoint, 50.0f,
		50.0f, Impact.ImpactNormal.Rotation().Quaternion(), FColor::Red, false, 2.f);

	if (mExplosionFX)
	{
		//FTransform const SpawnTransform(Impact.ImpactNormal.Rotation(), NudgedImpactLocation);
		FTransform const SpawnTransform(Impact.ImpactNormal.Rotation(), Impact.ImpactPoint);

		UE_LOG(LogTemp, Warning, TEXT("ImpactPoint X:%f Y:%f Z:%f"), Impact.ImpactPoint.X, Impact.ImpactPoint.Y, Impact.ImpactPoint.Z);

		FVector spawnPos = SpawnTransform.GetLocation();
				
				
		AProjectileExplosionEffect* const EffectActor = GetWorld()->SpawnActorDeferred<AProjectileExplosionEffect>(mExplosionFX, SpawnTransform);
		if (EffectActor)
		{
			UE_LOG(LogTemp, Warning, TEXT("SpawnTransform X:%f Y:%f Z:%f"), spawnPos.X,
				spawnPos.Y,
				spawnPos.Z);
			

			//EffectActor->SurfaceHit = Impact;
			UGameplayStatics::FinishSpawningActor(EffectActor, SpawnTransform);
						
		}
		ApplyDamage(Impact);
	}

	//bExploded = true;
}

void AWarmsPortfolioProjectile::ApplyDamage(const FHitResult& Impact)
{
	//Super::TakeDamage()
		//UGameplayStatics::ApplyDamage()
	UGameplayStatics::ApplyRadialDamage(GetWorld(), 12.12f, Impact.ImpactPoint, 300.0f
		, nullptr, TArray<AActor*>(), this, GetInstigatorController(), false);

	//DrawDebugSphere(GetWorld(), Impact.ImpactPoint, 100.0f, 0, FColor::Blue);
	DrawDebugCapsule(GetWorld(), Impact.ImpactPoint, 300.0f,
		300.0f, Impact.ImpactNormal.Rotation().Quaternion(), FColor::Blue, false, 2.f);
}
