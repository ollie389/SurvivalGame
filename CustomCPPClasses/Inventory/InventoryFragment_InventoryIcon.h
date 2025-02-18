// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Inventory/LyraInventoryItemDefinition.h"
#include "Inventory/IPickupable.h"

#include "Internationalization/Text.h"
#include "Styling/SlateBrush.h"
#include "UObject/UObjectGlobals.h"

#include "InventoryFragment_InventoryIcon.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class ERarity : uint8
{
	Poor,		// Grey
	Common,		// White
	Uncommon,	// Green
	Rare,		// Blue
	Epic,		// Purple
	Legendary	// Orange
};

class UObject;

UCLASS()
class LYRAGAME_API UInventoryFragment_InventoryIcon : public ULyraInventoryItemFragment
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance)
	bool bShowItem = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance, meta = (EditCondition = "bShowItem"))
	FSlateBrush Icon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance, meta = (EditCondition = "bShowItem"))
	ERarity RarityBorder = ERarity::Common;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance, meta = (EditCondition = "bShowItem"))
	FSlateBrush Embellishment;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance, meta = (EditCondition = "bShowItem"))
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance, meta = (EditCondition = "bShowItem"))
	FText DisplayDescription;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance, meta = (EditCondition = "bShowItem"))
	bool bCanStack = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance, meta = (EditCondition = "bShowItem && bCanStack"))
	int32 MaxStack = 200;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance, meta = (EditCondition = "bShowItem"))
	bool bCanBeDeleted = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance, meta = (EditCondition = "bShowItem"))
	bool bCanBeDropped = false;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Droppable, meta = (EditCondition = "bShowItem && bCanBeDropped"))
	//TSubclassOf<class ALyraWorldCollectable> OverridePickupClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Droppable, meta = (EditCondition = "bShowItem && bCanBeDropped"))
	class UStaticMesh* DropMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Droppable, meta = (EditCondition = "bShowItem && bCanBeDropped"))
	FTransform TransformOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Droppable, meta = (EditCondition = "bShowItem && bCanBeDropped"))
	bool bShowParticles = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Droppable, meta = (EditCondition = "bShowItem && bCanBeDropped"))
	bool bEnableDecay = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Droppable, meta = (EditCondition = "bShowItem && bCanBeDropped &&bEnableDecay"))
	float DecayLifespan = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Droppable, meta = (EditCondition = "bShowItem && bCanBeDropped"))
	FInventoryPickup PayloadInventory;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Storage)
	bool bCanBeStored;
};
