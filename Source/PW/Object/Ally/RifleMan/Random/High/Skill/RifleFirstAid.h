//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "RifleFirstAid.generated.h"

//상급 랜덤 강화, 자신의 체력을 최대 체력 비율만큼 회복하는 스킬
//회복량이 공격력이 아니라 최대 체력에서 나오므로 계산값만 갈아끼운다
UCLASS()
class PW_API URifleFirstAid : public UActiveSkillBase
{
	GENERATED_BODY()

public:
	URifleFirstAid();

	virtual void SetCalcedStats() override;

protected:
	//회복 비율, 0.2 = 최대 체력의 20%
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Heal")
	float healRatio = 0.2f;
};
