// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Ailment/AilmentBase.h"


void UAilmentBase::OnApply(ACharacterBase* affected, ACharacterBase* caster)
{
	// 파생 클래스에서 구현 — caster 스탯 이용해 Execute에 필요한 값들 설정
}

void UAilmentBase::Execute(ACharacterBase* affected)
{
}