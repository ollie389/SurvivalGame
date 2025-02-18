// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "../Plugins/Experimental/ModularGameplay/Source/ModularGameplay/Public/Components/GameFrameworkComponent.h" @Temp-Change

#include "LyraStatComponent.generated.h"

class ULyraStatComponent;

class ULyraAbilitySystemComponent;
class ULyraStatSet;
class UObject;
struct FFrame;
struct FGameplayEffectSpec;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLyraStat_LevelUpEvent, AActor*, OwningActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FLyraStat_AttributeChanged, ULyraStatComponent*, StatComponent, float, OldValue, float, NewValue, AActor*, Instigator);


/**
 * ULyraStatComponent
 *
 *	An actor component used to handle anything related to Stats and Exp
 */
UCLASS(Blueprintable, Meta = (BlueprintSpawnableComponent))
class LYRAGAME_API ULyraStatComponent : public UGameFrameworkComponent
{
	GENERATED_BODY()

public:

	ULyraStatComponent(const FObjectInitializer& ObjectInitializer);

	// Returns the Stat component if one exists on the specified actor.
	UFUNCTION(BlueprintPure, Category = "Lyra|Stat")
	static ULyraStatComponent* FindStatComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<ULyraStatComponent>() : nullptr); }

	// Initialize the component using an ability system component.
	UFUNCTION(BlueprintCallable, Category = "Lyra|Stat")
	void InitializeWithAbilitySystem(ULyraAbilitySystemComponent* InASC);

	// Uninitialize the component, clearing any references to the ability system.
	UFUNCTION(BlueprintCallable, Category = "Lyra|Stat")
	void UninitializeFromAbilitySystem();

	// Returns the current Exp value.
	UFUNCTION(BlueprintCallable, Category = "Lyra|Stat")
	float GetExp() const;

	// Returns the current maximum exp value.
	UFUNCTION(BlueprintCallable, Category = "Lyra|Stat")
	float GetMaxExp() const;

	// Returns the current Level value.
	UFUNCTION(BlueprintCallable, Category = "Lyra|Stat")
	float GetLevel() const;

	// Returns the current Exp in the range [0.0, 1.0].
	UFUNCTION(BlueprintCallable, Category = "Lyra|Stat")
	float GetExpNormalized() const;

public:

	// Delegate fired when the Stat value has changed. This is called on the client but the instigator may not be valid
	UPROPERTY(BlueprintAssignable)
	FLyraStat_AttributeChanged OnExpChanged;

	//// Delegate fired when the death sequence has started.
	//UPROPERTY(BlueprintAssignable)
	//FLyraStat_LevelUpEvent OnReadyToLevelUp;


protected:

	virtual void OnUnregister() override;

	void ClearGameplayTags();

	virtual void HandleExpChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);

protected:

	// Ability system used by this component.
	UPROPERTY()
	TObjectPtr<ULyraAbilitySystemComponent> AbilitySystemComponent;

	// Health set used by this component.
	UPROPERTY()
	TObjectPtr<const ULyraStatSet> StatSet;
};
