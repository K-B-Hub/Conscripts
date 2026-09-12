//Fill out your copyright notice in the Description page of Project Settings.

#include "Run/RunProgress.h"

void URunProgress::StartRun(FName inStoryRouteId)
{
	stageIndex = 0;
	bRunActive = true;
	storyRouteId = inStoryRouteId;
}

void URunProgress::EndRun()
{
	bRunActive = false;
}