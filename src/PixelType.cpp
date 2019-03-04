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
#include "PixelType.h"

CPixelType::CPixelType(int red, int green, int blue)
{
    DNumber = static_cast<EPlayerNumber>(red);
    DType = static_cast<EAssetTerrainType>(green);
}

CPixelType::CPixelType(CTerrainMap::ETileType type)
{
    DNumber = EPlayerNumber::Neutral;
    switch (type)
    {
        case CTerrainMap::ETileType::LightGrass:
        case CTerrainMap::ETileType::DarkGrass:
            DType = EAssetTerrainType::Grass;
            break;
        case CTerrainMap::ETileType::LightDirt:
        case CTerrainMap::ETileType::DarkDirt:
            DType = EAssetTerrainType::Dirt;
            break;
        case CTerrainMap::ETileType::Rock:
            DType = EAssetTerrainType::Rock;
            break;
        case CTerrainMap::ETileType::Forest:
            DType = EAssetTerrainType::Tree;
            break;
        case CTerrainMap::ETileType::Stump:
            DType = EAssetTerrainType::Stump;
            break;
        case CTerrainMap::ETileType::ShallowWater:
        case CTerrainMap::ETileType::DeepWater:
            DType = EAssetTerrainType::Water;
            break;
        case CTerrainMap::ETileType::Rubble:
            DType = EAssetTerrainType::Rubble;
            break;
        default:
            DType = EAssetTerrainType::None;
            break;
    }
}

CPixelType::CPixelType(const CPlayerAsset &asset)
{
    switch (asset.Type())
    {
        case EAssetType::Peasant:
            DType = EAssetTerrainType::Peasant;
            break;
        case EAssetType::Footman:
            DType = EAssetTerrainType::Footman;
            break;
        case EAssetType::Archer:
            DType = EAssetTerrainType::Archer;
            break;
        case EAssetType::Ranger:
            DType = EAssetTerrainType::Ranger;
            break;
        case EAssetType::GoldVein:
            DType = EAssetTerrainType::GoldVein;
            break;
        case EAssetType::GoldMine:
            DType = EAssetTerrainType::GoldMine;
            break;
        case EAssetType::TownHall:
            DType = EAssetTerrainType::TownHall;
            break;
        case EAssetType::Keep:
            DType = EAssetTerrainType::Keep;
            break;
        case EAssetType::Castle:
            DType = EAssetTerrainType::Castle;
            break;
        case EAssetType::Farm:
            DType = EAssetTerrainType::Farm;
            break;
        case EAssetType::Barracks:
            DType = EAssetTerrainType::Barracks;
            break;
        case EAssetType::LumberMill:
            DType = EAssetTerrainType::LumberMill;
            break;
        case EAssetType::Blacksmith:
            DType = EAssetTerrainType::Blacksmith;
            break;
        case EAssetType::ScoutTower:
            DType = EAssetTerrainType::ScoutTower;
            break;
        case EAssetType::GuardTower:
            DType = EAssetTerrainType::GuardTower;
            break;
        case EAssetType::CannonTower:
            DType = EAssetTerrainType::CannonTower;
            break;
        default:
            DType = EAssetTerrainType::None;
            break;
    }
    DNumber = asset.Number();
}

CPixelType::CPixelType(const CPixelType &pixeltype)
{
    DType = pixeltype.DType;
    DNumber = pixeltype.DNumber;
}

CPixelType &CPixelType::operator=(const CPixelType &pixeltype)
{
    if (this != &pixeltype)
    {
        DType = pixeltype.DType;
        DNumber = pixeltype.DNumber;
    }
    return *this;
}

uint32_t CPixelType::ToPixelColor() const
{
    uint32_t RetVal = static_cast<uint32_t>(DNumber);

    RetVal <<= 16;
    RetVal |= (static_cast<uint32_t>(DType)) << 8;
    return RetVal;
}

EAssetType CPixelType::AssetType() const
{
    switch (DType)
    {
        case EAssetTerrainType::Peasant:
            return EAssetType::Peasant;
        case EAssetTerrainType::Footman:
            return EAssetType::Footman;
        case EAssetTerrainType::Archer:
            return EAssetType::Archer;
        case EAssetTerrainType::Ranger:
            return EAssetType::Ranger;
        case EAssetTerrainType::GoldVein:
            return EAssetType::GoldVein;
        case EAssetTerrainType::GoldMine:
            return EAssetType::GoldMine;
        case EAssetTerrainType::TownHall:
            return EAssetType::TownHall;
        case EAssetTerrainType::Keep:
            return EAssetType::Keep;
        case EAssetTerrainType::Castle:
            return EAssetType::Castle;
        case EAssetTerrainType::Farm:
            return EAssetType::Farm;
        case EAssetTerrainType::Barracks:
            return EAssetType::Barracks;
        case EAssetTerrainType::LumberMill:
            return EAssetType::LumberMill;
        case EAssetTerrainType::Blacksmith:
            return EAssetType::Blacksmith;
        case EAssetTerrainType::ScoutTower:
            return EAssetType::ScoutTower;
        case EAssetTerrainType::GuardTower:
            return EAssetType::GuardTower;
        case EAssetTerrainType::CannonTower:
            return EAssetType::CannonTower;
        default:
            return EAssetType::None;
    }
}

CPixelType CPixelType::GetPixelType(std::shared_ptr<CGraphicSurface> surface,
                                    const CPixelPosition &pos)
{
    return GetPixelType(surface, pos.X(), pos.Y());
}

CPixelType CPixelType::GetPixelType(std::shared_ptr<CGraphicSurface> surface,
                                    int xpos, int ypos)
{
    uint32_t PixelColor = surface->PixelAt(xpos, ypos);
    return CPixelType((PixelColor >> 16) & 0xFF, (PixelColor >> 8) & 0xFF,
                      PixelColor & 0xFF);
}
