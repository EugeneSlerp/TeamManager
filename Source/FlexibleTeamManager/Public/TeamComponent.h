// Copyright (c) 2025 EugeneSlerp. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "TeamComponent.generated.h"

UENUM(BlueprintType)
enum class ERelation : uint8
{
	None,
	Friendly,
	Neutral,
	Hostile
};

static FString RelationToString(ERelation Relation)
{
	switch (Relation)
	{
		case ERelation::None:
			return "None";
			break;
		case ERelation::Friendly:
			return "Friendly";
			break;
		case ERelation::Neutral:
			return "Neutral";
			break;
		case ERelation::Hostile:
			return "Hostile";
			break;
	}
	return "None";
}

USTRUCT(BlueprintType)
struct FLocalRelations
{
	
	GENERATED_BODY()
public:
	void AddRelation(FGuid Guid, ERelation Relation)
	{
		Relations.Add(Guid, Relation);
	}
	ERelation* FindRelation(FGuid Guid)
	{
		return Relations.Find(Guid);
	}
	void RemoveRelation(FGuid Guid)
	{
		Relations.Remove(Guid);
	}
	
	UPROPERTY(SaveGame)
	TMap<FGuid, ERelation> Relations;

};


USTRUCT(BlueprintType)
struct FFaction : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Faction", meta = (Categories="Team"))
	FGameplayTag Faction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Faction", meta = (
		Categories="Team",
		ForceInlineRow = "true",
		MapKeyDisplayName = "Faction Tag",
		MapValueDisplayName = "Relation"))
	TMap<FGameplayTag, ERelation> Relations;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Faction")
	ERelation DefaultRelation = ERelation::Neutral;

	ERelation GetRelationForOther(const FFaction& Other) const
	{
		return *Relations.Find(Other.Faction);
	}

	void AddRelation(const FGameplayTag& NewTag, ERelation NewRelation)
	{
		Relations.Add(NewTag, NewRelation);
	}

	void RemoveRelation(const FGameplayTag& Tag)
	{
		Relations.Remove(Tag);
	}

	void Copy(const FFaction& Other)
	{
		Faction = Other.Faction;
		Relations = Other.Relations;
		DefaultRelation = Other.DefaultRelation;
	}
	
};

class UTeamManager;

/*
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnChangeRelation, const FGameplayTag, OtherFactionTag, ERelation, NewRelation);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangeFaction, FGameplayTag, NewFaction);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnChangeLocalRelation, UTeamComponent*, TeamComponent, ERelation, Relation);

UCLASS(ClassGroup=(Team), meta=(BlueprintSpawnableComponent))
class FLEXIBLETEAMMANAGER_API UTeamComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintAssignable, Category="Team")
	FOnChangeRelation OnChangeRelation;

	UPROPERTY(BlueprintAssignable, Category="Team")
	FOnChangeFaction OnChangeFaction;

	UPROPERTY(BlueprintAssignable, Category="Team")
	FOnChangeLocalRelation OnChangeLocalRelation;
	

	UFUNCTION(BlueprintCallable, Category="Team")
	void ChangeFaction(UPARAM(meta = (Categories="Team")) FGameplayTag NewFactionTag, bool bClearLocalRelations = true);

	UFUNCTION(BlueprintCallable, Category="Team")
	void ClearLocalRelations();

	UFUNCTION(BlueprintCallable, Category="Team")
	void ClearLocalRelation(AActor* TargetActor);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Team")
	FFaction GetFaction() const {return CachedFaction;}

	FFaction& GetMutableFaction() {return CachedFaction;}

	const FGuid& GetGuid() const {return Guid;}
	FLocalRelations& GetLocalRelations() {return LocalRelations;}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Team")
	FGameplayTag GetFactionTag() const {return FactionTag;}

	UFUNCTION(BlueprintCallable, Category="Team")
	ERelation GetRelationForOther(UTeamComponent* Other) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Team")
	bool IsHostile(UTeamComponent* Other) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Team")
	bool IsNeutral(UTeamComponent* Other) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Team")
	bool IsFriendly(UTeamComponent* Other) const;

	UTeamManager* GetTeamManager() const;
protected:
	UPROPERTY(EditDefaultsOnly, Category="Team", meta = (Categories="Team"), SaveGame)
	FGameplayTag FactionTag;
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY()
	FFaction CachedFaction;
	UPROPERTY(SaveGame)
	FLocalRelations LocalRelations;
	UPROPERTY(SaveGame)
	FGuid Guid;

private:
	void OnChangeRelationBetweenFractions(const FGameplayTag Source, const FGameplayTag Target, ERelation Relation);
	void OnChangeRelationBetweenComps(UTeamComponent* Source, UTeamComponent* Target, ERelation Relation);
	FDelegateHandle OnChangeRelationHandle;
	FDelegateHandle OnChangeLocalRelationHandle;

	

};
