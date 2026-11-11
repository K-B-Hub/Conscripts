// Fill out your copyright notice in the Description page of Project Settings.

#include "GameInstance/BgmSubsystem.h"
#include "GameInstance/PWGameInstance.h"
#include "GameInstance/LoadingScreenSubsystem.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

void UBgmSubsystem::Play(USoundBase* sound)
{
	//끄기는 바로, 켜기는 로딩창이 걷힐 때까지 보류
	const UGameInstance* gi = GetWorld()->GetGameInstance();
	const ULoadingScreenSubsystem* loading = gi ? gi->GetSubsystem<ULoadingScreenSubsystem>() : nullptr;
	if (sound && loading && loading->IsCovering())
	{
		pendingSound = sound;
		return;
	}
	pendingSound = nullptr;

	//끝난 비루핑 곡은 자동 파괴되어 IsValid로 걸러진다
	if (IsValid(current) && current->Sound == sound && current->IsPlaying()) return;

	const UPWGameInstance* gameInstance = GetWorld()->GetGameInstance<UPWGameInstance>();
	const float fadeSeconds = gameInstance ? gameInstance->GetBgmFadeSeconds() : 0.f;

	if (IsValid(current)) current->FadeOut(fadeSeconds, 0.f);
	previous = current;
	current = sound ? UGameplayStatics::CreateSound2D(this, sound) : nullptr;
	if (!current) return;

	//에셋의 사운드 클래스와 무관하게 음악 음량 설정을 따르게 한다
	if (gameInstance && gameInstance->GetMusicSoundClass())
	{
		current->SoundClassOverride = gameInstance->GetMusicSoundClass();
	}

	//일시정지 메뉴에서 음악 음량을 조절하며 들을 수 있게 정지 중에도 재생
	current->bIsUISound = true;

	//CreateSound2D는 재생 전 생성이라 자동 파괴가 꺼져 있다
	current->bAutoDestroy = true;
	current->FadeIn(fadeSeconds);
}

void UBgmSubsystem::PlayPending()
{
	if (!pendingSound) return;

	USoundBase* sound = pendingSound;
	pendingSound = nullptr;
	Play(sound);
}
