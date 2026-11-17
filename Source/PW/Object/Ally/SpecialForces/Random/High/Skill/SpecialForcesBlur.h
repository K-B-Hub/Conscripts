//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "SpecialForcesBlur.generated.h"

//상급 랜덤 강화, 피해를 받을 때마다 회피가 오른다(최대 3중첩)
UCLASS()
class PW_API USpecialForcesBlur : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	USpecialForcesBlur();

	virtual void Execute_Conditional() override;

private:
	//직전 체력, Damaged는 회복에도 발동하므로 감소 여부를 직접 판정한다, 음수면 아직 기록 전
	UPROPERTY()
	int32 lastHp = -1;
};
