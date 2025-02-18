// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VoxelWorld.h"
#include "SetBaseLocations.generated.h"


UCLASS()
class LYRAGAME_API ASetBaseLocations : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASetBaseLocations();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FVector SetBaseDirection(int TeamID);

	UFUNCTION(BlueprintCallable)
	void SpawnBase(FVector BaseLocation, int TeamID);

	UFUNCTION(BlueprintCallable)
	void SetUpBases();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent)
	void SpawnBaseEvent(FVector Location, int TeamID);

public:
	UPROPERTY(BlueprintReadWrite)
	int NumTeams;

	UPROPERTY(BlueprintReadOnly)
	TArray<FVector>BaseLocations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName BorderChannel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName HeightChannel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WorldSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> VoxelBrushClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TEnumAsByte<EObjectTypeQuery>>VoxelTerrainObjectType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AVoxelWorld* VoxelWorld;
private:
	TArray<FTimerHandle> TimerArray;
};
