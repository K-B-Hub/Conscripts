//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/SkillBase.h"
#include "Enum/SkillTypes.h"
#include "PassiveSkillBase.generated.h"

class ACharacterBase;
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
	
	//동일 클래스 중복 획득 허용 여부, false면 이미 보유 시 재등록 생략
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PassiveType")
	bool bAllowDuplicate = false;

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
};
