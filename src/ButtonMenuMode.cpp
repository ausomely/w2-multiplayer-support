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
#include "ButtonMenuMode.h"
#include "ApplicationData.h"
#include "Debug.h"
#include "ButtonAlignment.h"
#include "VerticalButtonAlignment.h"

CButtonMenuMode::CButtonMenuMode()
{
    DButtonHovered = false;
}

//! @brief Handles keyboard and mouse input while the game is running
void CButtonMenuMode::Input(std::shared_ptr<CApplicationData> context)
{
    int CurrentX = context->DCurrentX;
    int CurrentY = context->DCurrentY;

    if (nullptr != DButtonStack)
    {
        if (!context->DLeftDown && DButtonStack->ButtonPressedInStack())
        {
            DButtonFunctions[DButtonStack->ButtonPressedIndex()](context);
        }
    }
}

void CButtonMenuMode::Calculate(std::shared_ptr<CApplicationData> context) {}

//! @brief Handles displaying of the buttons
void CButtonMenuMode::Render(std::shared_ptr<CApplicationData> context)
{
    int CurrentX = context->DCurrentX;
    int CurrentY = context->DCurrentY;
    bool LeftDown = context->DLeftDown;
    auto Surface = context->DWorkingBufferSurface;
    int TitleHeight, BufferWidth, BufferHeight;

    context->RenderMenuTitle(DTitle, TitleHeight, BufferWidth, BufferHeight);

    if (nullptr != DButtonStack)
    {
        DButtonStack->DrawStack(Surface, CurrentX, CurrentY, LeftDown);
    }

    if (nullptr != DButtonStack &&
        (!DButtonHovered && DButtonStack->PointerHovering()))
    {
        context->StartPlayingClip("tick");
    }

    if (context->ModeIsChanging())
    {
        context->StartPlayingClip("place");
    }

    DButtonHovered = DButtonStack->PointerHovering();
}
