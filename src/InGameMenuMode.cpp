#include "InGameMenuMode.h"
#include <MainMenuMode.h>
#include "ApplicationData.h"
#include "BattleMode.h"
#include "MainMenuMode.h"
#include "MemoryDataSource.h"
#include "SoundOptionsMode.h"

std::shared_ptr<CApplicationMode> CInGameMenuMode::DInGameMenuPointer;

CInGameMenuMode::CInGameMenuMode(const SPrivateConstructorType &key)
{
    DTitle = "In-game Options Menu";
    DButtonTexts.push_back("Leave Game");
    DButtonFunctions.push_back(MainMenuButtonCallback);
    DButtonTexts.push_back("Sound Options");
    DButtonFunctions.push_back(SoundOptionsButtonCallback);
    DButtonTexts.push_back("Return to Game");
    DButtonFunctions.push_back(ReturnToGameButtonCallback);
    DButtonTexts.push_back("Exit Game");
    DButtonFunctions.push_back(ExitGameButtonCallback);
}

//! @brief Opens in-game menu
void CInGameMenuMode::MainMenuButtonCallback(
    std::shared_ptr<CApplicationData> context)
{
    context->DActiveGame = false;
    context->ChangeApplicationMode(CMainMenuMode::Instance());
}

//! @brief Opens sound settings
void CInGameMenuMode::SoundOptionsButtonCallback(
    std::shared_ptr<CApplicationData> context)
{
    context->ChangeApplicationMode(CSoundOptionsMode::Instance());
}

//! @brief Resumes the match
void CInGameMenuMode::ReturnToGameButtonCallback(
    std::shared_ptr<CApplicationData> context)
{
    context->ChangeApplicationMode(CBattleMode::Instance());
}

//! @brief Closes the game
void CInGameMenuMode::ExitGameButtonCallback(
    std::shared_ptr<CApplicationData> context)
{
    context->DMainWindow->Close();
}

//! @brief Starts song for the menu
void CInGameMenuMode::InitializeChange(std::shared_ptr<CApplicationData> context)
{
    int CanvasWidth = context->DWorkingBufferSurface->Width();
    int CanvasHeight = context->DWorkingBufferSurface->Height();

    DButtonStack.reset(new CVerticalButtonAlignment(context, DButtonTexts,
        EPosition::Center, CanvasWidth, CanvasHeight));
    context->StopPlayingMusic();
    context->StartPlayingMusic("menu");
}

std::shared_ptr<CApplicationMode> CInGameMenuMode::Instance()
{
    if (DInGameMenuPointer == nullptr)
    {
        DInGameMenuPointer =
            std::make_shared<CInGameMenuMode>(SPrivateConstructorType());
    }
    return DInGameMenuPointer;
}
