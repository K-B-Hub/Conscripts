//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/SpecialForces/Random/Middle/Skill/SpecialForcesSmokeUse.h"
#include "Characters/CharacterBase.h"
#include "ActorComponent/TerrainComponent.h"
#include "Actors/Terrain/SmokeTerrain.h"

USpecialForcesSmokeUse::USpecialForcesSmokeUse()
{
	skillName = NSLOCTEXT("Skill", "SpecialForcesSmokeUse_Name", "연막 활용");
	skillDescription = NSLOCTEXT("Skill", "SpecialForcesSmokeUse_Description", "연막 안에 있을 때 치명 +20");
	skillGrade = EUpgradeGrade::Mid;

	passiveType = EPassiveType::Conditional;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::TerrainChanged;

	//조건부 발동이라 중복 획득 불가
	bAllowDuplicate = false;

	critical = 20.f;
}

void USpecialForcesSmokeUse::Execute_Conditional()
{
	ACharacterBase* o = owner.Get();
	if (!o) return;

	const UTerrainComponent* tc = o->GetTerrainComponent();
	if (!tc) return;

	//연막 BP 파생도 잡도록 클래스 계층으로 판정
	bool bShouldApply = false;
	for (const ATerrainBase* terrain : tc->GetCurrentTerrains())
	{
		if (IsValid(terrain) && terrain->IsA<ASmokeTerrain>())
		{
			bShouldApply = true;
			break;
		}
	}

	if (bShouldApply == bApplied) return;

	bApplied = bShouldApply;
	o->ApplyPassiveStatDelta(this, bApplied ? 1 : -1);
}
