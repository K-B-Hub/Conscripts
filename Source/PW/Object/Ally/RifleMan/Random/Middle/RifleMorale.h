//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "RifleMorale.generated.h"

//중급 랜덤 강화, 자신과 주변 아군의 명중을 2턴간 올린다
//부여할 버프는 BP의 buffs 배열에서 URifleMoraleBuff 파생을 지정한다
UCLASS()
class PW_API URifleMorale : public UActiveSkillBase
{
	GENERATED_BODY()

public:
	URifleMorale();
};