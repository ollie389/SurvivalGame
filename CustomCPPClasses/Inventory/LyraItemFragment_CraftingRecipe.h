// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/LyraInventoryItemDefinition.h"
#include "GameplayTagContainer.h"
#include "../../Inventory/LyraInventoryItemDefinition.h"
#include "LyraItemFragment_CraftingRecipe.generated.h"

/**
 * 
 */

USTRUCT(Blueprintable, BlueprintType)
struct LYRAGAME_API FCraftingIngredient
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Crafting)
	TSubclassOf<ULyraInventoryItemDefinition> ItemDefinition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Crafting)
	float ConsumptionCount;

	// Constructor to initialize properties
	FCraftingIngredient()
	{
		ConsumptionCount = 0.0f; // Initialize ConsumptionCount
	}

};

UCLASS(BlueprintType)
class LYRAGAME_API ULyraItemFragment_CraftingRecipe : public ULyraInventoryItemFragment
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Crafting)
	FGameplayTag CraftingStation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Crafting)
	float CraftingDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Crafting)
	TArray<FCraftingIngredient> Ingredients;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Crafting)
	TSubclassOf<ULyraInventoryItemDefinition> ResultingItemDef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Crafting)
	int ResultingItemCount;
};
