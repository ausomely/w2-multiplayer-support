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
#include "ButtonDescriptionRenderer.h"
#include "Debug.h"
#include "TextFormatter.h"

CButtonDescriptionRenderer::CButtonDescriptionRenderer(std::shared_ptr<CFontTileset> font)
{
    int Width;

    DFont = font;
    DForegroundColor = DFont->FindColor("gold");
    DBackgroundColor = DFont->FindColor("black");
    DFont->
        MeasureText("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz",
        Width, DTextHeight);
}

CButtonDescriptionRenderer::~CButtonDescriptionRenderer() {}

void CButtonDescriptionRenderer::DrawButtonDescription(
    std::shared_ptr<CGraphicSurface> surface, std::string description)
{
    int Width, Height;
    int TextYOffset;
    int WidthSeparation, XOffset;

    Width = surface->Width();
    Height = surface->Height();
    TextYOffset = Height / 2 - DTextHeight / 2;
    WidthSeparation = Width / 4;
    XOffset = 0;

    // Perhaps a semitransparent solid colored background could be loaded
    // Following line segfaults. Not sure what was trying to be done.
    //DIconTileset->DrawTile(surface, XOffset, 500, 1);
    DFont->DrawTextWithShadow(
        surface, XOffset, TextYOffset,
        DForegroundColor, DBackgroundColor, 1,
        description);
}
