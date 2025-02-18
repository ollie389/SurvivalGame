// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/LyraGameplayAbility.h"
#include "LyraGamePlayAbility_Consumeable.generated.h"

/**
 * 
 */

class ULyraInventoryManagerComponent;
class ULyraInventoryItemDefinition;
class ULyraInventoryItemInstance;
class UInputAction;

USTRUCT(Blueprintable, BlueprintType)
struct LYRAGAME_API FAbilityItems
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ability)
	TSubclassOf<ULyraInventoryItemDefinition> ItemDefinition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ability)
	float ConsumptionCount;

	// Constructor to initialize properties
	FAbilityItems()
	{
		ConsumptionCount = 0.0f; // Initialize ConsumptionCount
	}

};


USTRUCT(Blueprintable, BlueprintType)
struct LYRAGAME_API FAbilityItemsArray
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ability)
	TArray<FAbilityItems> AbilityItemsArray;
};



UCLASS()
class LYRAGAME_API ULyraGamePlayAbility_Consumeable : public ULyraGameplayAbility
{
	GENERATED_BODY()

private:

	TArray<ULyraInventoryItemInstance*> InventoryItems;

	ULyraInventoryItemInstance* EquippedWeaponItemInstance;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ULyraInventoryManagerComponent* InventoryManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FAbilityItems> RequiredItems;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool CurrentAmmoAbility; //If true will show ammo in the ammo count

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInputAction* InputAction;
	
protected:

	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

private:
	
	bool DoWeHaveEnough(const TArray<ULyraInventoryItemInstance*> InventoryItemsParam, const TSubclassOf<ULyraInventoryItemDefinition>ItemDef, const int Needed);

	void AdjustInventoryStack(const TArray<ULyraInventoryItemInstance*> InventoryItemsParam, const TSubclassOf<ULyraInventoryItemDefinition>ItemDef, const int Stacks);

	void SetWeaponActor(); //Set the variable EquippedWeapon by calling LyraPlayerCharacter->GetWeapon;

	void ReceivedItemCallback(FGameplayTag InChannel, const FAbilityItemsArray& InMessage);

public:
	void ALyraGamePlayAbility_Consumeable();

	UFUNCTION(BlueprintCallable)
	bool HasItemsForAbility();

	UFUNCTION(BlueprintCallable)
	void ConsumeItems();
};
