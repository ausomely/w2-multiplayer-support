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
#include "ListViewRenderer.h"

CListViewRenderer::CListViewRenderer(std::shared_ptr<CGraphicTileset> icons,
                                     std::shared_ptr<CFontTileset> font)
{
    DIconTileset = icons;
    DFont = font;
    DFontHeight = 1;
    DLastItemCount = 0;
    DLastItemOffset = 0;
    DLastViewWidth = 0;
    DLastViewHeight = 0;
    DLastUndisplayed = false;
}

CListViewRenderer::~CListViewRenderer() {}

int CListViewRenderer::ItemAt(int x, int y)
{
    if ((0 > x) || (0 > y))
    {
        return to_underlying(EListViewObject::None);
    }
    if ((DLastViewWidth <= x) || (DLastViewHeight <= y))
    {
        return to_underlying(EListViewObject::None);
    }
    if (x < (DLastViewWidth - DIconTileset->TileWidth()))
    {
        if ((y / DFontHeight) < DLastItemCount)
        {
            return DLastItemOffset + (y / DFontHeight);
        }
    }
    else if (y < DIconTileset->TileHeight())
    {
        if (DLastItemOffset)
        {
            return to_underlying(EListViewObject::UpArrow);
        }
    }
    else if (y > DLastViewHeight - DIconTileset->TileHeight())
    {
        if (DLastUndisplayed)
        {
            return to_underlying(EListViewObject::DownArrow);
        }
    }
    return to_underlying(EListViewObject::None);
}

void CListViewRenderer::DrawListView(std::shared_ptr<CGraphicSurface> surface,
                                     int selectedindex, int offsetindex,
                                     std::vector<std::string> &items)
{
    auto ResourceContext = surface->CreateResourceContext();
    int TextWidth, TextHeight;
    int MaxTextWidth;
    int BlackIndex = DFont->FindColor("black");
    int WhiteIndex = DFont->FindColor("white");
    int GoldIndex = DFont->FindColor("gold");
    int TextYOffset = 0;

    DLastViewWidth = surface->Width();
    DLastViewHeight = surface->Height();

    DLastItemCount = 0;
    DLastItemOffset = offsetindex;
    MaxTextWidth = DLastViewWidth - DIconTileset->TileWidth();
    // DIconTileset->DrawTileRectangle(drawable, 0, 0, DLastViewWidth,
    // DLastViewHeight, DIconTileset->FindTile("background"));
    ResourceContext->SetSourceRGBA(0x4000044C);  // Make this data driven
    ResourceContext->Rectangle(0, 0, DLastViewWidth, DLastViewHeight);
    ResourceContext->Fill();
    DIconTileset->DrawTile(surface, MaxTextWidth, 0,
                           offsetindex ? DIconTileset->FindTile("up-active")
                                       : DIconTileset->FindTile("up-inactive"));
    DLastUndisplayed = false;
    while ((offsetindex < items.size()) && (TextYOffset < DLastViewHeight))
    {
        std::string TempString = items[offsetindex];

        DFont->MeasureText(TempString, TextWidth, TextHeight);
        if (TextWidth >= MaxTextWidth)
        {
            while (TempString.length())
            {
                TempString = TempString.substr(0, TempString.length() - 1);
                DFont->MeasureText(TempString + "...", TextWidth, TextHeight);
                if (TextWidth < MaxTextWidth)
                {
                    TempString += "...";
                    break;
                }
            }
        }
        DFont->DrawTextWithShadow(
            surface, 0, TextYOffset,
            offsetindex == selectedindex ? WhiteIndex : GoldIndex, BlackIndex,
            1, TempString);
        DFontHeight = TextHeight;
        TextYOffset += DFontHeight;
        DLastItemCount++;
        offsetindex++;
    }
    if (DLastItemCount + DLastItemOffset < items.size())
    {
        DLastUndisplayed = true;
    }
    DIconTileset->DrawTile(
        surface, MaxTextWidth, DLastViewHeight - DIconTileset->TileWidth(),
        DLastUndisplayed ? DIconTileset->FindTile("down-active")
                         : DIconTileset->FindTile("down-inactive"));
}
