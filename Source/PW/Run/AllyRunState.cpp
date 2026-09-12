//Fill out your copyright notice in the Description page of Project Settings.

#include "Run/AllyRunState.h"
#include "Characters/AllyCharacterBase.h"

bool FAllyRunState::IsValid() const
{
	return AllyClass != nullptr;
}

FAllyRunState FAllyRunState::MakeFromClass(TSubclassOf<AAllyCharacterBase> allyClass, const FString& displayName)
{
	if (!allyClass) return FAllyRunState();

	//CDO를 그대로 캡처하면 레벨 1·강화 없음 상태의 직업 기본 스탯이 나온다
	FAllyRunState state = allyClass->GetDefaultObject<AAllyCharacterBase>()->CaptureRunState();
	state.DisplayName = displayName;
	return state;
}