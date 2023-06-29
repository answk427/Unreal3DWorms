// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemActorManager.h"

#include "DataTableStructures.h"
#include "WPDataManager.h"
#include "WPGameInstance.h"
#include "Weapons/Weapon.h"


AItemActor* UItemActorManager::AddItem(const FWPItem& Item)
{
	//���������̺���� �ش� �����ۿ� ���� ������ ������.
	auto GameInstance = Cast<UWPGameInstance>(GetWorld()->GetGameInstance());
	auto DataManager = GameInstance->DataManager;
	auto WeaponData = DataManager->GetWeaponData(Item.GameObjectType, Item.ItemName, TEXT("CreateItemActorFunc"));

	//�ش� �������� �޽��� �̿��� �����۾��� ���� 
	AWeapon* WeaponActor = NewObject<AWeapon>(this, WeaponData->WeaponClass);
	WeaponActor->InitMesh(); 
	AItemActor* WeaponItemActor = NewObject<AItemActor>(this, AItemActor::StaticClass());
	WeaponItemActor->Init(WeaponActor, Item);

	ItemActors.Add(Item, WeaponItemActor);

	/*UE_LOG(LogTemp, Warning, TEXT("AItemActor::AddItem this : %p, WeaponItemActor Name : %s, Addr : %p"),
		this, *WeaponItemActor->GetName(), WeaponItemActor);*/


	return WeaponItemActor;
}

AItemActor* UItemActorManager::CreateItemActor(const FWPItem& Item)
{
	if (auto Val = ItemActors.Find(Item))
		return *Val;

	return AddItem(Item);
}

AItemActor* UItemActorManager::SpawnItemActor(UWorld* World, const FWPItem& Item, const FVector& Location,
	const FRotator& Rotator)
{
	static int count = 0;
	//UE_LOG(LogTemp, Warning, TEXT("SpawnItemActor count : %d"), ++count);

	AItemActor* ItemActor;
	if (auto Val = ItemActors.Find(Item))
		ItemActor = *Val;
	else
		ItemActor = CreateItemActor(Item);

	FActorSpawnParameters Params;
	Params.Template = ItemActor;
	
	
/*
	if (!IsValid(ItemActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnItemActor NotValid"));
		return nullptr;
	}*/
	/*UE_LOG(LogTemp, Warning, TEXT("AItemActor::SpawnItemActor Timer. this : %p, ItemActor of TMap Name : %s, Addr : %p"),
		this, *ItemActor->GetName(), ItemActor);*/

	auto SpawnedActor = World->SpawnActor<AItemActor>(Location, Rotator, Params);
	SpawnedActor->SetItemInfo(ItemActor->GetItemInfo());
	return SpawnedActor;
	
}

AItemActor* UItemActorManager::SpawnRandomItemActor(UWorld* World, const FVector& MinRange, const FVector& MaxRange)
{
	FVector Location(FMath::FRandRange(MinRange.X, MaxRange.X),
		FMath::FRandRange(MinRange.Y, MaxRange.Y),
		FMath::FRandRange(MinRange.Z, MaxRange.Z));

	TArray<FWPItem> Keys;
	ItemActors.GetKeys(Keys);
	int RandomIdx = FMath::FRandRange(0, Keys.Num());

	return SpawnItemActor(World, Keys[RandomIdx], Location, FRotator(0.f,0.f,0.f));
}

void UItemActorManager::SetSpawnTimer(UWorld* World, float Time, const FVector& MinRange, const FVector& MaxRange)
{
	FTimerManager& Timer = World->GetTimerManager();
	
	
	Timer.SetTimer(mTimerHandle, FTimerDelegate::CreateLambda(
		[this, World, MinRange, MaxRange]()
		{
			SpawnRandomItemActor(World, MinRange, MaxRange);
		}
	), Time, true);
}

void UItemActorManager::ClearSpawnTimer(UWorld* World)
{
	World->GetTimerManager().ClearTimer(mTimerHandle);
}
