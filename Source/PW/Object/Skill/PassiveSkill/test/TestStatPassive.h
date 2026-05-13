// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "TestStatPassive.generated.h"

// 테스트용 Stat 패시브 — 단순 스탯 증가만 수행
// passiveType=Stat이므로 등록 즉시 ApplyPassiveStatDelta(+1)로 영구 적용됨
UCLASS()
class PW_API UTestStatPassive : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	UTestStatPassive();
};
