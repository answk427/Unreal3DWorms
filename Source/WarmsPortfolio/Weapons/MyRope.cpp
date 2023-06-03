// Fill out your copyright notice in the Description page of Project Settings.


#include "MyRope.h"
#include "../Character/PlayerCharacter.h"
#include "CableComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"



AMyRope::AMyRope()
{
	PrimaryActorTick.bCanEverTick = true;
	mRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(mRoot);
	mCable = CreateDefaultSubobject<UCableComponent>(TEXT("NinjaRope"));
	mCable->SetupAttachment(mRoot);
	mCable->SetVisibility(false);
	
	/*ConstructorHelpers::FObjectFinder<UStaticMesh> mesh(TEXT("StaticMesh'/Game/M5VFXVOL2/Props/Meshes/BF_mesh_brazier.BF_mesh_brazier'"));

	mMeshComponent->SetStaticMesh(mesh.Object);*/
}

void AMyRope::Init(APlayerCharacter* OwnerPlayer)
{
	OwnerCharacter = OwnerPlayer;

	const FName SocketName(TEXT("RightHandSocket"));
	mCable->SetAttachEndToComponent(OwnerPlayer->GetMesh(), SocketName);
}

void AMyRope::SetStart(FVector& Start)
{
	SetActorLocation(Start);

}

void AMyRope::AddForceCharacter(float XInput, float YInput)
{
	if (!OwnerCharacter.IsValid())
		return;

	APlayerCharacter* Character = OwnerCharacter.Get();
	UCharacterMovementComponent* MovementComponent = Character->GetCharacterMovement();

	//�ΰ��� �����͸� �������ͷ� ���
	FVector RightTangentVec, UpTangentVec;
	GetTangentVector(RightTangentVec, UpTangentVec);

	FVector ForceVec = RightTangentVec * XInput + UpTangentVec * YInput;
	//MovementComponent->AddForce(ForceVec.GetSafeNormal()*MovementPower);
	MovementComponent->AddInputVector(ForceVec.GetSafeNormal()*MovementPower);
	//Character->SetActorLocation(Character->GetActorLocation() + ForceVec.GetSafeNormal()*MovementPower);

	//ĳ���Ͱ� ������ �ٶ󺸵��� ��
	//FRotator Rot = UKismetMathLibrary::FindLookAtRotation(Character->GetActorLocation(), GetActorLocation());
	//Character->SetActorRotation(Rot);
}

void AMyRope::GetTangentVector(FVector& RightTangentVec, FVector& UpTangentVec)
{
	if (!OwnerCharacter.IsValid())
		return;

	APlayerCharacter* Character = OwnerCharacter.Get();

	FVector PlayerLocation = Character->GetActorLocation();

	//������ ���۵Ǵ� ���� �߽����� ���� �����Ѵٰ� ����
	FVector SphereMidPoint = GetActorLocation();

	//�÷��̾�� ���� �߽����� ���ϴ� ����. ���������� �ݴ����
	FVector InverseNormalVec = SphereMidPoint - PlayerLocation;

	//�÷��̾��� ������ ���ϴ� �������͸� ����
	RightTangentVec = -FVector::CrossProduct(InverseNormalVec.GetSafeNormal(), FVector::UpVector);
	//�÷��̾��� ������ ���ϴ� �������͸� ����
	UpTangentVec = -FVector::CrossProduct(RightTangentVec, InverseNormalVec);
}

void AMyRope::PullRope()
{
	if (!OwnerCharacter.IsValid() || !bActivated || mCableStarts.Num()==0)
		return;

	APlayerCharacter* Character = OwnerCharacter.Get();
	FVector PlayerLocation = Character->GetActorLocation();
	//UCharacterMovementComponent* MovementComponent = Character->GetCharacterMovement();

	FVector SphereMidPoint = mCableStarts.Last();

	FVector ToMidPoint = (SphereMidPoint - PlayerLocation).GetSafeNormal();
	UE_LOG(LogTemp, Warning, TEXT("%s, Magnitude : %f"), *ToMidPoint.ToString(), ToMidPoint.Size());

	//MovementComponent->AddForce(ToMidPoint * MovementPower);
	Character->SetActorLocation(PlayerLocation + ToMidPoint * MovementPower);
	//for(int i=0; i<MovementPower; ++i)
	//{
	//	//MovementComponent->AddInputVector(ToMidPoint * MovementPower*10.0f);
	//	UE_LOG(LogTemp, Warning, TEXT("PullRope"));
	//	MovementComponent->AddInputVector(ToMidPoint);
	//	
	//}
	
	
}

void AMyRope::PushRope()
{
	if (!OwnerCharacter.IsValid() || !bActivated || mCableStarts.Num() == 0)
		return;

	APlayerCharacter* Character = OwnerCharacter.Get();
	FVector PlayerLocation = Character->GetActorLocation();
	//UCharacterMovementComponent* MovementComponent = Character->GetCharacterMovement();

	FVector SphereMidPoint = mCableStarts.Last();

	FVector RopeToPlayer = (PlayerLocation- SphereMidPoint).GetSafeNormal();
	UE_LOG(LogTemp, Warning, TEXT("%s, Magnitude : %f"), *RopeToPlayer.ToString(), RopeToPlayer.Size());

	//MovementComponent->AddForce(ToMidPoint * MovementPower);
	Character->SetActorLocation(PlayerLocation + RopeToPlayer * MovementPower);
	//for(int i=0; i<MovementPower; ++i)
	//{
	//	//MovementComponent->AddInputVector(ToMidPoint * MovementPower*10.0f);
	//	UE_LOG(LogTemp, Warning, TEXT("PullRope"));
	//	MovementComponent->AddInputVector(ToMidPoint);
	//	
	//}
}

void AMyRope::Fire(const FVector& Start, const FVector& CameraPos, const FRotator& CameraRot)
{
	
	FHitResult OutHit;
	//FVector Start = GetActorLocation();
	FVector End = CameraPos + CameraRot.Vector() * FireDistance;

	FCollisionObjectQueryParams ObjectQueryParams(ECollisionChannel::ECC_WorldStatic);

	UKismetSystemLibrary::DrawDebugLine(GetWorld(), Start, End,
		FLinearColor::Green, 2.0f, 3.0f);

	if (GetWorld()->LineTraceSingleByObjectType(OutHit, Start, End, ObjectQueryParams))
	{
		UE_LOG(LogTemp, Warning, TEXT("LineTrace Success Hit Object : %s"), *OutHit.GetActor()->GetName());
		
		UKismetSystemLibrary::DrawDebugSphere(GetWorld(), OutHit.Location, 100, 12, FLinearColor::Red, 2.0f, 1.0f);
		UKismetSystemLibrary::DrawDebugSphere(GetWorld(), OutHit.ImpactPoint, 80, 12, FLinearColor::Green, 2.0f, 1.0f);
		UKismetSystemLibrary::DrawDebugSphere(GetWorld(), OutHit.TraceEnd, 60, 12, FLinearColor::Blue, 2.0f, 1.0f);
		AllClear();
		
		SetActorLocation(OutHit.ImpactPoint);
		mCableStarts.Add(OutHit.ImpactPoint);
		mCableNormals.Add(OutHit.ImpactNormal);

		//mCable->SetVisibility(true);
		//bActivated = true;
		//OwnerCharacter->Hanging = true;
		//OwnerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
		Activate();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("LineTrace Fail"));
	}


}

void AMyRope::TraceBetweenAdd()
{
	if (!OwnerCharacter.IsValid())
		return;


	//��������, ĳ���� ��
	FVector Start = OwnerCharacter->mWeaponPos->GetComponentLocation();

	//��������, �ֱ� ���̺��� ������ġ
	FVector End = LatestCable->GetComponentLocation();


	//UE_LOG(LogTemp, Warning, TEXT("ActorLocation : %s     /    CableComponentLocation : %s"), *Start.ToString(), *End.ToString());

	FHitResult OutHit;
	FCollisionObjectQueryParams ObjectQueryParams(ECollisionChannel::ECC_WorldStatic);


	/*UKismetSystemLibrary::DrawDebugLine(GetWorld(), Start, End,
		FLinearColor::Red, 0.1f, 5.0f); */
	
	//���̺�� �ɸ� ��ü�� ���� ��
	if (GetWorld()->LineTraceSingleByObjectType(OutHit, Start, End, ObjectQueryParams))
	{
		//�ʹ� ���� �Ÿ����� �˻�
		if (mCableStarts.Num() != 0)
		{
			
			FVector& LatestLocation = mCableStarts.Last();

			if (LatestLocation.Equals(OutHit.Location, ToleranceDistance))
				return;
		}
				

		/*UKismetSystemLibrary::DrawDebugSphere(GetWorld(), OutHit.Location, 100, 12, FLinearColor::Red, 2.0f, 1.0f);
		UKismetSystemLibrary::DrawDebugSphere(GetWorld(), OutHit.ImpactPoint, 80, 12, FLinearColor::Green, 2.0f, 1.0f);
		UKismetSystemLibrary::DrawDebugSphere(GetWorld(), OutHit.TraceEnd, 60, 12, FLinearColor::Blue, 2.0f, 1.0f);
*/
		FString CableName = TEXT("TESTCABLE");
		CableName = CableName + FString::FromInt(mCables.Num());

		UCableComponent* newCable = NewObject<UCableComponent>(this, UCableComponent::StaticClass(), *CableName);
		InitCable(newCable);

		//���̺��� Hit�� ��ü�� ����
		newCable->AttachToComponent(OutHit.GetComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		
		newCable->SetWorldLocation(OutHit.Location);
		newCable->SetAttachEndToComponent(OwnerCharacter->GetMesh(), TEXT("RightHandSocket"));
		

		//������ �ִ� ���̺��� ���� ���� ���� ���̺�� ����
		LatestCable->SetAttachEndToComponent(newCable);
		LatestCable = newCable;


		newCable->RegisterComponent();

		mCables.Add(newCable);
		mCableStarts.Add(OutHit.Location);
		mCableNormals.Add(OutHit.ImpactNormal);
		UE_LOG(LogTemp, Warning, TEXT("AMyRope TraceBetween Success"));
	}
}

void AMyRope::TraceBetweenRemove()
{

	if (!OwnerCharacter.IsValid())
		return;
	//���̺��� 2�� �̻��϶��� ����
	if (mCableStarts.Num() < 2)
		return;

	int idx = mCableStarts.Num() - 2;
	//�÷��̾��� �� ��ġ
	FVector PlayerLoc = OwnerCharacter->GetMesh()->GetSocketLocation(TEXT("RightHandSocket"));
	
	//�ֱ� ���̺��� ������ġ
	FVector& LatestCableLoc = mCableStarts.Last();

	//�÷��̾�� ���̺��� �� �پ����� ��
	if (PlayerLoc.Equals(LatestCableLoc, ToleranceDistance))
	{
		RemoveLatestCable();
		return;
	}

	FVector& LatestCableNormal = mCableNormals.Last();

	//�ֱ� ���̺� �ٷ� ���� ���̺��� ������ġ
	FVector& PreLatestCableLoc = mCableStarts[idx];
	FVector& PreLatestCableNormal = mCableNormals[idx];
	

#pragma region PlayerToPreTrace
	//�÷��̾�� ���� ���̺�������� �浹�ϴ� ��ü�� �ִ��� �˻�
	FHitResult OutHit;
	FCollisionObjectQueryParams ObjectQueryParams(ECollisionChannel::ECC_WorldStatic);
	FVector End = PlayerLoc + (PreLatestCableLoc - PlayerLoc)*0.91f;

	bool Result = GetWorld()->LineTraceSingleByObjectType(OutHit, PlayerLoc, End, ObjectQueryParams);
	//UKismetSystemLibrary::DrawDebugLine(GetWorld(), PlayerLoc, End,
	//	FLinearColor::Red, 2.0f, 3.0f);

	//�浹�ϴ� ��ü�� ������� ���̺��� Ǯ������
	if (Result)
	{
		//�浹������ �ʹ� ����� �Ÿ��� ��� ����
		if (!PreLatestCableLoc.Equals(OutHit.Location, ToleranceDistance))
			return;
	}
#pragma endregion

#pragma region CalcCableMiddleNormal
	//���̺��� �糡 ������ ������ ���̺� �߰��� ������ ����
	FVector MiddleNormal = ((PreLatestCableNormal + LatestCableNormal) * 0.5f).GetSafeNormal();

	//���̺��� �߰�����
	FVector MiddleStart = PreLatestCableLoc + (LatestCableLoc - PreLatestCableLoc) * 0.5f;

	//UKismetSystemLibrary::DrawDebugLine(GetWorld(), PreLatestCableLoc, PreLatestCableLoc + PreLatestCableNormal*200,
	//		FLinearColor::Green, 1.0f, 13.0f);
	//UKismetSystemLibrary::DrawDebugLine(GetWorld(), LatestCableLoc, LatestCableLoc + LatestCableNormal * 200,
	//	FLinearColor::Red, 1.0f, 13.0f);
	
	/*UKismetSystemLibrary::DrawDebugLine(GetWorld(), MiddleStart, MiddleStart + MiddleNormal * 200,
		FLinearColor::Blue, 1.0f, 13.0f);
	UKismetSystemLibrary::DrawDebugLine(GetWorld(), MiddleStart, MiddleStart + AdjustedMiddleNormal * 200,
		FLinearColor::Yellow, 1.0f, 13.0f);
	UKismetSystemLibrary::DrawDebugLine(GetWorld(), MiddleStart, MiddleStart + CrossMiddleNormal * 200,
		FLinearColor::Black, 1.0f, 13.0f);*/


	//���̺��� �߰������� ���� LineTrace, �浹�ϴ� ��ü�� ������ ��ü�� ��Ȯ�� ������ ���
	if (GetWorld()->LineTraceSingleByObjectType(OutHit, MiddleStart + MiddleNormal, MiddleStart - MiddleNormal*10, ObjectQueryParams))
	{
		//UKismetSystemLibrary::DrawDebugSphere(GetWorld(), OutHitTemp.Location, 100, 12, FLinearColor::Red, 2.0f, 1.0f);
		/*UKismetSystemLibrary::DrawDebugLine(GetWorld(), MiddleStart, MiddleStart + OutHitTemp.Normal * 200,
			FLinearColor::Blue, 1.0f, 13.0f);*/
		UE_LOG(LogTemp, Warning, TEXT("***********HITdaHit********"));
		MiddleNormal = OutHit.Normal;
	}
	//�浹�ϴ� ��ü�� ���ٸ� ���Ƿ� �������� ����ȭ
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("**********No HIT**************"));
		FVector N = (LatestCableLoc - PreLatestCableLoc).GetSafeNormal();
		FVector P = FVector::DotProduct(N, MiddleNormal) * N;
		MiddleNormal = (MiddleNormal - P).GetSafeNormal();
	}
#pragma endregion


	//FPlane Plane(AdjustedMiddleNormal, FVector::DotProduct(AdjustedMiddleNormal, MiddleStart));
	FPlane Plane(MiddleNormal, FVector::DotProduct(MiddleNormal, MiddleStart));

	UKismetSystemLibrary::DrawDebugPlane(GetWorld(), Plane, MiddleStart,
		FVector::Distance(LatestCableLoc, PreLatestCableLoc) * 0.65f, FLinearColor::Black, 0.1f);

	//�ֱ� ���̺��� ���� ���̺��� ���� ������ �Ǵ��� �˻�(������ ����)
	FVector LatestToPlayer = PlayerLoc - LatestCableLoc;
	float DotResult = FVector::DotProduct(MiddleNormal, LatestToPlayer.GetSafeNormal());
	UE_LOG(LogTemp, Warning, TEXT("Dot Result : %f"), DotResult);

	//////////////////////////////////////////////////////
	UKismetSystemLibrary::DrawDebugLine(GetWorld(), MiddleStart + MiddleNormal * 100, MiddleStart - MiddleNormal,
		FLinearColor::Blue, 2.0f, 3.0f);

	//�ֱ� ���̺���� �÷��̾�� ���ϴ� ���Ͱ� ���� ���̺��� ����� ���� ���ԵǸ� ���̺����
	if(DotResult >= 0) 
	{
		UE_LOG(LogTemp, Warning, TEXT("************** DotResult = 0 ****************"));
		
		RemoveLatestCable();
	}
}

bool AMyRope::ToleranceCheck(const FVector& Point)
{
	if (mCableStarts.Num() == 0)
		return true;

	
	FVector& LatestLocation = mCableStarts.Last();
	float Distance = FVector::Distance(Point, LatestLocation);
	
	return Distance < ToleranceDistance;
}

void AMyRope::InitCable(UCableComponent* Cable)
{
	Cable->bAttachStart = true;
	Cable->bAttachEnd = true;

	Cable->EndLocation = FVector(0.f, 0.f, 0.f);
	Cable->CableLength = 100.f;
	Cable->NumSegments = 2;
	Cable->NumSides = 10;

	Cable->SetRelativeLocation(FVector::ZeroVector);
	Cable->SetRelativeScale3D(FVector(1.f, 1.f, 1.f));
}

void AMyRope::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (!bActivated)
		return;
	TraceBetweenAdd();
	TraceBetweenRemove();
}

void AMyRope::BeginPlay()
{
	Super::BeginPlay();
}

void AMyRope::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	InitCable(mCable);
	mCable->SetVisibility(false);
	LatestCable = mCable;
	mCables.Add(mCable);
}

void AMyRope::AllClear()
{
	//0��° Cable�� mCable�̹Ƿ� �״�� �д�.
	for (int i= mCables.Num()-1; i> 0; --i)
	{
		mCables[i]->DestroyComponent();
		mCables.RemoveAt(i);
	}
		
	mCableStarts.Empty();
	mCableNormals.Empty();
	

	InitCable(mCable);
	const FName SocketName(TEXT("RightHandSocket"));

	mCable->SetAttachEndToComponent(OwnerCharacter->GetMesh(), SocketName);
	mCable->SetVisibility(false);
	LatestCable = mCable;
}

void AMyRope::RemoveLatestCable()
{
	mCableStarts.Pop();
	mCableNormals.Pop();
	mCables.Last()->DestroyComponent();
	mCables.Pop();
	LatestCable = mCables.Last();
	LatestCable->SetAttachEndToComponent(OwnerCharacter->GetMesh(), TEXT("RightHandSocket"));
}

void AMyRope::Activate()
{
	mCable->SetVisibility(true);
	bActivated = true;

	OwnerCharacter->Hanging = true;
	OwnerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
	//ĳ���Ͱ� ���� ������ �ٶ� �� �ֵ��� ����
	OwnerCharacter->bUseControllerRotationYaw = false;
	OwnerCharacter->mCamera->bUsePawnControlRotation = false;
	UE_LOG(LogTemp, Warning, TEXT("=============AMyRope Activate==========="));
}

void AMyRope::Deactivate()
{
	bActivated = false;
	AllClear();
	mCable->SetVisibility(false);
	OwnerCharacter->Hanging = false;

	FRotator rot = OwnerCharacter->GetActorRotation();
	rot.Pitch = 0.f;
	OwnerCharacter->SetActorRotation(rot);

	OwnerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	OwnerCharacter->bUseControllerRotationYaw = true;
	OwnerCharacter->mCamera->bUsePawnControlRotation = true;
	 

	UE_LOG(LogTemp, Warning, TEXT("=============AMyRope DeActivate==========="));
}
