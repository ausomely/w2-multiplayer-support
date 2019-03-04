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
#ifndef BUTTON_H
#define BUTTON_H

#include <string>
#include "ButtonRenderer.h"
#include "Rectangle.h"

class CButton
{
  protected:
    CButtonRenderer::EButtonState DState;
    std::shared_ptr<SRectangle> DShape;
    std::string DText;
    bool DInactive;

    void State(CButtonRenderer::EButtonState state);

  public:

    ~CButton() = default;
    CButton(std::string &text, int x, int y, int width, int height);

    bool Update(int x, int y, bool clicked);

    bool Pressed()
    {
        return CButtonRenderer::EButtonState::Pressed == DState;
    }
    CButtonRenderer::EButtonState State()
    {
        return DState;
    }
    std::string Text()
    {
        return DText;
    }
    int XPosition()
    {
        return DShape->DXPosition;
    }
    int YPosition()
    {
        return DShape->DYPosition;
    }
    int Width()
    {
        return DShape->DWidth;
    }
    int Height()
    {
        return DShape->DHeight;
    }
    void MarkInactive();
    void ClearInactive();
    bool Inactive()
    {
        return DInactive;
    }

};

#endif
