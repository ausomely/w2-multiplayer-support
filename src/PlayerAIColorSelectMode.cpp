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
#include <vector>
#include "PlayerAIColorSelectMode.h"
#include "ServerConnectMenuMode.h"
#include "ApplicationData.h"
#include "BattleMode.h"
#include "MainMenuMode.h"
#include "MapSelectionMode.h"
#include "MemoryDataSource.h"
#include "MultiPlayerOptionsMenuMode.h"
#include "Client.h"
#include "ChatOverlay.h"

std::shared_ptr<CPlayerAIColorSelectMode>
    CPlayerAIColorSelectMode::DPlayerAIColorSelectModePointer;

int CPlayerAIColorSelectMode::DCountdownTimer = 0;

CPlayerAIColorSelectMode::CPlayerAIColorSelectMode(
    const SPrivateConstructorType& key)
{

}

std::shared_ptr<CApplicationMode> CPlayerAIColorSelectMode::Instance()
{
    if (DPlayerAIColorSelectModePointer == nullptr)
    {
        DPlayerAIColorSelectModePointer =
        std::make_shared<CPlayerAIColorSelectMode>(
        SPrivateConstructorType());
    }
    return DPlayerAIColorSelectModePointer;
}

// Run the following code everytime this menu mode is entered
void CPlayerAIColorSelectMode::InitializeChange(
    std::shared_ptr<CApplicationData> context)
{
    DContext = context;
    DButtonHovered = false;
    DButtonFunctions.clear();
    DButtonLocations.clear();
    DButtonTexts.clear();
    DColorButtonLocations.clear();
    DPlayerTypeButtonLocations.clear();

    SetFontsAndColors();

    if (context->MultiPlayer())
    {
        if (nullptr == DChatOverlay)
        {
            DChatOverlay.reset(new CChatOverlay(context, EChatLocation::Lobby));
        }
        DChatOverlay->InitializeChat();
        context->Text.clear();
    }

    std::string CancelButtonText;
    switch (context->DGameSessionType)
    {
        case CApplicationData::gstSinglePlayer:
        {
            DTitle = "Select Colors / Difficulty";

            CancelButtonText = "Back";
            DButtonTexts.push_back("Play Game");
            DButtonFunctions.push_back(PlayGameButtonCallback);

        }
        break;
        case CApplicationData::gstMultiPlayerHost:
        {
            DTitle = "Select Team Colors / Player Types";

            CancelButtonText = "Cancel";
            DButtonTexts.push_back("Play Game");
            DButtonFunctions.push_back(MPHostPlayGameButtonCallback);

            // start updating roominfo
            context->ClientPointer->StartUpdateRoomInfo(context);
        }
        break;
        case CApplicationData::gstMultiPlayerClient:
        {
            DTitle = "Are You Ready?!?";
            CancelButtonText = "Leave Server";
            DButtonTexts.push_back("I'm Ready!");
            DButtonFunctions.push_back(MPClientReadyButtonCallback);

            // start updating roominfo
            context->ClientPointer->StartUpdateRoomInfo(context);

            context->DSelectedMapIndex = 0;
            context->DSelectedMap = CAssetDecoratedMap::DuplicateMap(0);
            context->DMapRenderer = std::make_shared<CMapRenderer>(
                std::make_shared<CMemoryDataSource>(
                    context->DMapRendererConfigurationData),
                context->DTerrainTileset, context->DSelectedMap);
            context->DAssetRenderer = std::make_shared<CAssetRenderer>(
                context->DAssetRecolorMap, context->DAssetTilesets,
                context->DMarkerTileset, context->DCorpseTileset,
                context->DFireTilesets, context->DBuildingDeathTileset,
                context->DArrowTileset, nullptr, context->DSelectedMap);
            context->DMiniMapRenderer = std::make_shared<CMiniMapRenderer>(
                context->DMapRenderer, context->DAssetRenderer, nullptr, nullptr,
                context->DDoubleBufferSurface->Format());

            *context->DSelectedMap =
                *CAssetDecoratedMap::GetMap(CAssetDecoratedMap::FindMapIndex(context->roomInfo.map()));
            context->DSelectedMapIndex = CAssetDecoratedMap::FindMapIndex(context->roomInfo.map());
        }
    }

    // Specify the cancel button text and callback function
    DButtonTexts.push_back(CancelButtonText);
    DButtonFunctions.push_back(CancelButtonCallback);

    int CanvasWidth = DContext->DWorkingBufferSurface->Width();
    int CanvasHeight = DContext->DWorkingBufferSurface->Height();
    DButtonStack.reset(new CVerticalButtonAlignment(DContext, DButtonTexts,
        EPosition::SouthEast, CanvasWidth, CanvasHeight));
    DButtonStack->SetWidth(DContext->DMiniMapSurface->Width());


}

// Play game button callback function for single player mode
void CPlayerAIColorSelectMode::PlayGameButtonCallback(
std::shared_ptr<CApplicationData> context)
{
    context->ChangeApplicationMode(CBattleMode::Instance());
}

void CPlayerAIColorSelectMode::MPHostPlayGameButtonCallback(
std::shared_ptr<CApplicationData> context)
{
    for(int Index = 2; Index <= context->roomInfo.capacity();Index++) {
        if(!context->roomInfo.ready(Index)) {
            // One player is not ready. Set countdown timer for warning message
            DCountdownTimer = 40;
            return;
        }
    }

    // send ready game start message to server
    context->ClientPointer->SendMessage("StartGame");
}

void CPlayerAIColorSelectMode::MPClientReadyButtonCallback(
std::shared_ptr<CApplicationData> context)
{
    // Toggle player ready status
    if (context->DReadyPlayers[to_underlying(context->DPlayerNumber)])
    {
        context->DReadyPlayers[to_underlying(context->DPlayerNumber)] = false;
    }
    else
    {
        context->DReadyPlayers[to_underlying(context->DPlayerNumber)] = true;
    }

    // send ready information
    for (int Index = 2; Index < to_underlying(EPlayerColor::Max); Index++)
    {
        context->roomInfo.set_ready(Index, context->DReadyPlayers[Index]);
    }
    context->ClientPointer->SendRoomInfo(context);
}


void CPlayerAIColorSelectMode::CancelButtonCallback(
std::shared_ptr<CApplicationData> context)
{
    if (CApplicationData::gstSinglePlayer != context->DGameSessionType)
    {
        context->roomList.Clear();
        context->roomInfo.Clear();
        // Notify game server
        context->ClientPointer->SendMessage("Leave");
        context->ClientPointer->io_service.run();
        context->ChangeApplicationMode(CMultiPlayerOptionsMenuMode::Instance());
    }
    else
    {
        context->ChangeApplicationMode(CMapSelectionMode::Instance());
    }
}

void CPlayerAIColorSelectMode::Input(std::shared_ptr<CApplicationData> context)
{
    // if you become the host!
    if(context->DPlayerNumber == EPlayerNumber::Player1 && context->MultiPlayer()
        && context->DGameSessionType == CApplicationData::gstMultiPlayerClient) {
        context->DGameSessionType = CApplicationData::gstMultiPlayerHost;
        DButtonHovered = false;
        DButtonFunctions.clear();
        DButtonTexts.clear();
        DTitle = "Select Team Colors / Player Types";
        std::string CancelButtonText = "Cancel";
        DButtonTexts.push_back("Play Game");
        DButtonFunctions.push_back(MPHostPlayGameButtonCallback);
        DButtonTexts.push_back(CancelButtonText);
        DButtonFunctions.push_back(CancelButtonCallback);
        int CanvasWidth = DContext->DWorkingBufferSurface->Width();
        int CanvasHeight = DContext->DWorkingBufferSurface->Height();
        DButtonStack.reset(new CVerticalButtonAlignment(DContext, DButtonTexts,
            EPosition::SouthEast, CanvasWidth, CanvasHeight));
        DButtonStack->SetWidth(DContext->DMiniMapSurface->Width());
    }

    // ready to start game!
    if(context->roomInfo.active()) {
        boost::asio::ip::tcp::no_delay option(true);
        context->ClientPointer->socket.set_option(option);
        context->ChangeApplicationMode(CBattleMode::Instance());
    }

    int CurrentX, CurrentY;

    // Get the X,Y coordinates of the pointer's position
    CurrentX = context->DCurrentX;
    CurrentY = context->DCurrentY;

    DPlayerNumberRequestingChange = EPlayerNumber::Neutral;
    DPlayerColorChangeRequest = EPlayerColor::None;
    DPlayerNumberRequestTypeChange = EPlayerNumber::Neutral;

    // True if left mouse button is clicked and it has not already been down
    if (context->DLeftClick && !context->DLeftDown)
    {
        if (CApplicationData::gstMultiPlayerClient != context->DGameSessionType)
        {
            // Iterate over all the colored button locations
            for (int Index = 0; Index < DColorButtonLocations.size(); Index++)
            {
                // Each index corresponds to an SRectangle object for a colored
                // button. The object can check if the pointer is within the bounds
                // of the button.
                if (DColorButtonLocations[Index].PointInside(CurrentX, CurrentY))
                {
                    int PlayerSelecting =
                        1 + (Index / (to_underlying(EPlayerNumber::Max) - 1));
                    int ColorSelecting =
                        1 + (Index % (to_underlying(EPlayerColor::Max) - 1));

                    // Coordinating selecting colors between all players while
                    // in a non-multiplayer session
                    if ((PlayerSelecting == to_underlying(context->DPlayerNumber)) ||
                        (CApplicationData::gstMultiPlayerClient !=
                         context->DGameSessionType))
                    {
                        /*
                        if ((PlayerSelecting ==
                             to_underlying(context->DPlayerNumber)) ||
                            (CApplicationData::ptHuman !=
                             context->DLoadingPlayerTypes[PlayerSelecting]))
                        {
                        */
                            // Track which player number is requesting the color change
                            DPlayerNumberRequestingChange =
                                static_cast<EPlayerNumber>(PlayerSelecting);

                            // Track the color change request
                            DPlayerColorChangeRequest =
                                static_cast<EPlayerColor>(ColorSelecting);
                        //}
                    }
                }
            }

            // Iterate over player type buttons, these can be the AI difficulty
            // level settings.
            for (int Index = 0; Index < DPlayerTypeButtonLocations.size(); Index++)
            {

                // If a pointer is over a button's location, then set a flag for
                // calculate() where the difficulty level is changed.
                // when a player has joiend it shouldn't change the type for that player
                if (DPlayerTypeButtonLocations[Index].PointInside(CurrentX, CurrentY) &&
                      (context->DLoadingPlayerTypes[Index + 2] != CApplicationData::ptHuman ||
                       (context->DLoadingPlayerTypes[Index + 2] == CApplicationData::ptHuman &&
                       context->DPlayerNames[Index + 2] == "None")))
                {
                    DPlayerNumberRequestTypeChange =
                        static_cast<EPlayerNumber>(Index + 2);
                    break;
                }
            }
        }
    }

    if (!context->DLeftDown && DButtonStack->ButtonPressedInStack())
    {
        DButtonFunctions[DButtonStack->ButtonPressedIndex()](context);
    }

    // Process chat text field
    if (context->MultiPlayer())
    {
        DChatOverlay->ProcessTextEntryFields(CurrentX, CurrentY, context->DLeftDown);
        DChatOverlay->ProcessKeyStrokes();
    }
}

void CPlayerAIColorSelectMode::Calculate(
    std::shared_ptr<CApplicationData> context)
{

    // Coordinate the color changes
    // True if the player requesting a change is not the non-playing neutral player
    if (EPlayerNumber::Neutral != DPlayerNumberRequestingChange)
    {
        // Assume at first that the new color requested is not assigned to a player
        EPlayerNumber NewColorInUseBy = EPlayerNumber::Neutral;

        // Iterate all over all the players
        for (int Index = 1; Index < to_underlying(EPlayerNumber::Max); Index++)
        {
            // True if the current player number index is not the player
            // number requesting the change
            if (Index != to_underlying(DPlayerNumberRequestingChange))
            {
                // True if the current player is playing the game
                if (CApplicationData::ptNone !=
                    context->DLoadingPlayerTypes[Index])
                {
                    // True if the color being changed to is already in use
                    // by the current player in the iteration
                    if (context->DLoadingPlayerColors[Index] ==
                        DPlayerColorChangeRequest)
                    {
                        // Current player number in the iteration is the
                        // one now using the color
                        NewColorInUseBy = static_cast<EPlayerNumber>(Index);
                        break;
                    }
                }
            }
        }

        // The following resolve the color selection conflict by swapping
        // to the previous color of the player number that requested the change
        // Only swap colors if the player number in conflict is actually playing
        if (EPlayerNumber::Neutral != NewColorInUseBy)
        {
            // Swap to color of the player number that requested the change
            context->DLoadingPlayerColors[to_underlying(NewColorInUseBy)] =
                context->DLoadingPlayerColors[to_underlying(
                    DPlayerNumberRequestingChange)];
        }

        // Set the new color for the player number requesting the change
        context->DLoadingPlayerColors[to_underlying(
            DPlayerNumberRequestingChange)] = DPlayerColorChangeRequest;

        // Register the color changes for all the players
        context->DAssetRenderer->UpdateLoadingPlayerColors(
            context->DLoadingPlayerColors);

        // Sync color change to server
        if (context->DGameSessionType == CApplicationData::gstMultiPlayerHost)
        {
            for (int Index = 1; Index < to_underlying(EPlayerColor::Max); Index++)
            {
                context->roomInfo.set_colors(Index, to_underlying(context->DLoadingPlayerColors[Index]));
            }
            context->ClientPointer->SendRoomInfo(context);
        }
    }

    // The following is to deal with the request to change the AI's difficulty
    // True if the type change is for an active player
    if (EPlayerNumber::Neutral != DPlayerNumberRequestTypeChange)
    {

        // True if this is a single player game
        if (CApplicationData::gstSinglePlayer == context->DGameSessionType)
        {
            // Based on current player type, set the next player type
            // according to the case's settings
            switch (context->DLoadingPlayerTypes[to_underlying(
                DPlayerNumberRequestTypeChange)])
            {
                // Current type is AIEasy, get set to AIMedium
                case CApplicationData::ptAIEasy:
                    context->DLoadingPlayerTypes[to_underlying(
                        DPlayerNumberRequestTypeChange)] =
                        CApplicationData::ptAIMedium;
                    break;

                // Current type is AIMedium, get set to AIHard
                case CApplicationData::ptAIMedium:
                    context->DLoadingPlayerTypes[to_underlying(
                        DPlayerNumberRequestTypeChange)] =
                        CApplicationData::ptAIHard;
                    break;

                // For all types that don't have a case, set the type to AIEasy
                default:
                    context->DLoadingPlayerTypes[to_underlying(
                        DPlayerNumberRequestTypeChange)] =
                        CApplicationData::ptAIEasy;
                    break;
            }
        }
        // Otherwise if this is a multiplayer game
        else if (CApplicationData::gstMultiPlayerHost ==
                 context->DGameSessionType)
        {
            // Based on current player type, set the next player type
            // according to the case's settings
            switch (context->DLoadingPlayerTypes[to_underlying(
                DPlayerNumberRequestTypeChange)])
            {
                // Current player type is human, set that player type to AIEasy
                case CApplicationData::ptHuman:
                    context->DLoadingPlayerTypes[to_underlying(
                        DPlayerNumberRequestTypeChange)] =
                        CApplicationData::ptAIEasy;
                    context->roomInfo.set_ready(to_underlying(DPlayerNumberRequestTypeChange),
                        true);
                    break;

                // Current type is AIEasy, get set to AIMedium
                case CApplicationData::ptAIEasy:
                    context->DLoadingPlayerTypes[to_underlying(
                        DPlayerNumberRequestTypeChange)] =
                        CApplicationData::ptAIMedium;
                    break;

                // Current type is AIMedium, get set to AIHard
                case CApplicationData::ptAIMedium:
                    context->DLoadingPlayerTypes[to_underlying(
                        DPlayerNumberRequestTypeChange)] =
                        CApplicationData::ptAIHard;
                    break;

                // Current type is AIHard, get set to None
                case CApplicationData::ptAIHard:
                    context->DLoadingPlayerTypes[to_underlying(
                        DPlayerNumberRequestTypeChange)] =
                        CApplicationData::ptNone;
                    context->roomInfo.set_capacity(context->roomInfo.capacity() - 1);
                    break;
                // For all types that don't have a case, set that player type
                // to a Human
                default:
                    context->DLoadingPlayerTypes[to_underlying(
                        DPlayerNumberRequestTypeChange)] =
                        CApplicationData::ptHuman;
                    context->roomInfo.set_capacity(context->roomInfo.capacity() + 1);
                    context->roomInfo.set_ready(to_underlying(DPlayerNumberRequestTypeChange),
                        false);
                    break;
            }

            // send over player types changes for a AI
            for (int Index = 2; Index <= context->DSelectedMap->PlayerCount(); Index++)
            {
                context->roomInfo.set_types(Index, to_underlying(context->DLoadingPlayerTypes[Index]));
            }
            context->ClientPointer->SendRoomInfo(context);
        }
    }
}

void CPlayerAIColorSelectMode::Render(std::shared_ptr<CApplicationData> context)
{
    // check the io_service event list to get update on room info
    if(CApplicationData::gstSinglePlayer != context->DGameSessionType) {
        context->ClientPointer->io_service.poll();
    }

    int CurrentX, CurrentY;
    int BufferWidth, BufferHeight;
    int TitleHeight;
    int TextWidth, TextHeight, MaxTextWidth;
    int ColumnWidth, RowHeight;
    int MiniMapWidth, MiniMapHeight, MiniMapCenter, MiniMapLeft;
    int TextTop, ButtonLeft, ButtonTop, AIButtonLeft, ColorButtonHeight;
    std::string TempString;
    std::array<std::string, to_underlying(EPlayerNumber::Max)> PlayerNames;
    CButtonRenderer::EButtonState ButtonState =
    CButtonRenderer::EButtonState::None;
    bool ButtonXAlign = false, ButtonHovered = false;

    // Get the current X,Y position of the pointer on the drawing surface
    CurrentX = context->DCurrentX;
    CurrentY = context->DCurrentY;

    // Sets the Y coordinate for TitleHeight
    // BufferWidth set to width of the drawing surface
    // BufferHeight set to height of the drawing surface
    context->RenderMenuTitle(DTitle, TitleHeight, BufferWidth, BufferHeight);

    // Get the width and the height of the mini map
    MiniMapWidth = context->DMiniMapSurface->Width();
    MiniMapHeight = context->DMiniMapSurface->Height();

    // Give the renderer the surface to draw the mini map onto
    context->DMiniMapRenderer->DrawMiniMap(context->DMiniMapSurface);

    // Get the X coordinate of the left edge of the mini map
    MiniMapLeft = BufferWidth - MiniMapWidth - context->DBorderWidth;

    // Draw the mini map onto its surface that is a part of the drawing surface
    context->DWorkingBufferSurface->Draw(
    context->DMiniMapSurface, MiniMapLeft,
    TitleHeight + context->DInnerBevel->Width(), -1, -1, 0, 0);

    // Draw bevel around the mini map
    context->DInnerBevel->DrawBevel(context->DWorkingBufferSurface, MiniMapLeft,
                                    TitleHeight + context->DInnerBevel->Width(),
                                    MiniMapWidth, MiniMapHeight);

    TextTop = TitleHeight + MiniMapHeight + context->DInnerBevel->Width() * 3;
    MiniMapCenter = MiniMapLeft + MiniMapWidth / 2;

    TempString =
    std::to_string(context->DSelectedMap->PlayerCount()) + " Players";
    context->DFonts[DLargeFontID]->MeasureText(TempString, TextWidth, TextHeight);
    DrawText(DContext, TempString, MiniMapCenter-TextWidth/2, TextTop, DWhiteColor);

    TextTop += DTextHeight;
    TempString = std::to_string(context->DSelectedMap->Width()) + " x " +
                 std::to_string(context->DSelectedMap->Height());
    context->DFonts[DLargeFontID]->MeasureText(TempString, TextWidth, TextHeight);
    DrawText(DContext, TempString, MiniMapCenter - 0.5*TextWidth, TextTop,
                     DWhiteColor);
    // Set Y coordinate for the top of current row to the bottom Y coordinate
    // of the title of that top, center of the screen
    TextTop = TitleHeight;
    TempString = "Player";
    DrawText(DContext, TempString, DContext->DBorderWidth, TextTop, DWhiteColor);

    TextTop += DTextHeight;
    context->DButtonRenderer->Text("AI Easy", true);
    ColorButtonHeight = context->DButtonRenderer->Height();
    RowHeight =
    context->DButtonRenderer->Height() + context->DInnerBevel->Width() * 2;
    if (RowHeight < DTextHeight)
    {
        RowHeight = DTextHeight;
    }
    context->DButtonRenderer->Text("X", true);
    context->DButtonRenderer->Height(ColorButtonHeight);
    ColumnWidth =
    context->DButtonRenderer->Width() + context->DInnerBevel->Width() * 2;
    MaxTextWidth = 0;
    for (int Index = 1; Index <= context->DSelectedMap->PlayerCount(); Index++)
    {
        if (Index == to_underlying(context->DPlayerNumber))
        {
            PlayerNames[Index] = TempString = std::to_string(Index) + ". You";
        }
        else if (CApplicationData::ptHuman !=
                 context->DLoadingPlayerTypes[Index] && CApplicationData::ptNone !=
                 context->DLoadingPlayerTypes[Index])
        {
            PlayerNames[Index] = TempString =
            std::to_string(Index) + ". AI";
        }
        else if (CApplicationData::ptHuman ==
                 context->DLoadingPlayerTypes[Index] && "None" != context->DPlayerNames[Index])
        {
            PlayerNames[Index] = TempString = std::to_string(Index) + ". " + context->DPlayerNames[Index];
        }
        context
        ->DFonts[DLargeFontID]
        ->MeasureText(TempString, TextWidth, TextHeight);
        if (MaxTextWidth < TextWidth)
        {
            MaxTextWidth = TextWidth;
        }
    }


    TempString = "Color";
    DrawText(DContext, TempString, DContext->DBorderWidth + MaxTextWidth +
       (ColumnWidth * (to_underlying(EPlayerColor::Max) + 1)) / 2 - TextWidth/2,
       TitleHeight, DWhiteColor);

    DColorButtonLocations.clear();

    // Draw rows of color buttons for each player in the game
    for (int Index = 1; Index <= context->DSelectedMap->PlayerCount(); Index++)
    {
        TempString = PlayerNames[Index];
        int Color;
        int PlayerNumber = to_underlying(context->DPlayerNumber);

        // First check is for multiplayer
        // If player has committed to the game, use activation color for name
        if (context->DReadyPlayers[Index] && context->DLoadingPlayerTypes[Index] ==
            CApplicationData::ptHuman && context->DPlayerNumber != EPlayerNumber::Player1)
        {
            Color = DRedColor;
        }
        // Otherwise, color the current player's name to stand out from others
        else if (Index == PlayerNumber)
        {
            Color = DGoldColor;
        }
        // Color all other players generically
        else
        {
            Color = DWhiteColor;
        }

        DrawText(DContext, TempString, DContext->DBorderWidth, TextTop, Color);

        // Iterate over all the possible player colors
        for (int ColorIndex = 1; ColorIndex < to_underlying(EPlayerColor::Max);
             ColorIndex++)
        {

            // Set X coordinate of left edge of button
            int ButtonLeft =
            context->DBorderWidth + MaxTextWidth + ColorIndex * ColumnWidth;

            // If the color is assigned to an active player, then mark the
            // button with an X
            context->DButtonRenderer->Text(
            to_underlying(context->DLoadingPlayerColors[Index]) ==
            ColorIndex
            ? "X"
            : "");

            // Set the color of the button the current player color number
            context->DButtonRenderer->ButtonColor(
            static_cast<EPlayerColor>(ColorIndex));

            // Draw the button starting at the X,Y coordinates ButtonLeft,TextTop
            // Initial button state is none
            context->DButtonRenderer->DrawButton(
            context->DWorkingBufferSurface, ButtonLeft, TextTop,
            CButtonRenderer::EButtonState::None);

            // SRectangle contains all the coordinates to make the shape
            // of the button.
            // Push button dimensions onto vector of button locations
            DColorButtonLocations.push_back(SRectangle(
            {ButtonLeft, TextTop, context->DButtonRenderer->Width(),
             context->DButtonRenderer->Height()}));

            // Set X coordinate of the AI difficulty button
            // This will continue to get pushed to the right until all the
            // color boxes have been drawn
            AIButtonLeft = ButtonLeft + ColumnWidth;
        }

        // Set the Y coordinate to the top of the next row down
        TextTop += RowHeight;
    }

    context->DButtonRenderer->ButtonColor(EPlayerColor::None);
    TempString = "AI Easy";
    context->DButtonRenderer->Text(TempString);
    context->DButtonRenderer->Width(context->DButtonRenderer->Width() * 3 / 2);

    TextTop = TitleHeight;
    TempString = "Difficulty";
    DrawText(DContext, TempString, AIButtonLeft + 0.5*
        (DContext->DButtonRenderer->Width() - TextWidth), TextTop, DWhiteColor);

    ButtonXAlign = false;
    if ((AIButtonLeft <= CurrentX) &&
        (AIButtonLeft + context->DButtonRenderer->Width() > CurrentX))
    {
        ButtonXAlign = true;
    }
    TextTop += RowHeight + TextHeight;
    DPlayerTypeButtonLocations.clear();
    for (int Index = 2; Index <= context->DSelectedMap->PlayerCount(); Index++)
    {
        switch (context->DLoadingPlayerTypes[Index])
        {
            case CApplicationData::ptHuman:
                context->DButtonRenderer->Text("Human");
                break;
            case CApplicationData::ptAIEasy:
                context->DButtonRenderer->Text("AI Easy");
                break;
            case CApplicationData::ptAIMedium:
                context->DButtonRenderer->Text("AI Medium");
                break;
            case CApplicationData::ptAIHard:
                context->DButtonRenderer->Text("AI Hard");
                break;
            default:
                context->DButtonRenderer->Text("Closed");
                break;
        }
        ButtonState = CButtonRenderer::EButtonState::None;
        if (ButtonXAlign)
        {
            if ((TextTop <= CurrentY) &&
                ((TextTop + context->DButtonRenderer->Height() > CurrentY)))
            {
                ButtonState = context->DLeftDown
                              ? CButtonRenderer::EButtonState::Pressed
                              : CButtonRenderer::EButtonState::Hover;
                ButtonHovered = true;
            }
        }
        context->DButtonRenderer->DrawButton(
        context->DWorkingBufferSurface, AIButtonLeft, TextTop, ButtonState);
        DPlayerTypeButtonLocations.push_back(SRectangle(
        {AIButtonLeft, TextTop, context->DButtonRenderer->Width(),
         context->DButtonRenderer->Height()}));

        TextTop += RowHeight;
    }

    ButtonLeft =
    BufferWidth - context->DButtonRenderer->Width() - context->DBorderWidth;

    // ButtonTop gets height of drawing surface minus 2.25 times the height
    // of button and minus the border width.
    // 2.25 button heights
    // Sets the Y coordinate position of top edge of button
    ButtonTop = BufferHeight - (context->DButtonRenderer->Height() * 9 / 4) -
                context->DBorderWidth;

    TempString = " ";
    switch (context->DGameSessionType)
    {
        case CApplicationData::gstMultiPlayerHost:
        {
            if (DCountdownTimer)
            {
                TempString = "No other players!";
                DCountdownTimer--;
            }
        }
        break;
        case CApplicationData::gstMultiPlayerClient:
        {
            if (context->DReadyPlayers[to_underlying(context->DPlayerNumber)]) {
                TempString = "Waiting for Host!";
            }
        }
    }

    context->DFonts[DLargeFontID]->MeasureText(TempString, TextWidth, TextHeight);
    int Xoffset = DButtonStack->Xoffset() + (DButtonStack->Width()-TextWidth)/2;
    int Yoffset = DButtonStack->Yoffset() - 2*TextHeight;
    DrawText(DContext, TempString, Xoffset, Yoffset, DGoldColor);

    DButtonStack->DrawStack(DContext->DWorkingBufferSurface, CurrentX, CurrentY,
       DContext->DLeftDown);

    // Draw the chat window
    if (context->MultiPlayer())
    {
        DChatOverlay->DrawChatText();
        DChatOverlay->DrawTextEntryField(context->DLoadingPlayerColors[to_underlying(context->DPlayerNumber)]);
        context->DWorkingBufferSurface->Draw(DChatOverlay->Surface(),
            DChatOverlay->Xoffset(), DChatOverlay->Yoffset(), -1, -1, 0, 0);
    }

    // If the button has not been previously hovered and it is currently hovered
    if (!DButtonHovered && DButtonStack->PointerHovering())
    {
        context->StartPlayingClip("tick");
    }

    // If the application mode is changing
    if (context->ModeIsChanging())
    {
        context->StartPlayingClip("place");
    }

    // Keep track of the button hovered state
    DButtonHovered = DButtonStack->PointerHovering();
}

void CPlayerAIColorSelectMode::DrawText(std::shared_ptr<CApplicationData> context,
                                      std::string text, int xpos, int ypos,
                                      int color)
{
    context->DFonts[DLargeFontID]->MeasureText(text, DTextWidth, DTextHeight);
    context->DFonts[DLargeFontID]->DrawTextWithShadow(
        context->DWorkingBufferSurface, xpos, ypos, color, DShadowColor, 1,
        text);
}

// Set font size ID and color values for fonts
void CPlayerAIColorSelectMode::SetFontsAndColors()
{
    DLargeFontID = to_underlying(CUnitDescriptionRenderer::EFontSize::Large);
    DGoldColor = DContext->DFonts[DLargeFontID]->FindColor("gold");
    DWhiteColor = DContext->DFonts[DLargeFontID]->FindColor("white");
    DShadowColor = DContext->DFonts[DLargeFontID]->FindColor("black");
    DRedColor = DContext->DFonts[DLargeFontID]->FindColor("red");
}
