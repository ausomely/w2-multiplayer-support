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

CButtonAlignment::CButtonAlignment(std::shared_ptr<CApplicationData> context,
    EPosition placement)
{
    DContext = context;
    DPlacement = placement;
    DButtonHovered = false;
    DButtonPressedInStack = false;
    DColorSet = false;
    auto FontID = to_underlying(CUnitDescriptionRenderer::EFontSize::Large);
    DButRen.reset(new CButtonRenderer(DContext->DButtonRecolorMap,
        DContext->DInnerBevel, DContext->DOuterBevel, DContext->DFonts[FontID]));
}

// Measure the button texts so all buttons are consistently sized
void CButtonAlignment::MeasureButtons(std::vector<std::string> texts)
{
    DButRen->SetBaseDimensions(); // Set to minimum size

    for (auto &text : texts)
    {
        int Width;
        int Height;

        // false tells Text() not to shrink to size of text
        DButRen->Text(text, false);

        // Find maximum width of buttons
        Width = DButRen->Width();
        if (Width > DMaxWidth)
        {
            DMaxWidth = Width;
        }

        // Find maximum height of buttons
        Height = DButRen->Height();
        if (Height > DMaxHeight)
        {
            DMaxHeight = Height;
        }
    }
}

void
CButtonAlignment::DrawStack(std::shared_ptr<CGraphicSurface> surface, int x, int y,
    bool clicked)
{
    DButtonHovered = false;
    DButtonPressedInStack = false;
//    DContext->DButtonRenderer->SetBaseDimensions();

    for (int Index = 0; Index < DButtons.size(); ++Index)
    {
        if (DButtons[Index]->Update(x, y, clicked))
        {
            DButtonHovered = true;
        }

        // Skip drawing the button if it has empty text
        if (0 <= DButtons[Index]->XPosition())
        {
            DButRen->Text(DButtons[Index]->Text());
            if (HasColorSet())
            {
                DButRen->ButtonColor(DColor);
            }
//            DContext->DButtonRenderer->Width(DMaxWidth);
//            DContext->DButtonRenderer->Height(DMaxHeight);
            DButRen->DrawButton(surface, DButtons[Index]->XPosition(),
                DButtons[Index]->YPosition(), DButtons[Index]->State());
        }

        if (DButtons[Index]->Pressed())
        {
            DButtonPressedInStack = true;
        }
    }
}

// Reconfigure buttons. Used when width/height has changed.
void CButtonAlignment::ReconfigureButtons()
{
    std::vector<std::string> Texts;

    for (auto Button : DButtons)
    {
        Texts.push_back(Button->Text());
    }

    DButtons.clear();
    DButtons.reserve(Texts.size());
    FindXOffset(DCanvasWidth);
    FindYOffset(DCanvasHeight);
    CreateButtons(Texts);
}

// Set width of buttons in the stack
void CButtonAlignment::SetWidth(int width)
{
    DMaxWidth = width;
    DButRen->SetBaseDimensions();
    DButRen->Width(DMaxWidth);
    DButRen->Height(DMaxHeight);
    ReconfigureButtons();
}

// Set height of buttons in the stack
void CButtonAlignment::SetHeight(int height)
{
    DMaxHeight = height;
    DButRen->SetBaseDimensions();
    DButRen->Height(DMaxHeight);
    DButRen->Width(DMaxWidth);
    ReconfigureButtons();

}

// Return stack index of button pressed, or sentinel value if none are pressed
int CButtonAlignment::ButtonPressedIndex()
{
    if (ButtonPressedInStack())
    {
        for (int Index = 0; Index < DButtons.size(); ++Index)
        {
            if (DButtons[Index]->Pressed())
            {
                return Index;
            }
        }
    }

    // No buttons pressed
    return -1;
}

void CButtonAlignment::SetButtonColor(EPlayerColor color)
{
    DColor = color;
    DColorSet = true;
}

// Mark a button inactive within the stack at the given index
void CButtonAlignment::MarkInactiveButton(int index)
{
    // Fail silently if index is outside range

    if (index < DButtons.size())
    {
        DButtons[index]->MarkInactive();
    }
}

// Clear inactive button setting
void CButtonAlignment::ClearInactiveButton(int index)
{
    // Fail silently if index is outside range

    if (index < DButtons.size())
    {
        DButtons[index]->ClearInactive();
    }
}
