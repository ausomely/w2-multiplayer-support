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

#ifndef SURFACEBUILDER_H
#define SURFACEBUILDER_H

#include "ApplicationData.h"

class CSurfaceBuilder
{
    std::shared_ptr<CGraphicSurface> DSurface;
    uint32_t DColor;
    int DWidth;
    int DHeight;

  public:
    CSurfaceBuilder(int width, int height, uint32_t color)
    {
        // Surface color
        DColor = color;

        // Set width and height
        DWidth = width;
        DHeight = height;
    }

    std::shared_ptr<CGraphicSurface> GetOverlaySurface()
    {
        if (nullptr == DSurface)
        {
            DSurface = CGraphicFactory::CreateSurface(
                DWidth, DHeight, CGraphicSurface::ESurfaceFormat::ARGB32);

            auto ResourceContext = DSurface->CreateResourceContext();
            ResourceContext->SetSourceRGB(DColor);
            ResourceContext->Rectangle(0, 0, DWidth, DHeight);
            ResourceContext->Fill();

        }
        return DSurface;
    }
};

#endif
