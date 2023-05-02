// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimation.h"
#include "PlayerCharacter.h"

#include <GameFramework/PawnMovementComponent.h>

UPlayerAnimation::UPlayerAnimation()
{ 
	
}


void UPlayerAnimation::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	//현재 애니메이션 클래스를 가진 Pawn
	auto Pawn = TryGetPawnOwner();
	
	if (IsValid(Pawn))
	{
		Speed = Pawn->GetVelocity().Size();
	}

	auto Character = Cast<APlayerCharacter>(Pawn);

	if (Character)
	{
		IsFalling = Character->GetMovementComponent()->IsFalling();
		Hanging = Character->Hanging;
		Vertical = Character->mVertical;
		Horizontal = Character->mHorizontal;

		//UE_LOG(LogTemp, Error, TEXT("UPlayerAnimation IsFalling : %d, Vertical : %f, Horizontal : %f"), IsFalling, Vertical, Horizontal);
	}


}