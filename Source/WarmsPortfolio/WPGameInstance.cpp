// Fill out your copyright notice in the Description page of Project Settings.


#include "WPGameInstance.h"
#include "CaptureHelper.h"

UWPGameInstance::UWPGameInstance()
{
	DataManager = CreateDefaultSubobject<UWPDataManager>(TEXT("DataManager"));
}


void UWPGameInstance::Init()
{
	Super::Init();
	DataManager->InitDataTables();

	UE_LOG(LogClass, Warning, TEXT("%s"), TEXT("Game Instance Init!"));

	UCaptureHelper::Instance()->Init(GetWorld());
}

