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
#include "AssetDecoratedMap.h"
#include <algorithm>
#include <queue>
#include "CommentSkipLineDataSource.h"
#include "Debug.h"
#include "Tokenizer.h"

std::map<std::string, int> CAssetDecoratedMap::DMapNameTranslation;
std::vector<std::shared_ptr<CAssetDecoratedMap> > CAssetDecoratedMap::DAllMaps;

CAssetDecoratedMap::CAssetDecoratedMap() : CTerrainMap() {}

CAssetDecoratedMap::CAssetDecoratedMap(const CAssetDecoratedMap &map)
        : CTerrainMap(map )
{
    DAssets = map.DAssets;
    DLumberAvailable = map.DLumberAvailable;
    DStoneAvailable = map.DStoneAvailable;
    DAssetInitializationList = map.DAssetInitializationList;
    DResourceInitializationList = map.DResourceInitializationList;

    for(auto asset: DAssets)
    {
        // If asset not building and is not already in map
        if (asset->Speed() && 0 == AssetsDirectionTime.count(asset) )
        {
            AssetsDirectionTime.insert(std::pair<std::shared_ptr<CPlayerAsset>,int>(asset, 1));
            AssetsDirection.insert(std::pair<std::shared_ptr<CPlayerAsset>,int>(asset, to_underlying(asset->Direction())));
        }
    }
}

CAssetDecoratedMap::~CAssetDecoratedMap() {}

CAssetDecoratedMap &CAssetDecoratedMap::operator=(const CAssetDecoratedMap &map)
{
    if (this != &map)
    {
        CTerrainMap::operator=(map);
        DAssets = map.DAssets;
        DLumberAvailable = map.DLumberAvailable;
        DStoneAvailable = map.DStoneAvailable;
        DAssetInitializationList = map.DAssetInitializationList;
        DResourceInitializationList = map.DResourceInitializationList;
    }
    return *this;
}

bool CAssetDecoratedMap::LoadMaps(std::shared_ptr<CDataContainer> container)
{
    auto FileIterator = container->First();
    if (FileIterator == nullptr)
    {
        PrintError("FileIterator == nullptr\n");
        return false;
    }
    while ((FileIterator != nullptr) && (FileIterator->IsValid()))
    {
        std::string Filename = FileIterator->Name();
        FileIterator->Next();
        if (Filename.rfind(".map") == (Filename.length() - 4))
        {
            std::shared_ptr<CAssetDecoratedMap> TempMap =
                std::make_shared<CAssetDecoratedMap>();

            if (!TempMap->LoadMap(container->DataSource(Filename)))
            {
                PrintError("Failed to load map \"%s\".\n", Filename.c_str());
                continue;
            }
            else
            {
                PrintDebug(DEBUG_LOW, "Loaded map \"%s\".\n", Filename.c_str());
            }
            TempMap->RenderTerrain();
            DMapNameTranslation[TempMap->MapName()] = DAllMaps.size();
            DAllMaps.push_back(TempMap);
        }
    }
    PrintDebug(DEBUG_LOW, "Maps loaded\n");
    return true;
}

int CAssetDecoratedMap::FindMapIndex(const std::string &name)
{
    auto Iterator = DMapNameTranslation.find(name);

    if (Iterator != DMapNameTranslation.end())
    {
        return Iterator->second;
    }
    return -1;
}

std::shared_ptr<const CAssetDecoratedMap> CAssetDecoratedMap::GetMap(int index)
{
    if ((0 > index) || (DAllMaps.size() <= index))
    {
        return std::shared_ptr<const CAssetDecoratedMap>();
    }
    return std::const_pointer_cast<const CAssetDecoratedMap>(DAllMaps[index]);
}

std::shared_ptr<CAssetDecoratedMap> CAssetDecoratedMap::DuplicateMap(int index)
{
    if ((0 > index) || (DAllMaps.size() <= index))
    {
        return std::shared_ptr<CAssetDecoratedMap>();
    }
    return std::make_shared<CAssetDecoratedMap>(*DAllMaps[index]);
}

bool CAssetDecoratedMap::AddAsset(std::shared_ptr<CPlayerAsset> asset)
{
    DAssets.push_back(asset);
    return true;
}

bool CAssetDecoratedMap::RemoveAsset(std::shared_ptr<CPlayerAsset> asset)
{
    DAssets.remove(asset);
    return true;
}

bool CAssetDecoratedMap::CanPlaceAsset(const CTilePosition &pos, int size,
                                       std::shared_ptr<CPlayerAsset> ignoreasset)
{
    int RightX, BottomY;

    
    for (int YOff = 0; YOff < size; YOff++)
    {
        for (int XOff = 0; XOff < size; XOff++)
        {
            auto TileTerrainType = TileType(pos.X() + XOff, pos.Y() + YOff);

            // if((ETileType::Grass != TileTerrainType)&&(ETileType::Dirt !=
            // TileTerrainType)&&(ETileType::Stump !=
            // TileTerrainType)&&(ETileType::Rubble != TileTerrainType)){
            if (!CanPlaceOn(TileTerrainType))
            {
                return false;
            }
        }
    }
    RightX = pos.X() + size;
    BottomY = pos.Y() + size;
    if (RightX >= Width())
    {
        return false;
    }
    if (BottomY >= Height())
    {
        return false;
    }
    for (auto Asset : DAssets)
    {
        int Offset = EAssetType::GoldMine == Asset->Type() ? 1 : 0;
      
        if (EAssetType::None == Asset->Type())
        {
            continue;
        }
        if (ignoreasset == Asset)
        {
            continue;
        }
        if (RightX <= Asset->TilePositionX() - Offset)
        {
            continue;
        }
        if (pos.X() >= (Asset->TilePositionX() + Asset->Size() + Offset))
        {
            continue;
        }
        if (BottomY <= Asset->TilePositionY() - Offset)
        {
            continue;
        }
        if (pos.Y() >= (Asset->TilePositionY() + Asset->Size() + Offset))
        {
            continue;
        }
        return false;
    }
    return true;
}

CTilePosition CAssetDecoratedMap::FindAssetPlacement(
    std::shared_ptr<CPlayerAsset> placeasset,
    std::shared_ptr<CPlayerAsset> fromasset,
    const CTilePosition &nexttiletarget)
{
    int TopY, BottomY, LeftX, RightX;
    int BestDistance = -1, CurDistance;
    CTilePosition BestPosition(-1, -1);
    TopY = fromasset->TilePositionY() - placeasset->Size();
    BottomY = fromasset->TilePositionY() + fromasset->Size();
    LeftX = fromasset->TilePositionX() - placeasset->Size();
    RightX = fromasset->TilePositionX() + fromasset->Size();

    while (true)
    {
        int Skipped = 0;
        if (0 <= TopY)
        {
            int ToX = std::min(RightX, Width() - 1);
            for (int CurX = std::max(LeftX, 0); CurX <= ToX; CurX++)
            {
                if (CanPlaceAsset(CTilePosition(CurX, TopY), placeasset->Size(),
                                  placeasset))
                {
                    CTilePosition TempPosition(CurX, TopY);
                    CurDistance = TempPosition.DistanceSquared(nexttiletarget);
                    if ((-1 == BestDistance) || (CurDistance < BestDistance))
                    {
                        BestDistance = CurDistance;
                        BestPosition = TempPosition;
                    }
                }
            }
        }
        else
        {
            Skipped++;
        }
        if (Width() > RightX)
        {
            int ToY = std::min(BottomY, Height() - 1);
            for (int CurY = std::max(TopY, 0); CurY <= ToY; CurY++)
            {
                if (CanPlaceAsset(CTilePosition(RightX, CurY),
                                  placeasset->Size(), placeasset))
                {
                    CTilePosition TempPosition(RightX, CurY);
                    CurDistance = TempPosition.DistanceSquared(nexttiletarget);
                    if ((-1 == BestDistance) || (CurDistance < BestDistance))
                    {
                        BestDistance = CurDistance;
                        BestPosition = TempPosition;
                    }
                }
            }
        }
        else
        {
            Skipped++;
        }
        if (Height() > BottomY)
        {
            int ToX = std::max(LeftX, 0);
            for (int CurX = std::min(RightX, Width() - 1); CurX >= ToX; CurX--)
            {
                if (CanPlaceAsset(CTilePosition(CurX, BottomY),
                                  placeasset->Size(), placeasset))
                {
                    CTilePosition TempPosition(CurX, BottomY);
                    CurDistance = TempPosition.DistanceSquared(nexttiletarget);
                    if ((-1 == BestDistance) || (CurDistance < BestDistance))
                    {
                        BestDistance = CurDistance;
                        BestPosition = TempPosition;
                    }
                }
            }
        }
        else
        {
            Skipped++;
        }
        if (0 <= LeftX)
        {
            int ToY = std::max(TopY, 0);
            for (int CurY = std::min(BottomY, Height() - 1); CurY >= ToY;
                 CurY--)
            {
                if (CanPlaceAsset(CTilePosition(LeftX, CurY),
                                  placeasset->Size(), placeasset))
                {
                    CTilePosition TempPosition(LeftX, CurY);
                    CurDistance = TempPosition.DistanceSquared(nexttiletarget);
                    if ((-1 == BestDistance) || (CurDistance < BestDistance))
                    {
                        BestDistance = CurDistance;
                        BestPosition = TempPosition;
                    }
                }
            }
        }
        else
        {
            Skipped++;
        }
        if (4 == Skipped)
        {
            break;
        }
        if (-1 != BestDistance)
        {
            break;
        }
        TopY--;
        BottomY++;
        LeftX--;
        RightX++;
    }
    return BestPosition;
}

std::weak_ptr<CPlayerAsset> CAssetDecoratedMap::FindNearestAsset(
    const CPixelPosition &pos, EPlayerNumber number, EAssetType type)
{
    std::shared_ptr<CPlayerAsset> BestAsset;
    int BestDistanceSquared = -1;

    for (auto &Asset : DAssets)
    {
        if ((Asset->Type() == type) && (Asset->Number() == number) &&
            (EAssetAction::Construct != Asset->Action()))
        {
            int CurrentDistance = Asset->Position().DistanceSquared(pos);

            if ((-1 == BestDistanceSquared) ||
                (CurrentDistance < BestDistanceSquared))
            {
                BestDistanceSquared = CurrentDistance;
                BestAsset = Asset;
            }
        }
    }
    return BestAsset;
}

void CAssetDecoratedMap::RemoveLumber(const CTilePosition &pos,
                                      const CTilePosition &from, int amount)
{
    int Index = 0;

    for (int YOff = 0; YOff < 2; YOff++)
    {
        for (int XOff = 0; XOff < 2; XOff++)
        {
            int XPos = pos.X() + XOff;
            int YPos = pos.Y() + YOff;
            Index |= (ETerrainTileType::Forest == DTerrainMap[YPos][XPos]) &&
                             DPartials[YPos][XPos]
                         ? 1 << (YOff * 2 + XOff)
                         : 0;
        }
    }
    if (Index && (0xF != Index))
    {
        switch (Index)
        {
            case 1:
                Index = 0;
                break;
            case 2:
                Index = 1;
                break;
            case 3:
                Index = from.X() > pos.X() ? 1 : 0;
                break;
            case 4:
                Index = 2;
                break;
            case 5:
                Index = from.Y() < pos.Y() ? 0 : 2;
                break;
            case 6:
                Index = from.Y() > pos.Y() ? 2 : 1;
                break;
            case 7:
                Index = 2;
                break;
            case 8:
                Index = 3;
                break;
            case 9:
                Index = from.Y() > pos.Y() ? 0 : 3;
                break;
            case 10:
                Index = from.Y() > pos.Y() ? 3 : 1;
                break;
            case 11:
                Index = 0;
                break;
            case 12:
                Index = from.X() < pos.X() ? 2 : 3;
                break;
            case 13:
                Index = 3;
                break;
            case 14:
                Index = 1;
                break;
        }
        switch (Index)
        {
            case 0:
                DLumberAvailable[pos.Y()][pos.X()] -= amount;
                if (0 >= DLumberAvailable[pos.Y()][pos.X()])
                {
                    DLumberAvailable[pos.Y()][pos.X()] = 0;
                    ChangeTerrainTilePartial(pos.X(), pos.Y(), 0);
                }
                break;
            case 1:
                DLumberAvailable[pos.Y()][pos.X() + 1] -= amount;
                if (0 >= DLumberAvailable[pos.Y()][pos.X() + 1])
                {
                    DLumberAvailable[pos.Y()][pos.X() + 1] = 0;
                    ChangeTerrainTilePartial(pos.X() + 1, pos.Y(), 0);
                }
                break;
            case 2:
                DLumberAvailable[pos.Y() + 1][pos.X()] -= amount;
                if (0 >= DLumberAvailable[pos.Y() + 1][pos.X()])
                {
                    DLumberAvailable[pos.Y() + 1][pos.X()] = 0;
                    ChangeTerrainTilePartial(pos.X(), pos.Y() + 1, 0);
                }
                break;
            case 3:
                DLumberAvailable[pos.Y() + 1][pos.X() + 1] -= amount;
                if (0 >= DLumberAvailable[pos.Y() + 1][pos.X() + 1])
                {
                    DLumberAvailable[pos.Y() + 1][pos.X() + 1] = 0;
                    ChangeTerrainTilePartial(pos.X() + 1, pos.Y() + 1, 0);
                }
                break;
        }
    }
}

void CAssetDecoratedMap::RemoveStone(const CTilePosition &pos,
                                      const CTilePosition &from, int amount)
{
    int Index = 0;

    for (int YOff = 0; YOff < 2; YOff++)
    {
        for (int XOff = 0; XOff < 2; XOff++)
        {
            int XPos = pos.X() + XOff;
            int YPos = pos.Y() + YOff;
            Index |= (ETerrainTileType::Rock == DTerrainMap[YPos][XPos]) &&
                     DPartials[YPos][XPos]
                     ? 1 << (YOff * 2 + XOff)
                     : 0;
        }
    }
    if (Index && (0xF != Index))
    {
        switch (Index)
        {
            case 1:
                Index = 0;
                break;
            case 2:
                Index = 1;
                break;
            case 3:
                Index = from.X() > pos.X() ? 1 : 0;
                break;
            case 4:
                Index = 2;
                break;
            case 5:
                Index = from.Y() < pos.Y() ? 0 : 2;
                break;
            case 6:
                Index = from.Y() > pos.Y() ? 2 : 1;
                break;
            case 7:
                Index = 2;
                break;
            case 8:
                Index = 3;
                break;
            case 9:
                Index = from.Y() > pos.Y() ? 0 : 3;
                break;
            case 10:
                Index = from.Y() > pos.Y() ? 3 : 1;
                break;
            case 11:
                Index = 0;
                break;
            case 12:
                Index = from.X() < pos.X() ? 2 : 3;
                break;
            case 13:
                Index = 3;
                break;
            case 14:
                Index = 1;
                break;
        }
        switch (Index)
        {
            case 0:
                DStoneAvailable[pos.Y()][pos.X()] -= amount;
                if (0 >= DStoneAvailable[pos.Y()][pos.X()])
                {
                    DStoneAvailable[pos.Y()][pos.X()] = 0;
                    ChangeTerrainTilePartial(pos.X(), pos.Y(), 0);
                }
                break;
            case 1:
                DStoneAvailable[pos.Y()][pos.X() + 1] -= amount;
                if (0 >= DStoneAvailable[pos.Y()][pos.X() + 1])
                {
                    DStoneAvailable[pos.Y()][pos.X() + 1] = 0;
                    ChangeTerrainTilePartial(pos.X() + 1, pos.Y(), 0);
                }
                break;
            case 2:
                DStoneAvailable[pos.Y() + 1][pos.X()] -= amount;
                if (0 >= DStoneAvailable[pos.Y() + 1][pos.X()])
                {
                    DStoneAvailable[pos.Y() + 1][pos.X()] = 0;
                    ChangeTerrainTilePartial(pos.X(), pos.Y() + 1, 0);
                }
                break;
            case 3:
                DStoneAvailable[pos.Y() + 1][pos.X() + 1] -= amount;
                if (0 >= DStoneAvailable[pos.Y() + 1][pos.X() + 1])
                {
                    DStoneAvailable[pos.Y() + 1][pos.X() + 1] = 0;
                    ChangeTerrainTilePartial(pos.X() + 1, pos.Y() + 1, 0);
                }
                break;
        }
    }
}

bool CAssetDecoratedMap::LoadMap(std::shared_ptr<CDataSource> source)
{
    CCommentSkipLineDataSource LineSource(source, '#');
    std::string TempString;
    std::vector<std::string> Tokens;
    SResourceInitialization TempResourceInit;
    SAssetInitialization TempAssetInit;
    int ResourceCount, AssetCount, InitialLumber, InitialStone = 400;
    bool ReturnStatus = false;

    if (!CTerrainMap::LoadMap(source))
    {
        return false;
    }
    try
    {
        if (!LineSource.Read(TempString))
        {
            PrintError("Failed to read map resource count.\n");
            goto LoadMapExit;
        }
        ResourceCount = std::stoi(TempString);
        DResourceInitializationList.clear();
        for (int Index = 0; Index <= ResourceCount; Index++)
        {
            if (!LineSource.Read(TempString))
            {
                PrintError("Failed to read map resource %d.\n", Index);
                goto LoadMapExit;
            }
            CTokenizer::Tokenize(Tokens, TempString);
            if (3 >= Tokens.size())
            {
                PrintError("Too few tokens for resource %d.\n", Index);
                goto LoadMapExit;
            }
            TempResourceInit.DNumber =
                static_cast<EPlayerNumber>(std::stoi(Tokens[0]));
            if ((0 == Index) &&
                (EPlayerNumber::Neutral != TempResourceInit.DNumber))
            {
                PrintError(
                    "Expected first resource to be for player neutral.\n");
                goto LoadMapExit;
            }
            TempResourceInit.DGold = std::stoi(Tokens[1]);
            TempResourceInit.DLumber = std::stoi(Tokens[2]);
            TempResourceInit.DStone = std::stoi(Tokens[3]);

            if (EPlayerNumber::Neutral == TempResourceInit.DNumber)
            {
                InitialLumber = TempResourceInit.DLumber;
                InitialStone = TempResourceInit.DStone;
            }

            DResourceInitializationList.push_back(TempResourceInit);
        }

        if (!LineSource.Read(TempString))
        {
            PrintError("Failed to read map asset count.\n");
            goto LoadMapExit;
        }
        AssetCount = std::stoi(TempString);
        DAssetInitializationList.clear();
        for (int Index = 0; Index < AssetCount; Index++)
        {
            if (!LineSource.Read(TempString))
            {
                PrintError("Failed to read map asset %d.\n", Index);
                goto LoadMapExit;
            }
            CTokenizer::Tokenize(Tokens, TempString);
            if (4 > Tokens.size())
            {
                PrintError("Too few tokens for asset %d.\n", Index);
                goto LoadMapExit;
            }
            TempAssetInit.DType = Tokens[0];
            TempAssetInit.DNumber =
                static_cast<EPlayerNumber>(std::stoi(Tokens[1]));
            TempAssetInit.DTilePosition.X(std::stoi(Tokens[2]));
            TempAssetInit.DTilePosition.Y(std::stoi(Tokens[3]));

            if ((0 > TempAssetInit.DTilePosition.X()) ||
                (0 > TempAssetInit.DTilePosition.Y()))
            {
                PrintError("Invalid resource position %d (%d, %d).\n", Index,
                           TempAssetInit.DTilePosition.X(),
                           TempAssetInit.DTilePosition.Y());
                goto LoadMapExit;
            }
            if ((Width() <= TempAssetInit.DTilePosition.X()) ||
                (Height() <= TempAssetInit.DTilePosition.Y()))
            {
                PrintError("Invalid resource position %d (%d, %d).\n", Index,
                           TempAssetInit.DTilePosition.X(),
                           TempAssetInit.DTilePosition.Y());
                goto LoadMapExit;
            }
            DAssetInitializationList.push_back(TempAssetInit);
        }

        DLumberAvailable.resize(DTerrainMap.size());
        for (int RowIndex = 0; RowIndex < DLumberAvailable.size(); RowIndex++)
        {
            DLumberAvailable[RowIndex].resize(DTerrainMap[RowIndex].size());
            for (int ColIndex = 0; ColIndex < DTerrainMap[RowIndex].size();
                 ColIndex++)
            {
                if (ETerrainTileType::Forest == DTerrainMap[RowIndex][ColIndex])
                {
                    DLumberAvailable[RowIndex][ColIndex] =
                        DPartials[RowIndex][ColIndex] ? InitialLumber : 0;
                }
                else
                {
                    DLumberAvailable[RowIndex][ColIndex] = 0;
                }
            }
        }

        DStoneAvailable.resize(DTerrainMap.size());
        for (int RowIndex = 0; RowIndex < DStoneAvailable.size(); RowIndex++)
        {
            DStoneAvailable[RowIndex].resize(DTerrainMap[RowIndex].size());
            for (int ColIndex = 0; ColIndex < DTerrainMap[RowIndex].size();
                 ColIndex++)
            {
                if (ETerrainTileType::Rock == DTerrainMap[RowIndex][ColIndex])
                {
                    DStoneAvailable[RowIndex][ColIndex] =
                            DPartials[RowIndex][ColIndex] ? InitialStone : 0;
                }
                else
                {
                    DStoneAvailable[RowIndex][ColIndex] = 0;
                }
            }
        }

        ReturnStatus = true;
    }
    catch (std::exception &E)
    {
        PrintError("%s\n", E.what());
    }

LoadMapExit:
    return ReturnStatus;
}

const std::list<std::shared_ptr<CPlayerAsset> > &CAssetDecoratedMap::Assets()
    const
{
    return DAssets;
}

const std::list<CAssetDecoratedMap::SAssetInitialization>
    &CAssetDecoratedMap::AssetInitializationList() const
{
    return DAssetInitializationList;
}

const std::list<CAssetDecoratedMap::SResourceInitialization>
    &CAssetDecoratedMap::ResourceInitializationList() const
{
    return DResourceInitializationList;
}

std::shared_ptr<CAssetDecoratedMap> CAssetDecoratedMap::CreateInitializeMap()
    const
{
    std::shared_ptr<CAssetDecoratedMap> ReturnMap =
        std::make_shared<CAssetDecoratedMap>();

    if (ReturnMap->DMap.size() != DMap.size())
    {
        ReturnMap->DTerrainMap = DTerrainMap;
        ReturnMap->DPartials = DPartials;

        // Initialize to empty grass
        ReturnMap->DMap.resize(DMap.size());
        for (auto &Row : ReturnMap->DMap)
        {
            Row.resize(DMap[0].size());
            for (auto &Cell : Row)
            {
                Cell = ETileType::None;
            }
        }
        ReturnMap->DMapIndices.resize(DMap.size());
        for (auto &Row : ReturnMap->DMapIndices)
        {
            Row.resize(DMapIndices[0].size());
            for (auto &Cell : Row)
            {
                Cell = 0;
            }
        }
    }
    return ReturnMap;
}

std::shared_ptr<CVisibilityMap> CAssetDecoratedMap::CreateVisibilityMap() const
{
    return std::make_shared<CVisibilityMap>(Width(), Height(),
                                            CPlayerAssetType::MaxSight());
}

bool CAssetDecoratedMap::UpdateMap(const CVisibilityMap &vismap,
                                   const CAssetDecoratedMap &resmap)
{
    auto Iterator = DAssets.begin();

    if (DMap.size() != resmap.DMap.size())
    {
        DTerrainMap = resmap.DTerrainMap;
        DPartials = resmap.DPartials;
        DMap.resize(resmap.DMap.size());
        for (auto &Row : DMap)
        {
            Row.resize(resmap.DMap[0].size());
            for (auto &Cell : Row)
            {
                Cell = ETileType::None;
            }
        }
        DMapIndices.resize(resmap.DMapIndices.size());
        for (auto &Row : DMapIndices)
        {
            Row.resize(resmap.DMapIndices[0].size());
            for (auto &Cell : Row)
            {
                Cell = 0;
            }
        }
    }
    while (Iterator != DAssets.end())
    {
        CTilePosition CurPosition = (*Iterator)->TilePosition();
        int AssetSize = (*Iterator)->Size();
        bool RemoveAsset = false;
        if ((*Iterator)->Speed() ||
            (EAssetAction::Decay == (*Iterator)->Action()) ||
            (EAssetAction::Attack == (*Iterator)->Action()))
        {  // Remove all movable units
            Iterator = DAssets.erase(Iterator);
            continue;
        }
        for (int YOff = 0; YOff < AssetSize; YOff++)
        {
            int YPos = CurPosition.Y() + YOff;
            for (int XOff = 0; XOff < AssetSize; XOff++)
            {
                int XPos = CurPosition.X() + XOff;

                CVisibilityMap::ETileVisibility VisType =
                    vismap.TileType(XPos, YPos);
                if ((CVisibilityMap::ETileVisibility::Partial == VisType) ||
                    (CVisibilityMap::ETileVisibility::PartialPartial ==
                     VisType) ||
                    (CVisibilityMap::ETileVisibility::Visible == VisType))
                {  // Remove visible so they can be updated
                    RemoveAsset = EAssetType::None != (*Iterator)->Type();
                    break;
                }
            }
            if (RemoveAsset)
            {
                break;
            }
        }
        if (RemoveAsset)
        {
            Iterator = DAssets.erase(Iterator);
            continue;
        }
        Iterator++;
    }
    for (int YPos = 0; YPos < DMap.size(); YPos++)
    {
        for (int XPos = 0; XPos < DMap[YPos].size(); XPos++)
        {
            CVisibilityMap::ETileVisibility VisType =
                vismap.TileType(XPos - 1, YPos - 1);
            if ((CVisibilityMap::ETileVisibility::Partial == VisType) ||
                (CVisibilityMap::ETileVisibility::PartialPartial == VisType) ||
                (CVisibilityMap::ETileVisibility::Visible == VisType))
            {
                DMap[YPos][XPos] = resmap.DMap[YPos][XPos];
                DMapIndices[YPos][XPos] = resmap.DMapIndices[YPos][XPos];
            }
        }
    }
    for (auto &Asset : resmap.DAssets)
    {
        CTilePosition CurPosition = Asset->TilePosition();
        int AssetSize = Asset->Size();
        bool AddAsset = false;

        for (int YOff = 0; YOff < AssetSize; YOff++)
        {
            int YPos = CurPosition.Y() + YOff;
            for (int XOff = 0; XOff < AssetSize; XOff++)
            {
                int XPos = CurPosition.X() + XOff;

                CVisibilityMap::ETileVisibility VisType =
                    vismap.TileType(XPos, YPos);
                if ((CVisibilityMap::ETileVisibility::Partial == VisType) ||
                    (CVisibilityMap::ETileVisibility::PartialPartial ==
                     VisType) ||
                    (CVisibilityMap::ETileVisibility::Visible == VisType))
                {  // Add visible resources
                    AddAsset = true;
                    break;
                }
            }
            if (AddAsset)
            {
                DAssets.push_back(Asset);
                break;
            }
        }
    }

    return true;
}

#define SEARCH_STATUS_UNVISITED 0
#define SEARCH_STATUS_QUEUED 1
#define SEARCH_STATUS_VISITED 2

typedef struct
{
    int DX;
    int DY;
} SSearchTile, *SSearchTileRef;

CTilePosition CAssetDecoratedMap::FindNearestReachableTileType(
    const CTilePosition &pos, ETileType type)
{
    std::queue<SSearchTile> SearchQueue;
    SSearchTile CurrentSearch, TempSearch;
    int MapWidth = Width();
    int MapHeight = Height();
    int SearchXOffsets[] = {0, 1, 0, -1};
    int SearchYOffsets[] = {-1, 0, 1, 0};

    if (DSearchMap.size() != DMap.size())
    {
        DSearchMap.resize(DMap.size());
        for (auto &Row : DSearchMap)
        {
            Row.resize(DMap[0].size());
            for (auto &Cell : Row)
            {
                Cell = 0;
            }
        }
        int LastYIndex = DMap.size() - 1;
        int LastXIndex = DMap[0].size() - 1;
        for (int Index = 0; Index < DMap.size(); Index++)
        {
            DSearchMap[Index][0] = SEARCH_STATUS_VISITED;
            DSearchMap[Index][LastXIndex] = SEARCH_STATUS_VISITED;
        }
        for (int Index = 1; Index < LastXIndex; Index++)
        {
            DSearchMap[0][Index] = SEARCH_STATUS_VISITED;
            DSearchMap[LastYIndex][Index] = SEARCH_STATUS_VISITED;
        }
    }
    for (int Y = 0; Y < MapHeight; Y++)
    {
        for (int X = 0; X < MapWidth; X++)
        {
            DSearchMap[Y + 1][X + 1] = SEARCH_STATUS_UNVISITED;
        }
    }
    for (auto Asset : DAssets)
    {
        if (Asset->TilePosition() != pos)
        {
            for (int Y = 0; Y < Asset->Size(); Y++)
            {
                for (int X = 0; X < Asset->Size(); X++)
                {
                    DSearchMap[Asset->TilePositionY() + Y + 1]
                              [Asset->TilePositionX() + X + 1] =
                                  SEARCH_STATUS_VISITED;
                }
            }
        }
    }

    CurrentSearch.DX = pos.X() + 1;
    CurrentSearch.DY = pos.Y() + 1;
    SearchQueue.push(CurrentSearch);
    while (SearchQueue.size())
    {
        CurrentSearch = SearchQueue.front();
        SearchQueue.pop();
        DSearchMap[CurrentSearch.DY][CurrentSearch.DX] = SEARCH_STATUS_VISITED;
        for (int Index = 0; Index < (sizeof(SearchXOffsets) / sizeof(int));
             Index++)
        {
            TempSearch.DX = CurrentSearch.DX + SearchXOffsets[Index];
            TempSearch.DY = CurrentSearch.DY + SearchYOffsets[Index];
            if (SEARCH_STATUS_UNVISITED ==
                DSearchMap[TempSearch.DY][TempSearch.DX])
            {
                ETileType CurTileType = DMap[TempSearch.DY][TempSearch.DX];

                DSearchMap[TempSearch.DY][TempSearch.DX] = SEARCH_STATUS_QUEUED;
                if (type == CurTileType)
                {
                    return CTilePosition(TempSearch.DX - 1, TempSearch.DY - 1);
                }
                // if((ETileType::Grass == CurTileType)||(ETileType::Dirt ==
                // CurTileType)||(ETileType::Stump ==
                // CurTileType)||(ETileType::Rubble ==
                // CurTileType)||(ETileType::None == CurTileType)){
                    //RANGERTRACKING
                CPlayerAsset TempAsset;

                if (IsTraversable(CurTileType, TempAsset,false))
                {
                    SearchQueue.push(TempSearch);
                }
            }
        }
    }
    return CTilePosition(-1, -1);
}
