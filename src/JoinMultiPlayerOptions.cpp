#include "JoinMultiPlayerOptions.h"
#include <MainMenuMode.h>
#include "ApplicationData.h"
#include "BattleMode.h"
#include "MainMenuMode.h"
#include "MemoryDataSource.h"
#include "MultiPlayerOptionsMenuMode.h"
#include "ServerConnectMenuMode.h"
#include "SoundOptionsMode.h"
#include "Client.h"

std::shared_ptr<CApplicationMode>
    CJoinMultiPlayerOptions::DJoinMultiPlayerOptionsPointer;

CJoinMultiPlayerOptions::CJoinMultiPlayerOptions(
    const SPrivateConstructorType &key)
{
    DTitle = "Join Multiplayer-game Options Menu";
    DButtonTexts.push_back("Direct Connetion");
    DButtonFunctions.push_back(DirectConnectionButtonCallback);
    DButtonTexts.push_back("ServerConnection");
    DButtonFunctions.push_back(ServerConnectionButtonCallback);
    DButtonTexts.push_back("");
    DButtonFunctions.push_back(nullptr);
    DButtonTexts.push_back("Back");
    DButtonFunctions.push_back(MultiPlayerGameButtonCallback);
}

//! @brief Opens Join Multiplayer-game menu
void CJoinMultiPlayerOptions::DirectConnectionButtonCallback(
    std::shared_ptr<CApplicationData> context)
{
}

//! @brief Opens server connection options
void CJoinMultiPlayerOptions::ServerConnectionButtonCallback(
    std::shared_ptr<CApplicationData> context)
{
    context->ClientPointer->SendMessage("Join");
    context->ChangeApplicationMode(CServerConnectMenuMode::Instance());
}

//! @brief Returns to Multiplayer options
void CJoinMultiPlayerOptions::MultiPlayerGameButtonCallback(
    std::shared_ptr<CApplicationData> context)
{
    context->ChangeApplicationMode(CMultiPlayerOptionsMenuMode::Instance());
}

// //! @brief Starts song for the menu
//void CInGameMenuMode::InitializeChange(std::shared_ptr<CApplicationData> context)
//{
//    context->DSoundLibraryMixer->StopSong();
//    context->DSoundLibraryMixer->PlaySong(
//            context->DSoundLibraryMixer->FindSong("menu"), context->DMusicVolume);
//}

std::shared_ptr<CApplicationMode> CJoinMultiPlayerOptions::Instance()
{
    if (DJoinMultiPlayerOptionsPointer == nullptr)
    {
        DJoinMultiPlayerOptionsPointer =
            std::make_shared<CJoinMultiPlayerOptions>(
                SPrivateConstructorType());
    }
    return DJoinMultiPlayerOptionsPointer;
}
