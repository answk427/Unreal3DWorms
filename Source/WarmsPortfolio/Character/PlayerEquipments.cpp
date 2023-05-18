// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerEquipments.h"
#include "../Weapons/Weapon.h"


FPlayerEquipments::FPlayerEquipments()
{

}

void FPlayerEquipments::EquipWeapon(const FWPItem& item, const FWeaponData& WeaponData)
{
	UE_LOG(LogTemp, Warning, TEXT("EquipWeapon"));


	//이미 같은 아이템을 장착중이면 리턴
	if (CurrentWeapon != nullptr && CurrentWeapon->first.ItemName == item.ItemName)
		return;

	
	CurrentWeapon.release();

	std::unique_ptr<FWeaponData> pData;
	switch(item.GameObjectType)
	{
	case EObjectTypeName::Projectile:
		{
		pData = std::make_unique<FProjectileData>((FProjectileData&)WeaponData);
		break;
		}
	default:
		pData = std::make_unique<FWeaponData>(WeaponData);
	}

	CurrentWeapon = std::make_unique<WeaponItemPair>
		(std::make_pair<FWPItem, std::unique_ptr<FWeaponData>>
		(FWPItem(), std::move(pData)));


	CurrentWeapon->first = item;
	//*CurrentWeapon->second = WeaponData;
}





