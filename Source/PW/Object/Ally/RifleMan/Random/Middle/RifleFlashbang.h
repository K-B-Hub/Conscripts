//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "RifleFlashbang.generated.h"

//중급 랜덤 강화, 착지 지점 범위의 모든 캐릭터의 명중과 회피를 떨어뜨린다
//부여할 디버프는 BP의 buffs 배열에서 URifleFlashbangDebuff 파생을 지정한다
UCLASS()
class PW_API URifleFlashbang : public UActiveSkillBase
{
	GENERATED_BODY()

public:
	URifleFlashbang();
};