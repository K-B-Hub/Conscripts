//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Skill/ActiveSkillBase.h"
#include "PW.h"
#include "Characters/CharacterBase.h"
#include "ActorComponent/BuffComponent.h"
#include "Object/Buff/BuffBase.h"
#include "ActorComponent/AilmentComponent.h"
#include "Object/Ailment/AilmentBase.h"
#include "ActorComponent/PassiveSkillComponent.h"
#include "Actors/Terrain/TerrainBase.h"
#include "Components/BoxComponent.h"

bool UActiveSkillBase::CanExecute() const
{
	ACharacterBase* ownerPtr = GetOwner();
	if (!ownerPtr) return false;

	//행동력 부족 시 사용 불가
	if (ownerPtr->GetCurrentActionPoint() < GetActionPointCost()) return false;

	//전투 자원 부족 시 사용 불가
	if (ownerPtr->GetBattleResource() < battleResourceCost) return false;

	return true;
}

float UActiveSkillBase::GetEffectivePickRange() const
{
	//점프처럼 궤적 이동에 이동력을 쓰는 스킬은 사거리가 곧 현재 이동력으로 갈 수 있는 거리
	//(별도 pickRange 상한 없음 — 이동력이 상한 역할)
	ACharacterBase* ownerPtr = GetOwner();
	if (UsesArcMovement() && ownerPtr && arcMoveCostMultiplier > 0.f)
	{
		return ownerPtr->GetCurrentMovingPoint() * CmPerMeter / arcMoveCostMultiplier;
	}
	return pickRange;
}

void UActiveSkillBase::SetCalcedStats()
{
	if (owner == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActiveSkillBase] SetCalcedStats: 스킬 소유자 없음"));
		return;
	}

	//(캐릭터 atk + 스킬 고정 피해) * 스킬 계수
	//변동 피해는 하한·상한을 모두 들고 있다가 적중 시점에 굴린다, 인디케이터는 이 범위를 그대로 보여준다
	calcDamage = owner->GetAtk() * damageRatio + baseDamage;
	calcDamageMax = HasDamageRange()
		? owner->GetAtk() * damageRatioMax + baseDamage
		: calcDamage;
	//캐릭터 명중 + 스킬 보너스 명중
	calcAccuracy = owner->GetAccuracy() + bonusAccuracy;
	//캐릭터 치명타 + 스킬 보너스 치명타
	//광역은 기본 봉쇄, 투척만 해금 패시브로 예외가 되어 일반 공격과 동일하게 적용
	const UPassiveSkillComponent* passiveComp = owner->GetPassiveSkillComponent();
	const bool bThrowCritUnlocked = (skillType == ESkillType::Throw)
		&& passiveComp && passiveComp->HasThrowCriticalUnlock();
	const bool bAreaCritBlocked = (damageType == EDamageType::Area) && !bThrowCritUnlocked;
	calcCritical = bAreaCritBlocked ? 0.f : owner->GetCritical() + bonusCritical;
	//캐릭터 피해 증폭 + 스킬 보너스 피해 증폭
	calcDamageAmplfication = owner->GetDamageAmplification() + bonusDamageAmplication;
	//캐릭터 관통력 + 스킬 보너스 관통력
	calcPenetration = owner->GetPenetration() + bonusPenetration;
}

int32 UActiveSkillBase::GetActionPointCost() const
{
	ACharacterBase* ownerPtr = GetOwner();
	const UPassiveSkillComponent* passiveComp = ownerPtr ? ownerPtr->GetPassiveSkillComponent() : nullptr;
	if (!passiveComp) return actionPointCost;

	//할인이 쌓여도 공짜 스킬은 되지 않는다
	const int32 discount = passiveComp->GetActionPointDiscount(GetClass(), skillType);
	return FMath::Max(1, actionPointCost - discount);
}

void UActiveSkillBase::BeginUse()
{
	ACharacterBase* ownerPtr = GetOwner();
	if (!ownerPtr) return;

	ownerPtr->ReduceActionPoint(GetActionPointCost());
	ownerPtr->ReduceBattleResource(battleResourceCost);
}

void UActiveSkillBase::OnCommit()
{
	if (!spawnTerrainClass) return;

	const ACharacterBase* ownerPtr = GetOwner();
	UWorld* world = ownerPtr ? ownerPtr->GetWorld() : nullptr;
	if (!world) return;

	//시전 지점은 지면 좌표, 박스 중심을 반높이만큼 올려 지형이 땅 위에 서게 한다
	const ATerrainBase* defaultTerrain = spawnTerrainClass->GetDefaultObject<ATerrainBase>();
	const UBoxComponent* defaultBox = defaultTerrain ? defaultTerrain->GetTerrainBox() : nullptr;
	FVector spawnLocation = skillPointLocation;
	if (defaultBox)
	{
		spawnLocation.Z += defaultBox->GetUnscaledBoxExtent().Z;
	}

	world->SpawnActor<ATerrainBase>(spawnTerrainClass, FTransform(spawnLocation));
}

void UActiveSkillBase::Execute(const ACharacterBase* target)
{
	if (!target) return;

	ACharacterBase* ownerPtr = GetOwner();

	//버프 적용
	if (buffs.Num() > 0)
	{
		if (UBuffComponent* targetBuffComp = target->GetBuffComponent())
		{
			for (const TSubclassOf<UBuffBase>& buffClass : buffs)
			{
				targetBuffComp->AddBuff(buffClass, ownerPtr);
			}
		}
	}

	//상태이상 적용
	if (ailments.Num() > 0)
	{
		if (UAilmentComponent* targetAilmentComp = target->GetAilmentComponent())
		{
			for (const TSubclassOf<UAilmentBase>& ailmentClass : ailments)
			{
				targetAilmentComp->AddAilment(ailmentClass, ownerPtr);
			}
		}
	}

	//밀치기, force 0이면 비활성
	if (knockbackForce > 0.f)
	{
		const FVector origin = (knockbackOrigin == EKnockbackOrigin::SkillPoint || !ownerPtr)
			? skillPointLocation
			: ownerPtr->GetActorLocation();
		//이동은 대상 액터를 직접 변경, 컴포넌트 경유 변경과 동일한 const 예외
		const_cast<ACharacterBase*>(target)->ApplyKnockback(origin, knockbackForce, knockbackAngle);
	}
}
