//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Enemy/Buff/AlarmBuff.h"
#include "Characters/CharacterBase.h"
#include "Characters/EnemyBase.h"
#include "AI/AIController/EnemyAIController.h"
#include "GameMode/BattleGameMode.h"

UAlarmBuff::UAlarmBuff()
{
	buffName = NSLOCTEXT("Buff", "Alarm_Name", "알람");
	buffDescription = NSLOCTEXT("Buff", "Alarm_Description", "구원 요청이 진행 중이다. 만료되면 맵의 모든 적이 전투에 합류한다. 만료 전에 시전자를 처치하면 무산된다");

	buffTurn = 3;
	isStart = false;
	isEnd = true;
	isStackable = false;

	//알람의 전략 가치, 스탯 델타가 없어 수동 환산치로 AI 점수 부여
	aiBaseValue = 10.f;
}

void UAlarmBuff::Execute(ACharacterBase* affected)
{
	Super::Execute(affected);

	//만료 턴에만 발동, 그 외 턴은 아무 작용 없음
	if (remainingTurn != 1 || !affected) return;

	UWorld* world = affected->GetWorld();
	ABattleGameMode* gm = world ? world->GetAuthGameMode<ABattleGameMode>() : nullptr;
	if (!gm) return;

	//이미 다른 알람이 발동했으면 중복 발동 없음
	if (gm->IsAlarmUsed()) return;

	//발동 시점에 전역 마킹, 이후 모든 보유자 재사용 불가
	gm->MarkAlarmUsed();

	UE_LOG(LogTemp, Log, TEXT("[AlarmBuff] %s 알람 발동 — 맵 전체 전투 합류"), *affected->GetName());

	//맵의 모든 적을 전투에 합류, 알람 발동 위치를 접근 목표로 전달
	const FVector alarmOrigin = affected->GetActorLocation();
	for (AEnemyBase* enemy : gm->GetEnemies())
	{
		if (!IsValid(enemy) || enemy->IsDead()) continue;
		if (AEnemyAIController* aic = Cast<AEnemyAIController>(enemy->GetController()))
		{
			aic->JoinCombat(EJoinCombatReason::Alarm, alarmOrigin);
		}
	}
}
