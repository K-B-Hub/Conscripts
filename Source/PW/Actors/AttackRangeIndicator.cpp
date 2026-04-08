// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/AttackRangeIndicator.h"
#include "Components/DecalComponent.h"
#include "Components/SphereComponent.h"
#include "Characters/CharacterBase.h"
#include "Characters/AllyCharacterBase.h"
#include "Characters/EnemyBase.h"
#include "ActorComponent/SkillComponent.h"
#include "Object/Skill/ActiveSkillBase.h"

AAttackRangeIndicator::AAttackRangeIndicator()
{
	PrimaryActorTick.bCanEverTick = true;

	rootScene = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = rootScene;

	// 오버랩 감지용 구체 — Pawn 채널만 오버랩, 나머지 무시
	overlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
	overlapSphere->SetupAttachment(RootComponent);
	overlapSphere->SetSphereRadius(1.f);
	overlapSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	overlapSphere->SetCollisionObjectType(ECC_WorldDynamic);
	overlapSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	overlapSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	overlapSphere->SetGenerateOverlapEvents(true);

	// 범위 공격 데칼 — 기본 비활성, InitIndicator에서 필요 시 활성화
	areaDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("AreaDecal"));
	areaDecal->SetupAttachment(RootComponent);
	areaDecal->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));
	areaDecal->SetVisibility(false);
}

void AAttackRangeIndicator::BeginPlay()
{
	Super::BeginPlay();

	overlapSphere->OnComponentBeginOverlap.AddDynamic(this, &AAttackRangeIndicator::OnOverlapBegin);
	overlapSphere->OnComponentEndOverlap.AddDynamic(this, &AAttackRangeIndicator::OnOverlapEnd);
}

void AAttackRangeIndicator::InitIndicator(ACharacterBase* InCaster, UActiveSkillBase* Skill)
{
	caster = InCaster;
	cachedSkill = Skill;
	pickRange = Skill->pickRange;
	bIsAreaAttack = (Skill->areaTarget != EAreaTarget::None);
	bFixedAtCaster = (Skill->selectMode == ESelectMode::Self);

	if (bIsAreaAttack)
	{
		// 범위 공격 — 데칼 표시, 오버랩 구체 크기를 범위에 맞게 설정
		if (areaDecalMaterial)
		{
			areaDecal->SetDecalMaterial(areaDecalMaterial);
		}
		areaDecal->SetVisibility(true);

		float OverlapRadius = 0.f;
		switch (Skill->areaForm)
		{
		case EAreaForm::Circle:
			OverlapRadius = Skill->areaParameter1;
			areaDecal->DecalSize = FVector(decalProjectionDepth, Skill->areaParameter1, Skill->areaParameter1);
			break;
		case EAreaForm::Cone:
			OverlapRadius = Skill->areaParameter1;
			areaDecal->DecalSize = FVector(decalProjectionDepth, Skill->areaParameter1, Skill->areaParameter1);
			break;
		case EAreaForm::Ray:
			OverlapRadius = FMath::Max(Skill->areaParameter1, Skill->areaParameter2) * 0.5f;
			areaDecal->DecalSize = FVector(decalProjectionDepth, Skill->areaParameter1 * 0.5f, Skill->areaParameter2 * 0.5f);
			break;
		}
		overlapSphere->SetSphereRadius(FMath::Max(OverlapRadius, 1.f));
	}
	else
	{
		// 단일 대상 — 데칼 비활성, 매우 작은 구체로 점 오버랩
		areaDecal->SetVisibility(false);
		overlapSphere->SetSphereRadius(1.f);
	}
}

// ─── Tick: 마우스 추적 + 사거리 제한 ────────────────────────────────────────────

void AAttackRangeIndicator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Self 모드 — 시전자 위치에 고정, 마우스 추적 불필요
	if (bFixedAtCaster)
	{
		if (caster.IsValid())
		{
			SetActorLocation(caster->GetActorLocation(), false, nullptr, ETeleportType::TeleportPhysics);
		}
		return;
	}

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;

	FHitResult HitResult;
	if (!PC->GetHitResultUnderCursor(ECC_Visibility, false, HitResult)) return;

	FVector TargetLocation = HitResult.Location;

	// pickRange > 0이면 시전자 기준 사거리 내로 제한
	if (pickRange > 0.f && caster.IsValid())
	{
		const FVector CasterLocation = caster->GetActorLocation();
		FVector Offset = TargetLocation - CasterLocation;
		Offset.Z = 0.f;

		if (Offset.SizeSquared() > pickRange * pickRange)
		{
			Offset = Offset.GetSafeNormal() * pickRange;
			TargetLocation = CasterLocation + Offset;
			TargetLocation.Z = HitResult.Location.Z;
		}
	}

	SetActorLocation(TargetLocation, false, nullptr, ETeleportType::TeleportPhysics);
}

// ─── EAreaTarget 필터 ────────────────────────────────────────────────────────

bool AAttackRangeIndicator::IsAreaTarget(ACharacterBase* Character) const
{
	if (!cachedSkill) return false;

	switch (cachedSkill->areaTarget)
	{
	case EAreaTarget::EnemyOnly:
		return Cast<AEnemyBase>(Character) != nullptr;
	case EAreaTarget::AllyOnly:
		return Cast<AAllyCharacterBase>(Character) != nullptr;
	case EAreaTarget::All:
		return true;
	case EAreaTarget::None:
		// 단일 대상 모드 — EAreaTarget 필터 없음, EPickTeam은 클릭 시 BattleController에서 검증
		return true;
	}
	return false;
}

// ─── 오버랩 콜백 ────────────────────────────────────────────────────────────────

void AAttackRangeIndicator::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacterBase* Character = Cast<ACharacterBase>(OtherActor);
	if (!Character) return;

	// 시전자 자신은 제외
	if (caster.IsValid() && Character == caster.Get()) return;

	// EAreaTarget 기준으로 스킬 적용 대상이 아니면 무시
	if (!IsAreaTarget(Character)) return;

	overlappingTargets.AddUnique(Character);

	// SkillComponent 타겟 목록에 직접 추가
	if (caster.IsValid())
	{
		if (USkillComponent* SkillComp = caster->FindComponentByClass<USkillComponent>())
		{
			SkillComp->AddTarget(Character);
		}
	}

	// Skill의 calc 스탯으로 전투 예측 계산 및 위젯 표시
	if (cachedSkill)
	{
		Character->CalculateDamage(
			cachedSkill->calcDamage,
			cachedSkill->calcAccuracy,
			cachedSkill->calcCritical,
			cachedSkill->calcDamageAmplfication,
			cachedSkill->calcPenetration
		);
		Character->ShowSkillInfo();
	}
}

void AAttackRangeIndicator::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ACharacterBase* Character = Cast<ACharacterBase>(OtherActor);
	if (!Character) return;

	// IsAreaTarget을 통과하지 못한 캐릭터는 목록에 없으므로 그냥 제거 시도
	overlappingTargets.Remove(Character);

	// SkillComponent 타겟 목록에서 직접 제거
	if (caster.IsValid())
	{
		if (USkillComponent* SkillComp = caster->FindComponentByClass<USkillComponent>())
		{
			SkillComp->RemoveTarget(Character);
		}
	}

	// 전투 예측 위젯 숨김 및 pending 값 초기화
	Character->HideSkillInfo();
	Character->ClearPendingDamage();
}