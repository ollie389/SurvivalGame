// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/LyraCharacter.h"
#include "LyraPlayerCharacter.generated.h"

/**
 * 
 */
UCLASS(Config = Game, Meta = (ShortTooltip = "The base player character pawn class used by this project."))
class LYRAGAME_API ALyraPlayerCharacter : public ALyraCharacter
{
	GENERATED_BODY()
public:
	ALyraPlayerCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//UFUNCTION(BlueprintCallable, Category = "Lyra|Character|Custom")
	//void SpawnFromPoint(float radius, TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes, UClass* ClassToSpawn);

	//UFUNCTION(BlueprintCallable, Category = "Lyra|Character|Custom")
	//bool DestroyFoliage(TSubclassOf<AActor>TreeActorToSpawn, FHitResult HitResult, bool SpawnCutHalf, UNiagaraSystem* SystemTemplate, UStaticMesh* TreeCollisionMesh, UMaterialInterface* CapMaterial, float SliceHeight = 10.0, float Lifespan = 300.0);

	//UFUNCTION(Server, Reliable)
	//void ServerDestroyFoliage(TSubclassOf<AActor>TreeActorToSpawn, FHitResult HitResult, bool SpawnCutHalf, UNiagaraSystem* SystemTemplate, UStaticMesh* TreeCollisionMesh, UMaterialInterface* CapMaterial, float SliceHeight, float Lifespan);

	//UFUNCTION(Server, Reliable)
	//void ServerDestroyTree(TSubclassOf<AActor>TreeActorToSpawn, UWorld* World, FVoxelPointHandle PointHandle, FHitResult HitResult, bool SpawnCutHalf, UNiagaraSystem* SystemTemplate, UStaticMesh* TreeCollisionMesh, UMaterialInterface* CapMaterial, float SliceHeight = 55.0, float Lifespan = 300.0);

	//UFUNCTION()
	//void DestroyGrass(UWorld* World, FVoxelPointHandle PointHandle, FHitResult HitResult, bool SpawnCutHalf, UNiagaraSystem* SystemTemplate, float SliceHeight = 10.0, float Lifespan = 300.0);

	UFUNCTION()
	void DestroyProceduralMeshComponent(UProceduralMeshComponent* ProceduralMeshComponent, AProceduralMeshTreeActor* CutActor);

	//UFUNCTION(NetMulticast, Reliable)
	//void MultiCastHideFoliage(UWorld* World, FVoxelPointHandle PointHandle, UNiagaraSystem* SystemTemplate);

	//UFUNCTION(NetMulticast, Reliable)
	//void MultiCastSetPointAttribute(const FVoxelPointHandle& Handle,
		//const FName Name,
		//const FVoxelPinType& Type,
		//const FVoxelPinValue& Value);

	UFUNCTION()
	void DisplayCutDownTree(AProceduralMeshTreeActor* CutActor, float MeshCollisionHeightRatio, FVector ForwardVector);

	/*UPROPERTY(VisibleAnywhere)
	TSet<FVoxelPointHandle> InteractingPoints;*/

	UPROPERTY(VisibleAnywhere)
	UProceduralMeshComponent* CutOtherHalfTree;

	UFUNCTION()
	void HideStaticMeshComponent(UStaticMeshComponent* StaticMeshComponent, AProceduralMeshTreeActor* CutActor, FVector ForwardVector);

	UFUNCTION(NetMulticast, Unreliable)
	void MultiCastPlaySound(UWorld* World, USoundBase* SoundEffect, FVector Location);

	UFUNCTION(BlueprintImplementableEvent)
	void GetWeaponActor(AActor*& WeaponActor);

private:
	/*TQueue<FVoxelPointHandle> SavedSpawnPoints;
	TQueue<FDateTime> SavedSpawnTimes;
	TQueue<AActor*> SavedSpawnedActors;

	TQueue<FVoxelPointHandle> SavedCutGrassPoints;
	TQueue<FDateTime> SavedCutGrassTimes;*/
};
