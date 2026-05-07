// Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Skill/ActiveSkillBase.h"
#include "Characters/CharacterBase.h"
#include "Animation/AnimMontage.h"
#include "ActorComponent/BuffComponent.h"
#include "Object/Buff/BuffBase.h"

bool UActiveSkillBase::CanExecute() const
{
	ACharacterBase* ownerPtr = GetOwner();
	if (!ownerPtr) return false;

	// 행동력 부족 시 사용 불가
	if (ownerPtr->GetCurrentActionPoint() < actionPointCost) return false;

	// 전투 자원 부족 시 사용 불가
	if (ownerPtr->GetBattleResource() < battleResourceCost) return false;

	return true;
}

void UActiveSkillBase::SetCalcedStats()
{
	ACharacterBase* ownerPtr = GetOwner();
	if (!ownerPtr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActiveSkillBase] SetCalcedStats: 스킬 소유자 없음"));
		return;
	}

	// (캐릭터 atk + 스킬 고정 피해) * 스킬 계수
	calcDamage = ownerPtr->GetAtk() * damageRatio + baseDamage;
	// 캐릭터 명중 + 스킬 보너스 명중
	calcAccuracy = ownerPtr->GetAccuracy() + bonusAccuracy;
	// 캐릭터 치명타 + 스킬 보너스 치명타
	if (damageType == EDamageType::Area)
	{
		calcCritical = 0;
	}
	else
	{
		calcCritical = ownerPtr->GetCritical() + bonusCritical;
	}
	// 캐릭터 피해 증폭 + 스킬 보너스 피해 증폭
	calcDamageAmplfication = ownerPtr->GetDamageAmplification() + bonusDamageAmplication;
	// 캐릭터 관통력 + 스킬 보너스 관통력
	calcPenetration = ownerPtr->GetPenetration() + bonusPenetration;
}

void UActiveSkillBase::BeginUse()
{
	ACharacterBase* ownerPtr = GetOwner();
	if (!ownerPtr) return;

	ownerPtr->ReduceActionPoint(actionPointCost);
	ownerPtr->ReduceBattleResource(battleResourceCost);

	if (skillMontage)
	{
		const float Duration = ownerPtr->PlayAnimMontage(skillMontage, montagePlayRate);
		UE_LOG(LogTemp, Log, TEXT("[ActiveSkillBase] 몽타주 재생: %s (Duration: %.2f) — 0이면 ABP의 AnimGraph에 DefaultSlot 노드 필요"),
			*skillMontage->GetName(), Duration);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ActiveSkillBase] skillMontage가 nullptr — 에셋 경로 확인 필요"));
	}
}

void UActiveSkillBase::Execute(const ACharacterBase* target)
{
	Super::Execute(target);

	if (!target) return;

	ACharacterBase* ownerPtr = GetOwner();

	// 버프 적용 — 스킬에 등록된 버프를 적중 대상의 BuffComponent에 추가
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
}
