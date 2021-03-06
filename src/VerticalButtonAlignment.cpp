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
#include "Button.h"
#include "VerticalButtonAlignment.h"
#include "Debug.h"

CVerticalButtonAlignment::CVerticalButtonAlignment(
    std::shared_ptr<CApplicationData> context, std::vector<std::string> texts,
    EPosition placement, int width, int height)
    : CButtonAlignment(context, placement)
{
    DButtons.reserve(texts.size());

    DButtonGap = 12;

    DCanvasWidth = width;
    DCanvasHeight = height;
    DMaxHeight = 0;
    DMaxWidth = 0;

    MeasureButtons(texts);

    // Proportionally bump up button sizes
    DMaxWidth = DStackWidth = DMaxWidth * 5 / 4;
    DMaxHeight = DMaxHeight * 3 / 2;
    DButRen->Width(DMaxWidth);
    DButRen->Height(DMaxHeight);

    FindXOffset(DCanvasWidth);
    FindYOffset(DCanvasHeight);
    CreateButtons(texts);

}

// Based on canvas placement, find the X offset of vertically aligned buttons
void CVerticalButtonAlignment::FindXOffset(int Width)
{
    switch (DPlacement)
    {
        // Middle of canvas
        case EPosition::Center:
        case EPosition::North:
        case EPosition::South:
        {
            DXOffset = static_cast<int>(.5 * Width - 0.5 * DMaxWidth);
        }
            break;

            // Eastern side of canvas
        case EPosition::NorthEast:
        case EPosition::East:
        case EPosition::SouthEast:
        {
            DXOffset = Width - DMaxWidth - DContext->DBorderWidth;
        }
            break;

            // Western side of canvas
        default:
        {
            DXOffset = DContext->DBorderWidth;
        }
    }
}

// Find Y offset of top most button in the group
void CVerticalButtonAlignment::FindYOffset(int Height)
{
    size_t StackHeight;
    size_t NumberOfButtons = DButtons.capacity();

    // Find stack height the buttons
    if (1 < NumberOfButtons)
    {
        DStackHeight =
            NumberOfButtons * DMaxHeight + (NumberOfButtons - 1) * DButtonGap;
    }
    else
    {
        DStackHeight = static_cast<size_t>(DMaxHeight);
    }

    switch (DPlacement)
    {
        // Middle of canvas
        case EPosition::West:
        case EPosition::Center:
        case EPosition::East:
        {
            DYOffset = static_cast<int>(0.5 * Height - 0.5 * DStackHeight);
        }
            break;

            // South side of canvas
        case EPosition::SouthWest:
        case EPosition::South:
        case EPosition::SouthEast:
        {
            DYOffset =
                static_cast<int>(Height - DStackHeight - DContext->DBorderWidth);
        }
            break;

            // North side of canvas
        default:
        {
            DYOffset = DContext->DBorderWidth;
        }
    }
}

// Create buttons whose dimensions are calculated from X, Y offsets and
// pre-measured button width and height
void CVerticalButtonAlignment::CreateButtons(std::vector<std::string> texts)
{
    int CurrentYOffset = DYOffset;

    for (auto &text : texts)
    {
        // Create the button off-screen if the text is an empty string
        if (text.empty())
        {
            DButtons.push_back(
                std::make_shared<CButton>(text, -1, -1, 0, 0));
        }
        // Otherwise set the coordinates for the button on-screen
        else
        {
            DButtons.push_back(
                std::make_shared<CButton>(text, DXOffset, CurrentYOffset, DMaxWidth,
                    DMaxHeight));
        }

        CurrentYOffset += DMaxHeight + DButtonGap;
    }
}

