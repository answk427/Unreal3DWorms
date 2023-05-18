// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "DataTableStructures.generated.h"

class AProjectile;
class AWeapon;

USTRUCT()
struct FWeaponData : public FTableRowBase
{
	GENERATED_BODY()

	FWeaponData() : AttackPower(10.0f){}
	
	virtual void Print()
	{
		UE_LOG(LogTemp, Warning, TEXT("FWeaponData"));
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	FName WeaponName;
	//공격력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	float AttackPower;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon)
	TSubclassOf<AWeapon> WeaponClass;
};

USTRUCT()
struct FProjectileData : public FWeaponData
{
	GENERATED_BODY()

	FProjectileData() :
		Radius(5.0f), MaxSpeed(3000.0f), InitialSpeed(1000.f), Weight(10.0f),
		ExplodeRange(100.0f)
	{}

	virtual void Print()
	{
		UE_LOG(LogTemp, Warning, TEXT("FProjectileData"));
	}
	//Name

	//원형 콜리더 Radius
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	float Radius;

	//투사체 최대 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	float MaxSpeed;

	//투사체 초기 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	float InitialSpeed;

	//투사체 무게
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	float Weight;
	

	//폭발범위
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile)
	float ExplodeRange;
		

	/*void operator=(const FProjectileData& other)
	{
		Radius = other.Radius;
		MaxSpeed = other.MaxSpeed;
		Weight = other.Weight;
		AttackPower = other.AttackPower;
		ExplodeRange = other.ExplodeRange;
		Texture2D = other.Texture2D;
		WeaponClass = WeaponClass;
	}*/
};

//게임이 시작될 때 로딩 할 데이터테이블의 목록
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
