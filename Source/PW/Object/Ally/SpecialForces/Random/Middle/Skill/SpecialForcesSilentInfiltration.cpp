//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/SpecialForces/Random/Middle/Skill/SpecialForcesSilentInfiltration.h"
#include "Characters/CharacterBase.h"
#include "GameMode/BattleGameMode.h"

USpecialForcesSilentInfiltration::USpecialForcesSilentInfiltration()
{
	skillName = NSLOCTEXT("Skill", "SpecialForcesSilentInfiltration_Name", "고요한 침투");
	skillDescription = NSLOCTEXT("Skill", "SpecialForcesSilentInfiltration_Description", "주변 6m 내에 적이 없을 때 이동력 +3m");
	skillGrade = EUpgradeGrade::Mid;

	passiveType = EPassiveType::Conditional;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::MoveComplete;

	//조건부 발동이라 중복 획득 불가
	bAllowDuplicate = false;

	movingPoint = 3.f;
}

void USpecialForcesSilentInfiltration::Execute_Conditional()
{
	ACharacterBase* o = owner.Get();
	if (!o) return;

	const UWorld* world = o->GetWorld();
	const ABattleGameMode* gm = world ? world->GetAuthGameMode<ABattleGameMode>() : nullptr;
	if (!gm) return;

	//소지자의 반대 진영을 센다, 적 AI도 같은 패시브를 재사용
	const bool bShouldApply = gm->CountTeamInRange(!o->IsAlly(), o->GetActorLocation(), searchRange) == 0;
	if (bShouldApply == bApplied) return;

	bApplied = bShouldApply;
	o->ApplyPassiveStatDelta(this, bApplied ? 1 : -1);
}
