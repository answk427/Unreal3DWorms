// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GraveActor.generated.h"

UCLASS()
class WARMSPORTFOLIO_API AGraveActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGraveActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* GraveMesh;

	UPROPERTY()
	UMaterialInterface* GraveMaterial;

	void SetMeshColor(FColor Color);

	FName ColorParamName = TEXT("ColorParam");
};
