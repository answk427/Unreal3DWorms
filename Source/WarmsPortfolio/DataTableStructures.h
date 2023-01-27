// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "Weapons/Projectile.h"
#include "DataTableStructures.generated.h"

USTRUCT()
struct FProjectileData : public FTableRowBase
{
	GENERATED_BODY()

	FProjectileData(){}

	//Name

	//���� �ݸ��� Radius
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	float Radius;

	//����ü �ִ� �ӵ�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	float MaxSpeed;

	//����ü ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	float Weight;

	//���ݷ�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	float AttackPower;

	//���߹���
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	float ExplodeRange;

	UPROPERTY()
	TSubclassOf<UTexture2D> Texture2D;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	TSubclassOf<AProjectile> ProjectileClass;
};




UCLASS()
class WARMSPORTFOLIO_API ADataTableStructures : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADataTableStructures();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
