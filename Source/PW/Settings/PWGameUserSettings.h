//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "PWGameUserSettings.generated.h"

//기기 지역 설정, 런 진행도(UPWSaveGame)와 분리된다 — 세이브 초기화가 해상도를 날리면 안 되므로
//해상도·창모드·VSync·프레임제한·품질은 UGameUserSettings가 이미 전부 들고 있어 여기서 다시 선언하지 않는다
//따라서 이 클래스는 게임 고유 값만 든다
UCLASS()
class PW_API UPWGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()

public:
	//설정 접근 단일 지점, 엔진이 첫 프레임 전에 ini를 로드해 두므로 언제든 유효하다
	static UPWGameUserSettings* Get();

	//기본값 복원 버튼의 대상, 엔진 항목은 Super가 처리한다
	virtual void SetToDefaults() override;

	//적 턴 연출 배속, 딜레이를 이 값으로 나누고 이동 속도에 곱한다
	//ini는 수동 편집이 가능하므로 읽는 쪽에서 범위를 보장한다
	float GetEnemyTurnSpeed() const;
	void SetEnemyTurnSpeed(float newSpeed) { enemyTurnSpeed = newSpeed; }

	//적 턴에 카메라가 행동 주체를 따라가는지
	bool IsEnemyTurnCameraFollowEnabled() const { return bEnemyTurnCameraFollow; }
	void SetEnemyTurnCameraFollowEnabled(bool bEnabled) { bEnemyTurnCameraFollow = bEnabled; }

	//설정 화면이 제시할 배속 선택지, 오름차순 전제
	//저장값이 인덱스가 아니라 배율 자체라 목록이 바뀌어도 기존 ini의 의미가 유지된다
	static const TArray<float>& GetEnemyTurnSpeedOptions();

private:
	UPROPERTY(config)
	float enemyTurnSpeed = 1.f;

	UPROPERTY(config)
	bool bEnemyTurnCameraFollow = true;
};