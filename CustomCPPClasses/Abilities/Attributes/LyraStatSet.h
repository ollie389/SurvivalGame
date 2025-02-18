// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/LyraAttributeSet.h"
#include "NativeGameplayTags.h"

#include "LyraStatSet.generated.h"

class UObject;
struct FFrame;

UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_ExpGain);

struct FGameplayEffectModCallbackData;

/**
 * ULyraStatSet
 *
 *	Class that defines attributes that are necessary for the stat system
 */
UCLASS(BlueprintType)
class ULyraStatSet : public ULyraAttributeSet
{
	GENERATED_BODY()

public:

	ULyraStatSet();

	ATTRIBUTE_ACCESSORS(ULyraStatSet, Exp);
	ATTRIBUTE_ACCESSORS(ULyraStatSet, MaxExp);
	ATTRIBUTE_ACCESSORS(ULyraStatSet, Level);
	ATTRIBUTE_ACCESSORS(ULyraStatSet, AP);
	ATTRIBUTE_ACCESSORS(ULyraStatSet, AD);
	//ATTRIBUTE_ACCESSORS(ULyraStatSet, ExpGain);

	// Delegate when exp changes due to exp gain, some information may be missing on the client
	mutable FLyraAttributeEvent OnExpChanged;

	// Delegate when exp to next level changes
	mutable FLyraAttributeEvent OnMaxExpChanged;

	// Delegate to broadcast when the level changes
	mutable FLyraAttributeEvent OnLevelUpReady;

	UFUNCTION(BlueprintCallable)
	void ManualBroadcastExp();

protected:

	UFUNCTION()
	void OnRep_Exp(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxExp(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Level(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_AP(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_AD(const FGameplayAttributeData& OldValue);

	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;

private:

	// The current exp attribute.  The exp will be capped by the max exp attribute.  Exp is hidden from modifiers so only executions can modify it.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Exp, Category = "Lyra|Exp", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Exp;

	// The current max exp attribute.  Max exp is an attribute since gameplay effects can modify it.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxExp, Category = "Lyra|Exp", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxExp;

	// The current level attribute.  Level is an attribute since gameplay effects can modify it.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Level, Category = "Lyra|Exp", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Level;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AP, Category = "Lyra|Exp", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AP;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AD, Category = "Lyra|Exp", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AD;

	// Used to track when the exp reaches max.
	bool bAtMaxExp;

	// Store the exp before any changes 
	float MaxExpBeforeAttributeChange;
	float ExpBeforeAttributeChange;

	// -------------------------------------------------------------------
	//	Meta Attribute (please keep attributes that aren't 'stateful' below 
	// -------------------------------------------------------------------

	//// Incoming exp. This is mapped directly to +ExpGain
	//UPROPERTY(BlueprintReadOnly, Category = "Lyra|Exp", Meta = (AllowPrivateAccess = true))
	//FGameplayAttributeData ExpGain;
};
