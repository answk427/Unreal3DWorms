// Fill out your copyright notice in the Description page of Project Settings.


#include "Grenade.h"

#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "../UI/CountDownWidget.h"
#include "Components/TextBlock.h"

AGrenade::AGrenade()
{
	mCollisionComp->BodyInstance.bNotifyRigidBodyCollision = false;

	mCountDownWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("CountDownText"));
	mCountDownWidget->SetupAttachment(mCollisionComp);
	mCountDownWidget->SetRelativeLocation(FVector(0.f, 0.f, -10.f));
	mCountDownWidget->SetWidgetSpace(EWidgetSpace::Screen);

	static ConstructorHelpers::FClassFinder<UCountDownWidget> UW(
		TEXT("WidgetBlueprint'/Game/UI/BP_CountDown.BP_CountDown_C'")
	);

	if(UW.Succeeded())
	{
		mCountDownWidget->SetWidgetClass(UW.Class);
		mCountDownWidget->SetDrawSize(FVector2D(10.0f, 10.0f));
	}

	//InitialLifeSpan = 10;
}

void AGrenade::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UE_LOG(LogTemp, Warning, TEXT("Grenade Tick Velocity : %f"), mProjectileMovement->Velocity.Size());
	

	if(mProjectileMovement->Velocity.IsNearlyZero() && !bStopped)
	{
		//if문 안쪽으로 한번만 들어오도록 플래그 설정
		bStopped = true;

		FHitResult CurrPos;
		CurrPos.ImpactPoint = GetActorLocation();
		CurrPos.ImpactNormal = GetActorUpVector();

		auto* CountWidget = Cast<UCountDownWidget>(mCountDownWidget->GetUserWidgetObject());
		if (CountWidget)
			CountWidget->CountStart(mExplodeDelay);
		else
			UE_LOG(LogTemp, Warning, TEXT("Grenade Tick widget null"));
		//폭발이펙트 스폰
		Explode(CurrPos);
	}

	FVector pos = mCountDownWidget->GetComponentLocation();
	//UE_LOG(LogTemp, Warning, TEXT("Grenade UI X:%f Y:%f Z:%f"), pos.X, pos.Y, pos.Z);
}

void AGrenade::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	UE_LOG(LogTemp, Warning, TEXT("AGranade Destroyed"));
}

void AGrenade::BeginPlay()
{
	AActor::BeginPlay();
	SetLifeSpan(0.0f);

	auto* countWidget = Cast<UCountDownWidget>(mCountDownWidget->GetUserWidgetObject());

	if (countWidget)
		countWidget->CountText->SetText(FText::FromString(""));
	else
		UE_LOG(LogTemp, Warning, TEXT("Grenade BeginPlay widget null"));
			
}

void AGrenade::Explode(const FHitResult& CurrPos)
{
	FTimerHandle timerHandle;
	GetWorld()->GetTimerManager().SetTimer(timerHandle, FTimerDelegate::CreateLambda(
		[&, CurrPos]()
	{
		Super::Explode(CurrPos);

		FTimerHandle timerHandleDestroy;
		//폭발 이펙트 1초 뒤 발사체 액터 파괴
		GetWorld()->GetTimerManager().SetTimer(timerHandleDestroy,
			FTimerDelegate::CreateLambda(
				[&]() {Destroy(); }),
			1.0f, false, 1.0f);

	}), 1.0f, false, mExplodeDelay);
}
