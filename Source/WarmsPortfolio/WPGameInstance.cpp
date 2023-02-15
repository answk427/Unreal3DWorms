// Fill out your copyright notice in the Description page of Project Settings.


#include "WPGameInstance.h"


UWPGameInstance::UWPGameInstance()
{
	DataManager = MakeUnique<WPDataManager>();
	
}


void UWPGameInstance::Init()
{
	Super::Init();
	DataManager->InitDataTables();

	UE_LOG(LogClass, Warning, TEXT("%s"), TEXT("Game Instance Init!"));
}

