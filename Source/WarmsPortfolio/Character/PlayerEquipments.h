// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/ItemStruct.h"
#include "DataTableStructures.h"
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

	using WeaponItemPair = std::pair<FWPItem, std::unique_ptr<FWeaponData>>;
private:
	//FWeaponData�� ���פ��ͷ ιٲ���. std::pair<FWPItem, std::unique_ptr<FWeaponData>>Է?
	std::unique_ptr<WeaponItemPair> CurrentWeapon = nullptr;
	
public:
	FPlayerEquipments();

	FPlayerEquipments& operator=(const FPlayerEquipments& other)
	{
		UE_LOG(LogTemp, Warning, TEXT("Function called from file %s Function called from function %s at line %s"), __FILE__, __func__, __LINE__);
		return *this;
	}

	//ι�� ��� ��Լ
	void EquipWeapon(const FWPItem& item, const FWeaponData& WeaponData);

	const WeaponItemPair* GetWeapon() const
	{
		return CurrentWeapon.get();
	}
	
};
