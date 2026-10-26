//Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/Terrain/FireTerrain.h"
#include "PW.h"
#include "Components/BoxComponent.h"

AFireTerrain::AFireTerrain()
{
	terrainName = NSLOCTEXT("Terrain", "Fire_Name", "화염");
	terrainDescription = NSLOCTEXT("Terrain", "Fire_Description", "체류 중 턴을 시작하거나 끝낼 때마다 2의 피해를 입는다");

	damagePerTurn = 2;
	lifeRound = 3;

	//스킬이 명시한 "사용 지점으로부터 1m" 범위, 지형은 박스 판정이라 정사각형이 된다
	terrainBox->SetBoxExtent(FVector(1.f * CmPerMeter, 1.f * CmPerMeter, 2.f * CmPerMeter));
}
