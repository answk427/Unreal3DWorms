// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class USceneComponent;
class AProjectile;
class AWarmsPortfolioProjectile;
class UWidgetComponent;
class UStatComponent;
class UDataTable;
struct FInventory;

UCLASS()
class WARMSPORTFOLIO_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* mSpringArm;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* mCamera;

	UPROPERTY(VisibleDefaultsOnly)
	USceneComponent* mGunPos;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Projectile")
	TSubclassOf<AProjectile> mProjectile;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Projectile")
	TSubclassOf<AWarmsPortfolioProjectile> mProjectileWarms;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Projectile")
	TSubclassOf<AWarmsPortfolioProjectile> mProjectileWarms2;


	//���� ���� ������Ʈ
	UPROPERTY(VisibleAnywhere)
	UStatComponent* mStatComponent;
	//Hp Bar ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	UWidgetComponent* mHpBarWidget;

	//�� ĳ���͸��� ������ �ִ� �κ��丮
	TSharedPtr<FInventory> mInventory;
	//�κ��丮 �������Ʈ ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UUserWidget> mInventoryWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Widget")
	UUserWidget* mInventoryWidget;
 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	class UInventoryWeaponEntry* mEntry;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UUserWidget> mEntryClass;

public:
	//void TestFunc();
	
	//XY��鿡���� ������
	void UpDown(float Value);
	void LeftRight(float Value);
	void Yaw(float Value);

	//�߻�ü�� �߻��ϴ� �Լ�
	void OnFire();
	void OnFireRight();

	void InitInventoryWidget();
	void OpenInventory();

	
public:
	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator,
		AActor* DamageCauser) override;

	UPROPERTY()
	float mVertical = 0;

	UPROPERTY()
	float mHorizontal = 0;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	virtual void PostInitializeComponents() override;
	virtual void PreInitializeComponents() override;
	//virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved,
	//	FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	static UDataTable* mProjectileTable;
};
