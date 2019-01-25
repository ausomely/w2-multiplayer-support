#include "GameOverMenuMode.h"
#include "ApplicationData.h"
#include "MainMenuMode.h"
#include "MemoryDataSource.h"

std::shared_ptr<CApplicationMode> CGameOverMenuMode::DGameOverMenuPointer;

CGameOverMenuMode::CGameOverMenuMode(const SPrivateConstructorType &key)
{
    DTitle = "Game over";
    DButtonTexts.push_back("Main Menu");
    DButtonFunctions.push_back(MainMenuButtonCallback);
    DButtonTexts.push_back("");
    DButtonFunctions.push_back(nullptr);
    DButtonTexts.push_back("Exit Game");
    DButtonFunctions.push_back(ExitGameButtonCallback);
}

//! @brief Return to Main menu of the game
void CGameOverMenuMode::MainMenuButtonCallback(
    std::shared_ptr<CApplicationData> context)
{
    context->DSoundLibraryMixer->StopSong();
    context->DSoundLibraryMixer->PlaySong(
        context->DSoundLibraryMixer->FindSong("menu"), context->DMusicVolume);
    context->ChangeApplicationMode(CMainMenuMode::Instance());
}

//! @brief Closes the game
void CGameOverMenuMode::ExitGameButtonCallback(
    std::shared_ptr<CApplicationData> context)
{
    context->DMainWindow->Close();
}

std::shared_ptr<CApplicationMode> CGameOverMenuMode::Instance()
{
    if (DGameOverMenuPointer == nullptr)
    {
        DGameOverMenuPointer =
            std::make_shared<CGameOverMenuMode>(SPrivateConstructorType());
    }
    return DGameOverMenuPointer;
}
