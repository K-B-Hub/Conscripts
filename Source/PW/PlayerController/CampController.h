//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CampController.generated.h"

class UCampDialogueData;
class UCampOptionWidget;
class UCampRecruitWidget;
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

	//확정한 인원을 합류시키고 야영지를 떠난다, 충원 위젯이 호출
	void RecruitCharacters(const TArray<TSubclassOf<AAllyCharacterBase>>& characterClasses);

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

	//메인 캠프에 마우스가 올라오면 강조를 세게, 벗어나면 평소 강도로 되돌린다
	UFUNCTION()
	void HandleCampHovered(AActor* touchedActor);

	UFUNCTION()
	void HandleCampUnhovered(AActor* touchedActor);

	//충원 시작, 캐릭터 선택 화면을 연다, 다 고르면 야영지를 떠난다
	void BeginRecruitFlow(int32 count);

	//지금 부를 수 있는 캐릭터, 야영지에 있는 동료와 이번 런 전사자를 뺀다
	//로스터가 아니라 야영지 인원을 보는 이유는 연속 충원 중 방금 합류한 동료도 걸러야 하기 때문
	TArray<TSubclassOf<AAllyCharacterBase>> BuildRecruitCandidates() const;

	UPROPERTY()
	TObjectPtr<UCampOptionWidget> optionWidgetInstance = nullptr;

	UPROPERTY()
	TObjectPtr<UCampRecruitWidget> recruitWidgetInstance = nullptr;
};