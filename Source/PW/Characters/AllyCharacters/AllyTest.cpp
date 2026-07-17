// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/AllyCharacters/AllyTest.h"

#include "ActorComponent/PassiveSkillComponent.h"
#include "Object/Skill/PassiveSkill/test/TestStatPassive.h"
#include "Object/Skill/PassiveSkill/test/TestBeforeCalc.h"
#include "Object/Skill/PassiveSkill/test/TestAfterSlay.h"

#include "ActorComponent/SkillComponent.h"
#include "Object/Skill/ActiveSkill/test/RangeAttack.h"
#include "Object/Skill/ActiveSkill/test/SingleRangeAttack.h"
#include "Object/Skill/ActiveSkill/test/MultiPick.h"
#include "Object/Skill/ActiveSkill/test/RangedMultiPick.h"
#include "Object/Skill/ActiveSkill/test/SingleBuff.h"
#include "Object/Skill/ActiveSkill/test/SingleAilment.h"
#include "Object/Skill/PassiveSkill/test/TestAfterDamage.h"
#include "Object/Skill/PassiveSkill/test/TestBeforeMove.h"
#include "Object/Skill/PassiveSkill/test/TestTurnStart.h"
#include "Object/Skill/PassiveSkill/test/TestTurnEnd.h"
#include "Object/Skill/PassiveSkill/test/TestDamaged.h"
#include "Object/Skill/PassiveSkill/test/TestMoveComplete.h"
#include "Object/Skill/PassiveSkill/test/TestAllyDeath.h"
#include "Object/Skill/PassiveSkill/test/TestEnemyDeath.h"
#include "Object/Skill/PassiveSkill/test/TestRoundStart.h"
#include "Object/Skill/ActiveSkill/test/TestHeal.h"

AAllyTest::AAllyTest()
{
	atk = 10;
}

void AAllyTest::BeginPlay()
{
	Super::BeginPlay();
}

void AAllyTest::SetDefaultSkills()
{
	Super::SetDefaultSkills();
	
	if (skillComponent)
	{
		skillComponent->AddSkill(USingleRangeAttack::StaticClass());
		skillComponent->AddSkill(URangeAttack::StaticClass());
		skillComponent->AddSkill(UMultiPick::StaticClass());
		skillComponent->AddSkill(URangedMultiPick::StaticClass());
		skillComponent->AddSkill(USingleBuff::StaticClass());
		skillComponent->AddSkill(USingleAilment::StaticClass());
		skillComponent->AddSkill(UTestHeal::StaticClass());
	}
}

void AAllyTest::SetDefaultPassives()
{
	Super::SetDefaultPassives();
	
	if (passiveSkillComponent)
	{
		passiveSkillComponent->AddPassive(UTestStatPassive::StaticClass());
		passiveSkillComponent->AddPassive(UTestBeforeCalc::StaticClass());
		passiveSkillComponent->AddPassive(UTestAfterDamage::StaticClass());
		passiveSkillComponent->AddPassive(UTestAfterSlay::StaticClass());
		passiveSkillComponent->AddPassive(UTestBeforeMove::StaticClass());
		passiveSkillComponent->AddPassive(UTestTurnStart::StaticClass());
		passiveSkillComponent->AddPassive(UTestTurnEnd::StaticClass());
		passiveSkillComponent->AddPassive(UTestDamaged::StaticClass());
		passiveSkillComponent->AddPassive(UTestMoveComplete::StaticClass());
		passiveSkillComponent->AddPassive(UTestAllyDeath::StaticClass());
		passiveSkillComponent->AddPassive(UTestEnemyDeath::StaticClass());
		passiveSkillComponent->AddPassive(UTestRoundStart::StaticClass());
	}
}
