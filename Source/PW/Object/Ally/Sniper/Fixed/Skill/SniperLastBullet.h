//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "SniperLastBullet.generated.h"

//레벨 80 고정 강화, 탄환을 모두 쏟아 한 발에 건다
UCLASS()
class PW_API USniperLastBullet : public UActiveSkillBase
{
	GENERATED_BODY()

public:
	USniperLastBullet();
};
