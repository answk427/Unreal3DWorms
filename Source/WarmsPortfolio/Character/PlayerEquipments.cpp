// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerEquipments.h"
#include "../Weapons/Weapon.h"


FPlayerEquipments::FPlayerEquipments()
{

}

void FPlayerEquipments::EquipWeapon(const FWPItem& item, const FWeaponData& WeaponData)
{
	UE_LOG(LogTemp, Warning, TEXT("EquipWeapon"));

	//아무것도 장착하고 있지 않을때
	if (CurrentWeapon == nullptr)
		CurrentWeapon = std::make_unique<std::pair<FWPItem, FWeaponData>>(FWPItem(), FWeaponData());

	//이미 같은 아이템을 장착중이면 리턴
	if (CurrentWeapon->first.ItemName == item.ItemName)
		return;
	
	CurrentWeapon->first = item;
	CurrentWeapon->second = WeaponData;
}





