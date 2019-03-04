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
#include "MapRenderer.h"
#include <iomanip>
#include <sstream>
#include "CommentSkipLineDataSource.h"
#include "Debug.h"
#include "PixelType.h"
#include "Tokenizer.h"

CMapRenderer::CMapRenderer(std::shared_ptr<CDataSource> config,
                           std::shared_ptr<CGraphicTileset> tileset,
                           std::shared_ptr<CTerrainMap> map)
{
    CCommentSkipLineDataSource LineSource(config, '#');
    std::string TempString;
    int ItemCount;
    DTileset = tileset;
    DMap = map;

    DPixelIndices.resize(to_underlying(CTerrainMap::ETileType::Max));

    if (!LineSource.Read(TempString))
    {
        return;
    }
    ItemCount = std::stoi(TempString);
    for (int Index = 0; Index < ItemCount; Index++)
    {
        std::vector<std::string> Tokens;
        if (!LineSource.Read(TempString))
        {
            return;
        }
        CTokenizer::Tokenize(Tokens, TempString);
        uint32_t ColorValue = std::stoul(Tokens[1], nullptr, 0);
        int PixelIndex = 0;
        if (Tokens[0] == "light-grass")
        {
            PixelIndex = to_underlying(CTerrainMap::ETileType::LightGrass);
        }
        else if (Tokens[0] == "dark-grass")
        {
            PixelIndex = to_underlying(CTerrainMap::ETileType::DarkGrass);
        }
        else if (Tokens[0] == "light-dirt")
        {
            PixelIndex = to_underlying(CTerrainMap::ETileType::LightDirt);
        }
        else if (Tokens[0] == "dark-dirt")
        {
            PixelIndex = to_underlying(CTerrainMap::ETileType::DarkDirt);
        }
        else if (Tokens[0] == "rock")
        {
            PixelIndex = to_underlying(CTerrainMap::ETileType::Rock);
        }
        else if (Tokens[0] == "forest")
        {
            PixelIndex = to_underlying(CTerrainMap::ETileType::Forest);
        }
        else if (Tokens[0] == "stump")
        {
            PixelIndex = to_underlying(CTerrainMap::ETileType::Stump);
        }
        else if (Tokens[0] == "shallow-water")
        {
            PixelIndex = to_underlying(CTerrainMap::ETileType::ShallowWater);
        }
        else if (Tokens[0] == "deep-water")
        {
            PixelIndex = to_underlying(CTerrainMap::ETileType::DeepWater);
        }
        else
        {
            PixelIndex = to_underlying(CTerrainMap::ETileType::Rubble);
        }
        DPixelIndices[PixelIndex] = ColorValue;
    }

    DTileIndices.resize(to_underlying(CTerrainMap::ETileType::Max));
    for (int Index = 0; Index < DTileIndices.size(); Index++)
    {
        DTileIndices[Index].resize(16);
    }
    for (int Index = 0; Index < 16; Index++)
    {
        int AltTileIndex;
        std::ostringstream TempStringStream;
        TempStringStream << std::hex << std::uppercase << Index;

        AltTileIndex = 0;
        while (true)
        {
            int Value = DTileset->FindTile(
                std::string("light-grass-") + TempStringStream.str() +
                std::string("-") + std::to_string(AltTileIndex));
            if (0 > Value)
            {
                break;
            }
            DTileIndices[to_underlying(CTerrainMap::ETileType::LightGrass)]
                        [Index]
                            .push_back(Value);
            AltTileIndex++;
        }
        AltTileIndex = 0;
        while (true)
        {
            int Value = DTileset->FindTile(
                std::string("dark-grass-") + TempStringStream.str() +
                std::string("-") + std::to_string(AltTileIndex));
            if (0 > Value)
            {
                break;
            }
            DTileIndices[to_underlying(CTerrainMap::ETileType::DarkGrass)][Index]
                .push_back(Value);
            AltTileIndex++;
        }

        AltTileIndex = 0;
        while (true)
        {
            int Value = DTileset->FindTile(
                std::string("light-dirt-") + TempStringStream.str() +
                std::string("-") + std::to_string(AltTileIndex));
            if (0 > Value)
            {
                break;
            }
            DTileIndices[to_underlying(CTerrainMap::ETileType::LightDirt)][Index]
                .push_back(Value);
            AltTileIndex++;
        }
        AltTileIndex = 0;
        while (true)
        {
            int Value = DTileset->FindTile(
                std::string("dark-dirt-") + TempStringStream.str() +
                std::string("-") + std::to_string(AltTileIndex));
            if (0 > Value)
            {
                break;
            }
            DTileIndices[to_underlying(CTerrainMap::ETileType::DarkDirt)][Index]
                .push_back(Value);
            AltTileIndex++;
        }
        AltTileIndex = 0;
        while (true)
        {
            int Value = DTileset->FindTile(
                std::string("rock-") + TempStringStream.str() +
                std::string("-") + std::to_string(AltTileIndex));
            if (0 > Value)
            {
                break;
            }
            DTileIndices[to_underlying(CTerrainMap::ETileType::Rock)][Index]
                .push_back(Value);
            AltTileIndex++;
        }
        AltTileIndex = 0;
        while (true)
        {
            int Value = DTileset->FindTile(
                    std::string("rubble-") + TempStringStream.str() +
                    std::string("-") + std::to_string(AltTileIndex));
            if (0 > Value)
            {
                break;
            }
            DTileIndices[to_underlying(CTerrainMap::ETileType::Rubble)][Index]
                    .push_back(Value);
            AltTileIndex++;
        }
        AltTileIndex = 0;
        while (true)
        {
            int Value = DTileset->FindTile(
                std::string("forest-") + TempStringStream.str() +
                std::string("-") + std::to_string(AltTileIndex));
            if (0 > Value)
            {
                break;
            }
            DTileIndices[to_underlying(CTerrainMap::ETileType::Forest)][Index]
                .push_back(Value);
            AltTileIndex++;
        }

        AltTileIndex = 0;
        while (true)
        {
            int Value = DTileset->FindTile(
                std::string("shallow-water-") + TempStringStream.str() +
                std::string("-") + std::to_string(AltTileIndex));
            if (0 > Value)
            {
                break;
            }
            DTileIndices[to_underlying(CTerrainMap::ETileType::ShallowWater)]
                        [Index]
                            .push_back(Value);
            AltTileIndex++;
        }
        AltTileIndex = 0;
        while (true)
        {
            int Value = DTileset->FindTile(
                std::string("deep-water-") + TempStringStream.str() +
                std::string("-") + std::to_string(AltTileIndex));
            if (0 > Value)
            {
                break;
            }
            DTileIndices[to_underlying(CTerrainMap::ETileType::DeepWater)][Index]
                .push_back(Value);
            AltTileIndex++;
        }
        AltTileIndex = 0;
        while (true)
        {
            int Value = DTileset->FindTile(
                std::string("stump-") + TempStringStream.str() +
                std::string("-") + std::to_string(AltTileIndex));
            if (0 > Value)
            {
                break;
            }
            DTileIndices[to_underlying(CTerrainMap::ETileType::Stump)][Index]
                .push_back(Value);
            AltTileIndex++;
        }
    }
//    for (int Index = 0; Index < 16; Index++)
//    {
//        DTileIndices[to_underlying(CTerrainMap::ETileType::Rubble)][Index]
//            .push_back(
//                DTileIndices[to_underlying(CTerrainMap::ETileType::Rock)][0][0]);
//    }
}

int CMapRenderer::MapWidth() const
{
    return DMap->Width();
}

int CMapRenderer::MapHeight() const
{
    return DMap->Height();
}

int CMapRenderer::DetailedMapWidth() const
{
    return DMap->Width() * DTileset->TileWidth();
}

int CMapRenderer::DetailedMapHeight() const
{
    return DMap->Height() * DTileset->TileHeight();
}

void CMapRenderer::DrawMap(std::shared_ptr<CGraphicSurface> surface,
                           std::shared_ptr<CGraphicSurface> typesurface,
                           const SRectangle &rect)
{
    int TileWidth, TileHeight;

    TileWidth = DTileset->TileWidth();
    TileHeight = DTileset->TileHeight();

    typesurface->Clear();
    for (int YIndex = rect.DYPosition / TileHeight,
             YPos = -(rect.DYPosition % TileHeight);
         YPos < rect.DHeight; YIndex++, YPos += TileHeight)
    {
        for (int XIndex = rect.DXPosition / TileWidth,
                 XPos = -(rect.DXPosition % TileWidth);
             XPos < rect.DWidth; XIndex++, XPos += TileWidth)
        {
            CPixelType PixelType(DMap->TileType(XIndex, YIndex));
            CTerrainMap::ETileType ThisTileType =
                DMap->TileType(XIndex, YIndex);
            int TileIndex = DMap->TileTypeIndex(XIndex, YIndex);

            if ((0 <= TileIndex) && (16 > TileIndex))
            {
                int DisplayIndex = -1;
                int AltTileCount =
                    DTileIndices[to_underlying(ThisTileType)][TileIndex].size();
                if (AltTileCount)
                {
                    int AltIndex = (XIndex + YIndex) % AltTileCount;

                    DisplayIndex = DTileIndices[to_underlying(ThisTileType)]
                                               [TileIndex][AltIndex];
                }
                if (-1 != DisplayIndex)
                {
                    DTileset->DrawTile(surface, XPos, YPos, DisplayIndex);
                    DTileset->DrawClipped(typesurface, XPos, YPos, DisplayIndex,
                                          PixelType.ToPixelColor());
                }
            }
            else
            {
                return;
            }
        }
    }
}

void CMapRenderer::DrawMiniMap(std::shared_ptr<CGraphicSurface> surface)
{
    auto ResourceContext = surface->CreateResourceContext();

    ResourceContext->SetLineWidth(1);
    ResourceContext->SetLineCap(CGraphicResourceContext::ELineCap::Square);
    for (int YPos = 0; YPos < DMap->Height(); YPos++)
    {
        int XPos = 0;

        while (XPos < DMap->Width())
        {
            auto TileType = DMap->TileType(XPos, YPos);
            int XAnchor = XPos;
            while ((XPos < DMap->Width()) &&
                   (DMap->TileType(XPos, YPos) == TileType))
            {
                XPos++;
            }
            if (CTerrainMap::ETileType::None != TileType)
            {
                ResourceContext->SetSourceRGB(
                    DPixelIndices[to_underlying(TileType)]);
                ResourceContext->MoveTo(XAnchor, YPos);
                ResourceContext->LineTo(XPos, YPos);
                ResourceContext->Stroke();
            }
        }
    }
}
