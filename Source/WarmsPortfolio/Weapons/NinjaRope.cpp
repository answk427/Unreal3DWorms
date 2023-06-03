// Fill out your copyright notice in the Description page of Project Settings.


#include "NinjaRope.h"
#include "CableComponent.h"
#include "Components/SphereComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "../Character/PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
ANinjaRope::ANinjaRope()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	mRoot = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	SetRootComponent(mRoot);
		
	mCable = CreateDefaultSubobject<UCableComponent>(TEXT("NinjaRope"));
	mCable->SetupAttachment(mRoot);

	mCableEnd = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CableEndActor"));
	mCableEnd->SetupAttachment(mRoot);
	
	mCableEndCollision = CreateDefaultSubobject<USphereComponent>(TEXT("CableEndCollision"));
	//mCableEndCollision->AttachToComponent(mCableEndSphere, FAttachmentTransformRules::KeepRelativeTransform);
	mCableEndCollision->SetupAttachment(mCableEnd);

	mPhysicsConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("PhysicsConstraint"));
	mPhysicsConstraint->SetupAttachment(mRoot);

	mCableEnd->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	//�������� mesh�� ������.
	//mMeshComponent->SetVisibility(false);

	mCableEndCollision->OnComponentBeginOverlap.AddDynamic(this, &ANinjaRope::OnComponentBeginOverlap);
}

void ANinjaRope::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("NinjaRope OnComponentBeginOverlap"));
	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
	if (Player == nullptr)
		return;

	//Player->AttachToComponent(mCableEnd, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	Player->AttachToComponent(mCableEnd, FAttachmentTransformRules::KeepRelativeTransform);

	//Player->bSimGravityDisabled = false;
	//Player->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	//Player->GetCharacterMovement()->GravityScale = 0.0f;
	Player->GetCharacterMovement()->Deactivate();
	

	//mCableEndCollision->AddForce(FVector(100.f, 0.f, 0.f));
	
}

// Called when the game starts or when spawned
void ANinjaRope::BeginPlay()
{
	Super::BeginPlay();
	//mCableEndCollision->OnComponentBeginOverlap.AddDynamic(this, &ANinjaRope::OnComponentBeginOverlap);
}

// Called every frame
void ANinjaRope::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

