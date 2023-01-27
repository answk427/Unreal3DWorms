// Fill out your copyright notice in the Description page of Project Settings.


#include "WPGameInstance.h"

#include "Engine/DataTable.h"

UWPGameInstance::UWPGameInstance()
{
	FString ProjectileTableName = TEXT("DataTable'/Game/DataTables/ProjectileCSV.ProjectileCSV'");

	static ConstructorHelpers::FObjectFinder<UDataTable> ProjectileTableObj(*ProjectileTableName);

	if(ProjectileTableObj.Succeeded())
		ProjectileTable = ProjectileTableObj.Object;
}

void UWPGameInstance::Init()
{
	Super::Init();
	UE_LOG(LogClass, Warning, TEXT("%s"), TEXT("Game Instance Init!"));
}
