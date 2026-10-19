#pragma once

#include "CoreMinimal.h"

//게임 내 1m에 해당하는 언리얼 단위(cm), 이동력·시야 등 m 단위 스탯 변환의 단일 출처 (캐릭터 2.55배 기준)
inline constexpr float CmPerMeter = 255.f;

//언리얼 기본 사람 크기(1m=100cm) 대비 캐릭터 배율, 캡슐·눈높이 등 몸 크기 값의 기준
inline constexpr float CharacterScale = CmPerMeter / 100.f;