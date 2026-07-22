// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/AllyCharacters/AllyTest.h"

#include "ActorComponent/PassiveSkillComponent.h"
#include "Object/Skill/PassiveSkill/test/TestStatPassive.h"
#include "Object/Skill/PassiveSkill/test/TestBeforeCalc.h"
#include "Object/Skill/PassiveSkill/test/TestAfterSlay.h"

#include "ActorComponent/SkillComponent.h"
#include "Object/Skill/ActiveSkill/test/RangeAttack.h"
#include "Object/Skill/ActiveSkill/test/SingleRangeAttack.h"
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
		skillComponent->AddSkill(UTestHeal::StaticClass());
	}
}

void AAllyTest::SetDefaultPassives()
{
	Super::SetDefaultPassives();

}
