// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/EnemyCharacters/EnemyTestAlarm.h"

#include "ActorComponent/SkillComponent.h"
#include "Object/Enemy/ActiveSkill/AlarmSkill.h"
#include "Object/Skill/ActiveSkill/test/SingleRangeAttack.h"
#include "Object/Skill/ActiveSkill/test/TestDebuffSkill.h"
#include "Object/Skill/ActiveSkill/test/TestHeal.h"
#include "Object/Skill/ActiveSkill/test/RangeAttack.h"
#include "Object/Skill/ActiveSkill/test/MultiPick.h"
#include "Object/Skill/ActiveSkill/test/SelfBuffSkill.h"
#include "Object/Skill/ActiveSkill/test/SingleBuff.h"
#include "Object/Skill/ActiveSkill/test/SingleAilment.h"

void AEnemyTestAlarm::SetDefaultSkills()
{
	Super::SetDefaultSkills();
	if (skillComponent)
	{
		skillComponent->AddSkill(UAlarmSkill::StaticClass());
		skillComponent->AddSkill(USingleRangeAttack::StaticClass());
	}
}
