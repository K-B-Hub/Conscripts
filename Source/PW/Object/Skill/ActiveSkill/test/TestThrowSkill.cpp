//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Skill/ActiveSkill/test/TestThrowSkill.h"
#include "Animation/AnimMontage.h"

UTestThrowSkill::UTestThrowSkill()
{
	skillName = NSLOCTEXT("Skill", "TestThrow_Name", "투척(테스트)");
	skillDescription = NSLOCTEXT("Skill", "TestThrow_Description", "지면 지점에 포물선으로 던져 착지점 범위에 피해를 준다");

	skillType = ESkillType::Throw;
	damageType = EDamageType::Area;
	selectMode = ESelectMode::GroundPoint;
	pickTeam = EPickTeam::EnemyOnly;
	areaTarget = EAreaTarget::All;
	areaForm = EAreaForm::Circle;
	areaParameter1 = 200.f;

	pickRange = 800.f;
	pickCount = 1;

	battleResourceCost = 0;
	actionPointCost = 1;

	damageRatio = 1.0f;
	bonusAccuracy = 35.f;
	bonusCritical = 0.f;
	baseDamage = 1;
	bonusPenetration = 0;
	bonusDamageAmplication = 0;

	//궤적을 표시하되 캐릭터는 이동하지 않음(투척물 개념), UsesArcMovement는 오버라이드 안 함
	arcApexRatio = 0.5f;

	//플레이스홀더 몽타주, 실제 투척 애니메이션으로 교체 예정
	static ConstructorHelpers::FObjectFinder<UAnimMontage> MontageAsset(TEXT("/Game/Animation/Animations/Rifleman/AnimMontage/AM_Rifle_Fire_Montage"));
	if (MontageAsset.Succeeded())
	{
		skillMontage = MontageAsset.Object;
	}
}