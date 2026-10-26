//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "RifleVolleyFire.generated.h"

//레벨 80 고정 강화, 전방 부채꼴 범위를 쓸어버린다
UCLASS()
class PW_API URifleVolleyFire : public UActiveSkillBase
{
	GENERATED_BODY()

public:
	URifleVolleyFire();
};
