// Fill out your copyright notice in the Description page of Project Settings.


#include "SoundManager.h"
#include "DataTableStructures.h"
#include "Kismet/GameplayStatics.h"

USoundManager::USoundManager()
{
	ConstructorHelpers::FObjectFinder<UDataTable> SoundTableFinder(TEXT("DataTable'/Game/DataTables/SoundDataTable.SoundDataTable'"));

	if(SoundTableFinder.Succeeded())
	{
		SoundDataTable = SoundTableFinder.Object;
	}
}

void USoundManager::Init()
{
	TArray<FSoundData*> OutRows;
	SoundDataTable->GetAllRows<FSoundData>(TEXT("SoundGetAllRows"), OutRows);

	for(auto Data: OutRows)
	{
		Sounds.Add(Data->SoundName, Data->Sound);
	}
}

USoundBase* USoundManager::GetSound(const FString& SoundName)
{
	auto Sound = Sounds.Find(SoundName);

	if (Sound == nullptr)
		return nullptr;

	return *Sound;
		
}


void USoundManager::PlaySound(const FString& SoundName, const FVector& PlayLocation)
{
	auto Sound = Sounds.Find(SoundName);

	if (Sound == nullptr)
		return;

	UGameplayStatics::PlaySoundAtLocation(GetWorld(), *Sound, PlayLocation, FRotator());
}

void USoundManager::PostInitProperties()
{
	UObject::PostInitProperties();
	Init();
}
