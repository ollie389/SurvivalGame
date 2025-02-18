// Copyright Epic Games, Inc. All Rights Reserved.

#include "LyraStatSet.h"
#include "AbilitySystem/Attributes/LyraAttributeSet.h"
#include "LyraGameplayTags.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Engine/World.h"
#include "GameplayEffectExtension.h"
#include "Messages/LyraVerbMessage.h"
#include "GameFramework/GameplayMessageSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(LyraStatSet)

UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_ExpGain, "Gameplay.ExpGain");

ULyraStatSet::ULyraStatSet()
	: Exp(0.0f)
	, MaxExp(100.0f)
	, Level(1.0f)
	, AP(0.0f)
	, AD(0.0f)
{
	bAtMaxExp = false;
	MaxExpBeforeAttributeChange = 0.0f;
	ExpBeforeAttributeChange = 0.0f;
}

void ULyraStatSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ULyraStatSet, Exp, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULyraStatSet, MaxExp, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULyraStatSet, Level, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULyraStatSet, AP, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ULyraStatSet, AD, COND_OwnerOnly, REPNOTIFY_Always);
}

void ULyraStatSet::OnRep_Exp(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULyraStatSet, Exp, OldValue);

	// Call the change callback, but without an instigator
	// This could be changed to an explicit RPC in the future
	// These events on the client should not be changing attributes

	const float CurrentExp = GetExp();
	const float EstimatedMagnitude = CurrentExp - OldValue.GetCurrentValue();
	const float CurrentMaxExp = GetMaxExp();

	OnExpChanged.Broadcast(nullptr, nullptr, nullptr, EstimatedMagnitude, OldValue.GetCurrentValue(), CurrentExp);

	if (!bAtMaxExp && CurrentExp >= CurrentMaxExp)
	{
		OnLevelUpReady.Broadcast(nullptr, nullptr, nullptr, EstimatedMagnitude, OldValue.GetCurrentValue(), CurrentExp);
	}

	bAtMaxExp = (CurrentExp >= CurrentMaxExp);
}

void ULyraStatSet::OnRep_MaxExp(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULyraStatSet, MaxExp, OldValue);

	// Call the change callback, but without an instigator
	// This could be changed to an explicit RPC in the future
	OnMaxExpChanged.Broadcast(nullptr, nullptr, nullptr, GetMaxExp() - OldValue.GetCurrentValue(), OldValue.GetCurrentValue(), GetMaxExp());
}

void ULyraStatSet::OnRep_Level(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULyraStatSet, Level, OldValue);

	// Call the change callback, but without an instigator
	// This could be changed to an explicit RPC in the future
}

void ULyraStatSet::OnRep_AP(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULyraStatSet, AP, OldValue);
}

void ULyraStatSet::OnRep_AD(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ULyraStatSet, AD, OldValue);
}

//bool ULyraStatSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
//{
//	if (!Super::PreGameplayEffectExecute(Data))
//	{
//		return false;
//	}
//
//	// Save the current exp
//	ExpBeforeAttributeChange = GetExp();
//	MaxExpBeforeAttributeChange = GetMaxExp();
//
//	return true;
//}

//void ULyraStatSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
//{
//	Super::PostGameplayEffectExecute(Data);
//
//	float MinimumExp = 0.0f;
//
//	const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
//	AActor* Instigator = EffectContext.GetOriginalInstigator();
//	AActor* Causer = EffectContext.GetEffectCauser();
//
//	if (Data.EvaluatedData.Attribute == GetExpGainAttribute())
//	{
//		//// Send a standardized verb message that other systems can observe
//		//if (Data.EvaluatedData.Magnitude > 0.0f)
//		//{
//		//	FLyraVerbMessage Message;
//		//	Message.Verb = TAG_Lyra_ExpGain_Message;
//		//	Message.Instigator = Data.EffectSpec.GetEffectContext().GetEffectCauser();
//		//	Message.InstigatorTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();
//		//	Message.Target = GetOwningActor();
//		//	Message.TargetTags = *Data.EffectSpec.CapturedTargetTags.GetAggregatedTags();
//		//	//@TODO: Fill out context tags, and any non-ability-system source/instigator tags
//		//	//@TODO: Determine if it's an opposing team kill, self-own, team kill, etc...
//		//	Message.Magnitude = Data.EvaluatedData.Magnitude;
//
//		//	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
//		//	MessageSystem.BroadcastMessage(Message.Verb, Message);
//		//}
//
//		// Convert into +Exp and then clamp
//		SetExp(FMath::Clamp(GetExp() + GetExpGain(), 0.0f, GetMaxExp()));
//		float xpgain = GetExpGain();
//		SetExpGain(0.0f);
//	}
//	else if (Data.EvaluatedData.Attribute == GetMaxExpAttribute())
//	{
//		// Notify on any requested max exp changes
//		OnMaxExpChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, MaxExpBeforeAttributeChange, GetMaxExp());
//	}
//
//	// If exp has actually changed activate callbacks
//	if (GetExp() != ExpBeforeAttributeChange)
//	{
//		OnExpChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, ExpBeforeAttributeChange, GetExp());
//	}
//
//	if ((GetExp() >= GetMaxExp()) && !bAtMaxExp)
//	{
//		OnLevelUpReady.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, ExpBeforeAttributeChange, GetExp());
//	}
//
//	// Check exp again in case an event above changed it.
//	bAtMaxExp = (GetExp() >= GetMaxExp());
//}

bool ULyraStatSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}


	return true;
}

void ULyraStatSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	float MinimumExp = 0.0f;

	const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
	AActor* Instigator = EffectContext.GetOriginalInstigator();
	AActor* Causer = EffectContext.GetEffectCauser();

	// If exp has actually changed activate callbacks
	if (GetExp() != ExpBeforeAttributeChange)
	{
		OnExpChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, ExpBeforeAttributeChange, GetExp());
		ExpBeforeAttributeChange = GetExp();
	}

	if ((GetExp() >= GetMaxExp()) && !bAtMaxExp)
	{
		OnLevelUpReady.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, ExpBeforeAttributeChange, GetExp());
		MaxExpBeforeAttributeChange = GetMaxExp();
	}

	// Check exp again in case an event above changed it.
	bAtMaxExp = (GetExp() >= GetMaxExp());
}

void ULyraStatSet::ManualBroadcastExp()
{
	float CurrentExp = GetExp();
	OnExpChanged.Broadcast(nullptr, nullptr, nullptr, CurrentExp-0, 0, CurrentExp);
}


void ULyraStatSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}

void ULyraStatSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}

void ULyraStatSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	ClampAttribute(Attribute, NewValue);
}

void ULyraStatSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetExpAttribute())
	{
		// Do not allow exp to go negative or above max exp.
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxExp());
	}
}

