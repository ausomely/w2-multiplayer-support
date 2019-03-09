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
#ifndef BUTTONALIGNMENT_H
#define BUTTONALIGNMENT_H

#include <vector>
#include "Button.h"
#include "GameDataTypes.h"

enum class EPosition
{
    Center = 0,
    North,
    NorthEast,
    East,
    SouthEast,
    South,
    SouthWest,
    West,
    NorthWest,
    Max
};


class CButtonAlignment
{
  protected:

    bool DButtonHovered;
    int DButtonGap;
    int DMaxWidth;
    int DMaxHeight;
    int DXOffset;
    int DYOffset;
    EPosition DPlacement;

    bool DColorSet;
    EPlayerColor DColor;

    std::shared_ptr<CApplicationData> DContext;
    int DCanvasWidth;
    int DCanvasHeight;

    std::unique_ptr<CButtonRenderer> DButRen;
    std::vector<std::shared_ptr<CButton> > DButtons;
    bool DButtonPressedInStack;

  public:
    virtual ~CButtonAlignment() = default;
    explicit CButtonAlignment(std::shared_ptr<CApplicationData> context,
        EPosition placement);

    void MeasureButtons(std::vector<std::string> texts);
    virtual void FindXOffset(int CanvasWidth) = 0;
    virtual void FindYOffset(int CanvasHeight) = 0;

    void DrawStack(std::shared_ptr<CGraphicSurface> surface, int x, int y, bool clicked);
    void SetButtonColor(EPlayerColor color);
    void MarkInactiveButton(int index);
    void ClearInactiveButton(int index);
    bool HasColorSet()
    {
        return DColorSet;
    }
    bool ButtonPressedInStack()
    {
        return DButtonPressedInStack;
    }
    int ButtonPressedIndex();
    void ResetButtonStates();

    bool PointerHovering()
    {
        return DButtonHovered;
    }

};

#endif
