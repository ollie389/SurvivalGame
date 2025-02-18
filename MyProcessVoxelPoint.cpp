// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProcessVoxelPoint.h"

void UMyProcessVoxelPoint::FVoxelPrintTest()
{
	UWorld* World = GEngine->GetWorldContexts()[0].World();
	UKismetSystemLibrary::PrintString(World);
	//UKismetSystemLibrary::SphereTraceMultiForObjects(World, )
	return;
};

