//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "SniperPiercingShot.generated.h"

//레벨 45 고정 강화, 직선 위의 모든 캐릭터를 꿰뚫는다
UCLASS()
class PW_API USniperPiercingShot : public UActiveSkillBase
{
	GENERATED_BODY()

public:
	USniperPiercingShot();
};
