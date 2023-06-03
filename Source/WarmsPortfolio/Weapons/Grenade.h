// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "Components/WidgetComponent.h"
#include "Grenade.generated.h"

/**
 * 
 */

class UWidgetComponent;

UCLASS()
class WARMSPORTFOLIO_API AGrenade : public AProjectile
{
	GENERATED_BODY()

	AGrenade();

private:
	//Is Grenade Stop?
	bool bStopped = false;
	//Delay after Stop
	float mExplodeDelay = 3.0f;

public:
	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void BeginPlay() override;
	virtual void Explode(const FHitResult& Impact) override;
	
public:
	virtual void PostInitializeComponents() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	UWidgetComponent* mCountDownWidget;


};

inline void AGrenade::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	mCountDownWidget->InitWidget();
}
	