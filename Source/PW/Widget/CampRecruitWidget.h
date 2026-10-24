//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CampRecruitWidget.generated.h"

class UButton;
class UPanelWidget;
class UTextBlock;
class UJobSelectButton;
class AAllyCharacterBase;

//확정한 합류 인원을 통지
DECLARE_DELEGATE_OneParam(FOnRecruitConfirmed, const TArray<TSubclassOf<AAllyCharacterBase>>&);

//충원 시 캐릭터를 고르는 화면
//후보를 합류 목록으로 옮겨 인원을 채운 뒤 한 번에 확정한다
//야영지 충원과 전투 중 증원이 같은 위젯을 쓰므로 컨트롤러를 알지 않고 델리게이트로 통지한다
//후보 선별은 화면마다 다르므로(야영지는 로스터, 전투는 참전 인원) 띄우는 쪽이 목록을 넘긴다
UCLASS()
class PW_API UCampRecruitWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	//후보와 합류시킬 인원으로 화면을 연다, 후보가 인원보다 적으면 후보 수로 줄인다
	void Open(const TArray<TSubclassOf<AAllyCharacterBase>>& candidates, int32 count);

	//확정 통지, 위젯을 띄운 컨트롤러가 바인딩
	FOnRecruitConfirmed OnRecruitConfirmed;

protected:
	virtual void NativeConstruct() override;

	//고를 수 있는 후보, 합류 목록에 넣은 캐릭터는 여기서 사라진다
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPanelWidget> JobContainer;

	//이번에 합류할 인원
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPanelWidget> RosterContainer;

	//"1 / 2" 형태의 진행 표시
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CountText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ConfirmButton;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UJobSelectButton> jobButtonClass;

private:
	void BuildJobList();
	void BuildRosterList();
	//합류 인원이 정원을 채웠을 때만 확정 가능
	void RefreshConfirmState();

	//후보 클릭, 정원이 남아 있으면 합류 목록에 추가
	void HandleCharacterClicked(TSubclassOf<AAllyCharacterBase> characterClass);
	//합류 목록 클릭, 후보로 되돌린다
	void HandleRosterClicked(TSubclassOf<AAllyCharacterBase> characterClass);

	UFUNCTION()
	void HandleConfirmClicked();

	TArray<TSubclassOf<AAllyCharacterBase>> candidatePool;

	//확정 전 합류 목록
	TArray<TSubclassOf<AAllyCharacterBase>> pendingRecruits;

	int32 recruitCapacity = 0;
};
