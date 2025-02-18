// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine.h"
#include "MyProcessVoxelPoint.generated.h"

/**
 * 
 */
UCLASS()
class LYRAGAME_API UMyProcessVoxelPoint : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Voxel|Points")
	static void FVoxelPrintTest();
};
