// Copyright (c) 2025 EugeneSlerp. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "TeamComponent.h"
#include "Engine/DeveloperSettings.h"
#include "TeamDevelopersSettings.generated.h"

enum class ERelation : uint8;
/**
 * 
 */
UCLASS(DisplayName = "Team Settings", Config = Game, DefaultConfig)
class FLEXIBLETEAMMANAGER_API UTeamDevelopersSettings : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	/*
	 * Factions data table
	 */
	UPROPERTY(EditAnywhere, Config, Category = "Team")
	TSoftObjectPtr<UDataTable> FactionDataTable;

	/*
	 * When delete relation in faction relations, for default relations use priority relation between two relations
	 */
	UPROPERTY(EditAnywhere, Config, Category = "Team")
	TSet<ERelation> DefaultRelationPriority = {ERelation::Hostile, ERelation::Neutral, ERelation::Friendly};
protected:
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
};