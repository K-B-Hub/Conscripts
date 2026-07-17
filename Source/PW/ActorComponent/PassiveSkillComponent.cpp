//Fill out your copyright notice in the Description page of Project Settings.


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

	//중복 불가 패시브는 동일 클래스 보유 시 등록 생략
	if (!passiveClass->GetDefaultObject<UPassiveSkillBase>()->bAllowDuplicate)
	{
		for (const UPassiveSkillBase* passive : activePassives)
		{
			if (passive && passive->GetClass() == passiveClass)
			{
				UE_LOG(LogTemp, Log, TEXT("[PassiveSkillComponent] %s 중복 불가 패시브 '%s' 재등록 생략"),
					*ownerCharacter->GetName(), *passiveClass->GetName());
				return;
			}
		}
	}

	UPassiveSkillBase* instance = NewObject<UPassiveSkillBase>(this, passiveClass);
	if (!instance) return;

	instance->SetOwner(ownerCharacter);
	activePassives.Add(instance);

	switch (instance->passiveType)
	{
	case EPassiveType::Stat:
		//Stat 패시브는 즉시 스탯 적용
		statPassives.Add(instance);
		ownerCharacter->ApplyPassiveStatDelta(instance, +1);
		break;
	case EPassiveType::Reactive:
		reactivePassives.Add(instance);
		if (instance->reactiveType == EReactiveType::BeforeMove)
		{
			//BeforeMove 패시브의 자원 변동값 제거
			if (instance->hp != 0 || instance->movingPoint != 0.f || instance->actionPoint != 0)
			{
				UE_LOG(LogTemp, Warning, TEXT("[PassiveSkillComponent] BeforeMove 패시브 '%s'에 hp/movingPoint/actionPoint가 설정됨 (hp=%d, mp=%.1f, ap=%d) — hover 토글 부작용 차단 위해 0으로 강제"),
					*instance->GetClass()->GetName(), instance->hp, instance->movingPoint, instance->actionPoint);
				instance->hp = 0;
				instance->movingPoint = 0.f;
				instance->actionPoint = 0;
			}
			//현재 이동 상태 기준으로 즉시 적용
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
			if (instance->reactiveType == EReactiveType::BeforeMove)
			{
				//BeforeMove 적용값 복구
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
			//패시브 보너스를 이번 공격값에 합산
			dmg  += p->atk;
			amp  += p->damageAmplification;
			pen  += p->penetration;
			acc  += p->accuracy;
			crit += p->critical;
		}
	}

	//광역 피해는 치명타 불가 처리
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
