// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomCPPClasses/Character/LyraPlayerCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ProceduralMeshActor.h"
#include "NiagaraFunctionLibrary.h"
#include "ProceduralMeshTreeActor.h"
#include "KismetProceduralMeshLibrary.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Engine/StaticMesh.h"

ALyraPlayerCharacter::ALyraPlayerCharacter(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer) {

}



void ALyraPlayerCharacter::DisplayCutDownTree(AProceduralMeshTreeActor* CutActor, float MeshCollisionHeightRatio, FVector ForwardVector)
{
	///*** MOVED TO PROCEDURALMESHTREEACTOR***///

}



void ALyraPlayerCharacter::MultiCastPlaySound_Implementation(UWorld* World, USoundBase* SoundEffect, FVector Location)
{
	UGameplayStatics::PlaySoundAtLocation(World, SoundEffect, Location);
}



void ALyraPlayerCharacter::DestroyProceduralMeshComponent(UProceduralMeshComponent* ProceduralMeshComponent, AProceduralMeshTreeActor* CutActor)
{
	if (CutActor)
	{
		if (ProceduralMeshComponent && !(CutActor->Destroying))
		{
			ProceduralMeshComponent->DestroyComponent();
			CutActor->MultiCastDestroyProceduralMeshComponent();
		}
	}
}

void ALyraPlayerCharacter::HideStaticMeshComponent(UStaticMeshComponent* StaticMeshComponent, AProceduralMeshTreeActor* CutActor, FVector ForwardVector)
{
	StaticMeshComponent->SetVisibility(false);
}
