// Copyright (c) 2025 EugeneSlerp. All rights reserved.


#include "TeamManager.h"
#include "FlexibleTeamManager.h"
#include "TeamComponent.h"
#include "TeamDevelopersSettings.h"


void UTeamManager::RegisterInstanceTeamComponent(UTeamComponent* TeamComponent)
{
	if (IsValid(TeamComponent))
	{
		if (RegisteredTeamComponents.Contains(TeamComponent->GetGuid()))
		{
			return;
		}
		RegisteredTeamComponents.Add(TeamComponent->GetGuid(), TeamComponent);
		UE_LOG(LogFlexibleTeamManager, Log, TEXT("Actor %s is registered in fraction system"),
		       *TeamComponent->GetOwner()->GetFName().ToString())
	}
}

void UTeamManager::UnregisterInstanceTeamComponent(UTeamComponent* TeamComponent)
{
	if (IsRegisterTeamComp(TeamComponent))
	{
		ClearLocalRelationsForComp(TeamComponent);
		RegisteredTeamComponents.Remove(TeamComponent->GetGuid());
		UE_LOG(LogFlexibleTeamManager, Log, TEXT("Actor %s is unregistered in fraction system"),
		       *TeamComponent->GetOwner()->GetFName().ToString())
	}
}

void UTeamManager::AddLocalRelation(UTeamComponent* Source, UTeamComponent* Target, ERelation Relation)
{
	if (IsRegisterTeamComp(Source) && IsRegisterTeamComp(Target) && Relation != ERelation::None)
	{
		Source->GetLocalRelations().AddRelation(Target->GetGuid(), Relation);
		Target->GetLocalRelations().AddRelation(Source->GetGuid(), Relation);
		NotifyChangeRelationBetweenComps(Source, Target, Relation);
		UE_LOG(LogFlexibleTeamManager, Log, TEXT("Actor %s add new relation to %s a %s"),
		       *Source->GetOwner()->GetName(), *Target->GetOwner()->GetName(), *RelationToString(Relation));
	}
}

void UTeamManager::ClearLocalRelationForComp(UTeamComponent* Source, UTeamComponent* Target)
{
	if (IsRegisterTeamComp(Source) && IsRegisterTeamComp(Target))
	{
		if (!Source->GetLocalRelations().FindRelation(Target->GetGuid()))
		{
			return;
		}
		Source->GetLocalRelations().RemoveRelation(Target->GetGuid());
		Target->GetLocalRelations().RemoveRelation(Source->GetGuid());
		NotifyChangeRelationBetweenComps(Source, Target, Source->GetRelationForOther(Target));
		UE_LOG(LogFlexibleTeamManager, Log, TEXT("Actor %s clear relation to %s, current relation: %s"),
		       *Source->GetOwner()->GetName(), *Target->GetOwner()->GetName(),
		       *RelationToString(Source->GetRelationForOther(Target)));
	}
}

void UTeamManager::ClearLocalRelationsForComp(UTeamComponent* TeamComponent)
{
	if (!TeamComponent)
	{
		return;
	}
	for (auto& Pair : RegisteredTeamComponents)
	{
		if (Pair.Value.Get())
		{
			ClearLocalRelationForComp(TeamComponent, Pair.Value.Get());
		}
	}
}

void UTeamManager::AddRelationInFaction(FGameplayTag SourceFactionTag, FGameplayTag TargetFactionTag,
                                        ERelation Relation)
{
	if (!CachedFactions.Contains(SourceFactionTag) || !CachedFactions.Contains(TargetFactionTag) ||
		Relation == ERelation::None)
	{
		return;
	}
	FFaction* SourceFaction = CachedFactions.Find(SourceFactionTag);
	FFaction* TargetFaction = CachedFactions.Find(TargetFactionTag);
	SourceFaction->AddRelation(TargetFactionTag, Relation);
	TargetFaction->AddRelation(SourceFactionTag, Relation);
	for (auto& Pair : RegisteredTeamComponents)
	{
		if (!IsValid(Pair.Value.Get()))
		{
			continue;
		}
		if (Pair.Value->GetFactionTag() == SourceFactionTag)
		{
			Pair.Value->GetMutableFaction().Copy(*SourceFaction);
			Pair.Value->OnChangeRelation.Broadcast(TargetFactionTag, Relation);
		}
		else if (Pair.Value->GetFactionTag() == TargetFactionTag)
		{
			Pair.Value->GetMutableFaction().Copy(*TargetFaction);
			Pair.Value->OnChangeRelation.Broadcast(SourceFactionTag, Relation);
		}
	}
	UE_LOG(LogFlexibleTeamManager, Log, TEXT("Faction %s add relation to %s a %hhd"), *SourceFactionTag.ToString(),
	       *TargetFactionTag.ToString(), Relation);
}

void UTeamManager::ClearRelationInFaction(FGameplayTag SourceFactionTag, FGameplayTag TargetFactionTag)
{
	if (!CachedFactions.Contains(SourceFactionTag) || !CachedFactions.Contains(TargetFactionTag))
	{
		return;
	}
	FFaction* SourceFaction = CachedFactions.Find(SourceFactionTag);
	FFaction* TargetFaction = CachedFactions.Find(TargetFactionTag);
	SourceFaction->RemoveRelation(TargetFactionTag);
	TargetFaction->RemoveRelation(SourceFactionTag);

	for (auto& Pair : RegisteredTeamComponents)
	{
		if (!IsValid(Pair.Value.Get()))
		{
			continue;
		}
		if (Pair.Value->GetFactionTag() == SourceFactionTag)
		{
			Pair.Value->GetMutableFaction().Copy(*SourceFaction);
		}
		else if (Pair.Value->GetFactionTag() == TargetFactionTag)
		{
			Pair.Value->GetMutableFaction().Copy(*TargetFaction);
		}
	}
	ERelation PriorityRelation = GetPriorityDefaultRelationBetween(SourceFaction->DefaultRelation,
	                                                               TargetFaction->DefaultRelation);
	NotifyChangeRelationBetweenFactions(SourceFactionTag, TargetFactionTag, PriorityRelation);
	UE_LOG(LogFlexibleTeamManager, Log, TEXT("Faction %s clear relation to %s, default relation %hhd"),
	       *SourceFactionTag.ToString(), *TargetFactionTag.ToString(), PriorityRelation);
}


bool UTeamManager::HasFaction(FGameplayTag FactionTag)
{
	return FindFaction(FactionTag).Faction.IsValid();
}

FFaction UTeamManager::FindFaction(FGameplayTag FactionTag)
{
	if (FactionTag.IsValid())
	{
		if (FFaction* FoundFaction = CachedFactions.Find(FactionTag))
		{
			return *FoundFaction;
		}
	}
	return FFaction();
}

void UTeamManager::UpdateCachedFactionsFromComps()
{
	for (auto& Pair : RegisteredTeamComponents)
	{
		if (!IsValid(Pair.Value.Get()))
		{
			continue;
		}
		CachedFactions.Add(Pair.Value->GetFactionTag(), Pair.Value->GetFaction());
	}
}

TArray<FFaction> UTeamManager::GetCachedFactions() const
{
	TArray<FFaction> CachedFactionsArray;
	CachedFactions.GenerateValueArray(CachedFactionsArray);
	return CachedFactionsArray;
}

TArray<UTeamComponent*> UTeamManager::GetAllComponentsOfFaction(FGameplayTag FactionTag)
{
	if (!FactionTag.IsValid())
	{
		return TArray<UTeamComponent*>();
	}
	TArray<UTeamComponent*> Results;
	for (auto& Pair : RegisteredTeamComponents)
	{
		if (!IsValid(Pair.Value.Get()))
		{
			continue;
		}
		if (Pair.Value->GetFactionTag() == FactionTag)
		{
			Results.Add(Pair.Value.Get());
		}
	}
	return Results;
}

void UTeamManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	const UTeamDevelopersSettings* DevSet = GetDefault<UTeamDevelopersSettings>();
	if (DevSet->FactionDataTable)
	{
		FactionDataTable = DevSet->FactionDataTable.LoadSynchronous();

		if (FactionDataTable)
		{
			TArray<FFaction*> Factions;
			FactionDataTable->GetAllRows<FFaction>("", Factions);
			for (FFaction* Faction : Factions)
			{
				CachedFactions.Add(Faction->Faction, *Faction);
			}
			UE_LOG(LogFlexibleTeamManager, Log, TEXT("Team manager initialize success"))
		}
		else
		{
			UE_LOG(LogFlexibleTeamManager, Error,
			       TEXT("Faction data table not valid. Check team settings in project settings"))
		}
	}
}

bool UTeamManager::IsRegisterTeamComp(const UTeamComponent* TeamComponent) const
{
	return TeamComponent ? RegisteredTeamComponents.Contains(TeamComponent->GetGuid()) : false;
}

UTeamComponent* UTeamManager::GetTeamCompByGuid(const FGuid& Guid)
{
	return RegisteredTeamComponents.Find(Guid)->Get();
}

ERelation UTeamManager::GetPriorityDefaultRelationBetween(ERelation FirstRelation, ERelation SecondRelation)
{
	const UTeamDevelopersSettings* DevSet = GetDefault<UTeamDevelopersSettings>();
	TArray<ERelation> PriorityRelations = DevSet->DefaultRelationPriority.Array();
	if (PriorityRelations.Num() <= 0)
	{
		return ERelation::None;
	}
	int32 FirstIndex = PriorityRelations.Find(FirstRelation);
	int32 SecondIndex = PriorityRelations.Find(SecondRelation);
	if (FirstIndex == INDEX_NONE && SecondIndex == INDEX_NONE)
	{
		return PriorityRelations[0];
	}
	if (FirstIndex < SecondIndex && FirstIndex != INDEX_NONE)
	{
		return PriorityRelations[FirstIndex];
	}
	if (SecondIndex < FirstIndex && SecondIndex != INDEX_NONE)
	{
		return PriorityRelations[SecondIndex];
	}
	if (FirstIndex == SecondIndex)
	{
		return PriorityRelations[FirstIndex];
	}
	return PriorityRelations[0];
}

void UTeamManager::NotifyChangeRelationBetweenComps(UTeamComponent* Source, UTeamComponent* Target, ERelation Relation)
{
	if (OnChangeLocalRelationBetweenComps.IsBound())
	{
		OnChangeLocalRelationBetweenComps.Broadcast(Source, Target, Relation);
		OnChangeLocalRelationBetweenComps.Broadcast(Target, Source, Relation);
	}
}

void UTeamManager::NotifyChangeRelationBetweenFactions(const FGameplayTag Source, const FGameplayTag Target,
                                                       ERelation Relation)
{
	if (OnChangeRelationBetweenFactions.IsBound())
	{
		OnChangeRelationBetweenFactions.Broadcast(Source, Target, Relation);
		OnChangeRelationBetweenFactions.Broadcast(Target, Source, Relation);
	}
}
