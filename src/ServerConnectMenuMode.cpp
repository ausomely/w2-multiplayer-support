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
#include "ServerConnectMenuMode.h"
#include "ApplicationData.h"
#include "ButtonRenderer.h"
#include "JoinMultiPlayerOptions.h"
#include "MainMenuMode.h"
#include "MemoryDataSource.h"
#include "PlayerAIColorSelectMode.h"
#include "Rectangle.h"

std::shared_ptr<CServerConnectMenuMode>
    CServerConnectMenuMode::DServerConnectMenuModePointer;

CServerConnectMenuMode::CServerConnectMenuMode(
    const SPrivateConstructorType& key)
{
    DTitle = "Servers";

    DButtonTexts.push_back("Back");
    DButtonFunctions.push_back(BackButtonCallback);
}

std::shared_ptr<CApplicationMode> CServerConnectMenuMode::Instance()
{
    if (DServerConnectMenuModePointer == nullptr)
    {
        DServerConnectMenuModePointer =
            std::make_shared<CServerConnectMenuMode>(SPrivateConstructorType());
    }
    return DServerConnectMenuModePointer;
}

//! @brief Back to multi player options
void CServerConnectMenuMode::BackButtonCallback(
    std::shared_ptr<CApplicationData> context)
{
    context->ClientPointer->SendMessage("Back");
    context->ClientPointer->io_service.run();

    context->ChangeApplicationMode(CJoinMultiPlayerOptions::Instance());
}

// join button call back
void CServerConnectMenuMode::JoinButtonCallback(
    std::shared_ptr<CApplicationData> context)
{
    // cannot join when room is full
    if(context->roomList.roominfo()[context->DSelectedRoomNumber].size() !=
        context->roomList.roominfo()[context->DSelectedRoomNumber].capacity()) {
        context->ClientPointer->SendMessage(std::to_string(context->DSelectedRoomNumber));
        context->ClientPointer->io_service.run();

        context->roomInfo = context->roomList.roominfo()[context->DSelectedRoomNumber];
        // set DPlayerNumber
        context->DPlayerNumber = static_cast <EPlayerNumber> (context->roomInfo.size() + 1);

        context->ChangeApplicationMode(CPlayerAIColorSelectMode::Instance());
    }
}

// TODO: Complete this later
void CServerConnectMenuMode::Input(std::shared_ptr<CApplicationData> context) {
    CurrentX = context->DCurrentX;
    CurrentY = context->DCurrentY;

    // True if left mouse button is clicked and it was not already down
    if (context->DLeftClick && !context->DLeftDown)
    {
        // Iterate over join buttons
        for (int Index = 0; Index < DJoinButtonLocations.size(); Index++)
        {
            // Each index corresponds to an SRectangle object for a colored
            // button. The object can check if the pointer is within the bounds
            // of the button.
            if (DJoinButtonLocations[Index].PointInside(CurrentX, CurrentY))
            {
                // MLH: Need to wire up join buttons to a callback function
                context->DSelectedRoomNumber = Index;

                DJoinButtonFunctions[Index](context);
            }
        }

        // Handle Back button. Using index zero because there's only one button.
        if (DButtonLocations[0].PointInside(CurrentX, CurrentY))
        {
            DButtonFunctions[0](context);
        }
    }

    context->ClientPointer->io_service.poll();
}

void CServerConnectMenuMode::Calculate(std::shared_ptr<CApplicationData> context)
{
}

void CServerConnectMenuMode::InitializeChange(
    std::shared_ptr<CApplicationData> context)
{
    DButtonHovered = false;
    DButtonLocations.clear();
    DJoinButtonLocations.clear();
    DPlayerTypeButtonLocations.clear();
    context->roomList.Clear();

    // start updating room list
    context->ClientPointer->StartUpdateRoomList(context);
}

// Handle rendering of the game server information and join buttons
void CServerConnectMenuMode::Render(std::shared_ptr<CApplicationData> context)
{
    // clear vector first !
    DJoinButtonLocations.clear();
    DJoinButtonFunctions.clear();

    CButtonRenderer::EButtonState ButtonState;
    std::string TempString;

    LargeFontSize = to_underlying(CUnitDescriptionRenderer::EFontSize::Large);

    int XPosCol1 = context->DBorderWidth;
    int XPosCol2 = XPosCol1 + 125;
    int XPosCol3 = XPosCol2 + 150;
    int XPosCol4 = XPosCol3 + 275;
    int XPosCol5 = XPosCol4 + 75;

    int JoinButtonWidth = 50;
    int JoinButtonHeight = 30;


    int YOffsetFromTitle = 50;
    int YOffsetDataRows = 50;
    int YPosColumnHeader = TitleHeight + YOffsetFromTitle;
    int YPosRowStart = YPosColumnHeader + YOffsetDataRows;

    CurrentX = context->DCurrentX;
    CurrentY = context->DCurrentY;

    // Renders title on top
    context->RenderMenuTitle(DTitle, TitleHeight, BufferWidth, BufferHeight);

    GoldColor = context->DFonts[LargeFontSize]->FindColor("gold");
    WhiteColor = context->DFonts[LargeFontSize]->FindColor("white");
    ShadowColor = context->DFonts[LargeFontSize]->FindColor("black");

    // Hard coded Heading column
    TempString = "Game";
    DrawText(context, TempString, XPosCol1, YPosColumnHeader, WhiteColor);

    TempString = "Owner";
    DrawText(context, TempString, XPosCol2, YPosColumnHeader, WhiteColor);

    TempString = "Map";
    DrawText(context, TempString, XPosCol3, YPosColumnHeader, WhiteColor);

    TempString = "Players";
    DrawText(context, TempString, XPosCol4, YPosColumnHeader, WhiteColor);

    // Each game server gets a row of info and a join button
    // Need some way to get the total number of game servers
    int game_servers = context->roomList.roominfo().size();
    for (int i = 0, YPosDataRow = YPosRowStart; i < game_servers; i++)
    {
        std::string num_str = std::to_string(i + 1);
        std::string game = "Game ";
        DrawText(context, game + num_str, XPosCol1, YPosDataRow,
                 GoldColor);

        DrawText(context, context->roomList.roominfo()[i].host(), XPosCol2, YPosDataRow,
                 GoldColor);

        DrawText(context, context->roomList.roominfo()[i].map(), XPosCol3, YPosDataRow,
                 GoldColor);
        std::string player_count = std::to_string(context->roomList.roominfo()[i].size()) + " / "
                 + std::to_string(context->roomList.roominfo()[i].capacity());
        DrawText(context, player_count, XPosCol4, YPosDataRow,
                 GoldColor);

        context->DButtonRenderer->Text("Join", true);
        context->DButtonRenderer->Height(JoinButtonHeight);
        context->DButtonRenderer->Width(JoinButtonWidth);

        // If pointer is within X and Y coordinate range of button
        ButtonState = CButtonRenderer::EButtonState::None;
        if ((YPosDataRow <= CurrentY) &&
            (YPosDataRow + JoinButtonHeight > CurrentY) &&
            (XPosCol5 <= CurrentX) &&
            (XPosCol5 + JoinButtonWidth > CurrentX))
        {
            // Set to pressed if left mouse button down
            // Otherwise the pointer is hovering
            ButtonState = context->DLeftDown
                                ? CButtonRenderer::EButtonState::Pressed
                                : CButtonRenderer::EButtonState::Hover;
            ButtonHovered = true;
        }

        // Draw the button on the screen
        context->DButtonRenderer->DrawButton(context->DWorkingBufferSurface,
                                             XPosCol5, YPosDataRow,
                                             ButtonState);

        // SRectangle contains all the coordinates to make the shape
        // of the button.
        // Push button dimensions onto vector of button locations
        DJoinButtonLocations.push_back(
            SRectangle({XPosCol5, YPosDataRow, JoinButtonWidth, JoinButtonHeight}));
        DJoinButtonFunctions.push_back(JoinButtonCallback);

        // Set Y coordinate of next row of data
        YPosDataRow += YOffsetDataRows;

    }

    DButtonLocations.clear();

    // Renders the Back button on the screen
    RenderBackButton(context, ButtonState);

    // If the button has not been previously hovered and it's currently hovered
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

void CServerConnectMenuMode::DrawText(std::shared_ptr<CApplicationData> context,
                                      std::string text, int xpos, int ypos,
                                      int color)
{
    context->DFonts[LargeFontSize]->MeasureText(text, TextWidth, TextHeight);
    context->DFonts[LargeFontSize]->DrawTextWithShadow(
        context->DWorkingBufferSurface, xpos, ypos, color, ShadowColor, 1,
        text);
}

void CServerConnectMenuMode::RenderBackButton(std::shared_ptr<CApplicationData> context,
                                            CButtonRenderer::EButtonState ButtonState)
{
    // USEFULL NOTE: Always put the hover button code then Draw the button

    context->DButtonRenderer->ButtonColor(EPlayerColor::None);
    context->DButtonRenderer->Text(DButtonTexts[0], false);
    context->DButtonRenderer->Width(80);
    context->DButtonRenderer->Height(35);

    BackButtonLeft =
        BufferWidth - context->DButtonRenderer->Width() - context->DBorderWidth;

    BackButtonTop =
        BufferHeight - context->DButtonRenderer->Height() - context->DBorderWidth;

    // If pointer is within X and Y coordinate range of button
    ButtonState = CButtonRenderer::EButtonState::None;
    if ((BackButtonLeft <= CurrentX) &&
        (BackButtonLeft + context->DButtonRenderer->Width() > CurrentX)
        && (BackButtonTop <= CurrentY)
        && ((BackButtonTop + context->DButtonRenderer->Height() > CurrentY)))
    {
        // Set to pressed if left mouse button down
        // Otherwise the pointer is hovering, so set to hover
        ButtonState =
                context->DLeftDown ? CButtonRenderer::EButtonState::Pressed : CButtonRenderer::EButtonState::Hover;
            ButtonHovered = true;
    }

    // Draw the button with colors correlating to the button state
    // Point of origin for the button is (ButtonLeft, ButtonTop)
    context->DButtonRenderer->DrawButton(context->DWorkingBufferSurface,
                                         BackButtonLeft, BackButtonTop, ButtonState);
    // SRectangle contains all the coordinates to make the shape
    // of the button.
    // Push button dimensions onto vector of button locations
    DButtonLocations.push_back(
    SRectangle({BackButtonLeft, BackButtonTop, context->DButtonRenderer->Width(),
                context->DButtonRenderer->Height()}));
}
