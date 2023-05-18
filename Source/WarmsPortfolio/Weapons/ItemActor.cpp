// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemActor.h"

#include "Weapon.h"
#include "NiagaraComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"

// Sets default values
AItemActor::AItemActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	mItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemActorMesh"));
	SetRootComponent(mItemMesh);

	mItemEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ItemActorEffect"));

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NiagaraSystem(TEXT("NiagaraSystem'/Game/sA_PickupSet_1/Fx/NiagaraSystems/NS_ItemActor.NS_ItemActor'"));
	if(NiagaraSystem.Succeeded())
	{
		mItemEffect->SetAsset(NiagaraSystem.Object);
	}
	mItemEffect->SetupAttachment(mItemMesh);
	//mItemEffect->GetComponentTransform()
	//mItemEffect->GetRelativeTransform()
	mGetButtonWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("GetButtonWidget"));
	mGetButtonWidget->SetupAttachment(mItemMesh);
	mGetButtonWidget->SetWidgetSpace(EWidgetSpace::Screen);

	ConstructorHelpers::FClassFinder<UUserWidget> GetButton(TEXT("WidgetBlueprint'/Game/UI/GetButtonWidget.GetButtonWidget_C'"));
	if(GetButton.Succeeded())
	{
		mGetButtonWidget->SetWidgetClass(GetButton.Class);
		mGetButtonWidget->SetDrawSize(FVector2D(150.f, 100.f));
		mGetButtonWidget->SetVisibility(false);
	}
	
}

// Called when the game starts or when spawned
void AItemActor::BeginPlay()
{
	Super::BeginPlay();

	//메쉬의 크기에 맞게 이펙트 크기를 조정.
	FVector basicVec(1.f, 1.f, 1.f);
	FVector parentScale = mItemMesh->GetComponentScale();
	basicVec /= parentScale;
		
	mItemEffect->SetRelativeScale3D(basicVec);

	mCollider->OnComponentBeginOverlap.AddDynamic(this, &AItemActor::OnComponentBeginOverlap);
	mCollider->OnComponentEndOverlap.AddDynamic(this, &AItemActor::OnComponentEndOverlap);

	//메쉬의 머리쪽에 위젯이 나오도록 위치설정
	FVector Min, Max;
	mItemMesh->GetLocalBounds(Min, Max);
	float MeshHeight = Max.Z - Min.Z;
	UE_LOG(LogTemp, Warning, TEXT("Mesh Height : %f"), MeshHeight);
	mGetButtonWidget->SetRelativeLocation(FVector(0.f, 0.f, MeshHeight));
}

// Called every frame
void AItemActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void AItemActor::SetItemInfo(const FWPItem& Item)
{
	if (!mItemInfo.IsValid())
	{
		mItemInfo = MakeUnique<FWPItem>(Item);
	}
	else
	{
		*mItemInfo.Get() = Item;
	}
}

void AItemActor::Init(const AWeapon* WeaponSrc, const FWPItem& ItemSrc)
{
	mItemMesh->SetStaticMesh(WeaponSrc->mMeshComponent->GetStaticMesh());
	USkeletalMeshComponent* a;
	a->SkeletalMesh
	
	UStaticMeshComponent* StaticMeshComponentSrc = WeaponSrc->mMeshComponent;
	
	UStaticMesh* DuplicateMesh = DuplicateObject<UStaticMesh>(StaticMeshComponentSrc->GetStaticMesh(), StaticMeshComponentSrc);
	mItemMesh->SetStaticMesh(DuplicateMesh);
	mItemMesh->SetMaterial(0, StaticMeshComponentSrc->GetMaterial(0));
	mItemMesh->SetRelativeTransform(StaticMeshComponentSrc->GetRelativeTransform());
	mItemMesh->SetCollisionProfileName(StaticMeshComponentSrc->GetCollisionProfileName());
	mItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SetItemInfo(ItemSrc);

	const UShapeComponent* Collider = WeaponSrc->GetCollider();
	const USphereComponent* SphereComponent = Cast<USphereComponent>(Collider);
	

	if(Collider != nullptr)
	{
		UShapeComponent* newColl = DuplicateObject<USphereComponent>(SphereComponent, this);
		if (newColl != nullptr)
		{
			mCollider = newColl;
					
			//루트컴포넌트 변경
			auto PreRootComponent = GetRootComponent();
			SetRootComponent(mCollider);
			PreRootComponent->SetupAttachment(mCollider);

			mCollider->SetGenerateOverlapEvents(true);
			mCollider->SetCollisionProfileName(TEXT("ItemActor"));

			if (mCollider->OnComponentBeginOverlap.IsBound())
			{
				UE_LOG(LogTemp, Warning, TEXT("mCollider is Bounded"));
				if(mCollider->IsCollisionEnabled())
				{
					UE_LOG(LogTemp, Warning, TEXT("IsCollisionEnabled True"));
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("IsCollisionEnabled False"));
				}
			}
			else
				UE_LOG(LogTemp, Warning, TEXT("mCollider is Not Bounded"));
				

		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("newColl is Nullptr"));
		}
	}
	
		

}

void AItemActor::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("ItemActor BeginOverlap"));
	mGetButtonWidget->SetVisibility(true);
}

void AItemActor::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("ItemActor EndOverlap"));
	mGetButtonWidget->SetVisibility(false);
}


