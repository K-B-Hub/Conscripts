// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Buff/BuffBase.h"

void UBuffBase::OnApply(ACharacterBase* affected, ACharacterBase* caster)
{
	// 파생 클래스에서 구현 — caster 스탯을 통해 버프의 고유 Execute 로직에 필요한 값들 저장
}

void UBuffBase::Execute(ACharacterBase* affected)
{
	// 파생 클래스에서 구현 — 도트 피해, 회복, 상태이상 트리거 등
}