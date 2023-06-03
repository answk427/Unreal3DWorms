// Fill out your copyright notice in the Description page of Project Settings.


#include "WarmsGameModeBase.h"

#include "ItemActorManager.h"
#include "Character/PlayerCharacter.h"
#include "UI/MyHUD.h"
#include "WPGameInstance.h"

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

	ItemActorManager = CreateDefaultSubobject<UItemActorManager>(TEXT("ItemActorManager"));

	
}

void AWarmsGameModeBase::BeginPlay()
{
	UWPGameInstance* GameInstance = Cast<UWPGameInstance>(GetGameInstance());
	check(GameInstance);
	auto DataManager = GameInstance->DataManager;

	//������ ���̺�� �ִ� ��� �����ۿ� ���� �����ۿ���(����X, �����) ����
	for(int i=0; i<EObjectTypeName::UnknownItem; ++i)
	{
		auto Items = DataManager->GetAllItem((EObjectTypeName)i);
		for(auto& Item : Items)
		{
			ItemActorManager->AddItem(Item);
		}
	}

	//���� �ð����� ������ ���������� ����
	ItemActorManager->SetSpawnTimer(GetWorld(), 5.0f, FVector(-1000.f, -1000.f, 300.f),
		FVector(1000.f, 1000.f, 350.f));
}

void AWarmsGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}
