// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/GameStateComponent.h"

#include "LyraTeamCreationComponent.generated.h"

class ULyraExperienceDefinition;
class ALyraTeamPublicInfo;
class ALyraTeamPrivateInfo;
class ALyraPlayerState;
class AGameModeBase;
class APlayerController;
class ULyraTeamDisplayAsset;

UCLASS(Blueprintable)
class ULyraTeamCreationComponent : public UGameStateComponent
{
	GENERATED_BODY()

public:
	ULyraTeamCreationComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UObject interface
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
	//~End of UObject interface

	//~UActorComponent interface
	virtual void BeginPlay() override;
	//~End of UActorComponent interface

private:
	void OnExperienceLoaded(const ULyraExperienceDefinition* Experience);

protected:
	// List of teams to create (id to display asset mapping, the display asset can be left unset if desired)
	UPROPERTY(EditDefaultsOnly, Category = Teams)
	TMap<uint8, TObjectPtr<ULyraTeamDisplayAsset>> TeamsToCreate;

	/** @Game-Change start handle teams for non player characters, that won't spawn at PlayerStart and won't influence how many players there are per teams **/
	// Are there any teams that will be used for non-player characters? Example NPC that attack everyone and have their own team
	UPROPERTY(EditDefaultsOnly, Category = Teams, meta = (InlineEditConditionToggle))
	bool UseNPCTeams;

	// Which IDs won't be player characters. ServerAssignPlayersToTeams will ignore these indices when choosing a team for each player randomly.
	UPROPERTY(EditDefaultsOnly, Category = Teams, meta = (EditCondition = UseNPCTeams))
	TArray<int8> NPCTeamIDs;
	/** @Game-Change end handle teams for non player characters, that won't spawn at PlayerStart and won't influence how many players there are per teams **/

	UPROPERTY(EditDefaultsOnly, Category = Teams)
	TSubclassOf<ALyraTeamPublicInfo> PublicTeamInfoClass;

	UPROPERTY(EditDefaultsOnly, Category = Teams)
	TSubclassOf<ALyraTeamPrivateInfo> PrivateTeamInfoClass;

#if WITH_SERVER_CODE
protected:
	virtual void ServerCreateTeams();
	virtual void ServerAssignPlayersToTeams();

	/** Sets the team ID for the given player state. Spectator-only player states will be stripped of any team association. */
	virtual void ServerChooseTeamForPlayer(ALyraPlayerState* PS);

private:
	void OnPlayerInitialized(AGameModeBase* GameMode, AController* NewPlayer);
	void ServerCreateTeam(int32 TeamId, ULyraTeamDisplayAsset* DisplayAsset);

	/** returns the Team ID with the fewest active players, or INDEX_NONE if there are no valid teams */
	int32 GetLeastPopulatedTeamID() const;
#endif
};
