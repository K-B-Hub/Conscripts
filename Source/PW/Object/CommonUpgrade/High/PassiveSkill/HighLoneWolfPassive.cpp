//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/CommonUpgrade/High/PassiveSkill/HighLoneWolfPassive.h"
#include "Characters/CharacterBase.h"
#include "Characters/AllyCharacterBase.h"
#include "Characters/EnemyBase.h"
#include "GameMode/BattleGameMode.h"

UHighLoneWolfPassive::UHighLoneWolfPassive()
{
	skillName = NSLOCTEXT("Skill", "HighLoneWolf_Name", "고독한 늑대");
	skillDescription = NSLOCTEXT("Skill", "HighLoneWolf_Description", "주위 5m 내에 다른 아군이 없을 때 공격력, 속도, 기술, 방어력이 5씩 증가한다");

	passiveType = EPassiveType::Conditional;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::MoveComplete;

	//조건 충족 여부를 인스턴스가 래치하므로 중복 획득 불가
	bAllowDuplicate = false;

	atk = 5;
	speed = 5;
	skill = 5;
	def = 5;
}

void UHighLoneWolfPassive::Execute_Conditional()
{
	ACharacterBase* o = owner.Get();
	if (!o) return;

	UWorld* world = o->GetWorld();
	ABattleGameMode* gm = world ? world->GetAuthGameMode<ABattleGameMode>() : nullptr;
	if (!gm) return;

	//소지자와 같은 진영만 아군으로 취급, 적 AI도 동일 패시브를 재사용
	TArray<ACharacterBase*> teammates;
	if (o->IsAlly())
	{
		for (AAllyCharacterBase* a : gm->GetAllies()) teammates.Add(a);
	}
	else
	{
		for (AEnemyBase* e : gm->GetEnemies()) teammates.Add(e);
	}

	//높이차는 무시, 시야 판정과 동일하게 수평 거리로 측정
	const FVector ownerLoc = o->GetActorLocation();
	bool bIsolated = true;
	for (ACharacterBase* c : teammates)
	{
		if (!IsValid(c) || c == o || c->IsDead()) continue;
		if (FVector::Dist2D(ownerLoc, c->GetActorLocation()) <= isolationRange)
		{
			bIsolated = false;
			break;
		}
	}

	if (bIsolated == bApplied) return;

	bApplied = bIsolated;
	o->ApplyPassiveStatDelta(this, bApplied ? 1 : -1);
}