//Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/MovingPointWidget.h"
#include "Characters/CharacterBase.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UMovingPointWidget::InitMovingPoint(ACharacterBase* unit)
{
	trackedUnit = unit;

	//캐시를 비워 다음 틱이 한 번은 반드시 쓰게 한다
	//현재치가 같아도 최대치가 바뀌었을 수 있다 — 레벨업·패시브가 movingPoint를 올린다
	shownTenths = -1;
}

void UMovingPointWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsFocusable(false);
}

void UMovingPointWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!IsValid(trackedUnit)) return;

	//AAllyCharacterBase::Tick이 프레임마다 ConsumeMovingPoint를 부르므로 폴링이 유일한 실시간 경로다
	//변경 델리게이트를 만들어도 매 프레임 브로드캐스트라 비용이 같고 배선만 늘어난다
	const float current = trackedUnit->GetCurrentMovingPoint();
	const int32 tenths = FMath::RoundToInt(current * 10.f);
	if (tenths == shownTenths) return;

	shownTenths = tenths;

	const float max = trackedUnit->GetMovingPoint();
	if (ValueText)
	{
		ValueText->SetText(FText::FromString(FString::Printf(TEXT("%.1f / %.1f"), current, max)));
	}
	if (MovingPointBar)
	{
		//달리기 등으로 최대치를 넘을 수 있다, 1을 넘기면 Slate가 바를 넘쳐 그린다
		//숫자는 실제 값을 그대로 보여주고 바만 잘라낸다
		MovingPointBar->SetPercent(max > 0.f ? FMath::Min(1.f, current / max) : 0.f);
	}
}