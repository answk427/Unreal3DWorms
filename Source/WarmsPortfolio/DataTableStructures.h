// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "DataTableStructures.generated.h"

class AProjectile;

USTRUCT()
struct FProjectileData : public FTableRowBase
{
	GENERATED_BODY()

	FProjectileData() :
		Radius(5.0f), MaxSpeed(3000.0f), Weight(10.0f),
		AttackPower(10.0f), ExplodeRange(100.0f)
	{}

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

	/*void operator=(const FProjectileData& other)
	{
		Radius = other.Radius;
		MaxSpeed = other.MaxSpeed;
		Weight = other.Weight;
		AttackPower = other.AttackPower;
		ExplodeRange = other.ExplodeRange;
		Texture2D = other.Texture2D;
		ProjectileClass = ProjectileClass;
	}*/
};

//������ ���۵� �� �ε� �� ���������̺��� ���
USTRUCT()
struct FInitDataTables : public FTableRowBase
{
	GENERATED_BODY()
	
	FInitDataTables() {}
	
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TableInfo)
	FString TableName;*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TableInfo)
	UDataTable* DataTable;
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
