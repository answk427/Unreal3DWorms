// Fill out your copyright notice in the Description page of Project Settings.

#include "CountDownWidget.h"

#include "Components/TextBlock.h"

void UCountDownWidget::CountStart(float countTime)
{
	FTimerManager& timerManager = GetWorld()->GetTimerManager();
	mCountTime = countTime;

		
	//1�ʸ��� �ؽ�Ʈ�ڽ����� ����
	timerManager.SetTimer(mTimerHandle,
		FTimerDelegate::CreateLambda(
			[&]()
	{
		if (mCountTime < 1)
		{
			//Ÿ�̸� ����
			timerManager.ClearTimer(mTimerHandle);
			
			return;
		}
		
		CountText->SetText(FText::FromString(FString::FromInt(mCountTime--)));

		//CountText->SetText(FText::FromString("SSIBAL"));
	}), 1.0f, true, 0.f);

}
