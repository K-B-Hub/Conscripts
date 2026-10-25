//Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/Terrain/SmokeTerrain.h"
#include "PW.h"
#include "Components/BoxComponent.h"

ASmokeTerrain::ASmokeTerrain()
{
	terrainName = NSLOCTEXT("Terrain", "Smoke_Name", "연막");
	terrainDescription = NSLOCTEXT("Terrain", "Smoke_Description", "시야를 가려 체류 중인 캐릭터의 회피가 30 증가한다");

	evasion = 30.f;
	lifeRound = 3;

	//스킬이 명시한 "사용 지점으로부터 1m" 범위, 지형은 박스 판정이라 정사각형이 된다
	terrainBox->SetBoxExtent(FVector(1.f * CmPerMeter, 1.f * CmPerMeter, 2.f * CmPerMeter));

	//회피 상승의 기대 HP 환산은 상대 공격력에 달려 있어 정적으로 낼 수 없다, AI 평가는 중립
	aiBaseValue = 0.f;
	aiPassThroughValue = 0.f;
}