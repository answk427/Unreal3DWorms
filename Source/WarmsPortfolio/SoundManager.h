// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "SoundManager.generated.h"

/**
 * 
 */
class USoundBase;


UCLASS(BlueprintType)
class WARMSPORTFOLIO_API USoundManager : public UObject
{
	GENERATED_BODY()

	UPROPERTY()
	UDataTable* SoundDataTable;

	UPROPERTY()
	TMap<FString, USoundBase*> Sounds;

public:
	USoundManager();

	void Init();

	UFUNCTION(BlueprintCallable)
	USoundBase* GetSound(const FString& SoundName);

	UFUNCTION(BlueprintCallable)
	void PlaySound(const FString& SoundName, const FVector& PlayLocation);

	virtual void PostInitProperties() override;
};
