//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Rest/RestBase.h"
#include "ReinforcementRest.generated.h"

//전투 중 아군 합류, 인원만 다른 BP 파생으로 등급을 나눈다(중급 1명·상급 2명)
//직업 선택 화면이 필요한데 이 시점은 강화 선택 한가운데라 위젯이 겹친다
//그래서 여기서는 컨트롤러에 요청만 하고, 실제 합류는 강화 선택이 다 끝난 뒤에 일어난다
//로스터가 가득 차면 UUpgradeLibrary::CanAcquire가 후보 단계에서 걸러낸다
UCLASS()
class PW_API UReinforcementRest : public URestBase
{
	GENERATED_BODY()

public:
	virtual void Execute(ACharacterBase* instigator) override;

protected:
	//합류시킬 인원, 인원을 고치면 skillDescription도 함께 고쳐야 한다
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rest")
	int32 recruitCount = 1;
};