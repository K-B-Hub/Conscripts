// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/EnemyCharacters/EnemyTest.h"

#include "ActorComponent/SkillComponent.h"
#include "Object/Skill/ActiveSkill/test/SingleRangeAttack.h"
#include "Object/Skill/ActiveSkill/test/TestDebuffSkill.h"
#include "Object/Skill/ActiveSkill/test/TestHeal.h"
#include "Object/Skill/ActiveSkill/test/RangeAttack.h"
#include "Object/Skill/ActiveSkill/test/MultiPick.h"

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
	}
}
