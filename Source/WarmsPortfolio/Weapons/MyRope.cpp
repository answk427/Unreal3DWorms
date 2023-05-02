// Fill out your copyright notice in the Description page of Project Settings.


#include "MyRope.h"
#include "../Character/PlayerCharacter.h"
#include "CableComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"



AMyRope::AMyRope()
{
	//PrimaryActorTick.bCanEverTick = false;
	mRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(mRoot);
	mCable = CreateDefaultSubobject<UCableComponent>(TEXT("NinjaRope"));
	mCable->SetupAttachment(mRoot);
	mCable->SetVisibility(false);
	
	/*ConstructorHelpers::FObjectFinder<UStaticMesh> mesh(TEXT("StaticMesh'/Game/M5VFXVOL2/Props/Meshes/BF_mesh_brazier.BF_mesh_brazier'"));

	mStaticMesh->SetStaticMesh(mesh.Object);*/
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

	//두개의 접벡터를 기저벡터로 사용
	FVector RightTangentVec, UpTangentVec;
	GetTangentVector(RightTangentVec, UpTangentVec);

	FVector ForceVec = RightTangentVec * XInput + UpTangentVec * YInput;
	//MovementComponent->AddForce(ForceVec.GetSafeNormal()*MovementPower);
	MovementComponent->AddInputVector(ForceVec.GetSafeNormal()*MovementPower);
	//Character->SetActorLocation(Character->GetActorLocation() + ForceVec.GetSafeNormal()*MovementPower);

	/*FRotator Rot = UKismetMathLibrary::FindLookAtRotation(Character->GetActorLocation(), GetActorLocation());
	Character->SetActorRotation(Rot);*/
}

void AMyRope::GetTangentVector(FVector& RightTangentVec, FVector& UpTangentVec)
{
	if (!OwnerCharacter.IsValid())
		return;

	APlayerCharacter* Character = OwnerCharacter.Get();

	FVector PlayerLocation = Character->GetActorLocation();

	//로프가 시작되는 곳을 중심으로 구를 생성한다고 가정
	FVector SphereMidPoint = GetActorLocation();

	//플레이어에서 구의 중심으로 향하는 벡터. 법선벡터의 반대방향
	FVector InverseNormalVec = SphereMidPoint - PlayerLocation;

	//플레이어의 우측을 향하는 접선벡터를 구함
	RightTangentVec = -FVector::CrossProduct(InverseNormalVec.GetSafeNormal(), FVector::UpVector);
	//플레이어의 위쪽을 향하는 접선벡터를 구함
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

	if (GetWorld()->LineTraceSingleByObjectType(OutHit, Start, End, ObjectQueryParams))
	{
		UE_LOG(LogTemp, Warning, TEXT("LineTrace Success Hit Object : %s"), *OutHit.GetActor()->GetName());
		UKismetSystemLibrary::DrawDebugLine(GetWorld(), Start, End,
			FLinearColor::Green, 2.0f, 3.0f);
		UKismetSystemLibrary::DrawDebugSphere(GetWorld(), OutHit.Location, 100, 12, FLinearColor::Red, 2.0f, 1.0f);
		UKismetSystemLibrary::DrawDebugSphere(GetWorld(), OutHit.ImpactPoint, 80, 12, FLinearColor::Green, 2.0f, 1.0f);
		UKismetSystemLibrary::DrawDebugSphere(GetWorld(), OutHit.TraceEnd, 60, 12, FLinearColor::Blue, 2.0f, 1.0f);
		AllClear();
		mCable->SetVisibility(true);
		SetActorLocation(OutHit.ImpactPoint);
		mCableStarts.Add(OutHit.ImpactPoint);
		mCableNormals.Add(OutHit.ImpactNormal);
		bActivated = true;
		OwnerCharacter->Hanging = true;
		OwnerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
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


	//시작지점, 캐릭터 손
	FVector Start = OwnerCharacter->mGunPos->GetComponentLocation();

	//종료지점, 최근 케이블의 시작위치
	FVector End = LatestCable->GetComponentLocation();


	//UE_LOG(LogTemp, Warning, TEXT("ActorLocation : %s     /    CableComponentLocation : %s"), *Start.ToString(), *End.ToString());

	FHitResult OutHit;
	FCollisionObjectQueryParams ObjectQueryParams(ECollisionChannel::ECC_WorldStatic);


	/*UKismetSystemLibrary::DrawDebugLine(GetWorld(), Start, End,
		FLinearColor::Red, 0.1f, 5.0f); */
	
	//케이블에 걸린 물체가 있을 때
	if (GetWorld()->LineTraceSingleByObjectType(OutHit, Start, End, ObjectQueryParams))
	{
		//너무 좁은 거리인지 검사
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

		//케이블을 Hit된 물체에 부착
		newCable->AttachToComponent(OutHit.GetComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		newCable->SetWorldLocation(OutHit.Location);
		newCable->SetAttachEndToComponent(OwnerCharacter->GetMesh(), TEXT("RightHandSocket"));


		//이전에 있던 케이블의 끝은 새로 생긴 케이블에 연결
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
	//케이블이 2개 이상일때만 실행
	if (mCableStarts.Num() < 2)
		return;

	int idx = mCableStarts.Num() - 2;
	//플레이어의 손 위치
	FVector PlayerLoc = OwnerCharacter->GetMesh()->GetSocketLocation(TEXT("RightHandSocket"));
	
	//최근 케이블의 시작위치
	FVector& LatestCableLoc = mCableStarts.Last();

	//플레이어와 케이블이 딱 붙어있을 때
	if (PlayerLoc.Equals(LatestCableLoc, ToleranceDistance))
	{
		RemoveLatestCable();
		return;
	}

	FVector& LatestCableNormal = mCableNormals.Last();

	//최근 케이블 바로 이전 케이블의 시작위치
	FVector& PreLatestCableLoc = mCableStarts[idx];
	FVector& PreLatestCableNormal = mCableNormals[idx];
	

#pragma region PlayerToPreTrace
	//플레이어에서 이전 케이블방향으로 충돌하는 물체가 있는지 검사
	FHitResult OutHit;
	FCollisionObjectQueryParams ObjectQueryParams(ECollisionChannel::ECC_WorldStatic);
	FVector End = PlayerLoc + (PreLatestCableLoc - PlayerLoc)*0.91f;

	bool Result = GetWorld()->LineTraceSingleByObjectType(OutHit, PlayerLoc, End, ObjectQueryParams);
	//UKismetSystemLibrary::DrawDebugLine(GetWorld(), PlayerLoc, End,
	//	FLinearColor::Red, 2.0f, 3.0f);

	//충돌하는 물체가 있을경우 케이블을 풀지않음
	if (Result)
	{
		//충돌했지만 너무 가까운 거리일 경우 제외
		if (!PreLatestCableLoc.Equals(OutHit.Location, ToleranceDistance))
			return;
	}
#pragma endregion

#pragma region CalcCableMiddleNormal
	//케이블의 양끝 법선을 보간해 케이블 중간의 법선을 구함
	FVector MiddleNormal = ((PreLatestCableNormal + LatestCableNormal) * 0.5f).GetSafeNormal();

	//케이블의 중간지점
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


	//케이블의 중간지점을 향해 LineTrace, 충돌하는 물체가 있으면 물체의 정확한 법선을 사용
	if (GetWorld()->LineTraceSingleByObjectType(OutHit, MiddleStart + MiddleNormal, MiddleStart - MiddleNormal*10, ObjectQueryParams))
	{
		//UKismetSystemLibrary::DrawDebugSphere(GetWorld(), OutHitTemp.Location, 100, 12, FLinearColor::Red, 2.0f, 1.0f);
		/*UKismetSystemLibrary::DrawDebugLine(GetWorld(), MiddleStart, MiddleStart + OutHitTemp.Normal * 200,
			FLinearColor::Blue, 1.0f, 13.0f);*/
		UE_LOG(LogTemp, Warning, TEXT("***********HITdaHit********"));
		MiddleNormal = OutHit.Normal;
	}
	//충돌하는 물체가 없다면 임의로 법선벡터 직교화
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

	//최근 케이블이 이전 케이블의 평면과 평행이 되는지 검사(법선과 내적)
	FVector LatestToPlayer = PlayerLoc - LatestCableLoc;
	float DotResult = FVector::DotProduct(MiddleNormal, LatestToPlayer.GetSafeNormal());
	UE_LOG(LogTemp, Warning, TEXT("Dot Result : %f"), DotResult);

	//////////////////////////////////////////////////////
	UKismetSystemLibrary::DrawDebugLine(GetWorld(), MiddleStart + MiddleNormal * 100, MiddleStart - MiddleNormal,
		FLinearColor::Blue, 2.0f, 3.0f);

	//최근 케이블에서 플레이어로 향하는 벡터가 이전 케이블의 평면의 위로 가게되면 케이블삭제
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
	//TraceBetweenRemove();
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
	//0번째 Cable은 mCable이므로 그대로 둔다.
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
	
}

void AMyRope::Deactivate()
{
	bActivated = false;
	AllClear();
	mCable->SetVisibility(false);
	OwnerCharacter->Hanging = false;
	OwnerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}
