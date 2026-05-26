// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponent/PassiveSkillComponent.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "Characters/CharacterBase.h"
#include "Enum/SkillTypes.h"

UPassiveSkillComponent::UPassiveSkillComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPassiveSkillComponent::BeginPlay()
{
	Super::BeginPlay();

	ownerCharacter = Cast<ACharacterBase>(GetOwner());
}

void UPassiveSkillComponent::AddPassive(TSubclassOf<UPassiveSkillBase> passiveClass)
{
	if (!passiveClass || !ownerCharacter) return;

	UPassiveSkillBase* instance = NewObject<UPassiveSkillBase>(this, passiveClass);
	if (!instance) return;

	instance->SetOwner(ownerCharacter);
	activePassives.Add(instance);

	switch (instance->passiveType)
	{
	case EPassiveType::Stat:
		// 등록 즉시 영구 스탯 가산 — 제거 시까지 유지
		statPassives.Add(instance);
		ownerCharacter->ApplyPassiveStatDelta(instance, +1);
		break;
	case EPassiveType::Reactive:
		reactivePassives.Add(instance);
		// BeforeMove 패시브는 등록 시점에 현재 isMoved 상태로 즉시 동기화
		// (게임 시작 시 미이동 보너스 자동 적용, 전투 중 영입 시에도 현재 상태에 맞춰 적용)
		if (instance->reactiveType == EReactiveType::BeforeMove)
		{
			// BeforeMove는 인디케이터 hover로도 토글되므로 hp/movingPoint/actionPoint 변동은 부수효과가 큼
			// 데이터 실수 방지 — 0이 아니면 경고 + 강제 0
			if (instance->hp != 0 || instance->movingPoint != 0.f || instance->actionPoint != 0)
			{
				UE_LOG(LogTemp, Warning, TEXT("[PassiveSkillComponent] BeforeMove 패시브 '%s'에 hp/movingPoint/actionPoint가 설정됨 (hp=%d, mp=%.1f, ap=%d) — hover 토글 부작용 차단 위해 0으로 강제"),
					*instance->GetClass()->GetName(), instance->hp, instance->movingPoint, instance->actionPoint);
				instance->hp = 0;
				instance->movingPoint = 0.f;
				instance->actionPoint = 0;
			}
			instance->Execute_BeforeMove(ownerCharacter->IsMoved(), +1);
		}
		break;
	case EPassiveType::Conditional:
		conditionalPassives.Add(instance);
		break;
	}
}

void UPassiveSkillComponent::RemovePassiveAt(int32 Index)
{
	if (!activePassives.IsValidIndex(Index) || !ownerCharacter) return;

	if (UPassiveSkillBase* instance = activePassives[Index])
	{
		switch (instance->passiveType)
		{
		case EPassiveType::Stat:
			ownerCharacter->ApplyPassiveStatDelta(instance, -1);
			statPassives.Remove(instance);
			break;
		case EPassiveType::Reactive:
			// BeforeMove면 현재 적용된 보너스를 명시 revert (대칭성 — 영구 패시브라 사실상 호출 안 됨)
			if (instance->reactiveType == EReactiveType::BeforeMove)
			{
				instance->Execute_BeforeMove(ownerCharacter->IsMoved(), -1);
			}
			reactivePassives.Remove(instance);
			break;
		case EPassiveType::Conditional:
			conditionalPassives.Remove(instance);
			break;
		}
	}
	activePassives.RemoveAt(Index);
}

void UPassiveSkillComponent::DispatchBeforeDamageCalc(ACharacterBase* target, ESkillType skillType, EDamageType damageType,
	float& dmg, int32& amp, int32& pen, float& acc, float& crit)
{
	for (UPassiveSkillBase* p : reactivePassives)
	{
		if (!p || p->reactiveType != EReactiveType::BeforeDamageCalc) continue;

		if (p->Execute_BeforeDamageCalc(target, skillType, damageType))
		{
			// 고정 데미지 보너스는 atk 필드를 재사용 (Reactive 컨텍스트에서는 "이 공격에 더할 고정값")
			dmg  += p->atk;
			amp  += p->damageAmplification;
			pen  += p->penetration;
			acc  += p->accuracy;
			crit += p->critical;
		}
	}

	// Area는 치명타 불가 — SetCalcedStats에서 0으로 강제하지만, 패시브 보너스 합산이 다시 살릴 수 있어 후처리로 차단
	if (damageType == EDamageType::Area)
	{
		crit = 0.f;
	}
}

void UPassiveSkillComponent::DispatchAfterDamage(ACharacterBase* target)
{
	for (UPassiveSkillBase* p : reactivePassives)
	{
		if (!p || p->reactiveType != EReactiveType::AfterDamage) continue;
		p->Execute_AfterDamage(target);
	}
}

void UPassiveSkillComponent::DispatchAfterSlay(const FVector& slainLocation)
{
	for (UPassiveSkillBase* p : reactivePassives)
	{
		if (!p || p->reactiveType != EReactiveType::AfterSlay) continue;
		p->Execute_AfterSlay(slainLocation);
	}
}

void UPassiveSkillComponent::DispatchBeforeMove(bool bIsMoved, int32 sign)
{
	for (UPassiveSkillBase* p : reactivePassives)
	{
		if (!p || p->reactiveType != EReactiveType::BeforeMove) continue;
		p->Execute_BeforeMove(bIsMoved, sign);
	}
}

void UPassiveSkillComponent::DispatchConditional(EConditionalType type)
{
	if (type == EConditionalType::None) return;

	for (UPassiveSkillBase* p : conditionalPassives)
	{
		if (!p || p->conditionType != type) continue;
		p->Execute_Conditional();
	}
}

void UPassiveSkillComponent::RemovePassiveByClass(TSubclassOf<UPassiveSkillBase> passiveClass)
{
	if (!passiveClass) return;

	for (int32 i = 0; i < activePassives.Num(); ++i)
	{
		if (activePassives[i] && activePassives[i]->GetClass() == passiveClass)
		{
			RemovePassiveAt(i);
			return;
		}
	}
}
