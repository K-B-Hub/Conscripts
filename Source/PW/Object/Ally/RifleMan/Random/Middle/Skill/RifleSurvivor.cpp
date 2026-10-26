//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/RifleMan/Random/Middle/Skill/RifleSurvivor.h"
#include "Characters/CharacterBase.h"
#include "GameMode/BattleGameMode.h"

URifleSurvivor::URifleSurvivor()
{
	skillName = NSLOCTEXT("Skill", "RifleSurvivor_Name", "생존자");
	skillDescription = NSLOCTEXT("Skill", "RifleSurvivor_Description", "8라운드부터 명중이 10, 치명이 5 증가한다");
	skillGrade = EUpgradeGrade::Mid;

	passiveType = EPassiveType::Conditional;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::RoundStart;

	//조건 충족 여부를 인스턴스가 래치하므로 중복 획득 불가
	bAllowDuplicate = false;

	accuracy = 10.f;
	critical = 5.f;
}

void URifleSurvivor::Execute_Conditional()
{
	ACharacterBase* o = owner.Get();
	if (!o || bApplied) return;

	const UWorld* world = o->GetWorld();
	const ABattleGameMode* gameMode = world ? world->GetAuthGameMode<ABattleGameMode>() : nullptr;
	if (!gameMode || gameMode->GetCurrentRound() < startRound) return;

	//라운드는 되돌아가지 않아 한 번 켜지면 회수할 일이 없다
	bApplied = true;
	o->ApplyPassiveStatDelta(this, +1);
}