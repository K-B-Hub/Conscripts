//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CampController.generated.h"

class UCampDialogueData;
class UCampOptionWidget;
class UCampRecruitWidget;
class UUpgradeSelectWidget;
class USkillBase;
class AAllyCharacterBase;

//야영지 컨트롤러
//카메라가 고정이라 조작 입력이 없고 마우스 오버와 클릭만 다룬다
//말풍선은 캐릭터의 위젯 컴포넌트라 이 클래스가 위젯 인스턴스를 들고 있지 않는다
UCLASS()
class PW_API ACampController : public APlayerController
{
	GENERATED_BODY()

public:
	ACampController();

	//정비 선택 확정, 선택 위젯이 호출
	void ChooseCampOption(int32 optionIndex);

	//선택하지 않고 창만 닫는다
	void CloseCampOptions();

	//충원할 직업 선택, 충원 위젯이 호출
	void RecruitJob(int32 jobIndex);

protected:
	virtual void BeginPlay() override;

	//상태별 대사 풀, BP에서 지정
	UPROPERTY(EditDefaultsOnly, Category = "Camp")
	TObjectPtr<UCampDialogueData> dialogueData;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UCampOptionWidget> optionWidgetClass;

	//충원 시 직업을 고르는 화면
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UCampRecruitWidget> recruitWidgetClass;

	//충원 동료의 레벨업 강화 선택, 전투와 같은 위젯
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUpgradeSelectWidget> upgradeSelectWidgetClass;

private:
	//레벨에 배치된 자리와 메인 캠프에 마우스 이벤트를 연결
	void BindCampActors();

	//자리에 마우스가 올라오면 그 자리에 선 아군의 말풍선을 켠다
	UFUNCTION()
	void HandlePointHovered(AActor* touchedActor);

	UFUNCTION()
	void HandlePointUnhovered(AActor* touchedActor);

	//메인 캠프 클릭, 정비 선택지를 연다
	UFUNCTION()
	void HandleCampClicked(AActor* touchedActor, FKey buttonPressed);

	//충원 시작, 직업 선택 화면을 연다
	void BeginRecruitFlow(int32 count);

	//충원이 끝난 뒤 새 동료들의 레벨업 강화를 차례로 고르게 한다
	//다 끝나면 야영지를 떠난다
	void AdvanceUpgradeFlow();
	void ShowUpgradeSelect();
	void OnUpgradeChosen(TSubclassOf<USkillBase> chosen);

	UPROPERTY()
	TObjectPtr<UCampOptionWidget> optionWidgetInstance = nullptr;

	UPROPERTY()
	TObjectPtr<UCampRecruitWidget> recruitWidgetInstance = nullptr;

	UPROPERTY()
	TObjectPtr<UUpgradeSelectWidget> upgradeSelectWidgetInstance = nullptr;

	//아직 고르지 않은 충원 인원 수
	int32 remainingRecruits = 0;

	//강화 선택이 남은 새 동료들, 앞에서부터 처리한다
	UPROPERTY()
	TArray<TObjectPtr<AAllyCharacterBase>> upgradePendingAllies;

	//지금 강화를 고르는 중인 동료
	UPROPERTY()
	TObjectPtr<AAllyCharacterBase> currentUpgradeAlly = nullptr;
};