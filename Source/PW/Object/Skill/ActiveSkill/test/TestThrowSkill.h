//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "TestThrowSkill.generated.h"

//테스트용 투척 스킬, 지면 지점에 포물선으로 던져 착지점 범위 피해
//아치 궤적을 쓰되(arcApexRatio>0) 캐릭터는 이동하지 않음(UsesArcMovement 기본 false) — 아치 인디케이터 검증용
UCLASS()
class PW_API UTestThrowSkill : public UActiveSkillBase
{
	GENERATED_BODY()

public:
	UTestThrowSkill();
};