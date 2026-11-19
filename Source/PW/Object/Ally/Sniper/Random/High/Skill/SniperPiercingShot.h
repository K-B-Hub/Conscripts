//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "SniperPiercingShot.generated.h"

//저격수 직업 공용 상급, 직선 위의 모든 캐릭터를 꿰뚫는다
UCLASS()
class PW_API USniperPiercingShot : public UActiveSkillBase
{
	GENERATED_BODY()

public:
	USniperPiercingShot();
};
