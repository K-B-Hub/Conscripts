// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/EnemyCharacters/EnemyTest.h"

#include "ActorComponent/SkillComponent.h"
#include "Object/Skill/ActiveSkill/test/SingleRangeAttack.h"

AEnemyTest::AEnemyTest()
{
	
}

void AEnemyTest::SetDefaultSkills()
{
	Super::SetDefaultSkills();
	if (skillComponent)
		
	{
		skillComponent->AddSkill(USingleRangeAttack::StaticClass());
	}
}
