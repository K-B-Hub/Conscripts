//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/Sniper/Random/High/Ailment/SniperOpenWoundAilment.h"

USniperOpenWoundAilment::USniperOpenWoundAilment()
{
	ailmentName = NSLOCTEXT("Ailment", "SniperOpenWoundAilment_Name", "벌어진 상처");
	ailmentDescription = NSLOCTEXT("Ailment", "SniperOpenWoundAilment_Description", "상처가 아물지 않아 턴 시작마다 최대 체력의 10%에 해당하는 피해를 입는다");

	ailmentTurn = 2;
}
