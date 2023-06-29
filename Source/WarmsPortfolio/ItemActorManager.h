// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/ItemStruct.h"
#include "Weapons/ItemActor.h"
#include "UObject/NoExportTypes.h"
#include "ItemActorManager.generated.h"


/**
 * 
 */
struct FWPItem;

UCLASS()
class WARMSPORTFOLIO_API UItemActorManager : public UObject
{
	GENERATED_BODY()

private:
	UPROPERTY()
	TMap<FWPItem, AItemActor*> ItemActors;


	FTimerHandle mTimerHandle;
public:
	AItemActor* AddItem(const FWPItem& Item);
	AItemActor* CreateItemActor(const FWPItem& Item);
	AItemActor* SpawnItemActor(UWorld* World, const FWPItem& Item, const FVector& Location, const FRotator& Rotator);
	AItemActor* SpawnRandomItemActor(UWorld* World, const FVector& MinRange, const FVector& MaxRange);
	void SetSpawnTimer(UWorld* World, float Time, const FVector& MinRange, const FVector& MaxRange);
	void ClearSpawnTimer(UWorld* World);
	
};

