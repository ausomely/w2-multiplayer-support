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

#include "Button.h"
#include "ButtonRenderer.h"
#include "Debug.h"
#include "Rectangle.h"

CButton::CButton(std::string &text, int x, int y, int width, int height)
    : DShape(new SRectangle({x, y, width, height}))
{
    DState = CButtonRenderer::EButtonState::None;
    DText = text;
}


// Update the button's state given the current x, y, and clicked values
bool CButton::Update(int x, int y, bool clicked)
{
    // Check if pointer is within button coordinates
    if (DShape->PointInside(x, y))
    {
        // Check if the button is clicked
        if (clicked)
        {
            State(CButtonRenderer::EButtonState::Pressed);
        }
        // Button not clicked, so the pointer is hovering
        else
        {
            State(CButtonRenderer::EButtonState::Hover);
        }

        // Signal the pointer is over the button
        return true;
    }
    // Current pointer location is outside the button
    // Reset the button's state
    else if (CButtonRenderer::EButtonState::Pressed == State() ||
        CButtonRenderer::EButtonState::Hover == State())
    {
        State(CButtonRenderer::EButtonState::None);
    }

    // Signal the pointer is not over the button
    return false;
}