//Fill out your copyright notice in the Description page of Project Settings.

#include "Settings/PWGameUserSettings.h"
#include "Engine/Engine.h"

UPWGameUserSettings* UPWGameUserSettings::Get()
{
	return GEngine ? Cast<UPWGameUserSettings>(GEngine->GetGameUserSettings()) : nullptr;
}

void UPWGameUserSettings::SetToDefaults()
{
	Super::SetToDefaults();

	enemyTurnSpeed = 1.f;
	bEnemyTurnCameraFollow = true;
	skillCameraMode = ESkillCameraMode::All;
	skillBannerMode = ESkillCameraMode::EnemyOnly;
	cameraMoveSpeedScale = 1.f;
	masterVolume = 1.f;
	musicVolume = 1.f;
	sfxVolume = 1.f;
}

const TArray<float>& UPWGameUserSettings::GetEnemyTurnSpeedOptions()
{
	//TODO 체감 밸런스 미확정 — 적 70명 전장에서 돌려 보고 확정
	static const TArray<float> options = { 1.f, 2.f, 3.f, 4.f };
	return options;
}

ESkillCameraMode UPWGameUserSettings::GetSkillCameraMode() const
{
	//범위 밖 값은 기본값으로
	return skillCameraMode <= ESkillCameraMode::All ? skillCameraMode : ESkillCameraMode::All;
}

ESkillCameraMode UPWGameUserSettings::GetSkillBannerMode() const
{
	//범위 밖 값은 기본값으로
	return skillBannerMode <= ESkillCameraMode::All ? skillBannerMode : ESkillCameraMode::EnemyOnly;
}

float UPWGameUserSettings::GetEnemyTurnSpeed() const
{
	//0이면 딜레이 나눗셈이 터지므로 선택지 범위로 묶는다
	const TArray<float>& options = GetEnemyTurnSpeedOptions();
	return FMath::Clamp(enemyTurnSpeed, options[0], options.Last());
}

const TArray<float>& UPWGameUserSettings::GetCameraMoveSpeedOptions()
{
	static const TArray<float> options = { 0.5f, 0.75f, 1.f, 1.5f, 2.f };
	return options;
}

float UPWGameUserSettings::GetCameraMoveSpeedScale() const
{
	//0이면 카메라가 멈추므로 선택지 범위로 묶는다
	const TArray<float>& options = GetCameraMoveSpeedOptions();
	return FMath::Clamp(cameraMoveSpeedScale, options[0], options.Last());
}