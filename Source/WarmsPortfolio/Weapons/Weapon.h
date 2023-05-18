// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../GiveGameObjectType.h"
#include "Weapon.generated.h"

struct FWeaponData;

UCLASS()
class WARMSPORTFOLIO_API AWeapon : public AActor, public IGiveGameObjectType
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMeshComponent* mMeshComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* mStaticMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMesh* mSkeletalMesh = nullptr;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	
public:
	virtual void SetObjectType() override;
	
	virtual const UShapeComponent* GetCollider() const
	{
		return nullptr;
	}

	virtual void SetWeaponData(const FWeaponData* WeaponData);

	//������ �׸�
	virtual void DrawTrajectory();

	// Called every frame
	//virtual void Tick(float DeltaTime) override;

	void GetAdjustedStartLocation();

	virtual void Fire();
	//���ݹ�ư�� �� ������ ������ �Լ�
	virtual void Clicking(float DeltaTime);
};
