/*
    Copyright (c) 2015, Christopher Nitta
    All rights reserved.

    All source material (source code, images, sounds, etc.) have been provided
    to University of California, Davis students of course ECS 160 for educational
    purposes. It may not be distributed beyond those enrolled in the course
    without prior permission from the copyright holder.

    All sound files, sound fonts, midi files, and images that have been included
    that were extracted from original Warcraft II by Blizzard Entertainment
    were found freely available via internet sources and have been labeld as
    abandonware. They have been included in this distribution for educational
    purposes only and this copyright notice does not attempt to claim any
    ownership of this material.
*/
#include "TriggerController.h"
#include "ApplicationData.h"
#include "GameModel.h"

std::shared_ptr<CTriggerController> CTriggerController::DTriggerControllerPtr;

CTriggerController::CTriggerController(const SPrivateConstructorType &key,
    std::shared_ptr<CApplicationData> context)
{
    DContext = context;
    DContextRetrieval = CContextRetrieval::Instance(context);

}

std::shared_ptr<CTriggerController> CTriggerController::Instance(
    std::shared_ptr<CApplicationData> context)
{
    if (nullptr == DTriggerControllerPtr)
    {
        DTriggerControllerPtr =
            std::make_shared<CTriggerController>(SPrivateConstructorType{},
            context);
    }

    return DTriggerControllerPtr;
}

void CTriggerController::CheckTriggers()
{
    for (int Player = 1; Player < PlayerCount(); Player++)
    {
        for (auto Trigger : DTriggers)
        {
            if (Trigger->Check(static_cast<EPlayerNumber>(Player)))
            {
                Trigger->InitiateEffects(static_cast<EPlayerNumber>(Player));
            }
        }
    }
}

int CTriggerController::PlayerCount()
{
    return DContext->DSelectedMap->PlayerCount();
}
