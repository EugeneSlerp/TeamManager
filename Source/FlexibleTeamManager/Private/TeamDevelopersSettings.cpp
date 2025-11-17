// Copyright (c) 2025 EugeneSlerp. All rights reserved.


#include "TeamDevelopersSettings.h"
#include "FlexibleTeamManager.h"

void UTeamDevelopersSettings::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UTeamDevelopersSettings, FactionDataTable))
	{
		if (!FactionDataTable.GetAssetName().IsEmpty())
		{
			UDataTable* DT = FactionDataTable.LoadSynchronous();
			if (DT->RowStruct != FFaction::StaticStruct())
			{
				FactionDataTable.Reset();
				UE_LOG(LogFlexibleTeamManager, Error, TEXT("In team settings data table not has a faction struct"))
			}
		}
	}
}
