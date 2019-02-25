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

using TCallBackFunction = void (*)(std::shared_ptr<void> ptr);

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

    std::shared_ptr<CGraphicSurface> DSurface;

    std::shared_ptr<CApplicationData> DContext;

    std::vector<std::shared_ptr<CButton> > DButtons;
    bool DButtonPressedInStack;

  public:
    virtual ~CButtonAlignment() = default;
    explicit CButtonAlignment(EPosition placement);

    virtual void MeasureButtons(std::vector<std::string> &texts) = 0;
    virtual void FindXOffset() = 0;
    virtual void FindYOffset() = 0;
    virtual void CreateButtons(std::vector<std::string> &texts) = 0;

    void DrawStack(int x, int y, bool clicked);
    bool ButtonPressedInStack()
    {
        return DButtonPressedInStack;
    }
    int ButtonPressedIndex();
    void ResetButtonStates();
    bool PointerHovering();

};

#endif
