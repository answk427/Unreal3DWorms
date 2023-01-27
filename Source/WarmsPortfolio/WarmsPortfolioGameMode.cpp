// Copyright Epic Games, Inc. All Rights Reserved.

#include "WarmsPortfolioGameMode.h"
#include "WarmsPortfolioHUD.h"
#include "WarmsPortfolioCharacter.h"
#include "UObject/ConstructorHelpers.h"

AWarmsPortfolioGameMode::AWarmsPortfolioGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AWarmsPortfolioHUD::StaticClass();
}
