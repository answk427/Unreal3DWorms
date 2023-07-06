// Fill out your copyright notice in the Description page of Project Settings.


#include "WPGameInstance.h"

UWPGameInstance::UWPGameInstance()
{
}


void UWPGameInstance::Init()
{
	Super::Init();

	UE_LOG(LogClass, Warning, TEXT("%s"), TEXT("Game Instance Init!"));
}

void UWPGameInstance::BeginDestroy()
{
	Super::BeginDestroy();
}

