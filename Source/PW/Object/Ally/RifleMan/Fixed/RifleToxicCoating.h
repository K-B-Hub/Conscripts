//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "RifleToxicCoating.generated.h"

//레벨 70 고정 강화, 공격 후 대상에게 현재 체력 비례 지속 피해를 추가로 준다
UCLASS()
class PW_API URifleToxicCoating : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	URifleToxicCoating();

	virtual void Execute_AfterDamage(ACharacterBase* target) override;

protected:
	//대상의 현재 체력 대비 추가 피해 비율(%)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Toxic")
	int32 hpRatioPercent = 20;
};
