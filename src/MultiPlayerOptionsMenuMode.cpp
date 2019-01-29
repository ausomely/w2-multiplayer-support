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
#include "MultiPlayerOptionsMenuMode.h"
#include "ApplicationData.h"
#include "MainMenuMode.h"
#include "MapSelectionMode.h"
#include "Client.h"

std::shared_ptr<CApplicationMode>
    CMultiPlayerOptionsMenuMode::DMultiPlayerOptionsMenuModePointer;

CMultiPlayerOptionsMenuMode::CMultiPlayerOptionsMenuMode(
    const SPrivateConstructorType &key)
{
    DTitle = "Multi Player Game Options";
    DButtonTexts.push_back("Host Multi Player Game");
    DButtonFunctions.push_back(HostMultiPlayerButtonCallback);
    DButtonTexts.push_back("Join Multi Player Game");
    DButtonFunctions.push_back(JoinMultiPlayerButtonCallback);
    DButtonTexts.push_back("");
    DButtonFunctions.push_back(nullptr);
    DButtonTexts.push_back("Back");
    DButtonFunctions.push_back(MainMenuButtonCallback);
}

//! @brief Opens match hosting screen
void CMultiPlayerOptionsMenuMode::HostMultiPlayerButtonCallback(
    std::shared_ptr<CApplicationData> context)
{
    // changed to single player for testing
    //context->DGameSessionType = CApplicationData::gstSinglePlayer;
    context->DGameSessionType = CApplicationData::gstMultiPlayerHost;

    context->ChangeApplicationMode(CMapSelectionMode::Instance());
}

//! @brief Opens available match lists
void CMultiPlayerOptionsMenuMode::JoinMultiPlayerButtonCallback(
    std::shared_ptr<CApplicationData> context)
{
    context->ClientPointer->JoinMultiPlayer();
}

//! @brief Returns to Main menu of the game
void CMultiPlayerOptionsMenuMode::MainMenuButtonCallback(
    std::shared_ptr<CApplicationData> context)
{
    context->ClientPointer->CloseConnection();
    context->ChangeApplicationMode(CMainMenuMode::Instance());
}

//! @brief Creates an instance of Multiplayer options
std::shared_ptr<CApplicationMode> CMultiPlayerOptionsMenuMode::Instance()
{
    if (DMultiPlayerOptionsMenuModePointer == nullptr)
    {
        DMultiPlayerOptionsMenuModePointer =
            std::make_shared<CMultiPlayerOptionsMenuMode>(
                SPrivateConstructorType());
    }
    return DMultiPlayerOptionsMenuModePointer;
}
