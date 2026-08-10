//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Skill/ActiveSkillBase.h"
#include "Characters/CharacterBase.h"
#include "ActorComponent/BuffComponent.h"
#include "Object/Buff/BuffBase.h"
#include "ActorComponent/AilmentComponent.h"
#include "Object/Ailment/AilmentBase.h"

bool UActiveSkillBase::CanExecute() const
{
	ACharacterBase* ownerPtr = GetOwner();
	if (!ownerPtr) return false;

	//행동력 부족 시 사용 불가
	if (ownerPtr->GetCurrentActionPoint() < actionPointCost) return false;

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
		return ownerPtr->GetCurrentMovingPoint() * 100.f / arcMoveCostMultiplier;
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
	calcDamage = owner->GetAtk() * damageRatio + baseDamage;
	//캐릭터 명중 + 스킬 보너스 명중
	calcAccuracy = owner->GetAccuracy() + bonusAccuracy;
	//캐릭터 치명타 + 스킬 보너스 치명타
	if (damageType == EDamageType::Area)
	{
		calcCritical = 0;
	}
	else
	{
		calcCritical = owner->GetCritical() + bonusCritical;
	}
	//캐릭터 피해 증폭 + 스킬 보너스 피해 증폭
	calcDamageAmplfication = owner->GetDamageAmplification() + bonusDamageAmplication;
	//캐릭터 관통력 + 스킬 보너스 관통력
	calcPenetration = owner->GetPenetration() + bonusPenetration;
}

void UActiveSkillBase::BeginUse()
{
	ACharacterBase* ownerPtr = GetOwner();
	if (!ownerPtr) return;

	ownerPtr->ReduceActionPoint(actionPointCost);
	ownerPtr->ReduceBattleResource(battleResourceCost);
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
}
