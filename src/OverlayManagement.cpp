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
#include "ApplicationData.h"
#include "BattleMode.h"
#include "GraphicSurface.h"
#include "InGameMenuOverlay.h"
#include "OverlayManagement.h"
#include "OverlayMode.h"

std::shared_ptr<COverlayManagement> COverlayManagement::DOverlayManagementPointer;

COverlayManagement::COverlayManagement(const SPrivateConstructorType &key,
    std::shared_ptr<CApplicationData> &context)
{
    DContext = context;
    DSurface = DContext->DOverlaySurface;
    DBorderWidth = DContext->DBorderWidth;
}

//! Instantiate singleton overlay manager
std::shared_ptr<COverlayManagement> COverlayManagement::Initialize(
    std::shared_ptr<CApplicationData> context)
{
    if (nullptr == DOverlayManagementPointer)
    {
        DOverlayManagementPointer =
            std::make_shared<COverlayManagement>(SPrivateConstructorType(),
            context);
    }
    return DOverlayManagementPointer;
}

void COverlayManagement::Draw(int x, int y, bool clicked)
{
    if (nullptr != DOverlayMode)
    {
        DOverlayMode->Draw(x, y, clicked);
    }
}

void COverlayManagement::ProcessInput()
{
    if (nullptr != DOverlayMode)
    {
        DOverlayMode->Input();
    }
}

void COverlayManagement::LeaveGameFunction()
{
    DContext->ToggleOverlay();
    DContext->LeaveGame();
}

void COverlayManagement::ReturnToGameFunction()
{
    DContext->ToggleOverlay();
}
