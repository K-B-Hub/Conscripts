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
	// 반환: true면 호출자(AttackRangeIndicator)가 본 패시브의 보너스 필드들을 캐싱값에 합산
	virtual bool Execute_BeforeDamageCalc(ACharacterBase* target) { return false; }

	// Reactive — 소지자 이동 상태 전이 시 (실제 이동 + 인디케이터 자동이동 가상 토글)
	// 작성자 패턴: 조건 매칭 시 ApplyPassiveStatDelta(sign) 호출
	// 예) 이동 보너스: if (bIsMoved) owner->ApplyPassiveStatDelta(this, sign);
	//     미이동 보너스: if (!bIsMoved) owner->ApplyPassiveStatDelta(this, sign);
	virtual void Execute_BeforeMove(bool bIsMoved, int32 sign) {}

	// Reactive — 데미지 적용 후 (시그니처는 추후 검토)
	virtual void Execute_AfterDamage(ACharacterBase* target) {}

	// Reactive — 적 처치 후, 죽은 캐릭터의 위치 전달 (해당 지점 중심 효과 / 본인 보너스 부여 등)
	virtual void Execute_AfterSlay(FVector slainLocation) {}
};
