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
    std::shared_ptr<CApplicationData> context)
{
    DContext = context;
    auto SurfaceBuilder = new COverlaySurfaceBuilder(DContext->DViewportSurface,
        DContext->DViewportXOffset, DContext->DViewportYOffset);

    DSurface = SurfaceBuilder->GetOverlaySurface();

    DBorderWidth = DContext->DBorderWidth;
    DXoffset = SurfaceBuilder->DXoffset;
    DYoffset = SurfaceBuilder->DYoffset;
    DWidth = SurfaceBuilder->DWidth;
    DHeight = SurfaceBuilder->DHeight;


    DChangeOverlay = false;
    DLeaveGameFlag = false;
    DExitOverlayManager = false;
}

//! Instantiate singleton overlay manager
std::shared_ptr<COverlayManagement>
COverlayManagement::Initialize(std::shared_ptr<CApplicationData> context)
{
    if (nullptr == DOverlayManagementPointer)
    {
        DOverlayManagementPointer =
            std::make_shared<COverlayManagement>(SPrivateConstructorType {},
                context);
    }
    return DOverlayManagementPointer;
}

// Called from overlay classes; never called from the overlay manager
void COverlayManagement::SetMode(EOverlay mode)
{
    DChangeOverlay = true;
    ENextOverlay = mode;
}

void COverlayManagement::ChangeOverlay()
{
    BuildOverlay(ENextOverlay);

    // Reset change overlay state
    DChangeOverlay = false;
    ENextOverlay = EOverlay::None;
}


void COverlayManagement::BuildOverlay(EOverlay mode)
{
    switch (mode)
    {
        case EOverlay::InGameMenu:
        {
            DOverlayMode.reset(new CInGameMenuOverlay(DOverlayManagementPointer));
            break;
        }
        case EOverlay::SoundOptions:
        {
            DOverlayMode.reset(new CSoundOptionsOverlay(DOverlayManagementPointer));
            break;
        }
    }
}


void COverlayManagement::DrawBackground()
{
    int PageWidth = DSurface->Width();
    int PageHeight = DSurface->Height();
    DContext->DrawBackground(DSurface, PageWidth, PageHeight);
    DContext->DrawOuterBevel(DSurface, PageWidth, PageHeight);
}

void COverlayManagement::Draw(int x, int y, bool clicked)
{
    // Check if overlay mode is changing
    if (DChangeOverlay)
    {
        ChangeOverlay();
    }

    if (nullptr != DOverlayMode)
    {
        DrawBackground();
        DOverlayMode->Draw(x, y, clicked);
    }

}

void COverlayManagement::ProcessInput(int x, int y, bool clicked)
{
    if (nullptr != DOverlayMode)
    {
        DOverlayMode->Input(x, y, clicked);
    }

    if (DExitOverlayManager)
    {
        if (DLeaveGameFlag)
        {
            DContext->SetLeaveGameFlag(true);
        }

        DContext->DeactivateOverlay();

        // Set overlay manager to defaults
        SetDefaults();
    }
}

// Set default settings for the next time the manager is started
void COverlayManagement::SetDefaults()
{
    DOverlayMode.reset();
    DChangeOverlay = true;
    ENextOverlay = EOverlay::InGameMenu;
    DLeaveGameFlag = false;
    DExitOverlayManager = false;
}

// Set leave game flags. Only called by overlay mode. Never called from manager.
void COverlayManagement::LeaveGame()
{
    DExitOverlayManager = true;
    DLeaveGameFlag = true;
}

// Set exit manager flags. Only called by overlay mode. Never called from manager.
void COverlayManagement::ReturnToGame()
{
    DExitOverlayManager = true;
}

void COverlayManagement::ClearMouseButtonState()
{
    DContext->ClearMouseButtonState();
}
