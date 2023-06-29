// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStateWidget.h"

#include "PlayerStateEntry.h"
#include "Character/PlayerCharacter.h"
#include "Components/ListView.h"
#include "WarmsGameModeBase.h"
#include "Character/StatComponent.h"

void UPlayerStateWidget::NativeConstruct()
{
	Super::NativeConstruct();
	PlayerStateList->OnItemDoubleClicked().AddUObject(this, &UPlayerStateWidget::DoubleClicked);
	PlayerStateList->OnEntryWidgetGenerated().AddUObject(this, &UPlayerStateWidget::UpdateEntry);
}

void UPlayerStateWidget::DoubleClicked(UObject* Object)
{
	APlayerCharacter* Character = Cast<APlayerCharacter>(Object);

	if (!IsValid(Character))
		return;

	//엔트리가 더블클릭되면 해당 캐릭터를 조작하도록 변경합니다.
	auto GMB = Cast<AWarmsGameModeBase>(GetWorld()->GetAuthGameMode());
	GMB->ChangeCharacter(Character);
}

void UPlayerStateWidget::InitEntries(TArray<TWeakObjectPtr<APlayerCharacter>> Characters)
{
	PlayerStateList->ClearListItems();
	
	for(auto Player : Characters)
	{
		if (Player.IsValid())
			PlayerStateList->AddItem(Player.Get());
		else
			PlayerStateList->AddItem(nullptr);
	}

	PlayerStateList->RegenerateAllEntries();
	
}

void UPlayerStateWidget::UpdateEntries()
{
	const TArray<UObject*>& Items = PlayerStateList->GetListItems();

	auto WidgetClass = PlayerStateList->GetEntryWidgetClass();
	auto WidgetClass2 = PlayerStateList->GetDefaultEntryClass();

	auto Entrys = PlayerStateList->GetDisplayedEntryWidgets();
	UE_LOG(LogTemp, Warning, TEXT("Entrys Num : %d"), Entrys.Num());
	
	for(auto Entry : Entrys)
	{
		auto MyEntry = Cast<UPlayerStateEntry>(Entry);
		MyEntry->UpdateHpBar();
	}


	for(auto Item : Items)
	{
		auto EntryWidget = PlayerStateList->GetEntryWidgetFromItem(Item);

		UPlayerStateEntry* Entry = Cast<UPlayerStateEntry>(EntryWidget);
		check(Entry);
		
		Entry->UpdateHpBar();
	}
}

void UPlayerStateWidget::UpdateEntry(UUserWidget& Widget)
{
	auto Entry = Cast<UPlayerStateEntry>(&Widget);
	Entry->UpdateHpBar();
}
