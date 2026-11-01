// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"
#include "LoadingScreenSubsystem.generated.h"

class UUserWidget;
class SWidget;
struct FStreamableHandle;

//레벨 전환 내내 하나의 로딩창을 유지한다
//위젯을 월드가 아닌 GameViewportClient에 붙이므로 OpenLevel을 넘어 살아남는다
UCLASS()
class PW_API ULoadingScreenSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	//로딩창을 페이드 인하고 화면이 다 가려진 뒤에 맵을 연다
	//같은 프레임에 OpenLevel하면 로딩창이 그려지기 전에 블로킹 로드가 시작된다
	void TravelTo(const TSoftObjectPtr<UWorld>& map);

	//비동기 로드가 끝날 때까지 로딩창을 유지한다
	//추적만 하므로 에셋 수명은 요청자가 핸들을 쥐어 유지해야 한다
	void TrackLoadHandle(const TSharedPtr<FStreamableHandle>& handle);

	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	virtual ETickableTickType GetTickableTickType() const override;
	virtual bool IsTickableWhenPaused() const override { return true; }

private:
	void HandlePostLoadMap(UWorld* loadedWorld);

	//맵 로드 이후에도 로딩창을 유지해야 하는지
	bool ShouldKeepShowing();

	//월드·PC 초기화와 맵별 캐릭터 스폰 완료
	bool IsWorldReady(const UWorld* world) const;

	//알파를 진행 방향으로 옮긴다
	void UpdateFade(float deltaTime);

	void Show();

	//페이드 없이 즉시 제거
	void Hide();

	UPROPERTY()
	TObjectPtr<UUserWidget> loadingWidget;

	TSharedPtr<SWidget> loadingSlate;

	TArray<TWeakPtr<FStreamableHandle>> loadHandles;

	//열 대기 중인 맵, 페이드 인이 끝난 뒤 연다
	TSoftObjectPtr<UWorld> pendingMap;

	float fadeAlpha = 0.f;

	//false면 페이드 인, 끝나면 알파 1에서 멈춘다
	bool bFadingOut = false;

	uint64 shownFrame = 0;

	//OpenLevel 요청부터 새 맵 로드 완료까지
	bool bWaitingForMap = false;

	//유지 조건이 모두 풀린 시각, 0이면 아직 안 풀림
	double readySince = 0.0;
};
