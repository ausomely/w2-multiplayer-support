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
    std::shared_ptr<CApplicationData> &context,
    std::vector<std::string> &texts, EPosition placement)
        : CButtonAlignment(placement)
{
    DButtons.reserve(texts.size());

    DButtonGap = 13;
    DContext = context; // For the sake of convenience

    DMaxHeight = 0;
    DMaxWidth = 0;

    MeasureButtons(texts);

    // Bump up size to proportions in Main Menu
    DMaxWidth = DMaxWidth * 5/4;
    DMaxHeight = DMaxHeight * 3/2;

    FindXOffset();
    FindYOffset();
    CreateButtons(texts);

}

// Measure the button texts so all buttons are consistently sized
void CVerticalButtonAlignment::MeasureButtons(
    std::vector<std::string> &texts)
{
    DContext->DButtonRenderer->SetBaseDimensions(); // Set to minimum size

    for (auto &text : texts)
    {
        int Width;
        int Height;

        // false tells Text() not to shrink to size of text
        DContext->DButtonRenderer->Text(text, false);

        // Find maximum width of buttons
        Width = DContext->DButtonRenderer->Width();
        if (Width > DMaxWidth)
        {
            DMaxWidth = Width;
        }

        // Find maximum height of buttons
        Height = DContext->DButtonRenderer->Height();
        if (Height > DMaxHeight)
        {
            DMaxHeight = Height;
        }
    }

}

// Based on canvas placement, find the X offset of vertically aligned buttons
void CVerticalButtonAlignment::FindXOffset()
{
    int CanvasWidth = DContext->DOverlaySurface->Width();
    int BorderSize = DContext->DBorderWidth;

    switch (DPlacement)
    {
        // Middle of canvas
        case EPosition::Center:
        case EPosition::North:
        case EPosition::South:
        {
            DXOffset = static_cast<int>(.5 * CanvasWidth - 0.5 * DMaxWidth);
        }
        break;

        // Eastern side of canvas
        case EPosition::NorthEast:
        case EPosition::East:
        case EPosition::SouthEast:
        {
            DXOffset = CanvasWidth - DMaxWidth - BorderSize;
        }
        break;

        // Western side of canvas
        default:
        {
            DXOffset = BorderSize;
        }
    }
}

// Find Y offset of top most button in the group
void CVerticalButtonAlignment::FindYOffset()
{
    int CanvasHeight = DContext->DOverlaySurface->Height();
    int BorderSize = DContext->DBorderWidth;
    size_t StackHeight;
    size_t NumberOfButtons = DButtons.capacity();

    // Find stack height of the buttons
    if (1 < NumberOfButtons)
    {
        StackHeight = NumberOfButtons * DMaxHeight +
            (NumberOfButtons - 1) * DButtonGap;
    }
    else
    {
        StackHeight = static_cast<size_t>(DMaxHeight);
    }

    switch (DPlacement)
    {
        // Middle of canvas
        case EPosition::West:
        case EPosition::Center:
        case EPosition::East:
        {
            DYOffset = static_cast<int>(0.5 * CanvasHeight - 0.5 * StackHeight);
        }
        break;

        // South side of canvas
        case EPosition::SouthWest:
        case EPosition::South:
        case EPosition::SouthEast:
        {
            DYOffset = static_cast<int>(CanvasHeight - StackHeight - BorderSize);
        }
        break;

        // North side of canvas
        default:
        {
            DYOffset = BorderSize;
        }
    }
}

// Create buttons whose dimensions are calculated from X, Y offsets and
// pre-measured button width and height
void CVerticalButtonAlignment::CreateButtons(std::vector<std::string> &texts)
{
    int CurrentYOffset = DYOffset;

    for (int Index = 0; Index < texts.size(); Index++)
    {
        DButtons.push_back(std::make_shared<CButton>(texts[Index],
            DXOffset, CurrentYOffset, DMaxWidth, DMaxHeight));

        CurrentYOffset += DMaxHeight + DButtonGap;
    }
}
