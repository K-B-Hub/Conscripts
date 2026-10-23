//Fill out your copyright notice in the Description page of Project Settings.

#include "Run/AllyRunState.h"
#include "Characters/AllyCharacterBase.h"

bool FAllyRunState::IsValid() const
{
	return AllyClass != nullptr;
}

FAllyRunState FAllyRunState::MakeFromClass(TSubclassOf<AAllyCharacterBase> allyClass)
{
	if (!allyClass) return FAllyRunState();

	//CDO를 그대로 캡처하면 레벨 1·강화 없음 상태의 캐릭터 기본 스탯이 나온다
	return allyClass->GetDefaultObject<AAllyCharacterBase>()->CaptureRunState();
}