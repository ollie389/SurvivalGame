// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/LyraAbilitySet.h"
#include "LyraActorWithAbilities.generated.h"

class ULyraAbilitySystemComponent;

/**
 * Lyra Actor with Abilities
 *
 * An actor with an Ability System.
 *
 * You can optionally define an ability set that will be granted to this actor
 * when it spawns (via PostInitializeComponents; so they will be ready by the
 * time BeginPlay is executed).
 */
UCLASS(DisplayName = "Lyra Actor with Abilities")
class LYRAGAME_API ALyraActorWithAbilities : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()

	/** AbilitySet this actor should be granted on spawn, if any */
	UPROPERTY(EditDefaultsOnly, Category = "Lyra|Ability")
	TObjectPtr<ULyraAbilitySet> AbilitySetOnSpawn;

public:
	// Set Class Defaults
	ALyraActorWithAbilities(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	//~AActor implementation
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of AActor implementation

	/**
	 * This gets called when it's time to initialize the actor's ASC
	 *
	 * Override this as needed.  In your overrides, you MUST call Super()
	 * **BEFORE** you try to use the ASC at all.  The ASC is not functional
	 * before you call Super().
	 */
	virtual void InitializeAbilitySystem();

	/**
	 * This gets called when it's time to uninitialize the actor's ASC
	 */
	virtual void UninitializeAbilitySystem();

public:
	//~IAbilitySystemComponent interface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	ULyraAbilitySystemComponent* GetLyraAbilitySystemComponentChecked() const;
	//~End IAbilitySystemComponent interface

public:
	// Public Static Const Component Names
	static const FName AbilitySystemComponentName;

private:
	// The ability system component for this actor
	UPROPERTY(EditDefaultsOnly, Category = "Lyra|Ability")
	TObjectPtr<ULyraAbilitySystemComponent> AbilitySystemComponent;

	// List of handles granted to this actor on spawn; these get cleaned up in EndPlay
	UPROPERTY(VisibleInstanceOnly, Category = "Lyra|Ability")
	FLyraAbilitySet_GrantedHandles GrantedHandlesOnSpawn;

};