//Fill out your copyright notice in the Description page of Project Settings.

#include "GameMode/CampGameMode.h"
#include "PlayerController/CampController.h"
#include "Actors/CampSpawnPoint.h"
#include "Characters/AllyCharacterBase.h"
#include "GameInstance/PWGameInstance.h"
#include "Run/RunProgress.h"
#include "Camera/CameraActor.h"
#include "EngineUtils.h"

ACampGameMode::ACampGameMode()
{
	PlayerControllerClass = ACampController::StaticClass();

	//야영지는 조작할 대상이 없고 카메라는 레벨에 배치된 CameraActor가 맡는다
	DefaultPawnClass = nullptr;
}

void ACampGameMode::BeginPlay()
{
	Super::BeginPlay();

	//모든 액터의 BeginPlay 완료를 보장하기 위해 다음 틱으로 지연
	GetWorldTimerManager().SetTimerForNextTick([this]()
	{
		ApplyFixedCamera();
		SpawnRoster();
	});
}

void ACampGameMode::ApplyFixedCamera()
{
	APlayerController* playerController = GetWorld()->GetFirstPlayerController();
	if (!playerController) return;

	for (TActorIterator<ACameraActor> It(GetWorld()); It; ++It)
	{
		playerController->SetViewTarget(*It);
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[CampGameMode] 레벨에 CameraActor가 없어 시점이 고정되지 않습니다"));
}

void ACampGameMode::SpawnRoster()
{
	for (TActorIterator<ACampSpawnPoint> It(GetWorld()); It; ++It)
	{
		spawnPoints.Add(*It);
	}

	const UPWGameInstance* gameInstance = GetGameInstance<UPWGameInstance>();
	const URunProgress* runProgress = gameInstance ? gameInstance->GetRunProgress() : nullptr;
	if (!runProgress) return;

	const TArray<FAllyRunState>& roster = runProgress->GetRoster();
	if (roster.Num() > spawnPoints.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("[CampGameMode] 자리(%d)가 로스터(%d)보다 적어 일부가 서지 못합니다"),
			spawnPoints.Num(), roster.Num());
	}

	for (const FAllyRunState& state : roster)
	{
		ACampSpawnPoint* point = TakeFreePoint();
		if (!point || !state.AllyClass) continue;

		AAllyCharacterBase* ally = GetWorld()->SpawnActor<AAllyCharacterBase>(
			state.AllyClass, point->GetActorLocation(), point->GetActorRotation());
		if (!ally) continue;

		ally->RestoreRunState(state);
		ally->PlayRandomCampIdle();

		point->SetOccupant(ally);
		campAllies.Add(ally);
	}

	UE_LOG(LogTemp, Log, TEXT("[CampGameMode] 야영지 도착, 인원 %d명"), campAllies.Num());
}

ACampSpawnPoint* ACampGameMode::TakeFreePoint()
{
	for (ACampSpawnPoint* point : spawnPoints)
	{
		if (point && !point->GetOccupant()) return point;
	}
	return nullptr;
}

int32 ACampGameMode::GetAverageLevel() const
{
	if (campAllies.Num() == 0) return 1;

	int32 total = 0;
	for (const AAllyCharacterBase* ally : campAllies)
	{
		if (ally) total += ally->GetLevel();
	}

	//내림으로 처리, 최소 1레벨은 보장
	return FMath::Max(1, total / campAllies.Num());
}

AAllyCharacterBase* ACampGameMode::AddRecruit(TSubclassOf<AAllyCharacterBase> jobClass, const FString& displayName, int32 level)
{
	const UPWGameInstance* gameInstance = GetGameInstance<UPWGameInstance>();
	if (gameInstance && campAllies.Num() >= gameInstance->GetMaxRosterSize())
	{
		UE_LOG(LogTemp, Log, TEXT("[CampGameMode] 로스터가 가득 차 충원하지 않습니다"));
		return nullptr;
	}

	ACampSpawnPoint* point = TakeFreePoint();
	if (!point || !jobClass) return nullptr;

	AAllyCharacterBase* ally = GetWorld()->SpawnActor<AAllyCharacterBase>(
		jobClass, point->GetActorLocation(), point->GetActorRotation());
	if (!ally) return nullptr;

	ally->displayName = displayName;

	//ForceLevelUpTo가 확률 성장을 굴리고 레벨당 강화를 대기 큐에 쌓는다
	//큐 소비(강화 선택)는 컨트롤러가 즉시 진행한다
	ally->ForceLevelUpTo(level);
	ally->PlayRandomCampIdle();

	point->SetOccupant(ally);
	campAllies.Add(ally);

	return ally;
}

void ACampGameMode::LeaveCamp()
{
	UPWGameInstance* gameInstance = GetGameInstance<UPWGameInstance>();
	URunProgress* runProgress = gameInstance ? gameInstance->GetRunProgress() : nullptr;
	if (!gameInstance || !runProgress) return;

	//정비 결과와 새 동료를 로스터에 반영, 전투 종료와 같은 회수 경로
	TArray<AAllyCharacterBase*> members;
	members.Reserve(campAllies.Num());
	for (const TObjectPtr<AAllyCharacterBase>& ally : campAllies)
	{
		members.Add(ally);
	}
	runProgress->CaptureFromWorld(members);

	UE_LOG(LogTemp, Log, TEXT("[CampGameMode] 야영지 출발, 인원 %d명"), runProgress->GetRoster().Num());

	if (runProgress->AdvanceStage())
	{
		if (gameInstance->TravelToCurrentStage(this)) return;

		//맵 지정이 빠져 더 갈 수 없는 것뿐이라 완주로 치지 않는다
		gameInstance->EndRunAndReturnToHub(this, false);
		return;
	}

	//야영지가 마지막 스테이지였다, 런 완주
	gameInstance->EndRunAndReturnToHub(this, true);
}