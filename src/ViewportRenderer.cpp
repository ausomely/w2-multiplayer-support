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
#include "ViewportRenderer.h"

CViewportRenderer::CViewportRenderer(std::shared_ptr<CMapRenderer> maprender,
                                     std::shared_ptr<CAssetRenderer> assetrender,
                                     std::shared_ptr<CFogRenderer> fogrender)
{
    DMapRenderer = maprender;
    DAssetRenderer = assetrender;
    DFogRenderer = fogrender;
    DViewportX = 0;
    DViewportY = 0;
    DLastViewportWidth = maprender->DetailedMapWidth();
    DLastViewportHeight = maprender->DetailedMapHeight();
}

CViewportRenderer::~CViewportRenderer() {}

int CViewportRenderer::ViewportX(int x)
{
    DViewportX = x;
    if (DViewportX + DLastViewportWidth >= DMapRenderer->DetailedMapWidth())
    {
        DViewportX = DMapRenderer->DetailedMapWidth() - DLastViewportWidth;
    }
    if (0 > DViewportX)
    {
        DViewportX = 0;
    }
    return DViewportX;
}

int CViewportRenderer::ViewportY(int y)
{
    DViewportY = y;
    if (DViewportY + DLastViewportHeight >= DMapRenderer->DetailedMapHeight())
    {
        DViewportY = DMapRenderer->DetailedMapHeight() - DLastViewportHeight;
    }
    if (0 > DViewportY)
    {
        DViewportY = 0;
    }
    return DViewportY;
}

void CViewportRenderer::CenterViewport(const CPixelPosition &pos)
{
    ViewportX(pos.X() - DLastViewportWidth / 2);
    ViewportY(pos.Y() - DLastViewportHeight / 2);
}

void CViewportRenderer::PanNorth(int pan)
{
    DViewportY -= pan;
    if (0 > DViewportY)
    {
        DViewportY = 0;
    }
}

void CViewportRenderer::PanEast(int pan)
{
    ViewportX(DViewportX + pan);
}

void CViewportRenderer::PanSouth(int pan)
{
    ViewportY(DViewportY + pan);
}

void CViewportRenderer::PanWest(int pan)
{
    DViewportX -= pan;
    if (0 > DViewportX)
    {
        DViewportX = 0;
    }
}

void CViewportRenderer::DrawViewport(
    std::shared_ptr<CGraphicSurface> surface,
    std::shared_ptr<CGraphicSurface> typesurface,
    const std::list<std::weak_ptr<CPlayerAsset> > &selectionmarkerlist,
    const SRectangle &selectrect, EAssetCapabilityType curcapability)
{
    SRectangle TempRectangle;
    EAssetType PlaceType = EAssetType::None;
    std::shared_ptr<CPlayerAsset> Builder;

    DLastViewportWidth = surface->Width();
    DLastViewportHeight = surface->Height();

    if (DViewportX + DLastViewportWidth >= DMapRenderer->DetailedMapWidth())
    {
        DViewportX = DMapRenderer->DetailedMapWidth() - DLastViewportWidth;
    }
    if (DViewportY + DLastViewportHeight >= DMapRenderer->DetailedMapHeight())
    {
        DViewportY = DMapRenderer->DetailedMapHeight() - DLastViewportHeight;
    }

    TempRectangle.DXPosition = DViewportX;
    TempRectangle.DYPosition = DViewportY;
    TempRectangle.DWidth = DLastViewportWidth;
    TempRectangle.DHeight = DLastViewportHeight;

    switch (curcapability)
    {
        case EAssetCapabilityType::BuildFarm:
            PlaceType = EAssetType::Farm;
            break;
        case EAssetCapabilityType::BuildTownHall:
            PlaceType = EAssetType::TownHall;
            break;
        case EAssetCapabilityType::BuildBarracks:
            PlaceType = EAssetType::Barracks;
            break;
        case EAssetCapabilityType::BuildLumberMill:
            PlaceType = EAssetType::LumberMill;
            break;
        case EAssetCapabilityType::BuildBlacksmith:
            PlaceType = EAssetType::Blacksmith;
            break;
        case EAssetCapabilityType::BuildScoutTower:
            PlaceType = EAssetType::ScoutTower;
            break;
        //case EAssetCapabilityType::BuildGoldMine:
        //    PlaceType = EAssetType::GoldMine;
        default:
            break;
    }
    DMapRenderer->DrawMap(surface, typesurface, TempRectangle);
    DAssetRenderer->DrawSelections(surface, TempRectangle, selectionmarkerlist,
                                   selectrect, EAssetType::None != PlaceType);
    DAssetRenderer->DrawAssets(surface, typesurface, TempRectangle);
    DAssetRenderer->DrawOverlays(surface, TempRectangle);

    if (selectionmarkerlist.size())
    {
        Builder = selectionmarkerlist.front().lock();
    }
    DAssetRenderer->DrawPlacement(
        surface, TempRectangle,
        CPixelPosition(selectrect.DXPosition, selectrect.DYPosition), PlaceType,
        Builder);
    DFogRenderer->DrawMap(surface, TempRectangle);
}
