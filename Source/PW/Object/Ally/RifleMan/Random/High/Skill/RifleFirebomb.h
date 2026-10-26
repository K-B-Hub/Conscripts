//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "RifleFirebomb.generated.h"

//상급 랜덤 강화, 착지 지점에 화염 지형을 만든다
//대상에게 직접 주는 효과는 없고 지형 생성은 베이스 OnCommit이 처리한다
UCLASS()
class PW_API URifleFirebomb : public UActiveSkillBase
{
	GENERATED_BODY()

public:
	URifleFirebomb();
};
