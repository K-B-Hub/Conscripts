//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Ailment/Stress/StunAilment.h"
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

	//행동 불가, 잔여 행동 몰수 후 즉시 턴 종료
	affected->RequestEndTurn();
}
