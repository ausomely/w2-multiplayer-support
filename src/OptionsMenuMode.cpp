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
#include "OptionsMenuMode.h"
#include "ApplicationData.h"
#include "MainMenuMode.h"
#include "NetworkOptionsMode.h"
#include "SoundOptionsMode.h"

std::shared_ptr<CApplicationMode> COptionsMenuMode::DOptionsMenuModePointer;

COptionsMenuMode::COptionsMenuMode(const SPrivateConstructorType &key)
{
    DTitle = "Options";
    DButtonTexts.push_back("Sound Options");
    DButtonFunctions.push_back(SoundOptionsButtonCallback);
    DButtonTexts.push_back("Network Options");
    DButtonFunctions.push_back(NetworkOptionsButtonCallback);
    DButtonTexts.push_back("");
    DButtonFunctions.push_back(nullptr);
    DButtonTexts.push_back("Back");
    DButtonFunctions.push_back(MainMenuButtonCallback);
}

void COptionsMenuMode::InitializeChange(std::shared_ptr<CApplicationData> context)
{
    int CanvasWidth = context->DWorkingBufferSurface->Width();
    int CanvasHeight = context->DWorkingBufferSurface->Height();

    DButtonStack.reset(new CVerticalButtonAlignment(context, DButtonTexts,
        EPosition::Center, CanvasWidth, CanvasHeight));
}

//! @brief Opens sound options
void COptionsMenuMode::SoundOptionsButtonCallback(
    std::shared_ptr<CApplicationData> context)
{
    context->ChangeApplicationMode(CSoundOptionsMode::Instance());
}

//! @brief Opens network options
void COptionsMenuMode::NetworkOptionsButtonCallback(
    std::shared_ptr<CApplicationData> context)
{
    context->ChangeApplicationMode(CNetworkOptionsMode::Instance());
}

//! @brief Return to Main Menu
void COptionsMenuMode::MainMenuButtonCallback(
    std::shared_ptr<CApplicationData> context)
{
    context->ChangeApplicationMode(CMainMenuMode::Instance());
}

//! @brief Creates an instance of Options screen
std::shared_ptr<CApplicationMode> COptionsMenuMode::Instance()
{
    if (DOptionsMenuModePointer == nullptr)
    {
        DOptionsMenuModePointer =
            std::make_shared<COptionsMenuMode>(SPrivateConstructorType());
    }
    return DOptionsMenuModePointer;
}
