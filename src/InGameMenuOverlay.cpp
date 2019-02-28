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
#include "InGameMenuOverlay.h"
#include "OverlayManagement.h"
#include "ApplicationData.h"
#include "Debug.h"
#include "VerticalButtonAlignment.h"

std::shared_ptr<CInGameMenuOverlay> CInGameMenuOverlay::DInGameMenuOverlayPointer;
std::shared_ptr<COverlayManagement> CInGameMenuOverlay::DOverlayManager;
std::shared_ptr<CApplicationData> CInGameMenuOverlay::DContext;

CInGameMenuOverlay::CInGameMenuOverlay(const SPrivateConstructorType &key)
{
    DOverlayManager = COverlayManagement::Manager();
    DContext = DOverlayManager->Context();

    DButtonTexts.push_back("Sound Options");
    DButtonTexts.push_back("Return to Game");
    DButtonTexts.push_back("Leave Game");

    DButtonStack = std::make_shared<CVerticalButtonAlignment>(DContext,
        DButtonTexts, EPosition::Center);
}

//! Instantiate
std::shared_ptr<COverlayMode> CInGameMenuOverlay::Initialize()
{
    if (nullptr == DInGameMenuOverlayPointer)
    {
        DInGameMenuOverlayPointer =
            std::make_shared<CInGameMenuOverlay>(SPrivateConstructorType());

    }
    return DInGameMenuOverlayPointer;
}

//! Handle inputs
void CInGameMenuOverlay::Input()
{
    int ButtonIndex;

    if (DButtonStack->ButtonPressedInStack())
    {
        switch (DButtonStack->ButtonPressedIndex())
        {
            case 0:
                //DOverlayManager->ChangeOverlay(CSoundOptionsOverlay::Initialize());
                break;
            case 1:
                DOverlayManager->ReturnToGameFunction();
                break;
            case 2:
                DOverlayManager->LeaveGameFunction();
                break;
        }


    }
}

//! Handle drawing
void CInGameMenuOverlay::Draw(int x, int y, bool clicked)
{
    DButtonStack->DrawStack(x, y, clicked);
}