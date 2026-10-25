//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "RifleSmokeGrenade.generated.h"

//중급 랜덤 강화, 착지 지점에 연막 지형을 만든다
//대상에게 직접 주는 효과는 없고 지형 생성은 베이스 OnCommit이 처리한다
UCLASS()
class PW_API URifleSmokeGrenade : public UActiveSkillBase
{
	GENERATED_BODY()

public:
	URifleSmokeGrenade();
};