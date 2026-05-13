// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/SkillBase.h"
#include "Enum/SkillTypes.h"
#include "PassiveSkillBase.generated.h"

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
	EPassiveType passiveType = EPassiveType::Stat;
	
	EReactiveType reactiveType = EReactiveType::None;
	
	EConditionalType conditionType = EConditionalType::None;
	
	//패시브의 특수 효과 실행(Conditional or Reactive 만 해당)
	void Execute(const ACharacterBase* target) override;
};
