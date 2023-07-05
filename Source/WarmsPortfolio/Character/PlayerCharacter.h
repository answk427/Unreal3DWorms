// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/PostProcessVolume.h"
#include "GameFramework/Character.h"
#include "Voxel/Public/VoxelCharacter.h"
#include "InOutWater.h"
#include "PlayerCharacter.generated.h"


class USpringArmComponent;
class UCameraComponent;
class USceneComponent;
class AProjectile;
class AWarmsPortfolioProjectile;
class UWidgetComponent;
class UStatComponent;
class UDataTable;
class AMyRope;
struct FInventory;
struct FPlayerEquipments;
class APostProcessVolume;
class AWeapon;
class UPlayerAnimation;

UCLASS()
class WARMSPORTFOLIO_API APlayerCharacter : public AVoxelCharacter, public IInOutWater
{
	GENERATED_BODY()

	DECLARE_MULTICAST_DELEGATE_OneParam(FStartDamaged, TWeakObjectPtr<APlayerCharacter>);
	DECLARE_MULTICAST_DELEGATE_OneParam(FEndDamaged, TWeakObjectPtr<APlayerCharacter>);
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnDie, TWeakObjectPtr<APlayerCharacter>);
	
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

public:
	FStartDamaged StartDamagedDelegate;
	FEndDamaged EndDamagedDelegate;
	FOnDie OnDieDelegate;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* mSpringArm;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* mCamera;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* mSpringArmSky;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* mCameraSky;



	//�÷��̾� ������ �հ���, ����������ġ Socket�� �ٿ��δ� SceneComponent
	UPROPERTY(VisibleDefaultsOnly)
	USceneComponent* mWeaponPos;

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
	//ĳ���Ͱ� �������� ���â
	TSharedPtr<FPlayerEquipments> mEquipments;
	//�κ��丮 �������Ʈ ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UUserWidget> mInventoryWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Widget")
	UUserWidget* mInventoryWidget;
 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	class UInventoryWeaponEntry* mEntry;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UUserWidget> mEntryClass;

	UPROPERTY()
	APostProcessVolume* ActionLine;
	UPROPERTY()
	AMyRope* mRope;

	UPROPERTY()
	bool Hanging = false;

	UPROPERTY(EditAnywhere)
	float RopePower = 10.0f;

	UPROPERTY(EditAnywhere)
	float ImpulseRate = 200.0f;

	UPROPERTY()
	bool bFireHoldDown = false;

	UPROPERTY()
	UPlayerAnimation* PlayerAnim;

	int TeamNumber;
	FLinearColor TeamColor;
private:
	UPROPERTY()
	AWeapon* mCurrentWeapon = nullptr;

	//입수할 때 물의 높이(월드좌표)
	float WaterZ = 0.f;
		
	
public:
	//void TestFunc();
	
	//XY��鿡���� ������
	void ToggleShowCursor();
	void UpDown(float Value);
	void LeftRight(float Value);
	void Yaw(float Value);
	void IgnoreYaw(float Value){}
	void IgnoreUpDown(float Value){}

	
	void Pull(float Value);
	void PullEnd();
	void Push(float Value);

	//�߻�ü�� �߻��ϴ� �Լ�
	void Aiming();
	void ClickedFire();
	void ReleasedFire();

	void OnFireRight();

	void InitInventoryWidget();
	void OpenInventory();

	void AcquireItem();

	//i value 0 = MainCamera, 1 = SkyCamera
	void ActiveOneCamera(int i);

	//폭발모션, 이펙트, 오브젝트 제거
	void DieChecking();

	void SetColor(FLinearColor& Color);
	void SetTeam(int Number);
	void Init(FLinearColor& Color, int TeamNum);
public:
	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator,
		AActor* DamageCauser) override;

	//데미지를 입고있을 때 true
	UPROPERTY(EditAnywhere, Category = "State")
	bool bTakingDamage = false;

	//물에 완전히 잠겼을 때 true
	bool bInWaterFull = false;

	float TakingDamageTime = 0.f;

	//죽었을때 미리 플래그를 해두는 변수
	UPROPERTY()
	bool bIsDead = false;

	UPROPERTY()
	float mVertical = 0;

	UPROPERTY()
	float mHorizontal = 0;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnSphereComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void PostInitializeComponents() override;
	virtual void PreInitializeComponents() override;
	virtual void Jump() override;

	virtual void PostLoad() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	//UFUNCTION(BlueprintCallable)
	virtual void EnterWater() override;

	//UFUNCTION(BlueprintCallable)
	virtual void ExitWater() override;

	void DieInWater();
	bool IsInWaterFull();

	//virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved,
	//	FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	static UDataTable* mProjectileTable;


};


inline void APlayerCharacter::PullEnd()
{
	ActionLine->bEnabled = false;
}
