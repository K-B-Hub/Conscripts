// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/AllyCharacters/AllyTest.h"

#include "ActorComponent/SkillComponent.h"
#include "Object/Skill/ActiveSkill/test/RangeAttack.h"
#include "Object/Skill/ActiveSkill/test/SingleRangeAttack.h"
#include "Object/Skill/ActiveSkill/test/MultiPick.h"
#include "Object/Skill/ActiveSkill/test/RangedMultiPick.h"
#include "Object/Skill/ActiveSkill/test/SingleBuff.h"
#include "Object/Skill/ActiveSkill/test/SingleAilment.h"

AAllyTest::AAllyTest()
{
	atk = 16;
	skill = 50; //테스트용
}

void AAllyTest::BeginPlay()
{
	Super::BeginPlay();
}

void AAllyTest::SetDefaultSkills()
{
	Super::SetDefaultSkills();
	
	USingleRangeAttack* TestSkill1 = NewObject<USingleRangeAttack>(this, USingleRangeAttack::StaticClass(), TEXT("TestSkill1"));
	if (TestSkill1)
	{
		skillComponent->AddSkill(TestSkill1);
	}
	URangeAttack* TestSkill2 = NewObject<URangeAttack>(this, URangeAttack::StaticClass(), TEXT("TestSkill2"));
	if (TestSkill2)
	{
		skillComponent->AddSkill(TestSkill2);
	}
	UMultiPick* TestSkill3 = NewObject<UMultiPick>(this, UMultiPick::StaticClass(), TEXT("TestSkill3"));
	if (TestSkill3)
	{
		skillComponent->AddSkill(TestSkill3);
	}
	URangedMultiPick* TestSkill4 = NewObject<URangedMultiPick>(this, URangedMultiPick::StaticClass(), TEXT("TestSkill4"));
	if (TestSkill4)
	{
		skillComponent->AddSkill(TestSkill4);
	}
	USingleBuff* TestSkill5 = NewObject<USingleBuff>(this, USingleBuff::StaticClass(), TEXT("TestSkill5"));
	if (TestSkill5)
	{
		skillComponent->AddSkill(TestSkill5);
	}
	USingleAilment* TestSkill6 = NewObject<USingleAilment>(this, USingleAilment::StaticClass(), TEXT("TestSkill6"));
	if (TestSkill6)
	{
		skillComponent->AddSkill(TestSkill6);
	}
}
