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
	PrimaryActorTick.bCanEverTick = true;

	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DummyRoot"));
	SetRootComponent(DummyRoot);
	/*mItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemActorMesh"));
	SetRootComponent(mItemMesh);*/

	mItemEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ItemActorEffect"));

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NiagaraSystem(TEXT("NiagaraSystem'/Game/sA_PickupSet_1/Fx/NiagaraSystems/NS_ItemActor.NS_ItemActor'"));
	if(NiagaraSystem.Succeeded())
	{
		mItemEffect->SetAsset(NiagaraSystem.Object);
	}
	mItemEffect->SetupAttachment(GetRootComponent());
	//mItemEffect->GetComponentTransform()
	//mItemEffect->GetRelativeTransform()
	mGetButtonWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("GetButtonWidget"));
	mGetButtonWidget->SetupAttachment(GetRootComponent());
	mGetButtonWidget->SetWidgetSpace(EWidgetSpace::Screen);

	ConstructorHelpers::FClassFinder<UUserWidget> GetButton(TEXT("WidgetBlueprint'/Game/UI/GetButtonWidget.GetButtonWidget_C'"));
	if(GetButton.Succeeded())
	{
		mGetButtonWidget->SetWidgetClass(GetButton.Class);
		mGetButtonWidget->SetDrawSize(FVector2D(150.f, 100.f));
		mGetButtonWidget->SetVisibility(false);
	}
		
}

void AItemActor::AdjustEffectSize()
{
	//�޽��� ũ�⿡ �°� ����Ʈ ũ�⸦ ����.
	FVector basicVec(1.f, 1.f, 1.f);
	FVector parentScale = mItemMesh->GetComponentScale();
	basicVec /= parentScale;

	mItemEffect->SetRelativeScale3D(basicVec);
}

void AItemActor::InitButtonWidget()
{
	//�޽��� �Ӹ��ʿ� ������ �������� ��ġ����
	UStaticMeshComponent* StaticMesh = Cast<UStaticMeshComponent>(mItemMesh);
	USkeletalMeshComponent* SkeletalMesh = Cast<USkeletalMeshComponent>(mItemMesh);

	
	float MeshHeight; 
	if(StaticMesh)
	{
		FVector Min, Max;
		StaticMesh->GetLocalBounds(Min, Max);
		MeshHeight = Max.Z - Min.Z;
	}
	else if(SkeletalMesh)
	{
		MeshHeight = SkeletalMesh->Bounds.GetBox().GetExtent().Z;
	}
	
	
	UE_LOG(LogTemp, Warning, TEXT("Mesh Height : %f"), MeshHeight);
	mGetButtonWidget->SetRelativeLocation(FVector(0.f, 0.f, MeshHeight));
}

void AItemActor::InitCollider(const AWeapon* WeaponSrc)
{
	const UShapeComponent* Collider = WeaponSrc->GetCollider();
	const USphereComponent* SphereComponent = Cast<USphereComponent>(Collider);
		
	if (Collider != nullptr)
	{
		UShapeComponent* newColl = DuplicateObject<USphereComponent>(SphereComponent, this);
		if (newColl != nullptr)
		{
			mCollider = newColl;

			mCollider->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

			mCollider->SetGenerateOverlapEvents(true);
			mCollider->SetCollisionProfileName(TEXT("ItemActor"));
			mCollider->SetCanEverAffectNavigation(false);
			if (mCollider->OnComponentBeginOverlap.IsBound())
			{
				UE_LOG(LogTemp, Warning, TEXT("mCollider is Bounded"));
				if (mCollider->IsCollisionEnabled())
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

void AItemActor::InitMeshComponent(const AWeapon* WeaponSrc)
{
	if (!WeaponSrc)
		return;
	UMeshComponent* SrcComponent = WeaponSrc->GetMesh();

	if (!SrcComponent)
		return;

	//Find WeaponSrc's MeshComponent Static or Skeleteal
	UStaticMeshComponent* MeshComponent = Cast<UStaticMeshComponent>(SrcComponent);
	USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(SrcComponent);

	//Duplicate Src to mItemMesh
	if (MeshComponent)
	{
		mItemMesh = NewObject<UStaticMeshComponent>(this);
		
		UStaticMeshComponent* ItemMesh = (UStaticMeshComponent*)mItemMesh;
		UStaticMesh* DuplicateMesh = DuplicateObject<UStaticMesh>(MeshComponent->GetStaticMesh(), mItemMesh);
		ItemMesh->SetStaticMesh(DuplicateMesh);
		ItemMesh->SetMaterial(0, MeshComponent->GetMaterial(0));
		ItemMesh->SetRelativeTransform(FTransform());
		ItemMesh->SetCollisionProfileName(MeshComponent->GetCollisionProfileName());
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	else if (SkeletalMeshComponent)
	{
		mItemMesh = NewObject<USkeletalMeshComponent>(this);
		UE_LOG(LogTemp, Warning, TEXT("AItemActor Init SkeletalMeshComponent pos : %s"), *mItemMesh->GetRelativeLocation().ToString());
		USkeletalMeshComponent* SkeletalItemMesh = (USkeletalMeshComponent*)mItemMesh;
		USkeletalMesh* DuplicateMesh = DuplicateObject<USkeletalMesh>(SkeletalMeshComponent->SkeletalMesh, mItemMesh);
		SkeletalItemMesh->SetSkeletalMesh(DuplicateMesh);
		SkeletalItemMesh->SetMaterial(0, SkeletalMeshComponent->GetMaterial(0));
		SkeletalItemMesh->SetRelativeTransform(FTransform());
		SkeletalItemMesh->SetCollisionProfileName(SkeletalMeshComponent->GetCollisionProfileName());
		SkeletalItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	mItemMesh->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	mItemMesh->RegisterComponent();
	mItemMesh->SetRelativeLocation(FVector::ZeroVector);
	
}

// Called when the game starts or when spawned
void AItemActor::BeginPlay()
{
	Super::BeginPlay();

	mCollider->OnComponentBeginOverlap.AddDynamic(this, &AItemActor::OnComponentBeginOverlap);
	mCollider->OnComponentEndOverlap.AddDynamic(this, &AItemActor::OnComponentEndOverlap);
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
	if (!WeaponSrc)
		return;

	InitCollider(WeaponSrc);
	InitMeshComponent(WeaponSrc);
	SetItemInfo(ItemSrc);
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


