// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../UI/ItemStruct.h"
#include "../DataTableStructures.h"
#include "memory"
#include "PlayerEquipments.generated.h"


/**
 * 
 */


class AWeapon;
struct FWeaponData;
struct FProjectileData;

USTRUCT()
struct WARMSPORTFOLIO_API FPlayerEquipments
{
	GENERATED_BODY()

private:
	std::unique_ptr<std::pair<FWPItem, FWeaponData>> CurrentWeapon = nullptr;
	
public:
	FPlayerEquipments();

	FPlayerEquipments& operator=(const FPlayerEquipments& other)
	{
		UE_LOG(LogTemp, Warning, TEXT("Function called from file %s Function called from function %s at line %s"), __FILE__, __func__, __LINE__);
		return *this;
	}

	//무기 착용 함수
	void EquipWeapon(const FWPItem& item, const FWeaponData& WeaponData);

	const std::pair<FWPItem, FWeaponData>* GetWeapon() const
	{
		return CurrentWeapon.get();
	}
	
};
