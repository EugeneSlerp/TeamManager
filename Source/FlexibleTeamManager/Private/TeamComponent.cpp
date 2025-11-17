// Copyright (c) 2025 EugeneSlerp. All rights reserved.


#include "TeamComponent.h"
#include "FlexibleTeamManager.h"
#include "TeamManager.h"

void UTeamComponent::ChangeFaction(FGameplayTag NewFactionTag, bool bClearLocalRelations)
{
	if (!NewFactionTag.IsValid())
	{
		return;
	}
	FFaction FoundFaction = GetTeamManager()->FindFaction(NewFactionTag);
	if (!FoundFaction.Faction.IsValid())
	{
		return;
	}
	CachedFaction = FoundFaction;
	FactionTag = NewFactionTag;
	if (bClearLocalRelations)
	{
		ClearLocalRelations();
	}
	OnChangeFaction.Broadcast(NewFactionTag);
	UE_LOG(LogFlexibleTeamManager, Log, TEXT("Faction changed on %s"), *FactionTag.ToString())
}

void UTeamComponent::ClearLocalRelations()
{
	if (UTeamManager* TM = GetTeamManager())
	{
		TM->ClearLocalRelationsForComp(this);
	}
}


void UTeamComponent::ClearLocalRelation(AActor* TargetActor)
{
	if (!TargetActor)
	{
		return;
	}
	if (UTeamManager* TM = GetTeamManager())
	{
		if (UTeamComponent* TC = TargetActor->FindComponentByClass<UTeamComponent>())
		{
			TM->ClearLocalRelationForComp(this, TC);
		}
	}
}

ERelation UTeamComponent::GetRelationForOther(UTeamComponent* Other) const
{
	if (!IsValid(Other))
	{
		return ERelation::None;
	}
	if (const ERelation* Relation = CachedFaction.Relations.Find(Other->GetFactionTag()))
	{
		return *Relation;
	}
	if (const ERelation* Relation = LocalRelations.Relations.Find(Other->GetGuid()))
	{
		return *Relation;
	}
	return UTeamManager::GetPriorityDefaultRelationBetween(GetFaction().DefaultRelation, Other->GetFaction().DefaultRelation);
}

bool UTeamComponent::IsHostile(UTeamComponent* Other) const
{
	if (!IsValid(Other))
	{
		return false;
	}
	if (GetRelationForOther(Other) != ERelation::Hostile)
	{
		return false;
	}
	return true;
}

bool UTeamComponent::IsNeutral(UTeamComponent* Other) const
{
	if (!IsValid(Other))
	{
		return false;
	}
	if (GetRelationForOther(Other) != ERelation::Neutral)
	{
		return false;
	}
	return true;
}

bool UTeamComponent::IsFriendly(UTeamComponent* Other) const
{
	if (!IsValid(Other))
	{
		return false;
	}
	if (GetRelationForOther(Other) != ERelation::Friendly)
	{
		return false;
	}
	return true;
}

void UTeamComponent::BeginPlay()
{
	Super::BeginPlay();
	if (!GetGuid().IsValid())
	{
		Guid = FGuid::NewGuid();
	}
	if (FactionTag == FGameplayTag::EmptyTag)
	{
		UE_LOG(LogFlexibleTeamManager, Error, TEXT("Component of actor %s not has a faction tag"), *GetOwner()->GetFName().ToString())
		DestroyComponent();
		return;
	}
	if (UTeamManager* TM = GetTeamManager())
	{
		TM->RegisterInstanceTeamComponent(this);
		ChangeFaction(FactionTag, true);
		OnChangeRelationHandle = TM->OnChangeRelationBetweenFactions.AddUObject(this, &UTeamComponent::OnChangeRelationBetweenFractions);
		OnChangeLocalRelationHandle = TM->OnChangeLocalRelationBetweenComps.AddUObject(this, &UTeamComponent::OnChangeRelationBetweenComps);
	}
}

void UTeamComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UTeamManager* TM = GetTeamManager())
	{
		if (OnChangeRelationHandle.IsValid())
		{
			TM->OnChangeRelationBetweenFactions.Remove(OnChangeRelationHandle);
		}
		if (OnChangeLocalRelationHandle.IsValid())
		{
			TM->OnChangeLocalRelationBetweenComps.Remove(OnChangeLocalRelationHandle);
		}
		TM->UnregisterInstanceTeamComponent(this);
	}
	Super::EndPlay(EndPlayReason);
}

void UTeamComponent::OnChangeRelationBetweenFractions(const FGameplayTag Source, const FGameplayTag Target,
	ERelation Relation)
{
	if (Source == GetFactionTag())
	{
		OnChangeRelation.Broadcast(Target, Relation);
	}
}

void UTeamComponent::OnChangeRelationBetweenComps(UTeamComponent* Source, UTeamComponent* Target, ERelation Relation)
{
	if (Source == this)
	{
		if (OnChangeLocalRelation.IsBound())
		{
			OnChangeLocalRelation.Broadcast(Target, Relation);
		}
	}
}

UTeamManager* UTeamComponent::GetTeamManager() const
{
	return GetWorld() ? GetWorld()->GetSubsystem<UTeamManager>() : nullptr;
}
