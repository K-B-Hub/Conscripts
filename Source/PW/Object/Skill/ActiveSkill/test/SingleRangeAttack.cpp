// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Skill/ActiveSkill/test/SingleRangeAttack.h"

USingleRangeAttack::USingleRangeAttack()
{
	skillName = NSLOCTEXT("Skill", "SingleRangeAttack_Name", "단일 원거리 공격");
	skillDescription = NSLOCTEXT("Skill", "SingleRangeAttack_Description", "단일 대상에게 사거리 내에서 공격합니다.");

	skillType = ESkillType::Ranged;
	damageType = EDamageType::Normal;
	selectMode = ESelectMode::SinglePick;
	pickTeam = EPickTeam::Any;
	areaTarget = EAreaTarget::None;
	//areaForm = EAreaForm::Circle;		//단일 공격이니 확인 안함
	//areaParameter1 = 0.f;				//단일 공격이니 확인 안함
	//areaParameter2 = 0.f;				//단일 공격이니 확인 안함

	pickRange = 1200.f;
	pickCount = 1;
	
	battleResourceCost = 0;
	actionPointCost = 2;
	
	damageRatio = 1.0f;
	bonusAccuracy = 35.f;
	bonusCritical = 0.f;
	baseDamage = 1;
	bonusPenetration = 0;
	bonusDamageAmplication = 0;
}

void USingleRangeAttack::Execute(const TArray<ACharacterBase*>& targets)
{
	Super::Execute(targets);
	
	
}
