// Copyright Epic Games, Inc. All Rights Reserved.


#include "S1GameModeBase.h"
#include "Character/S1MyPlayer.h"
#include "AIController.h"

AS1GameModeBase::AS1GameModeBase()
{
	DefaultPawnClass = AS1MyPlayer::StaticClass();
	PlayerControllerClass = AAIController::StaticClass();
}
