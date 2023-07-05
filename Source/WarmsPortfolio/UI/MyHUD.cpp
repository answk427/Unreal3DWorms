// Fill out your copyright notice in the Description page of Project Settings.


#include "MyHUD.h"

#include "Components/Image.h"

void UMyHUD::OnOffCrossHair(bool OnOff)
{
	if (OnOff)
		AimImage->SetVisibility(ESlateVisibility::Visible);
	else
		AimImage->SetVisibility(ESlateVisibility::Hidden);
}
