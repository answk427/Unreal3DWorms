// Fill out your copyright notice in the Description page of Project Settings.


#include "WarmsGameModeBase.h"
#include "Character/PlayerCharacter.h"
#include "UI/MyHUD.h"

AWarmsGameModeBase::AWarmsGameModeBase()
{
	DefaultPawnClass = APlayerCharacter::StaticClass();

	static ConstructorHelpers::FClassFinder<APlayerCharacter> BP_Character(TEXT("Blueprint'/Game/BluePrints/BP_PlayerCharacter_.BP_PlayerCharacter__C'"));

	if (BP_Character.Succeeded())
		DefaultPawnClass = BP_Character.Class;

	static ConstructorHelpers::FClassFinder<UMyHUD> UI_HUD(TEXT("WidgetBlueprint'/Game/UI/HUD.HUD_C'"));
	
	if (UI_HUD.Succeeded())
	{
		HUDWidget = UI_HUD.Class;
		currentWidget = CreateWidget(GetWorld(), HUDWidget);
		if (currentWidget)
		{
			currentWidget->AddToViewport();
		}
	}
}

void AWarmsGameModeBase::BeginPlay()
{
	
}
