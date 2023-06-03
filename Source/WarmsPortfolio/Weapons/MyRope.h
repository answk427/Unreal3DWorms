// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UtilityWeapon.h"
#include "MyRope.generated.h"

/**
 * 
 */

class APlayerCharacter;
class UCableComponent;

UCLASS()
class WARMSPORTFOLIO_API AMyRope : public AUtilityWeapon
{

	GENERATED_BODY()

public:
	AMyRope();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rope")
	UCableComponent* mCable;
		

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* mRoot;

	UPROPERTY(EditAnywhere, Category = "Rope")
	float MovementPower = 10.0f;

	UPROPERTY(EditAnywhere)
	float FireDistance = 10000.0f;

	UPROPERTY(VisibleAnywhere, Category = "Rope")
	TArray<UCableComponent*> mCables;

	UPROPERTY(VisibleAnywhere, Category = "Rope")
	TArray<FVector> mCableStarts;

	UPROPERTY(VisibleAnywhere, Category = "Rope")
	TArray<FVector> mCableNormals;

	UPROPERTY(VisibleAnywhere, Category = "Rope")
	UCableComponent* LatestCable;
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USphereComponent* mCableEndCollision;*/

	UPROPERTY(EditAnywhere, Category="Rope")
	float ToleranceDistance = 10.0f;

	UPROPERTY(EditAnywhere)
	bool bActivated = false;

	/*UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);*/


	TWeakObjectPtr<APlayerCharacter> OwnerCharacter;
protected:
	// Called when the game starts or when spawned
	//virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Init(APlayerCharacter* OwnerPlayer);
	void SetStart(FVector& Start);

	void AddForceCharacter(float XInput, float YInput);

	//���������� ������
	void PullRope();
	//�������� �־���
	void PushRope();
	void GetTangentVector(FVector& RightTangentVec, FVector& UpTangentVec);

	void Fire(const FVector& Start, const FVector& CameraPos, const FRotator& CameraRot);

	//�÷��̾�� ���� ���̿� ��ֹ��� ������ ���ο� ���̺� ����
	void TraceBetweenAdd();

	//�÷��̾�� ���� ���̿� ��ֹ��� ������ ���̺� ����
	void TraceBetweenRemove();

	//�ֱ� ���̺� ��ġ�� ���� Tolerance���� ������ True��ȯ
	bool ToleranceCheck(const FVector& Point);

	void InitCable(UCableComponent* Cable);

	void AllClear();

	void RemoveLatestCable();

	void Activate();
	void Deactivate();
protected:
	virtual void BeginPlay() override;

public:
	virtual void PostInitializeComponents() override;
};








