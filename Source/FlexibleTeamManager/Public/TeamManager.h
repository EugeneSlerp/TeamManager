// Copyright (c) 2025 EugeneSlerp. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameplayTagContainer.h"
#include "TeamComponent.h"
#include "TeamManager.generated.h"

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnChangeRelationBetweenFactions,const FGameplayTag, const FGameplayTag, ERelation);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnChangeRelationBetweenComps,UTeamComponent*, UTeamComponent*, ERelation);

/**
 * 
 */
UCLASS(DisplayName = "Team Manager")
class FLEXIBLETEAMMANAGER_API UTeamManager : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	FOnChangeRelationBetweenFactions OnChangeRelationBetweenFactions;
	FOnChangeRelationBetweenComps OnChangeLocalRelationBetweenComps;
	
	virtual void RegisterInstanceTeamComponent(UTeamComponent* TeamComponent);
	virtual void UnregisterInstanceTeamComponent(UTeamComponent* TeamComponent);

	void AddLocalRelation(UTeamComponent* Source, UTeamComponent* Target, ERelation Relation);
	void ClearLocalRelationForComp(UTeamComponent* Source, UTeamComponent* Target);
	void ClearLocalRelationsForComp(UTeamComponent* TeamComponent);

	void AddRelationInFaction(FGameplayTag SourceFactionTag, FGameplayTag TargetFactionTag, ERelation Relation);
	void ClearRelationInFaction(FGameplayTag SourceFactionTag, FGameplayTag TargetFactionTag);
	
	bool HasFaction(FGameplayTag FactionTag);
	FFaction FindFaction(FGameplayTag FactionTag);

	void UpdateCachedFactionsFromComps();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Team)
	TArray<FFaction> GetCachedFactions() const;
	TArray<UTeamComponent*> GetAllComponentsOfFaction(FGameplayTag FactionTag);
	UTeamComponent* GetTeamCompByGuid(const FGuid& Guid);
	static ERelation GetPriorityDefaultRelationBetween(ERelation FirstRelation, ERelation SecondRelation);

	void NotifyChangeRelationBetweenComps(UTeamComponent* Source, UTeamComponent* Target, ERelation Relation);
	void NotifyChangeRelationBetweenFactions(const FGameplayTag Source, const FGameplayTag Target, ERelation Relation);

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	bool IsRegisterTeamComp(const UTeamComponent* TeamComponent) const;

private:
	UPROPERTY()
	TMap<FGuid,TWeakObjectPtr<UTeamComponent>> RegisteredTeamComponents;

	UPROPERTY()
	TMap<FGameplayTag, FFaction> CachedFactions;
	
	UPROPERTY()
	UDataTable* FactionDataTable;

};
