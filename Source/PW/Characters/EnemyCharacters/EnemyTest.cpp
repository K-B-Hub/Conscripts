// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/EnemyCharacters/EnemyTest.h"

#include "ActorComponent/SkillComponent.h"
#include "Object/Skill/ActiveSkill/test/SingleRangeAttack.h"
#include "Object/Skill/ActiveSkill/test/TestDebuffSkill.h"
#include "Object/Skill/ActiveSkill/test/TestHeal.h"
#include "Object/Skill/ActiveSkill/test/RangeAttack.h"
#include "Object/Skill/ActiveSkill/test/MultiPick.h"
#include "Object/Skill/ActiveSkill/test/SelfBuffSkill.h"
#include "Object/Skill/ActiveSkill/test/SingleBuff.h"
#include "Object/Skill/ActiveSkill/test/SingleAilment.h"

AEnemyTest::AEnemyTest()
{
	atk = 12;
	skill = 50;
}

void AEnemyTest::SetDefaultSkills()
{
	Super::SetDefaultSkills();
	if (skillComponent)
	{
		skillComponent->AddSkill(USingleRangeAttack::StaticClass());
		skillComponent->AddSkill(UTestDebuffSkill::StaticClass());
		skillComponent->AddSkill(UTestHeal::StaticClass());
		skillComponent->AddSkill(URangeAttack::StaticClass());
		skillComponent->AddSkill(UMultiPick::StaticClass());
		//UtilityAI 버프/상태이상 후보 검증용
		skillComponent->AddSkill(USelfBuffSkill::StaticClass());
		skillComponent->AddSkill(USingleBuff::StaticClass());
		skillComponent->AddSkill(USingleAilment::StaticClass());
	}
}
