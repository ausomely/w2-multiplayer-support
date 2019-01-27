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
#include "MainMenuMode.h"
#include "ApplicationData.h"
#include "MapSelectionMode.h"
#include "MemoryDataSource.h"
#include "MultiPlayerOptionsMenuMode.h"
#include "OptionsMenuMode.h"
#include "Client.h"

std::shared_ptr<CApplicationMode> CMainMenuMode::DMainMenuModePointer;

CMainMenuMode::CMainMenuMode(const SPrivateConstructorType &key)
{
    DTitle = "The Game";
    DButtonTexts.push_back("Single Player Game");
    DButtonFunctions.push_back(SinglePlayerGameButtonCallback);
    DButtonTexts.push_back("Multi Player Game");
    DButtonFunctions.push_back(MultiPlayerGameButtonCallback);
    DButtonTexts.push_back("Options");
    DButtonFunctions.push_back(OptionsButtonCallback);
    DButtonTexts.push_back("");
    DButtonFunctions.push_back(nullptr);
    DButtonTexts.push_back("Exit Game");
    DButtonFunctions.push_back(ExitGameButtonCallback);
}

//! @brief Opens single player menu
void CMainMenuMode::SinglePlayerGameButtonCallback(
    std::shared_ptr<CApplicationData> context)
{
    context->DGameSessionType = CApplicationData::gstSinglePlayer;

    context->ChangeApplicationMode(CMapSelectionMode::Instance());
}

//! @brief Opens multi player menu
void CMainMenuMode::MultiPlayerGameButtonCallback(
    std::shared_ptr<CApplicationData> context)
{
    context->ClientPointer = std::make_shared< Client >();

    if(context->ClientPointer->Connect(context)) {
        if(context->ClientPointer->SendLoginInfo(context)) {
            context->ChangeApplicationMode(CMultiPlayerOptionsMenuMode::Instance());
        }
    }
}

//! @brief Opens game settings
void CMainMenuMode::OptionsButtonCallback(
    std::shared_ptr<CApplicationData> context)
{
    context->ChangeApplicationMode(COptionsMenuMode::Instance());
}

//! @brief Closes the game
void CMainMenuMode::ExitGameButtonCallback(
    std::shared_ptr<CApplicationData> context)
{
    context->DMainWindow->Close();
}

//! @brief Creates an instance of Main Menu
std::shared_ptr<CApplicationMode> CMainMenuMode::Instance()
{
    if (DMainMenuModePointer == nullptr)
    {
        DMainMenuModePointer =
            std::make_shared<CMainMenuMode>(SPrivateConstructorType());
    }
    return DMainMenuModePointer;
}
