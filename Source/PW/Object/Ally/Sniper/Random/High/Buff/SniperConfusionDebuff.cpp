//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/Sniper/Random/High/Buff/SniperConfusionDebuff.h"
#include "Characters/CharacterBase.h"

USniperConfusionDebuff::USniperConfusionDebuff()
{
	buffName = NSLOCTEXT("Buff", "SniperConfusionDebuff_Name", "혼란");
	buffDescription = NSLOCTEXT("Buff", "SniperConfusionDebuff_Description", "정신이 흐려져 이동할 수 없고 명중과 회피가 50 감소한다");

	accuracy = -50.f;
	evasion = -50.f;

	//이동력 감소량은 OnApply가 대상 이동력으로 채운다
	buffTurn = 1;
	isStart = false;
	isEnd = false;
	isStackable = false;

	//스탯 변화는 상대에 따라 가치가 달라 정적 환산이 어렵다, AI 평가는 중립
	aiBaseValue = 0.f;
}

void USniperConfusionDebuff::OnApply(ACharacterBase* affected, ACharacterBase* caster)
{
	Super::OnApply(affected, caster);

	if (!affected) return;

	//ApplyBuffDelta가 이 값을 가감하므로 적용 전에 채운다, 해제 시 같은 값으로 되돌아간다
	movingPoint = -FMath::Max(0.f, affected->GetMovingPoint());
}
