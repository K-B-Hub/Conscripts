// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "BgmSubsystem.generated.h"

class USoundBase;
class UAudioComponent;

//배경음 재생과 곡 전환 크로스페이드
//월드 단위라 레벨 전환 시 컴포넌트와 함께 사라진다
UCLASS()
class PW_API UBgmSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	//곡 전환, 같은 곡이 재생 중이면 무시하고 nullptr이면 페이드 아웃만 한다
	void Play(USoundBase* sound);

private:
	//소유자 없는 오디오 컴포넌트는 GC되면 루핑이 끊기므로 붙잡아 둔다
	UPROPERTY()
	TObjectPtr<UAudioComponent> current;

	//페이드 아웃 중인 이전 곡
	UPROPERTY()
	TObjectPtr<UAudioComponent> previous;
};
