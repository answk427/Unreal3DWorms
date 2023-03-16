// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterMovementTest.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ACharacterMovementTest::ACharacterMovementTest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	characterMovementComp = CreateDefaultSubobject<UCharacterMovementComponent>(TEXT("moveComp"));
}

// Called when the game starts or when spawned
void ACharacterMovementTest::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACharacterMovementTest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}  

