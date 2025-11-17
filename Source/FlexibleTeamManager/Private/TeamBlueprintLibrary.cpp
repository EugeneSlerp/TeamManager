// Copyright (c) 2025 EugeneSlerp. All rights reserved.


#include "TeamBlueprintLibrary.h"
#include "TeamComponent.h"
#include "TeamManager.h"

ERelation UTeamBlueprintLibrary::GetRelationTo(AActor* Source, AActor* Target)
{
	if (!CheckComponent(Source) || !CheckComponent(Target))
	{
		return ERelation::None;
	}
	UTeamComponent* SourceComp = GetTeamComponent(Source);
	UTeamComponent* TargetComp = GetTeamComponent(Target);
	if (!SourceComp->GetFactionTag().IsValid() || !TargetComp->GetFactionTag().IsValid())
	{
		return ERelation::None;
	}
	return SourceComp->GetRelationForOther(TargetComp);
}

bool UTeamBlueprintLibrary::IsHostile(AActor* Source, AActor* Target)
{
	if (!CheckComponent(Source) || !CheckComponent(Target))
	{
		return false;
	}
	UTeamComponent* SourceComp = GetTeamComponent(Source);
	UTeamComponent* TargetComp = GetTeamComponent(Target);
	return SourceComp->IsHostile(TargetComp);
}

bool UTeamBlueprintLibrary::IsNeutral(AActor* Source, AActor* Target)
{
	if (!CheckComponent(Source) || !CheckComponent(Target))
	{
		return false;
	}
	UTeamComponent* SourceComp = GetTeamComponent(Source);
	UTeamComponent* TargetComp = GetTeamComponent(Target);
	return SourceComp->IsNeutral(TargetComp);
}

bool UTeamBlueprintLibrary::IsFriendly(AActor* Source, AActor* Target)
{
	if (!CheckComponent(Source) || !CheckComponent(Target))
	{
		return false;
	}
	UTeamComponent* SourceComp = GetTeamComponent(Source);
	UTeamComponent* TargetComp = GetTeamComponent(Target);
	return SourceComp->IsFriendly(TargetComp);
}

FGameplayTag UTeamBlueprintLibrary::GetFactionTag(AActor* Target)
{
	if (!CheckComponent(Target))
	{
		return FGameplayTag::EmptyTag;
	}
	UTeamComponent* TargetComp = GetTeamComponent(Target);
	return TargetComp->GetFactionTag();
}

UTeamComponent* UTeamBlueprintLibrary::GetTeamComponent(AActor* Target)
{
	if (!IsValid(Target))
	{
		return nullptr;
	}
	if (AController* Controller = Cast<AController>(Target))
	{
		if (UTeamComponent* TeamComp = Controller->GetComponentByClass<UTeamComponent>())
		{
			return TeamComp;
		}
		if (IsValid(Controller->GetPawn()))
		{
			return Controller->GetPawn()->GetComponentByClass<UTeamComponent>();
		}
	}
	return Target->GetComponentByClass<UTeamComponent>();
}

FGuid UTeamBlueprintLibrary::GetGuid(AActor* Target)
{
	if (!CheckComponent(Target))
	{
		return FGuid();
	}
	UTeamComponent* TargetComp = GetTeamComponent(Target);
	return TargetComp->GetGuid();
	
}

void UTeamBlueprintLibrary::AddLocalRelation(AActor* Source, AActor* Target, ERelation Relation)
{
	if (!CheckComponent(Source) || !CheckComponent(Target) || Relation == ERelation::None)
	{
		return;
	}
	UTeamComponent* SourceComp = GetTeamComponent(Source);
	UTeamComponent* TargetComp = GetTeamComponent(Target);
	if (!IsValid(SourceComp->GetTeamManager()))
	{
		return;
	}
	UTeamManager* TM = SourceComp->GetTeamManager();
	TM->AddLocalRelation(SourceComp, TargetComp, Relation);
}

void UTeamBlueprintLibrary::ClearLocalRelation(AActor* Source, AActor* Target)
{
	if (!CheckComponent(Source) || !CheckComponent(Target))
	{
		return;
	}
	UTeamComponent* SourceComp = GetTeamComponent(Source);
	UTeamComponent* TargetComp = GetTeamComponent(Target);
	if (!IsValid(SourceComp->GetTeamManager()))
	{
		return;
	}
	UTeamManager* TM = SourceComp->GetTeamManager();
	TM->ClearLocalRelationForComp(SourceComp, TargetComp);
}

void UTeamBlueprintLibrary::AddRelationInFaction(const UObject* WorldContextObject, FGameplayTag SourceFactionTag,
	FGameplayTag TargetFactionTag, ERelation Relation)
{
	if (!IsValid(WorldContextObject) || !SourceFactionTag.IsValid() || !TargetFactionTag.IsValid() || Relation == ERelation::None)
	{
		return;
	}
	if (!IsValid(WorldContextObject->GetWorld()))
	{
		return;
	}
	UTeamManager* TM = WorldContextObject->GetWorld()->GetSubsystem<UTeamManager>();
	if (!IsValid(TM))
	{
		return;
	}
	TM->AddRelationInFaction(SourceFactionTag, TargetFactionTag, Relation);
}

void UTeamBlueprintLibrary::ClearRelationInFaction(const UObject* WorldContextObject, FGameplayTag SourceFactionTag,
	FGameplayTag TargetFactionTag)
{
	if (!IsValid(WorldContextObject) || !SourceFactionTag.IsValid() || !TargetFactionTag.IsValid())
	{
		return;
	}
	if (!IsValid(WorldContextObject->GetWorld()))
	{
		return;
	}
	UTeamManager* TM = WorldContextObject->GetWorld()->GetSubsystem<UTeamManager>();
	if (!IsValid(TM))
	{
		return;
	}
	TM->ClearRelationInFaction(SourceFactionTag, TargetFactionTag);
}

bool UTeamBlueprintLibrary::CheckComponent(AActor* Source)
{
	if (!IsValid(Source))
	{
		return false;
	}
	if (!IsValid(GetTeamComponent(Source)))
	{
		return false;
	}
	return true;
}
