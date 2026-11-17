//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "SpecialForcesEvasiveManeuver.generated.h"

//중급 랜덤 강화, 이번 턴에 이동했으면 다음 턴까지 회피가 오른다
//이동력 소모는 자기 턴 시작에 리셋되고 그 뒤 누군가 이동을 마칠 때 해제된다
UCLASS()
class PW_API USpecialForcesEvasiveManeuver : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	USpecialForcesEvasiveManeuver();

	virtual void Execute_Conditional() override;

private:
	//현재 스탯이 적용된 상태인지, 중복 가감 방지용
	UPROPERTY()
	bool bApplied = false;
};
