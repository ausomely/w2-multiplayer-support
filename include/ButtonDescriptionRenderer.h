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
#ifndef BUTTONDESCRIPTIONRENDERER_H
#define BUTTONDESCRIPTIONRENDERER_H
#include <vector>
#include "FontTileset.h"
#include "GameModel.h"

class CButtonDescriptionRenderer
{
  protected:
    std::shared_ptr<CGraphicTileset> DIconTileset;
    std::shared_ptr<CFontTileset> DFont;
    int DTextHeight;
    int DForegroundColor;
    int DBackgroundColor;

  public:
    CButtonDescriptionRenderer(std::shared_ptr<CFontTileset> font);
    ~CButtonDescriptionRenderer();

    void DrawButtonDescription(std::shared_ptr<CGraphicSurface> surface,
        std::string description);
};

#endif