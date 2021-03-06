#include "GameOverMenuMode.h"
#include "ApplicationData.h"
#include "MainMenuMode.h"
#include "PlayerAIColorSelectMode.h"
#include "MultiPlayerOptionsMenuMode.h"
#include "MemoryDataSource.h"

std::shared_ptr<CApplicationMode> CGameOverMenuMode::DGameOverMenuPointer;

CGameOverMenuMode::CGameOverMenuMode(const SPrivateConstructorType &key)
{
    DTitle = "Game over";
    DButtonTexts.push_back("Leave Room");
    DButtonFunctions.push_back(LeaveRoomButtonCallback);
    DButtonTexts.push_back("Return Room");
    DButtonFunctions.push_back(ReturnRoomButtonCallback);
    DButtonTexts.push_back("Exit Game");
    DButtonFunctions.push_back(ExitGameButtonCallback);
}

void CGameOverMenuMode::InitializeChange(
            std::shared_ptr<CApplicationData> context)
{
    int CanvasWidth = context->DWorkingBufferSurface->Width();
    int CanvasHeight = context->DWorkingBufferSurface->Height();

    DButtonStack.reset(new CVerticalButtonAlignment(context, DButtonTexts,
        EPosition::Center, CanvasWidth, CanvasHeight));
}

//! @brief Return to Main menu of the game
void CGameOverMenuMode::LeaveRoomButtonCallback(
    std::shared_ptr<CApplicationData> context)
{
    context->DSoundLibraryMixer->StopSong();
    context->DSoundLibraryMixer->PlaySong(
        context->DSoundLibraryMixer->FindSong("menu"), context->DMusicVolume);

    // set log out message
    if(context->DGameSessionType != CApplicationData::gstSinglePlayer) {
        context->ClientPointer->SendMessage("Leave");
        context->roomInfo.Clear();
        context->ChangeApplicationMode(CMultiPlayerOptionsMenuMode::Instance());
    }

    else {
        context->ChangeApplicationMode(CMainMenuMode::Instance());
    }
}

//! @brief return to the same room
void CGameOverMenuMode::ReturnRoomButtonCallback(
    std::shared_ptr<CApplicationData> context)
{
    context->DSoundLibraryMixer->StopSong();
    context->DSoundLibraryMixer->PlaySong(
        context->DSoundLibraryMixer->FindSong("menu"), context->DMusicVolume);

    // go back to in room session
    if(context->DGameSessionType != CApplicationData::gstSinglePlayer) {
        context->ClientPointer->SendMessage("Back");
        context->ClientPointer->GetRoomInfo(context);
    }
    context->ChangeApplicationMode(CPlayerAIColorSelectMode::Instance());
}

//! @brief Closes the game
void CGameOverMenuMode::ExitGameButtonCallback(
    std::shared_ptr<CApplicationData> context)
{
    // set log out message
    if(context->DGameSessionType != CApplicationData::gstSinglePlayer) {
        context->ClientPointer->SendMessage("Exit");
    }
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
