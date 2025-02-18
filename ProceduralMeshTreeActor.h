// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"
#include "../../Plugins/SmoothSync/Source/SmoothSyncPlugin/Public/SmoothSyncPlugin.h"
#include "../../Plugins/SmoothSync/Source/SmoothSyncPlugin/Public/SmoothSync.h"
#include "Components/GameFrameworkComponent.h"
#include "Components/StaticMeshComponent.h"
#include "LyraActorWithAbilitiesAndHealth.h"
#include "NiagaraFunctionLibrary.h"
#include "ProceduralMeshTreeActor.generated.h"


UCLASS(Blueprintable)
class LYRAGAME_API AProceduralMeshTreeActor : public ALyraActorWithAbilitiesAndHealth//AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AProceduralMeshTreeActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UProceduralMeshComponent* CutLeavesProceduralMeshComponent;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//LyraActorWithAbilities implementation
	virtual void InitializeAbilitySystem() override;
	//~End of LyraActorWithAbilities implementation

	// Static Mesh component
	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadWrite)
	UStaticMeshComponent* StaticMeshComponent;

	// Procedural Mesh component
	UPROPERTY(VisibleAnywhere)
	UProceduralMeshComponent* ProceduralMeshComponent;

	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadWrite)
	UStaticMeshComponent* CylinderMeshComponent;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* SceneComponent;

	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadWrite)
	UBoxComponent* CustomCollisionBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool Destroying;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UProceduralMeshComponent* CutOtherHalfTree;

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void MultiCastDestroyProceduralMeshComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FHitResult LastHitResult;

	UFUNCTION()
	virtual void OnHealthChange(ULyraHealthComponent* HealthComponentHandle, float Health, float MaxHealth, AActor* OwningActor);

	UFUNCTION(BlueprintImplementableEvent)
	void OnTakeDamage();

	UFUNCTION()
	void OnCustomCollisionOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnCustomComponentHit(
		UPrimitiveComponent* HitComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit);

	UFUNCTION(NetMulticast, Reliable)
	void MultiCastSetupTree(FTransform PointTransform, float SliceHeight, UMaterialInterface* CapMaterialParam, UStaticMesh* TreeCollisionMesh);

	UFUNCTION(NetMulticast, Reliable)
	void MultiCastSpawnTree(FTransform PointTransform, float SliceHeight);

	UFUNCTION(NetMulticast, Reliable)
	void MultiCastShowProceduralMesh();

	UFUNCTION()
	void DestroyProceduralMeshComponent();

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	UPROPERTY()
	USmoothSync* SmoothSync;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> DamageEffect;

	UFUNCTION()
	float GetHealth();

	UFUNCTION(BlueprintImplementableEvent)
	void TreeDestruction();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* Decal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector DecalSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator DecalRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* TreeHitSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* TreeCutSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* TreeFallSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraSystem* TreeHitEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FallingForce;

	UFUNCTION()
	void MyDestroyComponent();

	/*UPROPERTY(EditAnywhere)
	FVoxelPointHandle PointHandle;*/

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite)
	UMaterialInterface* CapMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector ForwardVector;

	UFUNCTION()
	void DisplayCutDownTree(float MeshCollisionHeightRatio, float SliceHeight);

	UFUNCTION()
	void ChangeVisibleMesh();

	UFUNCTION(NetMulticast,Reliable)
	void MultiCastHideStaticMeshComponent();

	UPROPERTY()
	bool HadHitEvent;

	UFUNCTION()
	bool GetStaticMeshVertexInfo(UStaticMeshComponent* SM, TArray<FVector>& VertexPositions, TArray<FColor>& VertexColors);

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float TreeSliceHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int IsSetUp;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UStaticMesh* TreeCollisionMeshParam;

};
