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
#include "PlayerAIColorSelectMode.h"
#include "ApplicationData.h"
#include "BattleMode.h"
#include "MainMenuMode.h"
#include "MapSelectionMode.h"
#include "MemoryDataSource.h"
#include "MultiPlayerOptionsMenuMode.h"

std::shared_ptr<CPlayerAIColorSelectMode>
    CPlayerAIColorSelectMode::DPlayerAIColorSelectModePointer;

std::array<bool, to_underlying(EPlayerNumber::Max)>
    CPlayerAIColorSelectMode::DReadyPlayers;

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
    DButtonHovered = false;
    DButtonFunctions.clear();
    DButtonLocations.clear();
    DColorButtonLocations.clear();
    DPlayerTypeButtonLocations.clear();

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

            // Reset ready players array anytime this mode is entered
            DReadyPlayers.fill(false);
            // Neutral player and the host are always marked as ready
            DReadyPlayers[to_underlying(EPlayerNumber::Neutral)] = true;
            DReadyPlayers[to_underlying(context->DPlayerNumber)] = true;

        }
        break;
        case CApplicationData::gstMultiPlayerClient:
        {
            DTitle = "Are You Ready?!?";

            // Query game server for player number
            // context->DPlayerNumber = context->ClientPointer // something

            CancelButtonText = "Leave Server";
            DButtonTexts.push_back("I'm Ready!");
            DButtonFunctions.push_back(MPClientReadyButtonCallback);

        }
    }

    // Specify the cancel button text and callback function
    DButtonTexts.push_back(CancelButtonText);
    DButtonFunctions.push_back(CancelButtonCallback);

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
    // Get player count from game server
    int PlayerCount = 0; // set to 0 as a place holder
    if (2 >= PlayerCount)
    {
        // No other players. Set countdown timer for warning message
        DCountdownTimer = 40;
        return;
    }

    // Check for empty player spaces
    // Indices 0 and 1 are Neutral player and the host
    for (int Index = 2; Index < context->DSelectedMap->PlayerCount(); Index++)
    {
        if (false == DReadyPlayers[Index])
        {
            context->DLoadingPlayerTypes[Index] = CApplicationData::ptNone;
        }
    }

    // Testing exchanging commands
    context->ClientPointer->SendMessage("Test");
    // send player types and colors to game server

    context->ChangeApplicationMode(CBattleMode::Instance());
}

void CPlayerAIColorSelectMode::MPClientReadyButtonCallback(
std::shared_ptr<CApplicationData> context)
{
    // Toggle player ready status
    if (DReadyPlayers[to_underlying(context->DPlayerNumber)])
    {
        DReadyPlayers[to_underlying(context->DPlayerNumber)] = false;
    }
    else
    {
        DReadyPlayers[to_underlying(context->DPlayerNumber)] = true;
    }

}


void CPlayerAIColorSelectMode::CancelButtonCallback(
std::shared_ptr<CApplicationData> context)
{
    if (CApplicationData::gstMultiPlayerClient == context->DGameSessionType ||
        CApplicationData::gstMultiPlayerHost == context->DGameSessionType)
    {
        context->ClientPointer->SendMessage("Leave");
        context->ChangeApplicationMode(CMultiPlayerOptionsMenuMode::Instance());
    }
    else
    {
        context->ChangeApplicationMode(CMapSelectionMode::Instance());
    }
}

void CPlayerAIColorSelectMode::Input(std::shared_ptr<CApplicationData> context)
{
    int CurrentX, CurrentY;

    // Get the X,Y coordinates of the pointer's position
    CurrentX = context->DCurrentX;
    CurrentY = context->DCurrentY;

    DPlayerNumberRequestingChange = EPlayerNumber::Neutral;
    DPlayerColorChangeRequest = EPlayerColor::None;
    DPlayerNumberRequesTypeChange = EPlayerNumber::Neutral;

    // True if left mouse button is clicked and it has not already been down
    if (context->DLeftClick && !context->DLeftDown &&
        (CApplicationData::gstMultiPlayerClient != context->DGameSessionType))
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
                    if ((PlayerSelecting ==
                         to_underlying(context->DPlayerNumber)) ||
                        (CApplicationData::ptHuman !=
                         context->DLoadingPlayerTypes[PlayerSelecting]))
                    {
                        // Track which player number is requesting the color change
                        DPlayerNumberRequestingChange =
                            static_cast<EPlayerNumber>(PlayerSelecting);

                        // Track the color change request
                        DPlayerColorChangeRequest =
                            static_cast<EPlayerColor>(ColorSelecting);
                    }
                }
            }
        }

        // Iterate over buttons on bottom, right-hand corner of screen
        for (int Index = 0; Index < DButtonLocations.size(); Index++)
        {

            // If a pointer is over a button's location, then pass the game's
            // context to the registered callback function.
            if (DButtonLocations[Index].PointInside(CurrentX, CurrentY))
            {
                DButtonFunctions[Index](context);
            }
        }

        // Iterate over player type buttons, these can be the AI difficulty
        // level settings.
        for (int Index = 0; Index < DPlayerTypeButtonLocations.size(); Index++)
        {

            // If a pointer is over a button's location, then set a flag for
            // calculate() where the difficulty level is changed.
            if (DPlayerTypeButtonLocations[Index].PointInside(CurrentX,
                                                              CurrentY))
            {
                DPlayerNumberRequesTypeChange =
                    static_cast<EPlayerNumber>(Index + 2);
                break;
            }
        }
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
            // send data
        }
        else if (context->DGameSessionType == CApplicationData::gstMultiPlayerClient)
        {
            // get data
        }
    }

    // The following is to deal with the request to change the AI's difficulty
    // True if the type change is for an active player
    if (EPlayerNumber::Neutral != DPlayerNumberRequesTypeChange)
    {

        // True if this is a single player game
        if (CApplicationData::gstSinglePlayer == context->DGameSessionType)
        {
            // Based on current player type, set the next player type
            // according to the case's settings
            switch (context->DLoadingPlayerTypes[to_underlying(
                DPlayerNumberRequesTypeChange)])
            {
                // Current type is AIEasy, get set to AIMedium
                case CApplicationData::ptAIEasy:
                    context->DLoadingPlayerTypes[to_underlying(
                        DPlayerNumberRequesTypeChange)] =
                        CApplicationData::ptAIMedium;
                    break;

                // Current type is AIMedium, get set to AIHard
                case CApplicationData::ptAIMedium:
                    context->DLoadingPlayerTypes[to_underlying(
                        DPlayerNumberRequesTypeChange)] =
                        CApplicationData::ptAIHard;
                    break;

                // For all types that don't have a case, set the type to AIEasy
                default:
                    context->DLoadingPlayerTypes[to_underlying(
                        DPlayerNumberRequesTypeChange)] =
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
                DPlayerNumberRequesTypeChange)])
            {
                // Current player type is human, set that player type to AIEasy
                case CApplicationData::ptHuman:
                    context->DLoadingPlayerTypes[to_underlying(
                        DPlayerNumberRequesTypeChange)] =
                        CApplicationData::ptAIEasy;
                    break;

                // Current type is AIEasy, get set to AIMedium
                case CApplicationData::ptAIEasy:
                    context->DLoadingPlayerTypes[to_underlying(
                        DPlayerNumberRequesTypeChange)] =
                        CApplicationData::ptAIMedium;
                    break;

                // Current type is AIMedium, get set to AIHard
                case CApplicationData::ptAIMedium:
                    context->DLoadingPlayerTypes[to_underlying(
                        DPlayerNumberRequesTypeChange)] =
                        CApplicationData::ptAIHard;
                    break;

                // Current type is AIHard, get set to None
                case CApplicationData::ptAIHard:
                    context->DLoadingPlayerTypes[to_underlying(
                        DPlayerNumberRequesTypeChange)] =
                        CApplicationData::ptNone;
                    break;
                // For all types that don't have a case, set that player type
                // to a Human
                default:
                    context->DLoadingPlayerTypes[to_underlying(
                        DPlayerNumberRequesTypeChange)] =
                        CApplicationData::ptHuman;
                    break;
            }
        }
    }
}

void CPlayerAIColorSelectMode::Render(std::shared_ptr<CApplicationData> context)
{
    int CurrentX, CurrentY;
    int BufferWidth, BufferHeight;
    int TitleHeight;
    int TextWidth, TextHeight, MaxTextWidth;
    int ColumnWidth, RowHeight;
    int MiniMapWidth, MiniMapHeight, MiniMapCenter, MiniMapLeft;
    int TextTop, ButtonLeft, ButtonTop, AIButtonLeft, ColorButtonHeight;
    int GoldColor, WhiteColor, ShadowColor;
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

    GoldColor =
    context
    ->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]
    ->FindColor("gold");
    WhiteColor =
    context
    ->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]
    ->FindColor("white");
    ShadowColor =
    context
    ->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]
    ->FindColor("black");

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
    context->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]
    ->MeasureText(TempString, TextWidth, TextHeight);
    context->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]
    ->DrawTextWithShadow(context->DWorkingBufferSurface,
                         MiniMapCenter - TextWidth / 2, TextTop, WhiteColor,
                         ShadowColor, 1, TempString);

    TextTop += TextHeight;
    TempString = std::to_string(context->DSelectedMap->Width()) + " x " +
                 std::to_string(context->DSelectedMap->Height());
    context->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]
    ->MeasureText(TempString, TextWidth, TextHeight);
    context->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]
    ->DrawTextWithShadow(context->DWorkingBufferSurface,
                         MiniMapCenter - TextWidth / 2, TextTop, WhiteColor,
                         ShadowColor, 1, TempString);

    // Set Y coordinate for the top of current row to the bottom Y coordinate
    // of the title of that top, center of the screen
    TextTop = TitleHeight;
    TempString = "Player";
    context->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]
    ->MeasureText(TempString, TextWidth, TextHeight);
    context->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]
    ->DrawTextWithShadow(context->DWorkingBufferSurface,
                         context->DBorderWidth, TextTop, WhiteColor,
                         ShadowColor, 1, TempString);

    TextTop += TextHeight;
    context->DButtonRenderer->Text("AI Easy", true);
    ColorButtonHeight = context->DButtonRenderer->Height();
    RowHeight =
    context->DButtonRenderer->Height() + context->DInnerBevel->Width() * 2;
    if (RowHeight < TextHeight)
    {
        RowHeight = TextHeight;
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
                 context->DLoadingPlayerTypes[Index])
        {
            PlayerNames[Index] = TempString =
            std::to_string(Index) + ". Player " + std::to_string(Index);
        }
        context
        ->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]
        ->MeasureText(TempString, TextWidth, TextHeight);
        if (MaxTextWidth < TextWidth)
        {
            MaxTextWidth = TextWidth;
        }
    }


    TempString = "Color";
    context->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]
    ->MeasureText(TempString, TextWidth, TextHeight);
    context->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]
    ->DrawTextWithShadow(
    context->DWorkingBufferSurface,
    context->DBorderWidth + MaxTextWidth +
    (ColumnWidth * (to_underlying(EPlayerColor::Max) + 1)) / 2 -
    TextWidth / 2,
    TitleHeight, WhiteColor, ShadowColor, 1, TempString);


    DColorButtonLocations.clear();

    // Draw rows of color buttons for each player in the game
    for (int Index = 1; Index <= context->DSelectedMap->PlayerCount(); Index++)
    {
        TempString = PlayerNames[Index];
        context
        ->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]
        ->MeasureText(TempString, TextWidth, TextHeight);
        context
        ->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]
        ->DrawTextWithShadow(
        context->DWorkingBufferSurface, context->DBorderWidth, TextTop,
        Index == to_underlying(context->DPlayerNumber) ? GoldColor
                                                       : WhiteColor,
        ShadowColor, 1, TempString);

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
    context->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]
    ->MeasureText(TempString, TextWidth, TextHeight);
    context->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]
    ->DrawTextWithShadow(
    context->DWorkingBufferSurface,
    AIButtonLeft + (context->DButtonRenderer->Width() - TextWidth) / 2,
    TextTop, WhiteColor, ShadowColor, 1, TempString);

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

    DButtonLocations.clear();

    // Following is setting attributes of the buttons on the bottom, right-hand
    // side of the window.

    context->DButtonRenderer->ButtonColor(EPlayerColor::None);

    // Read in button text for Play Game button
    context->DButtonRenderer->Text(DButtonTexts[0], true);
    // Set height of the button
    context->DButtonRenderer->Height(context->DButtonRenderer->Height() * 3/2);
    // Set width of the button
    context->DButtonRenderer->Width(MiniMapWidth);
    // ButtonLeft gets width of drawing surface minus width of button and minus
    // the border width.
    // Sets the X coordinate position of left edge of button
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
            if (DReadyPlayers[to_underlying(context->DPlayerNumber)]) {
                TempString = "Waiting for Host!";
            }
        }
    }

    context
    ->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]
    ->MeasureText(TempString, TextWidth, TextHeight);
    context
    ->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]
    ->DrawTextWithShadow(
    context->DWorkingBufferSurface, ButtonLeft + 7, ButtonTop - 50,
    GoldColor, ShadowColor, 1, TempString);


    // Set initial button state
    ButtonState = CButtonRenderer::EButtonState::None;

    // Check if X position of pointer is within X coordinate range of button
    if ((ButtonLeft <= CurrentX) &&
        (ButtonLeft + context->DButtonRenderer->Width() > CurrentX))
    {
        ButtonXAlign = true;
    }

    // If pointer is within X coordinate range of button
    if (ButtonXAlign)
    {
        // Check if Y position of pointer is within Y coordinate range of button
        if ((ButtonTop <= CurrentY) &&
            ((ButtonTop + context->DButtonRenderer->Height() > CurrentY)))
        {
            // Set to pressed if left mouse button down
            // Otherwise the pointer is hovering, so set to hover
            ButtonState = context->DLeftDown
                          ? CButtonRenderer::EButtonState::Pressed
                          : CButtonRenderer::EButtonState::Hover;
            ButtonHovered = true;
        }
    }

    // Draw the button with colors correlating to the button state
    // Point of origin for the button is (ButtonLeft, ButtonTop)
    context->DButtonRenderer->DrawButton(context->DWorkingBufferSurface,
                                         ButtonLeft, ButtonTop, ButtonState);
    // SRectangle contains all the coordinates to make the shape
    // of the button.
    // Push button dimensions onto vector of button locations
    DButtonLocations.push_back(
    SRectangle({ButtonLeft, ButtonTop, context->DButtonRenderer->Width(),
                context->DButtonRenderer->Height()}));

    // Set Y position of top edge of button
    ButtonTop = BufferHeight - context->DButtonRenderer->Height() -
                context->DBorderWidth;

    // NOTE: X position set previously in ButtonLeft will be reused

    // Clear button state
    ButtonState = CButtonRenderer::EButtonState::None;

    // Using value from check of button placed directly above this one
    // True if the pointer is within the X coordinate range of the button
    if (ButtonXAlign)
    {
        // True if the pointer is within the Y coordinate range of the button
        if ((ButtonTop <= CurrentY) &&
            ((ButtonTop + context->DButtonRenderer->Height() > CurrentY)))
        {
            // Set to pressed if left mouse button down
            // Otherwise the pointer is hovering, so set to hover
            ButtonState = context->DLeftDown
                          ? CButtonRenderer::EButtonState::Pressed
                          : CButtonRenderer::EButtonState::Hover;
            ButtonHovered = true;
        }
    }

    // Set attributes for the next button
    context->DButtonRenderer->Text(DButtonTexts[1], false);

    // Draw the button with colors correlating to the button state
    // Point of origin for the button is (ButtonLeft, ButtonTop)
    context->DButtonRenderer->DrawButton(context->DWorkingBufferSurface,
                                         ButtonLeft, ButtonTop, ButtonState);

    // SRectangle contains all the coordinates to make the shape
    // of the button.
    // Push button dimensions onto vector of button locations
    DButtonLocations.push_back(
    SRectangle({ButtonLeft, ButtonTop, context->DButtonRenderer->Width(),
                context->DButtonRenderer->Height()}));

    // If the button has not been previously hovered and it is currently hovered
    if (!DButtonHovered && ButtonHovered)
    {
        // Play a sound effect for the new hover event
        context->DSoundLibraryMixer->PlayClip(
        context->DSoundLibraryMixer->FindClip("tick"),
        context->DSoundVolume, 0.0);
    }

    // If the application mode is changing
    if (context->ModeIsChanging())
    {
        // Play a sound effect for the change in mode
        context->DSoundLibraryMixer->PlayClip(
        context->DSoundLibraryMixer->FindClip("place"),
        context->DSoundVolume, 0.0);
    }

    // Keep track of the button hovered state
    DButtonHovered = ButtonHovered;
}
