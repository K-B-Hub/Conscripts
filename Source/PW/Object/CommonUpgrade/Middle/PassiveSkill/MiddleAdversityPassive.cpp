//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/CommonUpgrade/Middle/PassiveSkill/MiddleAdversityPassive.h"
#include "Characters/CharacterBase.h"
#include "ActorComponent/TerrainComponent.h"

UMiddleAdversityPassive::UMiddleAdversityPassive()
{
	skillName = NSLOCTEXT("Skill", "MiddleAdversity_Name", "역경 돌파");
	skillDescription = NSLOCTEXT("Skill", "MiddleAdversity_Description", "지형에 있을 때 공격력이 3, 명중이 15, 치명타가 10 증가한다");

	passiveType = EPassiveType::Conditional;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::TerrainChanged;

	//조건 충족 여부를 인스턴스가 래치하므로 중복 획득 불가
	bAllowDuplicate = false;

	atk = 3;
	accuracy = 15.0f;
	critical = 10.0f;
}

void UMiddleAdversityPassive::Execute_Conditional()
{
	ACharacterBase* o = owner.Get();
	if (!o) return;

	const UTerrainComponent* tc = o->GetTerrainComponent();
	if (!tc) return;

	//지형 종류는 가리지 않고 체류 여부만 판정, 중첩 지형도 하나로 취급
	const bool bShouldApply = tc->IsInTerrain();
	if (bShouldApply == bApplied) return;

	bApplied = bShouldApply;
	o->ApplyPassiveStatDelta(this, bApplied ? 1 : -1);
}