// Fill out your copyright notice in the Description page of Project Settings.

#include "GameInstance/LoadingScreenSubsystem.h"
#include "GameInstance/PWGameInstance.h"
#include "GameInstance/BgmSubsystem.h"
#include "GameMode/BattleGameMode.h"
#include "GameMode/CampGameMode.h"
#include "Blueprint/UserWidget.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "Engine/StreamableManager.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "ShaderPipelineCache.h"

namespace
{
	//다른 UI 위에 덮이도록
	constexpr int32 LoadingScreenZOrder = 1000;
}

void ULoadingScreenSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ULoadingScreenSubsystem::HandlePostLoadMap);
	if (GEngine) GEngine->OnTravelFailure().AddUObject(this, &ULoadingScreenSubsystem::HandleTravelFailure);
}

void ULoadingScreenSubsystem::Deinitialize()
{
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
	if (GEngine) GEngine->OnTravelFailure().RemoveAll(this);
	Hide();

	Super::Deinitialize();
}

void ULoadingScreenSubsystem::TravelTo(const TSoftObjectPtr<UWorld>& map)
{
	pendingMap = map;
	Show();

	//로딩창 페이드와 함께 곡을 내린다, 월드가 내려가면 끊기므로 남은 페이드는 생략된다
	if (UBgmSubsystem* bgm = UWorld::GetSubsystem<UBgmSubsystem>(GetGameInstance()->GetWorld()))
	{
		bgm->Play(nullptr);
	}
}

void ULoadingScreenSubsystem::TrackLoadHandle(const TSharedPtr<FStreamableHandle>& handle)
{
	if (handle.IsValid()) loadHandles.Add(handle);
}

void ULoadingScreenSubsystem::Tick(float DeltaTime)
{
	UpdateFade(DeltaTime);

	if (!pendingMap.IsNull())
	{
		//OpenLevel은 다음 엔진 틱에 바로 블로킹 로드하므로 로딩창이 한 번 그려진 뒤 연다
		if (GFrameCounter == shownFrame) return;

		//반투명 상태로 로드가 멈추면 이전 화면이 비친 채 굳는다
		if (loadingSlate.IsValid() && fadeAlpha < 1.f) return;

		const TSoftObjectPtr<UWorld> map = pendingMap;
		pendingMap.Reset();
		bWaitingForMap = true;
		UGameplayStatics::OpenLevelBySoftObjectPtr(GetGameInstance(), map);
		return;
	}

	if (!loadingSlate.IsValid()) return;

	if (bFadingOut)
	{
		if (fadeAlpha <= 0.f) Hide();
		return;
	}

	//월드 시간은 일시정지·전환에 영향받으므로 실제 시간으로 잰다
	const double now = FPlatformTime::Seconds();
	const UPWGameInstance* gameInstance = Cast<UPWGameInstance>(GetGameInstance());

	if (ShouldKeepShowing())
	{
		//유지 조건이 끝내 안 풀려도 갇히지 않도록 맵 로드 후 상한이 지나면 걷는다
		//블로킹 로드 시간을 빼기 위해 요청 시각이 아니라 로드 완료 시각부터 잰다
		const float maxWaitSeconds = gameInstance ? gameInstance->GetLoadingMaxWaitSeconds() : 30.f;
		if (!bWaitingForMap && mapLoadedAt > 0.0 && now - mapLoadedAt >= maxWaitSeconds)
		{
			UE_LOG(LogTemp, Warning, TEXT("[LoadingScreen] 맵 로드 후 %.0f초가 지나도 준비 조건이 남아 로딩창을 걷습니다"), maxWaitSeconds);
			bFadingOut = true;
			return;
		}

		readySince = 0.0;
		return;
	}

	if (readySince == 0.0) readySince = now;

	//텍스처 스트리밍 등 완료 시점을 알 수 없는 로딩을 가린다
	const float holdSeconds = gameInstance ? gameInstance->GetLoadingHoldSeconds() : 0.f;
	if (now - readySince >= holdSeconds) bFadingOut = true;
}

void ULoadingScreenSubsystem::UpdateFade(float deltaTime)
{
	if (!loadingWidget) return;

	const UPWGameInstance* gameInstance = Cast<UPWGameInstance>(GetGameInstance());
	const float fadeSeconds = gameInstance ? gameInstance->GetLoadingFadeSeconds() : 0.f;

	//0초면 한 틱에 목표값으로 간다
	const float step = fadeSeconds > 0.f ? deltaTime / fadeSeconds : 1.f;
	fadeAlpha = FMath::Clamp(fadeAlpha + (bFadingOut ? -step : step), 0.f, 1.f);
	loadingWidget->SetRenderOpacity(fadeAlpha);
}

TStatId ULoadingScreenSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(ULoadingScreenSubsystem, STATGROUP_Tickables);
}

ETickableTickType ULoadingScreenSubsystem::GetTickableTickType() const
{
	//CDO는 틱하지 않는다
	return IsTemplate() ? ETickableTickType::Never : ETickableTickType::Always;
}

void ULoadingScreenSubsystem::HandlePostLoadMap(UWorld* loadedWorld)
{
	//PIE 다중 인스턴스면 다른 인스턴스의 로드도 통지된다
	if (loadedWorld != GetGameInstance()->GetWorld()) return;

	bWaitingForMap = false;
	readySince = 0.0;
	mapLoadedAt = FPlatformTime::Seconds();
}

void ULoadingScreenSubsystem::HandleTravelFailure(UWorld* world, ETravelFailure::Type failureType, const FString& error)
{
	//PIE 다중 인스턴스면 다른 인스턴스의 실패도 통지된다
	if (world && world != GetGameInstance()->GetWorld()) return;
	if (!bWaitingForMap) return;

	//남은 월드가 이전 맵이든 기본 맵이든 그 월드 기준으로 해제 조건을 다시 본다
	UE_LOG(LogTemp, Warning, TEXT("[LoadingScreen] 맵 이동 실패(%s): %s"), ETravelFailure::ToString(failureType), *error);
	bWaitingForMap = false;
	readySince = 0.0;
}

bool ULoadingScreenSubsystem::ShouldKeepShowing()
{
	//맵 패키지 로드
	if (bWaitingForMap) return true;

	const UWorld* world = GetGameInstance()->GetWorld();
	if (!world || !IsWorldReady(world)) return true;

	//요청자가 놓았거나 끝난 핸들은 정리
	loadHandles.RemoveAll([](const TWeakPtr<FStreamableHandle>& weak)
	{
		const TSharedPtr<FStreamableHandle> handle = weak.Pin();
		return !handle.IsValid() || !handle->IsLoadingInProgress();
	});
	if (loadHandles.Num() > 0) return true;

	//패키징 빌드에서만 의미가 있고 에디터에서는 항상 0이다
	return FShaderPipelineCache::NumPrecompilesRemaining() > 0;
}

bool ULoadingScreenSubsystem::IsWorldReady(const UWorld* world) const
{
	if (!world->HasBegunPlay()) return false;

	const APlayerController* playerController = world->GetFirstPlayerController();
	if (!playerController || !playerController->HasActorBegunPlay()) return false;

	if (const ABattleGameMode* battle = world->GetAuthGameMode<ABattleGameMode>())
	{
		return battle->AreEnemiesSpawned();
	}
	if (const ACampGameMode* camp = world->GetAuthGameMode<ACampGameMode>())
	{
		return camp->IsRosterSpawned();
	}
	return true;
}

void ULoadingScreenSubsystem::Show()
{
	shownFrame = GFrameCounter;
	readySince = 0.0;
	//이전 전환의 로드 시각으로 상한이 바로 걸리지 않게
	mapLoadedAt = 0.0;

	//페이드 아웃 중이면 새로 만들지 않고 현재 알파에서 되돌린다
	bFadingOut = false;
	if (loadingSlate.IsValid()) return;

	UPWGameInstance* gameInstance = Cast<UPWGameInstance>(GetGameInstance());
	UGameViewportClient* viewport = GetGameInstance()->GetGameViewportClient();
	if (!gameInstance || !viewport || !gameInstance->GetLoadingScreenClass()) return;

	//월드가 아닌 GameInstance를 Outer로 둬야 월드 파괴에 휩쓸리지 않는다
	loadingWidget = CreateWidget<UUserWidget>(gameInstance, gameInstance->GetLoadingScreenClass());
	if (!loadingWidget) return;

	//투명해도 Visible이라 페이드 인 동안 클릭을 막는다
	fadeAlpha = 0.f;
	loadingWidget->SetRenderOpacity(fadeAlpha);

	//AddToViewport는 월드 정리 시 제거되므로 뷰포트에 직접 붙인다
	loadingSlate = loadingWidget->TakeWidget();
	viewport->AddViewportWidgetContent(loadingSlate.ToSharedRef(), LoadingScreenZOrder);

	//화면이 가려진 동안은 프레임을 희생해 PSO를 빨리 컴파일한다
	FShaderPipelineCache::SetBatchMode(FShaderPipelineCache::BatchMode::Fast);
}

void ULoadingScreenSubsystem::Hide()
{
	if (!loadingSlate.IsValid()) return;

	if (UGameViewportClient* viewport = GetGameInstance()->GetGameViewportClient())
	{
		viewport->RemoveViewportWidgetContent(loadingSlate.ToSharedRef());
	}

	loadingSlate.Reset();
	loadingWidget = nullptr;
	readySince = 0.0;
	mapLoadedAt = 0.0;
	fadeAlpha = 0.f;
	bFadingOut = false;

	FShaderPipelineCache::SetBatchMode(FShaderPipelineCache::BatchMode::Background);
}
