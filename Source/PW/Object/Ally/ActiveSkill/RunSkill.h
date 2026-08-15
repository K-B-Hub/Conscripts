//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "RunSkill.generated.h"

//달리기 스킬, 자신에게 시전 — 행동력 1을 소모해 이동력을 추가로 얻는다
UCLASS()
class PW_API URunSkill : public UActiveSkillBase
{
	GENERATED_BODY()

public:
	URunSkill();

	//행동력 소모 후 이동력 추가 획득
	virtual void BeginUse() override;

protected:
	//달리기로 얻는 추가 이동력(m), 기본 최대치를 초과해 가산 (튜닝값)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Run")
	float runBonusMovingPoint = 5.f;
};