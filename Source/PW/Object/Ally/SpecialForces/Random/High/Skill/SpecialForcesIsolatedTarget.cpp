//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/SpecialForces/Random/High/Skill/SpecialForcesIsolatedTarget.h"
#include "Characters/CharacterBase.h"
#include "GameMode/BattleGameMode.h"

USpecialForcesIsolatedTarget::USpecialForcesIsolatedTarget()
{
	skillName = NSLOCTEXT("Skill", "SpecialForcesIsolatedTarget_Name", "고립된 표적");
	skillDescription = NSLOCTEXT("Skill", "SpecialForcesIsolatedTarget_Description", "주변 8m 내에 다른 적이 없는 대상 공격 시 치명 +20, 주는 피해 +20%");
	skillGrade = EUpgradeGrade::High;

	passiveType = EPassiveType::Reactive;
	reactiveType = EReactiveType::BeforeDamageCalc;
	conditionType = EConditionalType::None;

	//조건부 발동이라 중복 획득 불가
	bAllowDuplicate = false;

	critical = 20.f;
	damageAmplification = 20;
}

bool USpecialForcesIsolatedTarget::Execute_BeforeDamageCalc(ACharacterBase* target, ESkillType skillType, EDamageType damageType, const FVector& attackerLocation)
{
	if (!target) return false;
	ACharacterBase* o = owner.Get();
	if (!o) return false;

	//아군 대상 스킬은 제외
	if (o->IsAlly() == target->IsAlly()) return false;

	const UWorld* world = o->GetWorld();
	const ABattleGameMode* gm = world ? world->GetAuthGameMode<ABattleGameMode>() : nullptr;
	if (!gm) return false;

	//대상과 같은 진영을 대상 기준으로 센다, 대상 자신은 제외
	return gm->CountTeamInRange(target->IsAlly(), target->GetActorLocation(), isolationRange, target) == 0;
}
