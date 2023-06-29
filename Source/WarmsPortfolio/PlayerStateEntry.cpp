// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStateEntry.h"

#include "Character/PlayerCharacter.h"
#include "Character/StatComponent.h"
#include "Components/ProgressBar.h"


void UPlayerStateEntry::SetHpBar(float Ratio)
{
	//CharacterHp->SetPercent(Ratio);
	UpdateHpTarget = Ratio;
	bHpUpdate = true;
}

void UPlayerStateEntry::BindCharacter(TWeakObjectPtr<APlayerCharacter> Character)
{
	if (!Character.IsValid())
	{
		CharacterHp->SetFillColorAndOpacity(FLinearColor::Black);
		BindedCharacter = nullptr;
		//UpdateHpTarget = 0.f;
		//bHpUpdate = false;
		//CharacterHp->SetPercent(0);
		CharacterHp->SetPercent(1.f);
		SetHpBar(0.f);
		return;
	}
		

	BindedCharacter = Character;
	CharacterHp->SetPercent(0);
	SetHpBar(Character->mStatComponent->GetHpRatio());

	CharacterHp->SetFillColorAndOpacity(Character->TeamColor);
}

void UPlayerStateEntry::UpdateHpBar()
{
	if (BindedCharacter.IsValid())
	{
		SetHpBar(BindedCharacter->mStatComponent->GetHpRatio());
	}
	else
		SetHpBar(0);
}

void UPlayerStateEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	APlayerCharacter* Character = Cast<APlayerCharacter>(ListItemObject);
	BindCharacter(Character);
}


void UPlayerStateEntry::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if(bHpUpdate)
	{
		float NewPercent = FMath::FInterpTo(CharacterHp->Percent, UpdateHpTarget, InDeltaTime, 1);
		CharacterHp->SetPercent(NewPercent);
		if (NewPercent == UpdateHpTarget)
			bHpUpdate = false;
	}
	
}
