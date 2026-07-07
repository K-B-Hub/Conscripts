//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "AlarmSkill.generated.h"

//알람 스킬, 자신에게 알람 버프만 적용 — 만료 전 처치하지 못하면 맵 전체 전투 합류
//전투당 전역 1회 제한, 게임모드 플래그로 관리
UCLASS()
class PW_API UAlarmSkill : public UActiveSkillBase
{
	GENERATED_BODY()

public:
	UAlarmSkill();

	//전역 알람 사용 여부 추가 검사, 플래그 마킹은 버프 발동 시점(AlarmBuff)이라 무산 시 재사용 가능
	virtual bool CanExecute() const override;
};
