// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/AllyCharacters/AllyTest.h"

#include "ActorComponent/SkillComponent.h"
#include "Object/Skill/ActiveSkill/test/RangeAttack.h"
#include "Object/Skill/ActiveSkill/test/SingleRangeAttack.h"
#include "Object/Skill/ActiveSkill/test/TestHeal.h"
#include "Object/Skill/ActiveSkill/test/TestThrowSkill.h"
#include "Object/Ally/ActiveSkill/ProneSkill.h"
#include "Object/Ally/ActiveSkill/JumpSkill.h"
#include "Object/Ally/ActiveSkill/RunSkill.h"

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
		skillComponent->AddSkill(UProneSkill::StaticClass());
		skillComponent->AddSkill(URunSkill::StaticClass());
		skillComponent->AddSkill(UJumpSkill::StaticClass());
		skillComponent->AddSkill(UTestThrowSkill::StaticClass());
	}
}

void AAllyTest::SetDefaultPassives()
{
	Super::SetDefaultPassives();

}
