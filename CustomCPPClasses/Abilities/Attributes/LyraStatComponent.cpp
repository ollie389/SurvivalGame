// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomCPPClasses/Abilities/Attributes/LyraStatComponent.h"

#include "AbilitySystem/Attributes/LyraAttributeSet.h"
#include "LyraLogChannels.h"
#include "System/LyraAssetManager.h"
#include "System/LyraGameData.h"
#include "LyraGameplayTags.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "LyraStatSet.h"
#include "Messages/LyraVerbMessage.h"
#include "Messages/LyraVerbMessageHelpers.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameFramework/PlayerState.h"
#include "Engine/World.h"
#include "Teams/LyraTeamSubsystem.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(LyraStatComponent)

ULyraStatComponent::ULyraStatComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);

	AbilitySystemComponent = nullptr;
	StatSet = nullptr;
}

void ULyraStatComponent::OnUnregister()
{
	UninitializeFromAbilitySystem();

	Super::OnUnregister();
}

void ULyraStatComponent::InitializeWithAbilitySystem(ULyraAbilitySystemComponent* InASC)
{
	AActor* Owner = GetOwner();
	check(Owner);

	if (AbilitySystemComponent)
	{
		UE_LOG(LogLyra, Error, TEXT("LyraStatComponent: Stat component for owner [%s] has already been initialized with an ability system."), *GetNameSafe(Owner));
		return;
	}

	AbilitySystemComponent = InASC;
	if (!AbilitySystemComponent)
	{
		UE_LOG(LogLyra, Error, TEXT("LyraStatComponent: Cannot initialize Stat component for owner [%s] with NULL ability system."), *GetNameSafe(Owner));
		return;
	}

	StatSet = AbilitySystemComponent->GetSet<ULyraStatSet>();
	if (!StatSet)
	{
		UE_LOG(LogLyra, Error, TEXT("LyraStatComponent: Cannot initialize Stat component for owner [%s] with NULL Stat set on the ability system."), *GetNameSafe(Owner));
		return;
	}

	// Register to listen for attribute changes.
	StatSet->OnExpChanged.AddUObject(this, &ThisClass::HandleExpChanged);

	// TEMP: Reset attributes to default values.  Eventually this will be driven by a spread sheet.
	//AbilitySystemComponent->SetNumericAttributeBase(ULyraStatSet::GetExpAttribute(), 0);

	ClearGameplayTags();

	OnExpChanged.Broadcast(this, StatSet->GetExp(), StatSet->GetExp(), nullptr);
}

void ULyraStatComponent::UninitializeFromAbilitySystem()
{
	ClearGameplayTags();

	if (StatSet)
	{
		StatSet->OnExpChanged.RemoveAll(this);
	}

	StatSet = nullptr;
	AbilitySystemComponent = nullptr;
}

void ULyraStatComponent::ClearGameplayTags()
{
}

float ULyraStatComponent::GetExp() const
{
	return (StatSet ? StatSet->GetExp() : 0.0f);
}

float ULyraStatComponent::GetMaxExp() const
{
	return (StatSet ? StatSet->GetMaxExp() : 0.0f);
}

float ULyraStatComponent::GetLevel() const
{
	return (StatSet ? StatSet->GetLevel() : 0.0f);
}

float ULyraStatComponent::GetExpNormalized() const
{
	if (StatSet)
	{
		const float Exp = StatSet->GetExp();
		const float MaxExp = StatSet->GetMaxExp();

		return ((Exp > 0.0f) ? (Exp / MaxExp) : 0.0f);
	}

	return 0.0f;
}

void ULyraStatComponent::HandleExpChanged(AActor* Instigator, AActor* Causer, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue)
{
	OnExpChanged.Broadcast(this, OldValue, NewValue, Instigator);
}

