//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "RifleStickyBomb.generated.h"

//상급 랜덤 강화, 적 한 명에게 지연 폭발하는 폭탄을 붙인다
//폭발 피해는 BP의 ailments 배열에 지정한 URifleStickyBombAilment 파생이 담당한다
UCLASS()
class PW_API URifleStickyBomb : public UActiveSkillBase
{
	GENERATED_BODY()

public:
	URifleStickyBomb();
};
