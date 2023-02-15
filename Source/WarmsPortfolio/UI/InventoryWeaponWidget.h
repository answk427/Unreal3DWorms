	// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWeaponWidget.generated.h"

/**
 * 
 */
class UInventoryWeaponEntry;

UCLASS()
class WARMSPORTFOLIO_API UInventoryWeaponWidget : public UUserWidget
{
	GENERATED_BODY()


	static TMap<FName, UInventoryWeaponEntry*> WeaponEntries;
		
public:
	void AddEntry(const FName& WeaponName, const FName& TableName);
};
