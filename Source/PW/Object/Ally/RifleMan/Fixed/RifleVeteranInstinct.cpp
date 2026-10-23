//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/RifleMan/Fixed/RifleVeteranInstinct.h"
#include "Characters/CharacterBase.h"

URifleVeteranInstinct::URifleVeteranInstinct()
{
	skillName = NSLOCTEXT("Skill", "RifleVeteranInstinct_Name", "노련함");
	skillDescription = NSLOCTEXT("Skill", "RifleVeteranInstinct_Description", "턴 시작마다 탄창을 1개 회복하고 명중과 치명이 10 증가한다");

	passiveType = EPassiveType::Conditional;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::TurnStart;

	//조건 충족 여부를 인스턴스가 래치하므로 중복 획득 불가
	bAllowDuplicate = false;

	accuracy = 10.f;
	critical = 10.f;
}

void URifleVeteranInstinct::Execute_Conditional()
{
	ACharacterBase* o = owner.Get();
	if (!o) return;

	//Conditional은 등록 시 스탯이 적용되지 않아 첫 턴 시작에 한 번 직접 건다
	if (!bStatApplied)
	{
		bStatApplied = true;
		o->ApplyPassiveStatDelta(this, +1);
	}

	//최대치 초과분은 GainBattleResource가 클램프
	o->GainBattleResource(resourceGain);
}
