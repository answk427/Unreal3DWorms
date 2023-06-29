// Fill out your copyright notice in the Description page of Project Settings.


#include "HpBarWidget.h"

#include "Components/EditableTextBox.h"
#include "Components/ProgressBar.h"
#include "Character/StatComponent.h"
#include "Components/EditableText.h"

UHpBarWidget::UHpBarWidget(const FObjectInitializer& ObjInitializer)
	: Super(ObjInitializer)
{
	//HpText->SetText(FText::FromString("tetext"));
	
}

void UHpBarWidget::BindingStatComp(UStatComponent* StatComp)
{
	verify(StatComp != nullptr);

	currStatComponent = StatComp;
	StatComp->HpChanged.AddUObject(this, &UHpBarWidget::UpdateHpBar);
	UpdateHpBar();
}

void UHpBarWidget::UpdateHpBar()
{
	if(currStatComponent.IsValid())
	{
		HpProgressBar->SetPercent(currStatComponent->GetHpRatio());
		HpEditText->SetText(FText::FromString(FString::FromInt(currStatComponent->GetHp())));
		UE_LOG(LogTemp, Warning, TEXT("HP percent : %f"), currStatComponent->GetHpRatio());
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("currStatComponent Not Valid"));
}

void UHpBarWidget::SetHpBarColor(FLinearColor Color)
{
	HpProgressBar->SetFillColorAndOpacity(Color);
}


