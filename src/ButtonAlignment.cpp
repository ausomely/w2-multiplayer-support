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
#include "ApplicationData.h"
#include "ButtonAlignment.h"
#include "ButtonRenderer.h"
#include "GraphicSurface.h"
#include "Button.h"
#include "Debug.h"

CButtonAlignment::CButtonAlignment(EPosition placement)
{
    DPlacement = placement;
}

void CButtonAlignment::DrawStack(int x, int y, bool clicked)
{
    DButtonHovered = false;
    DButtonPressedInStack = false;
    DContext->DButtonRenderer->SetBaseDimensions();

    for (auto &Button : DButtons)
    {
        if (Button->Update(x, y, clicked))
        {
            DButtonHovered = true;
        }
        DContext->DButtonRenderer->Text(Button->Text(), false);
        DContext->DButtonRenderer->Width(DMaxWidth);
        DContext->DButtonRenderer->Height(DMaxHeight);
        DContext->DButtonRenderer->DrawButton(DContext->DOverlaySurface,
            Button->XPosition(), Button->YPosition(), Button->State());

        if (Button->Pressed())
        {
            DButtonPressedInStack = true;
        }
    }
}

// Return stack index of button pressed, or sentinel value if none are pressed
int CButtonAlignment::ButtonPressedIndex()
{
    if (ButtonPressedInStack())
    {
        for (size_t Index = 0; Index < DButtons.size(); Index++)
        {
            if (DButtons[Index]->Pressed())
            {
                return Index;
            }
        }
    }

    return -1;
}

bool CButtonAlignment::PointerHovering()
{
    return DButtonHovered;
}
