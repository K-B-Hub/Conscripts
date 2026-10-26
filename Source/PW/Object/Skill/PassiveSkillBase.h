//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/SkillBase.h"
#include "Enum/SkillTypes.h"
#include "PassiveSkillBase.generated.h"

class ACharacterBase;
class UActiveSkillBase;
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
	//고유 자원 최대치 증가분, 소총수의 탄창처럼 직업 자원을 늘리는 강화가 쓴다
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat")
	int32 maxBattleResource = 0;
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
	//치명타 배율 증가분, 1.0이면 배율 +1.0(=피해 100%p 증가)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat|Combat")
	float criticalDamage = 0.0;

	//동일 클래스 중복 획득 허용 여부, false면 이미 보유 시 재등록 생략
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PassiveType")
	bool bAllowDuplicate = false;

	//행동력 소모를 깎아줄 스킬, 지정된 클래스에만 적용되며 조건 검사를 거치지 않는다
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat|Cost")
	TSubclassOf<UActiveSkillBase> discountSkillClass;
	//행동력 소모를 깎아줄 스킬 유형, 비어 있으면 유형 기준 할인 없음
	//"근접 공격 전체" 처럼 클래스가 여럿인 대상을 한 번에 지정할 때 쓴다
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat|Cost")
	TArray<ESkillType> discountSkillTypes;
	//깎아줄 행동력, discountSkillClass·discountSkillTypes 둘 다 미지정이면 무의미
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat|Cost")
	int32 actionPointDiscount = 0;

	//스트레스 이벤트 추첨을 긍정 풀로 고정, 만땅 도달 시 턴 손실 페널티는 그대로 유지
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stress")
	bool bStressEventAlwaysPositive = false;

	//투척 스킬에 한해 광역 치명타 봉쇄를 해제, 보유만 해도 적용되며 조건 검사를 거치지 않는다
	//같은 광역이라도 투척이 아닌 스킬은 그대로 봉쇄된다
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat|Combat")
	bool bAllowThrowCritical = false;

	//패시브 스킬의 타입, Stat일 경우에는 별도의 검사 필요x, Conditional이나 Reactive 면 EConditionalType이나 EReactiveType에 따라 검사 후 Execute 필요
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PassiveType")
	EPassiveType passiveType = EPassiveType::Stat;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PassiveType")
	EReactiveType reactiveType = EReactiveType::None;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PassiveType")
	EConditionalType conditionType = EConditionalType::None;
	//피해 계산 전 패시브 조건 검사, attackerLocation은 자동이동 후 위치가 반영된 공격 시점 좌표
	virtual bool Execute_BeforeDamageCalc(ACharacterBase* target, ESkillType skillType, EDamageType damageType, const FVector& attackerLocation) { return false; }
	//이동 상태 변경 패시브 실행
	virtual void Execute_BeforeMove(bool bIsMoved, int32 sign) {}
	//피해 적용 후 패시브 실행
	virtual void Execute_AfterDamage(ACharacterBase* target) {}
	//처치 후 패시브 실행
	virtual void Execute_AfterSlay(FVector slainLocation) {}
	//조건부 패시브 실행
	virtual void Execute_Conditional() {}
	//치사 피해 직전 조건 검사, true를 반환하면 체력 1로 생존
	virtual bool Execute_BeforeDeath(int32 hpBefore, int32 incomingDamage) { return false; }
};
