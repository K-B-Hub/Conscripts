#pragma once

#include "CoreMinimal.h"

//게임 내 1m에 해당하는 언리얼 단위(cm), 이동력·시야 등 m 단위 스탯 변환의 단일 출처 (캐릭터 4.7배 기준)
inline constexpr float CmPerMeter = 470.f;

//언리얼 기본 사람 크기(1m=100cm) 대비 캐릭터 배율, 캡슐·눈높이 등 몸 크기 값의 기준
inline constexpr float CharacterScale = CmPerMeter / 100.f;

class UWorld;
class UNiagaraSystem;
class UNiagaraComponent;

//위치 고정 이펙트 스폰, SpawnSystemAtLocation은 숨김 액터(WorldSettings) 소유라 데칼이 게임에서 안 그려진다
PW_API UNiagaraComponent* SpawnFxAtLocation(UWorld* world, UNiagaraSystem* system, const FVector& location, const FRotator& rotation = FRotator::ZeroRotator);