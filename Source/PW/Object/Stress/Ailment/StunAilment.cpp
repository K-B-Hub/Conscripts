//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Stress/Ailment/StunAilment.h"
#include "Characters/CharacterBase.h"

UStunAilment::UStunAilment()
{
	ailmentName = NSLOCTEXT("Ailment", "Stun_Name", "멘탈 붕괴");
	ailmentDescription = NSLOCTEXT("Ailment", "Stun_Description", "충격으로 정신을 잃는다. 이번 턴에 아무 행동도 하지 못하고 즉시 턴이 종료된다");

	ailmentTurn = 1;
	isStart = true;
	isEnd = false;
	isStackable = false;
}

void UStunAilment::Execute(ACharacterBase* affected)
{
	Super::Execute(affected);

	if (!affected) return;

	//딜레이 동안 플레이어가 끼어들지 못하도록 턴 점유 선언
	affected->MarkAilmentDrivenTurn();

	//행동 불가, 잔여 행동 몰수 후 턴 종료
	//즉시 넘기면 실신이 보이지 않으므로 연출 딜레이를 두고 종료
	ScheduleWithPacing(affected, [](ACharacterBase* character) { character->RequestEndTurn(); });
}
