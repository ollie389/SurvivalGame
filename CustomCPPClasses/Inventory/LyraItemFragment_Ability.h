// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/LyraInventoryItemDefinition.h"
#include "GameplayTagContainer.h"
#include "LyraItemFragment_Ability.generated.h"

/**
 * 
 */

class UInputAction;

UCLASS()
class LYRAGAME_API ULyraItemFragment_Ability : public ULyraInventoryItemFragment
{
	GENERATED_BODY()

public:
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemAbility)
	//class ULyraGameplayAbility* ItemAbility; // Ability that activates when clicking the item

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemAbility)
	FGameplayTag ItemAbilityGameplayTag; // Gameplay tag to pass to ability system to activate the ability

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemAbility)
	FGameplayTag AbilityDurationMessageGameplayTag; // Item ability cooldown

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GrantAbility)
	TSubclassOf<ULyraGameplayAbility> RelatedGameplayAbility; // Ability to grant to ability system (doesnt have to activate when clicking the ability)
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = GrantAbility)
	UInputAction* InputAction;

	virtual void OnInstanceCreated(ULyraInventoryItemInstance* Instance) const override;

};
