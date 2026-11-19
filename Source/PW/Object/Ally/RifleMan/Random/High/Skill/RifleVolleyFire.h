//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "RifleVolleyFire.generated.h"

//Walter 전용 상급, 전방 부채꼴 범위를 쓸어버린다
UCLASS()
class PW_API URifleVolleyFire : public UActiveSkillBase
{
	GENERATED_BODY()

public:
	URifleVolleyFire();
};
