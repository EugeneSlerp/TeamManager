// Copyright (c) 2025 EugeneSlerp. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "TeamBlueprintLibrary.generated.h"

class UTeamComponent;
enum class ERelation : uint8;
/**
 * 
 */
UCLASS()
class FLEXIBLETEAMMANAGER_API UTeamBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable,BlueprintPure, Category = "Team", meta = (DefaultToSelf="Source"))
	static ERelation GetRelationTo(AActor* Source, AActor* Target);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Team", meta = (DefaultToSelf="Source"))
	static bool IsHostile(AActor* Source, AActor* Target);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Team", meta = (DefaultToSelf="Source"))
	static bool IsNeutral(AActor* Source, AActor* Target);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Team", meta = (DefaultToSelf="Source"))
	static bool IsFriendly(AActor* Source, AActor* Target);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Team", meta = (DefaultToSelf="Source"))
	static FGameplayTag GetFactionTag(AActor* Target);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Team", meta = (DefaultToSelf = "Target"))
	static UTeamComponent* GetTeamComponent(AActor* Target);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Team", meta = (DefaultToSelf = "Target"))
	static FGuid GetGuid(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Team", meta = (DefaultToSelf = "Source"))
	static void AddLocalRelation(AActor* Source, AActor* Target, ERelation Relation);

	UFUNCTION(BlueprintCallable, Category = "Team", meta = (DefaultToSelf = "Source"))
	static void ClearLocalRelation(AActor* Source, AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Team", meta = (WorldContext = "WorldContextObject"))
	static void AddRelationInFaction(const UObject* WorldContextObject,
		UPARAM(meta=(Categories="Team")) FGameplayTag SourceFactionTag,
		UPARAM(meta=(Categories="Team")) FGameplayTag TargetFactionTag, ERelation Relation);

	UFUNCTION(BlueprintCallable, Category = "Team", meta = (WorldContext = "WorldContextObject"))
	static void ClearRelationInFaction(const UObject* WorldContextObject,
		UPARAM(meta=(Categories="Team")) FGameplayTag SourceFactionTag,
		UPARAM(meta=(Categories="Team")) FGameplayTag TargetFactionTag);

private:
	static bool CheckComponent(AActor* Source);
};
