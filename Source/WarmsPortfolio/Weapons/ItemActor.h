// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UI/ItemStruct.h"
#include "ItemActor.generated.h"


class AWeapon;
class UNiagaraComponent;
class UWidgetComponent;

UCLASS()
class WARMSPORTFOLIO_API AItemActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemActor();
		
private:
	UPROPERTY()
	USceneComponent* DummyRoot;

	UPROPERTY(EditAnywhere)
	UMeshComponent* mItemMesh;

	UPROPERTY(EditAnywhere)
	UNiagaraComponent* mItemEffect;

	UPROPERTY(EditAnywhere)
	UShapeComponent* mCollider;

	TUniquePtr<FWPItem> mItemInfo;

	UPROPERTY(EditAnywhere)
	UWidgetComponent* mGetButtonWidget;


private:
	void AdjustEffectSize();
	void InitButtonWidget();
	void InitCollider(const AWeapon* WeaponSrc);
	void InitMeshComponent(const AWeapon* WeaponSrc);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	const FWPItem GetItemInfo() const { return *mItemInfo.Get(); }
	void SetItemInfo(const FWPItem& Item);

	void Init(const AWeapon* WeaponSrc, const FWPItem& ItemSrc);

	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};




