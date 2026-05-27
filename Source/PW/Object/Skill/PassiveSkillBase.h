// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/SkillBase.h"
#include "Enum/SkillTypes.h"
#include "PassiveSkillBase.generated.h"

class ACharacterBase;

/**
 *
 */
UCLASS()
class PW_API UPassiveSkillBase : public USkillBase
{
	GENERATED_BODY()

public:
	//패시브로 얻는 스탯 상승량
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat")
	int32 hp = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat")
	int32 atk = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat")
	int32 speed = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat")
	int32 skill = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat")
	int32 def = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat")
	float movingPoint = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat")
	int32 mentality = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat")
	int32 actionPoint = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat")
	int32 damageReduction = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat")
	int32 damageAmplification = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat")
	int32 penetration = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat")
	float sight = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat|Combat")
	float accuracy = 0.0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat|Combat")
	float evasion = 0.0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat|Combat")
	float critical = 0.0;
	
	//패시브 스킬의 타입, Stat일 경우에는 별도의 검사 필요x, Conditional이나 Reactive 면 EConditionalType이나 EReactiveType에 따라 검사 후 Execute 필요
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PassiveType")
	EPassiveType passiveType = EPassiveType::Stat;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PassiveType")
	EReactiveType reactiveType = EReactiveType::None;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PassiveType")
	EConditionalType conditionType = EConditionalType::None;

	// Reactive — 데미지 계산 직전, 공격 자체의 강화 조건 평가
	// skillType/damageType으로 공격의 정체를 판별 (예: Melee 한정 보너스, Area 제외 등)
	// 반환: true면 PassiveSkillComponent::DispatchBeforeDamageCalc가 본 패시브의 보너스 필드를 합산해
	//       SkillComponent의 데미지 계산 캐시(dmg/amp/pen/acc/crit)에 반영
	virtual bool Execute_BeforeDamageCalc(ACharacterBase* target, ESkillType skillType, EDamageType damageType) { return false; }

	// Reactive — 소지자 이동 상태 전이 시 (실제 이동 + 인디케이터 자동이동 가상 토글)
	// 작성자 패턴: 조건 매칭 시 ApplyPassiveStatDelta(sign) 호출
	// 예) 이동 보너스: if (bIsMoved) owner->ApplyPassiveStatDelta(this, sign);
	//     미이동 보너스: if (!bIsMoved) owner->ApplyPassiveStatDelta(this, sign);
	virtual void Execute_BeforeMove(bool bIsMoved, int32 sign) {}

	// Reactive — 데미지 적용 후 (시그니처는 추후 검토)
	virtual void Execute_AfterDamage(ACharacterBase* target) {}

	// Reactive — 적 처치 후, 죽은 캐릭터의 위치 전달 (해당 지점 중심 효과 / 본인 보너스 부여 등)
	virtual void Execute_AfterSlay(FVector slainLocation) {}

	// Conditional — 외부 자극 이벤트(턴시작/턴종료/피격/이동완료/사망/라운드시작)에 의해 호출
	// 컴포넌트가 conditionType 매칭으로 필터링 후 호출하므로 본 메서드는 어느 이벤트인지 알 필요 없음
	// owner는 GetOwner()로 접근, 효과는 자유 (버프/ApplyPassiveStatDelta/단발성 로직)
	virtual void Execute_Conditional() {}
};
