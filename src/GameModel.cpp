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
#include "GameModel.h"
#include <algorithm>
#include <iostream>
#include <unordered_map>
#include "Debug.h"
#include "UnitGrouping.h"
std::unordered_map<int, std::shared_ptr<CPlayerAsset>> CPlayerData::umap;

int RangeToDistanceSquared(int range)
{
    range *= CPosition::TileWidth();
    range *= range;
    range += CPosition::TileWidth() * CPosition::TileWidth();
    return range;
}

CPlayerData::CPlayerData(std::shared_ptr<CAssetDecoratedMap> map,
                         EPlayerNumber number, EPlayerColor color)
{
    DIsAI = true;
    DGameCycle = 0;
    DNumber = number;
    DColor = color;
    DActualMap = map;
    DAssetTypes = CPlayerAssetType::DuplicateRegistry(number, color);
    DPlayerMap = DActualMap->CreateInitializeMap();
    DVisibilityMap = DActualMap->CreateVisibilityMap();
    DGold = 0;
    DLumber = 0;

    DUpgrades.resize(to_underlying(EAssetCapabilityType::Max));
    for (int Index = 0; Index < DUpgrades.size(); Index++)
    {
        DUpgrades[Index] = false;
    }
    for (auto &ResourceInit : DActualMap->ResourceInitializationList())
    {
        if (ResourceInit.DNumber == number)
        {
            DGold = ResourceInit.DGold;
            DLumber = ResourceInit.DLumber;
        }
    }
    for (auto &AssetInit : DActualMap->AssetInitializationList())
    {
        if (AssetInit.DNumber == number)
        {
            PrintDebug(DEBUG_LOW, "Init %s %d (%d, %d)\n",
                       AssetInit.DType.c_str(), AssetInit.DNumber,
                       AssetInit.DTilePosition.X(),
                       AssetInit.DTilePosition.Y());
            std::shared_ptr<CPlayerAsset> InitAsset =
                CreateAsset(AssetInit.DType);
            InitAsset->TilePosition(AssetInit.DTilePosition);
            if (EAssetType::GoldMine ==
                CPlayerAssetType::NameToType(AssetInit.DType))
            {
                InitAsset->Gold(DGold);
            }
        }
    }
}

int CPlayerData::FoodConsumption() const
{
    int TotalConsumption = 0;

    for (auto &WeakAsset : DAssets)
    {
        if (auto Asset = WeakAsset.lock())
        {
            int AssetConsumption = Asset->FoodConsumption();
            if (0 < AssetConsumption)
            {
                TotalConsumption += AssetConsumption;
            }
        }
    }
    return TotalConsumption;
}

int CPlayerData::FoodProduction() const
{
    int TotalProduction = 0;

    for (auto &WeakAsset : DAssets)
    {
        if (auto Asset = WeakAsset.lock())
        {
            int AssetConsumption = Asset->FoodConsumption();
            if ((0 > AssetConsumption) &&
                ((EAssetAction::Construct != Asset->Action()) ||
                 (!Asset->CurrentCommand().DAssetTarget)))
            {
                TotalProduction += -AssetConsumption;
            }
        }
    }
    return TotalProduction;
}

std::shared_ptr<CPlayerAsset> CPlayerData::CreateMarker(
    const CPixelPosition &pos, bool addtomap)
{
    std::shared_ptr<CPlayerAsset> NewMarker =
        (*DAssetTypes)["None"]->Construct();
    CTilePosition TilePosition;

    TilePosition.SetFromPixel(pos);
    NewMarker->TilePosition(TilePosition);
    if (addtomap)
    {
        DPlayerMap->AddAsset(NewMarker);
    }

    return NewMarker;
}

std::shared_ptr<CPlayerAsset> CPlayerData::CreateAsset(
    const std::string &assettypename)
{
    std::shared_ptr<CPlayerAsset> CreatedAsset =
        (*DAssetTypes)[assettypename]->Construct();

    CreatedAsset->CreationCycle(DGameCycle);
    DAssets.push_back(CreatedAsset);
    DActualMap->AddAsset(CreatedAsset);

    //Adds and asset ID to each created player asset.
    CreatedAsset->SetId(CreatedAsset->GetIdCounter());
    umap[CreatedAsset->Id()] = CreatedAsset;
    //std::cout << "Found " << umap.find(CreatedAsset->Id())->first << "\n";
    //std::cout << umap.find(CreatedAsset->Id())->second->Name() << "\n";

    CreatedAsset->IncIdCounter();

    //std::cout << CreatedAsset->GetIdCounter() << "\n";

    return CreatedAsset;
}

void CPlayerData::DeleteAsset(std::shared_ptr<CPlayerAsset> asset)
{
    /*Removes dead asset from any groups*/
    std::shared_ptr<CUnitGrouping> UnitGroups;
    std::list<std::weak_ptr<CPlayerAsset>> SelectedAsset;
    SelectedAsset.push_back(asset);
    UnitGroups->RemoveUnits(SelectedAsset);

    auto Iterator = DAssets.begin();
    while (Iterator != DAssets.end())
    {
        if (Iterator->lock() == asset)
        {
            DAssets.erase(Iterator);
            break;
        }
        Iterator++;
    }
    DActualMap->RemoveAsset(asset);
}

bool CPlayerData::AssetRequirementsMet(const std::string &assettypename)
{
    std::vector<int> AssetCount;

    AssetCount.resize(to_underlying(EAssetType::Max));

    for (auto WeakAsset : DAssets)
    {
        if (auto Asset = WeakAsset.lock())
        {
            if (EAssetAction::Construct != Asset->Action())
            {
                AssetCount[to_underlying(Asset->Type())]++;
            }
        }
    }
    for (auto Requirement : (*DAssetTypes)[assettypename]->AssetRequirements())
    {
        if (0 == AssetCount[to_underlying(Requirement)])
        {
            if ((EAssetType::Keep == Requirement) &&
                (AssetCount[to_underlying(EAssetType::Castle)]))
            {
                continue;
            }
            if ((EAssetType::TownHall == Requirement) &&
                (AssetCount[to_underlying(EAssetType::Keep)] ||
                 AssetCount[to_underlying(EAssetType::Castle)]))
            {
                continue;
            }
            return false;
        }
    }
    return true;
}

void CPlayerData::UpdateVisibility()
{
    std::list<std::shared_ptr<CPlayerAsset>> RemoveList;

    DVisibilityMap->Update(DAssets);
    DPlayerMap->UpdateMap(*DVisibilityMap, *DActualMap);
    for (auto &Asset : DPlayerMap->Assets())
    {
        if ((EAssetType::None == Asset->Type()) &&
            (EAssetAction::None == Asset->Action()))
        {
            Asset->IncrementStep();
            if (CPlayerAsset::UpdateFrequency() < Asset->Step() * 2)
            {
                RemoveList.push_back(Asset);
            }
        }
    }
    for (auto &Asset : RemoveList)
    {
        DPlayerMap->RemoveAsset(Asset);
    }
}

std::list<std::weak_ptr<CPlayerAsset>> CPlayerData::SelectAssets(
    const SRectangle &selectarea, EAssetType assettype, bool selectidentical)
{
    std::list<std::weak_ptr<CPlayerAsset>> ReturnList;

    if ((!selectarea.DWidth) || (!selectarea.DHeight))
    {
        std::weak_ptr<CPlayerAsset> BestAsset = SelectAsset(
            CPixelPosition(selectarea.DXPosition, selectarea.DYPosition),
            assettype);
        if (auto LockedAsset = BestAsset.lock())
        {
            ReturnList.push_back(BestAsset);
            if (selectidentical && LockedAsset->Speed())
            {
                for (auto &WeakAsset : DAssets)
                {
                    if (auto Asset = WeakAsset.lock())
                    {
                        if ((LockedAsset != Asset) &&
                            (Asset->Type() == assettype))
                        {
                            ReturnList.push_back(Asset);
                        }
                    }
                }
            }
        }
    }
    else
    {
        bool AnyMovable = false;
        for (auto &WeakAsset : DAssets)
        {
            if (auto Asset = WeakAsset.lock())
            {
                if ((selectarea.DXPosition <= Asset->PositionX()) &&
                    (Asset->PositionX() <
                     selectarea.DXPosition + selectarea.DWidth) &&
                    (selectarea.DYPosition <= Asset->PositionY()) &&
                    (Asset->PositionY() <
                     selectarea.DYPosition + selectarea.DHeight))
                {
                    if (AnyMovable)
                    {
                        if (Asset->Speed())
                        {
                            ReturnList.push_back(Asset);
                        }
                    }
                    else
                    {
                        if (Asset->Speed())
                        {
                            ReturnList.clear();
                            ReturnList.push_back(Asset);
                            AnyMovable = true;
                        }
                        else
                        {
                            if (ReturnList.empty())
                            {
                                ReturnList.push_back(Asset);
                            }
                        }
                    }
                }
            }
        }
    }
    return ReturnList;
}

std::weak_ptr<CPlayerAsset> CPlayerData::SelectAsset(const CPixelPosition &pos,
                                                     EAssetType assettype)
{
    std::shared_ptr<CPlayerAsset> BestAsset;
    int BestDistanceSquared = -1;

    if (EAssetType::None != assettype)
    {
        for (auto &WeakAsset : DAssets)
        {
            if (auto Asset = WeakAsset.lock())
            {
                if (Asset->Type() == assettype)
                {
                    int CurrentDistance =
                        Asset->Position().DistanceSquared(pos);

                    if ((-1 == BestDistanceSquared) ||
                        (CurrentDistance < BestDistanceSquared))
                    {
                        BestDistanceSquared = CurrentDistance;
                        BestAsset = Asset;
                    }
                }
            }
        }
    }
    return BestAsset;
}

std::weak_ptr<CPlayerAsset> CPlayerData::FindNearestOwnedAsset(
    const CPixelPosition &pos, const std::vector<EAssetType> assettypes)
{
    std::shared_ptr<CPlayerAsset> BestAsset;
    int BestDistanceSquared = -1;

    for (auto &WeakAsset : DAssets)
    {
        if (auto Asset = WeakAsset.lock())
        {
            for (auto &AssetType : assettypes)
            {
                if ((Asset->Type() == AssetType) &&
                    ((EAssetAction::Construct != Asset->Action()) ||
                     (EAssetType::Keep == AssetType) ||
                     (EAssetType::Castle == AssetType)))
                {
                    int CurrentDistance =
                        Asset->Position().DistanceSquared(pos);

                    if ((-1 == BestDistanceSquared) ||
                        (CurrentDistance < BestDistanceSquared))
                    {
                        BestDistanceSquared = CurrentDistance;
                        BestAsset = Asset;
                    }
                    break;
                }
            }
        }
    }
    return BestAsset;
}

std::shared_ptr<CPlayerAsset> CPlayerData::FindNearestAsset(
    const CPixelPosition &pos, EAssetType assettype)
{
    std::shared_ptr<CPlayerAsset> BestAsset;
    int BestDistanceSquared = -1;

    for (auto &Asset : DPlayerMap->Assets())
    {
        if (Asset->Type() == assettype)
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

std::weak_ptr<CPlayerAsset> CPlayerData::FindNearestEnemy(
    const CPixelPosition &pos, int range)
{
    std::shared_ptr<CPlayerAsset> BestAsset;
    int BestDistanceSquared = -1;

    // Assume tile width == tile height
    if (0 < range)
    {
        range = RangeToDistanceSquared(range);
    }
    for (auto Asset : DPlayerMap->Assets())
    {
        if ((Asset->Number() != DNumber) &&
            (Asset->Number() != EPlayerNumber::Neutral) && (Asset->Alive()))
        {
            auto Command = Asset->CurrentCommand();
            if (EAssetAction::Capability == Command.DAction)
            {
                if ((Command.DAssetTarget) &&
                    (EAssetAction::Construct == Command.DAssetTarget->Action()))
                {
                    continue;
                }
            }
            if ((EAssetAction::ConveyGold != Command.DAction) &&
                (EAssetAction::ConveyLumber != Command.DAction) &&
                (EAssetAction::MineGold != Command.DAction))
            {
                int CurrentDistance =
                    Asset->ClosestPosition(pos).DistanceSquared(
                        pos);  // Asset->Position().DistanceSquared(pos);

                if ((0 > range) || (CurrentDistance <= range))
                {
                    if ((-1 == BestDistanceSquared) ||
                        (CurrentDistance < BestDistanceSquared))
                    {
                        BestDistanceSquared = CurrentDistance;
                        BestAsset = Asset;
                    }
                }
            }
        }
    }
    return BestAsset;
}

CTilePosition CPlayerData::FindBestAssetPlacement(
    const CTilePosition &pos, std::shared_ptr<CPlayerAsset> builder,
    EAssetType assettype, int buffer)
{
    auto AssetType = (*DAssetTypes)[CPlayerAssetType::TypeToName(assettype)];
    int PlacementSize = AssetType->Size() + 2 * buffer;
    int MaxDistance = std::max(DPlayerMap->Width(), DPlayerMap->Height());

    for (int Distance = 1; Distance < MaxDistance; Distance++)
    {
        CTilePosition BestPosition;
        int BestDistance = -1;
        int LeftX = pos.X() - Distance;
        int TopY = pos.Y() - Distance;
        int RightX = pos.X() + Distance;
        int BottomY = pos.Y() + Distance;
        bool LeftValid = true;
        bool RightValid = true;
        bool TopValid = true;
        bool BottomValid = true;

        if (0 > LeftX)
        {
            LeftValid = false;
            LeftX = 0;
        }
        if (0 > TopY)
        {
            TopValid = false;
            TopY = 0;
        }
        if (DPlayerMap->Width() <= RightX)
        {
            RightValid = false;
            RightX = DPlayerMap->Width() - 1;
        }
        if (DPlayerMap->Height() <= BottomY)
        {
            BottomValid = false;
            BottomY = DPlayerMap->Height() - 1;
        }
        if (TopValid)
        {
            for (int Index = LeftX; Index <= RightX; Index++)
            {
                CTilePosition TempPosition(Index, TopY);
                if (DPlayerMap->CanPlaceAsset(TempPosition, PlacementSize,
                                              builder))
                {
                    int CurrentDistance =
                        builder->TilePosition().DistanceSquared(TempPosition);
                    if ((-1 == BestDistance) ||
                        (CurrentDistance < BestDistance))
                    {
                        BestDistance = CurrentDistance;
                        BestPosition = TempPosition;
                    }
                }
            }
        }
        if (RightValid)
        {
            for (int Index = TopY; Index <= BottomY; Index++)
            {
                CTilePosition TempPosition(RightX, Index);
                if (DPlayerMap->CanPlaceAsset(TempPosition, PlacementSize,
                                              builder))
                {
                    int CurrentDistance =
                        builder->TilePosition().DistanceSquared(TempPosition);
                    if ((-1 == BestDistance) ||
                        (CurrentDistance < BestDistance))
                    {
                        BestDistance = CurrentDistance;
                        BestPosition = TempPosition;
                    }
                }
            }
        }
        if (BottomValid)
        {
            for (int Index = LeftX; Index <= RightX; Index++)
            {
                CTilePosition TempPosition(Index, BottomY);
                if (DPlayerMap->CanPlaceAsset(TempPosition, PlacementSize,
                                              builder))
                {
                    int CurrentDistance =
                        builder->TilePosition().DistanceSquared(TempPosition);
                    if ((-1 == BestDistance) ||
                        (CurrentDistance < BestDistance))
                    {
                        BestDistance = CurrentDistance;
                        BestPosition = TempPosition;
                    }
                }
            }
        }
        if (LeftValid)
        {
            for (int Index = TopY; Index <= BottomY; Index++)
            {
                CTilePosition TempPosition(LeftX, Index);
                if (DPlayerMap->CanPlaceAsset(TempPosition, PlacementSize,
                                              builder))
                {
                    int CurrentDistance =
                        builder->TilePosition().DistanceSquared(TempPosition);
                    if ((-1 == BestDistance) ||
                        (CurrentDistance < BestDistance))
                    {
                        BestDistance = CurrentDistance;
                        BestPosition = TempPosition;
                    }
                }
            }
        }
        if (-1 != BestDistance)
        {
            return CTilePosition(BestPosition.X() + buffer,
                                 BestPosition.Y() + buffer);
        }
    }

    return CTilePosition(-1, -1);
}

int CPlayerData::PlayerAssetCount(EAssetType type)
{
    int Count = 0;

    for (auto Asset : DPlayerMap->Assets())
    {
        if ((Asset->Number() == DNumber) && (type == Asset->Type()))
        {
            Count++;
        }
    }

    return Count;
}

int CPlayerData::FoundAssetCount(EAssetType type)
{
    int Count = 0;

    for (auto Asset : DPlayerMap->Assets())
    {
        if (type == Asset->Type())
        {
            Count++;
        }
    }

    return Count;
}

std::list<std::weak_ptr<CPlayerAsset>> CPlayerData::IdleAssets() const
{
    std::list<std::weak_ptr<CPlayerAsset>> AssetList;

    for (auto WeakAsset : DAssets)
    {
        if (auto Asset = WeakAsset.lock())
        {
            if ((EAssetAction::None == Asset->Action()) &&
                (EAssetType::None != Asset->Type()))
            {
                AssetList.push_back(Asset);
            }
        }
    }

    return AssetList;
}

void CPlayerData::AddUpgrade(const std::string &upgradename)
{
    auto Upgrade = CPlayerUpgrade::FindUpgradeFromName(upgradename);

    if (Upgrade)
    {
        for (auto AssetType : Upgrade->AffectedAssets())
        {
            std::string AssetName = CPlayerAssetType::TypeToName(AssetType);
            auto AssetIterator = DAssetTypes->find(AssetName);

            if (AssetIterator != DAssetTypes->end())
            {
                AssetIterator->second->AddUpgrade(Upgrade);
            }
        }
        DUpgrades[to_underlying(CPlayerCapability::NameToType(upgradename))] =
            true;
    }
}

CGameModel::CGameModel(
    int mapindex, uint64_t seed,
    const std::array<EPlayerColor, to_underlying(EPlayerNumber::Max)> &newcolors)
{
    DHarvestTime = 5;
    DHarvestSteps = CPlayerAsset::UpdateFrequency() * DHarvestTime;
    DMineTime = 5;
    DMineSteps = CPlayerAsset::UpdateFrequency() * DMineTime;
    DConveyTime = 1;
    DConveySteps = CPlayerAsset::UpdateFrequency() * DConveyTime;
    DDeathTime = 1;
    DDeathSteps = CPlayerAsset::UpdateFrequency() * DDeathTime;
    DDecayTime = 4;
    DDecaySteps = CPlayerAsset::UpdateFrequency() * DDecayTime;
    DLumberPerHarvest = 100;
    DGoldPerMining = 100;

    DRandomNumberGenerator.Seed(seed);
    DAssetsRandomNumberGenerator.Seed(seed);
    DActualMap = CAssetDecoratedMap::DuplicateMap(mapindex);

    for (int PlayerIndex = 0; PlayerIndex < to_underlying(EPlayerNumber::Max);
         PlayerIndex++)
    {
        DPlayers.at(PlayerIndex) = std::make_shared<CPlayerData>(
            DActualMap, static_cast<EPlayerNumber>(PlayerIndex),
            newcolors[PlayerIndex]);
    }
    DAssetOccupancyMap.resize(DActualMap->Height());
    for (auto &Row : DAssetOccupancyMap)
    {
        Row.resize(DActualMap->Width());
    }
    DDiagonalOccupancyMap.resize(DActualMap->Height());
    for (auto &Row : DDiagonalOccupancyMap)
    {
        Row.resize(DActualMap->Width());
    }
}

bool CGameModel::ValidAsset(std::shared_ptr<CPlayerAsset> asset)
{
    for (auto &Asset : DActualMap->Assets())
    {
        if (asset == Asset)
        {
            return true;
        }
    }
    return false;
}

std::shared_ptr<CPlayerData> CGameModel::Player(EPlayerNumber number) const
{
    if ((0 > to_underlying(number)) || (EPlayerNumber::Max <= number))
    {
        return nullptr;
    }
    return DPlayers.at(to_underlying(number));
}

// Clear all player assets in evaluation list
void CGameModel::ClearAssetsEvalList()
{
    DSortedAssets.clear();
}

// Copy and sort all player assets to an evaluation list
void CGameModel::CreateAssetsEvalList()
{
    for (auto &Asset : DActualMap->Assets())
    {
        Asset->SetRandomId(DAssetsRandomNumberGenerator.Random());
        DSortedAssets.push_back(Asset);
    }

    std::sort(DSortedAssets.begin(), DSortedAssets.end(), SortAssets);
}

// Comparison function to sort assets
bool CGameModel::SortAssets(const std::shared_ptr<CPlayerAsset> &a,
                            const std::shared_ptr<CPlayerAsset> &b)
{
    // In the first two cases, always choose the movable asset
    if ((a->Speed() != 0) && (b->Speed() == 0))
    {
        return true;  // a is movable and is in the right place in the list
    }
    else if ((a->Speed() == 0) && (b->Speed() != 0))
    {
        return false;  // b is movable and is not, so a is not in right place
    }
    // Both assets are either movable or immmovable
    else
    {
        // Small chance both assets have the same random ID
        if (a->RandomId() == b->RandomId())
        {
            // Selection based on lowest unique ID
            return a->Id() < b->Id();
        }

        return a->RandomId() < b->RandomId();
    }
}

void CGameModel::Timestep()
{
    std::vector<SGameEvent> CurrentEvents;
    SGameEvent TempEvent;

    for (auto &Row : DAssetOccupancyMap)
    {
        for (auto &Cell : Row)
        {
            Cell = nullptr;
        }
    }

    for (auto &Row : DDiagonalOccupancyMap)
    {
        for (int Index = 0; Index < Row.size(); Index++)
        {
            Row[Index] = false;
        }
    }

    for (auto &Asset : DActualMap->Assets())
    {
        if ((EAssetAction::ConveyGold != Asset->Action()) &&
            (EAssetAction::ConveyLumber != Asset->Action()) &&
            (EAssetAction::MineGold != Asset->Action()))
        {
            DAssetOccupancyMap[Asset->TilePositionY()][Asset->TilePositionX()] =
                Asset;
        }
    }

    for (int PlayerIndex = 1; PlayerIndex < to_underlying(EPlayerColor::Max);
         PlayerIndex++)
    {
        if (DPlayers.at(PlayerIndex)->IsAlive())
        {
            DPlayers.at(PlayerIndex)->UpdateVisibility();
        }
    }

    // Create randomized asset evaluation list
    CreateAssetsEvalList();

    // Put player assets into action
    auto AllAssets = AssetsEvalList();
    for (auto &Asset : AllAssets)
    {
        if (EAssetAction::None == Asset->Action())
        {
            Asset->PopCommand();
        }

        if (EAssetAction::Capability == Asset->Action())
        {
            SAssetCommand Command = Asset->CurrentCommand();
            if (Command.DActivatedCapability)
            {
                if (Command.DActivatedCapability->IncrementStep())
                {
                    // All Done
                }
            }
            else
            {
                auto PlayerCapability =
                    CPlayerCapability::FindCapability(Command.DCapability);

                Asset->PopCommand();
                if (PlayerCapability->CanApply(
                        Asset, DPlayers.at(to_underlying(Asset->Number())),
                        Command.DAssetTarget))
                {
                    PlayerCapability->ApplyCapability(
                        Asset, DPlayers.at(to_underlying(Asset->Number())),
                        Command.DAssetTarget);
                }
                else
                {
                    // Can't apply notify problem
                }
            }
        }
        else if (EAssetAction::HarvestLumber == Asset->Action())
        {
            SAssetCommand Command = Asset->CurrentCommand();
            CTilePosition TilePosition = Command.DAssetTarget->TilePosition();
            EDirection HarvestDirection =
                Asset->TilePosition().AdjacentTileDirection(TilePosition);

            if (CTerrainMap::ETileType::Forest !=
                DActualMap->TileType(TilePosition))
            {
                HarvestDirection = EDirection::Max;
                TilePosition = Asset->TilePosition();
            }
            if (EDirection::Max == HarvestDirection)
            {
                if (TilePosition == Asset->TilePosition())
                {
                    CTilePosition TilePosition =
                        DPlayers.at(to_underlying(Asset->Number()))
                            ->PlayerMap()
                            ->FindNearestReachableTileType(
                                Asset->TilePosition(),
                                CTerrainMap::ETileType::Forest);
                    // Find new lumber
                    Asset->PopCommand();
                    if (0 <= TilePosition.X())
                    {
                        CPixelPosition NewPosition;
                        NewPosition.SetFromTile(TilePosition);
                        Command.DAssetTarget =
                            DPlayers.at(to_underlying(Asset->Number()))
                                ->CreateMarker(NewPosition, false);
                        Asset->PushCommand(Command);
                        Command.DAction = EAssetAction::Walk;
                        Asset->PushCommand(Command);
                        Asset->ResetStep();
                    }
                }
                else
                {
                    SAssetCommand NewCommand = Command;

                    NewCommand.DAction = EAssetAction::Walk;
                    Asset->PushCommand(NewCommand);
                    Asset->ResetStep();
                }
            }
            else
            {
                TempEvent.DType = EEventType::Harvest;
                TempEvent.DAsset = Asset;
                CurrentEvents.push_back(TempEvent);
                Asset->Direction(HarvestDirection);
                Asset->IncrementStep();
                if (DHarvestSteps <= Asset->Step())
                {
                    std::weak_ptr<CPlayerAsset> NearestRepository =
                        DPlayers.at(to_underlying(Asset->Number()))
                            ->FindNearestOwnedAsset(
                                Asset->Position(),
                                {EAssetType::TownHall, EAssetType::Keep,
                                 EAssetType::Castle, EAssetType::LumberMill});

                    DActualMap->RemoveLumber(
                        TilePosition, Asset->TilePosition(), DLumberPerHarvest);

                    if (!NearestRepository.expired())
                    {
                        Command.DAction = EAssetAction::ConveyLumber;
                        Command.DAssetTarget = NearestRepository.lock();
                        Asset->PushCommand(Command);
                        Command.DAction = EAssetAction::Walk;
                        Asset->PushCommand(Command);
                        Asset->Lumber(DLumberPerHarvest);
                        Asset->ResetStep();
                    }
                    else
                    {
                        Asset->PopCommand();
                        Asset->Lumber(DLumberPerHarvest);
                        Asset->ResetStep();
                    }
                }
            }
        }
        else if (EAssetAction::MineGold == Asset->Action())
        {
            SAssetCommand Command = Asset->CurrentCommand();
            CPixelPosition ClosestPosition =
                Command.DAssetTarget->ClosestPosition(Asset->Position());
            CTilePosition TilePosition;
            EDirection MineDirection;

            TilePosition.SetFromPixel(ClosestPosition);
            MineDirection =
                Asset->TilePosition().AdjacentTileDirection(TilePosition);
            if ((EDirection::Max == MineDirection) &&
                (TilePosition != Asset->TilePosition()))
            {
                SAssetCommand NewCommand = Command;

                NewCommand.DAction = EAssetAction::Walk;
                Asset->PushCommand(NewCommand);
                Asset->ResetStep();
            }
            else
            {
                if (0 == Asset->Step())
                {
                    if ((Command.DAssetTarget->CommandCount() + 1) *
                            DGoldPerMining <=
                        Command.DAssetTarget->Gold())
                    {
                        SAssetCommand NewCommand;
                        NewCommand.DAction =
                            EAssetAction::Build;  // Pretend active
                        NewCommand.DAssetTarget = Asset;

                        Command.DAssetTarget->EnqueueCommand(NewCommand);
                        Asset->IncrementStep();
                        Asset->TilePosition(
                            Command.DAssetTarget->TilePosition());
                    }
                    else
                    {
                        // Look for new mine or give up?
                        Asset->PopCommand();
                    }
                }
                else
                {
                    Asset->IncrementStep();
                    if (DMineSteps <= Asset->Step())
                    {
                        std::shared_ptr<CPlayerAsset> OldTarget =
                            Command.DAssetTarget;
                        std::weak_ptr<CPlayerAsset> NearestRepository =
                            DPlayers.at(to_underlying(Asset->Number()))
                                ->FindNearestOwnedAsset(
                                    Asset->Position(),
                                    {EAssetType::TownHall, EAssetType::Keep,
                                     EAssetType::Castle});
                        CTilePosition NextTarget(
                            DPlayers.at(to_underlying(Asset->Number()))
                                    ->PlayerMap()
                                    ->Width() -
                                1,
                            DPlayers.at(to_underlying(Asset->Number()))
                                    ->PlayerMap()
                                    ->Height() -
                                1);

                        Command.DAssetTarget->DecrementGold(DGoldPerMining);
                        Command.DAssetTarget->PopCommand();
                        if (0 >= Command.DAssetTarget->Gold())
                        {
                            SAssetCommand NewCommand;

                            NewCommand.DAction = EAssetAction::Death;
                            Command.DAssetTarget->ClearCommand();
                            Command.DAssetTarget->PushCommand(NewCommand);
                            Command.DAssetTarget->ResetStep();
                        }
                        Asset->Gold(DGoldPerMining);
                        if (!NearestRepository.expired())
                        {
                            Command.DAction = EAssetAction::ConveyGold;
                            Command.DAssetTarget = NearestRepository.lock();
                            Asset->PushCommand(Command);
                            Command.DAction = EAssetAction::Walk;
                            Asset->PushCommand(Command);
                            Asset->ResetStep();
                            NextTarget = Command.DAssetTarget->TilePosition();
                        }
                        else
                        {
                            Asset->PopCommand();
                        }
                        Asset->TilePosition(
                            DPlayers.at(to_underlying(Asset->Number()))
                                ->PlayerMap()
                                ->FindAssetPlacement(Asset, OldTarget,
                                                     NextTarget));
                    }
                }
            }
        }
        else if (EAssetAction::StandGround == Asset->Action())
        {
            SAssetCommand Command = Asset->CurrentCommand();
            auto NewTarget = DPlayers.at(to_underlying(Asset->Number()))
                                 ->FindNearestEnemy(Asset->Position(),
                                                    Asset->EffectiveRange());

            if (NewTarget.expired())
            {
                Command.DAction = EAssetAction::None;
            }
            else
            {
                Command.DAction = EAssetAction::Attack;
                Command.DAssetTarget = NewTarget.lock();
            }
            Asset->PushCommand(Command);
            Asset->ResetStep();
        }
        else if (EAssetAction::Repair == Asset->Action())
        {
            SAssetCommand CurrentCommand = Asset->CurrentCommand();
            if (CurrentCommand.DAssetTarget->Alive())
            {
                EDirection RepairDirection =
                    Asset->TilePosition().AdjacentTileDirection(
                        CurrentCommand.DAssetTarget->TilePosition(),
                        CurrentCommand.DAssetTarget->Size());
                if (EDirection::Max == RepairDirection)
                {
                    SAssetCommand NextCommand = Asset->NextCommand();

                    CurrentCommand.DAction = EAssetAction::Walk;
                    Asset->PushCommand(CurrentCommand);
                    Asset->ResetStep();
                }
                else
                {
                    Asset->Direction(RepairDirection);
                    Asset->IncrementStep();
                    // Assume same movement as attack
                    if (Asset->Step() == Asset->AttackSteps())
                    {
                        if (DPlayers.at(to_underlying(Asset->Number()))->Gold() &&
                            DPlayers.at(to_underlying(Asset->Number()))
                                ->Lumber())
                        {
                            int RepairPoints =
                                (CurrentCommand.DAssetTarget->MaxHitPoints() *
                                 (Asset->AttackSteps() +
                                  Asset->ReloadSteps())) /
                                (CPlayerAsset::UpdateFrequency() *
                                 CurrentCommand.DAssetTarget->BuildTime());

                            if (0 == RepairPoints)
                            {
                                RepairPoints = 1;
                            }
                            DPlayers.at(to_underlying(Asset->Number()))
                                ->DecrementGold(1);
                            DPlayers.at(to_underlying(Asset->Number()))
                                ->DecrementLumber(1);
                            CurrentCommand.DAssetTarget->IncrementHitPoints(
                                RepairPoints);
                            if (CurrentCommand.DAssetTarget->HitPoints() ==
                                CurrentCommand.DAssetTarget->MaxHitPoints())
                            {
                                TempEvent.DType = EEventType::WorkComplete;
                                TempEvent.DAsset = Asset;
                                DPlayers.at(to_underlying(Asset->Number()))
                                    ->AddGameEvent(TempEvent);
                                Asset->PopCommand();
                            }
                        }
                        else
                        {
                            // Stop repair
                            Asset->PopCommand();
                        }
                    }
                    if (Asset->Step() >=
                        (Asset->AttackSteps() + Asset->ReloadSteps()))
                    {
                        Asset->ResetStep();
                    }
                }
            }
            else
            {
                Asset->PopCommand();
            }
        }
        else if (EAssetAction::Attack == Asset->Action())
        {
            SAssetCommand CurrentCommand = Asset->CurrentCommand();
            if (EAssetType::None == Asset->Type())
            {
                CPixelPosition ClosestTargetPosition =
                    CurrentCommand.DAssetTarget->ClosestPosition(
                        Asset->Position());
                CPixelPosition DeltaPosition(
                    ClosestTargetPosition.X() - Asset->PositionX(),
                    ClosestTargetPosition.Y() - Asset->PositionY());
                int Movement = CPosition::TileWidth() * 5 /
                               CPlayerAsset::UpdateFrequency();
                int TargetDistance =
                    Asset->Position().Distance(ClosestTargetPosition);
                int Divisor = (TargetDistance + Movement - 1) / Movement;

                if (Divisor)
                {
                    DeltaPosition.X(DeltaPosition.X() / Divisor);
                    DeltaPosition.Y(DeltaPosition.Y() / Divisor);
                }
                Asset->PositionX(Asset->PositionX() + DeltaPosition.X());
                Asset->PositionY(Asset->PositionY() + DeltaPosition.Y());
                Asset->Direction(
                    Asset->Position().DirectionTo(ClosestTargetPosition));
                if (CPosition::HalfTileWidth() * CPosition::HalfTileHeight() >
                    Asset->Position().DistanceSquared(ClosestTargetPosition))
                {
                    TempEvent.DType = EEventType::MissleHit;
                    TempEvent.DAsset = Asset;
                    CurrentEvents.push_back(TempEvent);

                    if (CurrentCommand.DAssetTarget->Alive())
                    {
                        SAssetCommand TargetCommand =
                            CurrentCommand.DAssetTarget->CurrentCommand();
                        TempEvent.DType = EEventType::Attacked;
                        TempEvent.DAsset = CurrentCommand.DAssetTarget;
                        DPlayers
                            .at(to_underlying(
                                CurrentCommand.DAssetTarget->Number()))
                            ->AddGameEvent(TempEvent);
                        if (EAssetAction::MineGold != TargetCommand.DAction)
                        {
                            if ((EAssetAction::ConveyGold ==
                                 TargetCommand.DAction) ||
                                (EAssetAction::ConveyLumber ==
                                 TargetCommand.DAction))
                            {
                                // Damage the target
                                CurrentCommand.DAssetTarget =
                                    TargetCommand.DAssetTarget;
                            }
                            else if ((EAssetAction::Capability ==
                                      TargetCommand.DAction) &&
                                     TargetCommand.DAssetTarget)
                            {
                                if (CurrentCommand.DAssetTarget->Speed() &&
                                    (EAssetAction::Construct ==
                                     TargetCommand.DAssetTarget->Action()))
                                {
                                    CurrentCommand.DAssetTarget =
                                        TargetCommand.DAssetTarget;
                                }
                            }
                            CurrentCommand.DAssetTarget->DecrementHitPoints(
                                Asset->HitPoints());
                            if (!CurrentCommand.DAssetTarget->Alive())
                            {
                                SAssetCommand Command =
                                    CurrentCommand.DAssetTarget
                                        ->CurrentCommand();

                                TempEvent.DType = EEventType::Death;
                                TempEvent.DAsset = CurrentCommand.DAssetTarget;
                                CurrentEvents.push_back(TempEvent);
                                // Remove constructing
                                if ((EAssetAction::Capability ==
                                     Command.DAction) &&
                                    (Command.DAssetTarget))
                                {
                                    if (EAssetAction::Construct ==
                                        Command.DAssetTarget->Action())
                                    {
                                        DPlayers
                                            .at(to_underlying(
                                                Command.DAssetTarget->Number()))
                                            ->DeleteAsset(Command.DAssetTarget);
                                    }
                                }
                                else if (EAssetAction::Construct ==
                                         Command.DAction)
                                {
                                    if (Command.DAssetTarget)
                                    {
                                        Command.DAssetTarget->ClearCommand();
                                    }
                                }
                                CurrentCommand.DAssetTarget->Direction(
                                    DirectionOpposite(Asset->Direction()));
                                Command.DAction = EAssetAction::Death;
                                CurrentCommand.DAssetTarget->ClearCommand();
                                CurrentCommand.DAssetTarget->PushCommand(
                                    Command);
                                CurrentCommand.DAssetTarget->ResetStep();
                            }
                        }
                    }
                    DPlayers.at(to_underlying(Asset->Number()))
                        ->DeleteAsset(Asset);
                }
            }
            else if (CurrentCommand.DAssetTarget->Alive())
            {
                if (1 == Asset->EffectiveRange())
                {
                    EDirection AttackDirection =
                        Asset->TilePosition().AdjacentTileDirection(
                            CurrentCommand.DAssetTarget->TilePosition(),
                            CurrentCommand.DAssetTarget->Size());
                    if (EDirection::Max == AttackDirection)
                    {
                        SAssetCommand NextCommand = Asset->NextCommand();

                        if (EAssetAction::StandGround != NextCommand.DAction)
                        {
                            CurrentCommand.DAction = EAssetAction::Walk;
                            Asset->PushCommand(CurrentCommand);
                            Asset->ResetStep();
                        }
                        else
                        {
                            Asset->PopCommand();
                        }
                    }
                    else
                    {
                        Asset->Direction(AttackDirection);
                        Asset->IncrementStep();
                        if (Asset->Step() == Asset->AttackSteps())
                        {
                            int Damage =
                                Asset->EffectiveBasicDamage() -
                                CurrentCommand.DAssetTarget->EffectiveArmor();
                            Damage = 0 > Damage ? 0 : Damage;
                            Damage += Asset->EffectivePiercingDamage();
                            if (DRandomNumberGenerator.Random() & 0x1)
                            {  // 50% chance half damage
                                Damage /= 2;
                            }
                            CurrentCommand.DAssetTarget->DecrementHitPoints(
                                Damage);
                            TempEvent.DType = EEventType::MeleeHit;
                            TempEvent.DAsset = Asset;
                            CurrentEvents.push_back(TempEvent);
                            TempEvent.DType = EEventType::Attacked;
                            TempEvent.DAsset = CurrentCommand.DAssetTarget;
                            DPlayers
                                .at(to_underlying(
                                    CurrentCommand.DAssetTarget->Number()))
                                ->AddGameEvent(TempEvent);
                            if (!CurrentCommand.DAssetTarget->Alive())
                            {
                                SAssetCommand Command =
                                    CurrentCommand.DAssetTarget
                                        ->CurrentCommand();

                                TempEvent.DType = EEventType::Death;
                                TempEvent.DAsset = CurrentCommand.DAssetTarget;
                                CurrentEvents.push_back(TempEvent);
                                // Remove constructing
                                if ((EAssetAction::Capability ==
                                     Command.DAction) &&
                                    (Command.DAssetTarget))
                                {
                                    if (EAssetAction::Construct ==
                                        Command.DAssetTarget->Action())
                                    {
                                        DPlayers
                                            .at(to_underlying(
                                                Command.DAssetTarget->Number()))
                                            ->DeleteAsset(Command.DAssetTarget);
                                    }
                                }
                                else if (EAssetAction::Construct ==
                                         Command.DAction)
                                {
                                    if (Command.DAssetTarget)
                                    {
                                        Command.DAssetTarget->ClearCommand();
                                    }
                                }
                                Command.DCapability =
                                    EAssetCapabilityType::None;
                                Command.DAssetTarget = nullptr;
                                Command.DActivatedCapability = nullptr;
                                CurrentCommand.DAssetTarget->Direction(
                                    DirectionOpposite(AttackDirection));
                                Command.DAction = EAssetAction::Death;
                                CurrentCommand.DAssetTarget->ClearCommand();
                                CurrentCommand.DAssetTarget->PushCommand(
                                    Command);
                                CurrentCommand.DAssetTarget->ResetStep();
                            }
                        }
                        if (Asset->Step() >=
                            (Asset->AttackSteps() + Asset->ReloadSteps()))
                        {
                            Asset->ResetStep();
                        }
                    }
                }
                else
                {  // EffectiveRanged

                    CPixelPosition ClosestTargetPosition =
                        CurrentCommand.DAssetTarget->ClosestPosition(
                            Asset->Position());
                    if (ClosestTargetPosition.DistanceSquared(
                            Asset->Position()) >
                        RangeToDistanceSquared(Asset->EffectiveRange()))
                    {
                        SAssetCommand NextCommand = Asset->NextCommand();

                        if (EAssetAction::StandGround != NextCommand.DAction)
                        {
                            CurrentCommand.DAction = EAssetAction::Walk;
                            Asset->PushCommand(CurrentCommand);
                            Asset->ResetStep();
                        }
                        else
                        {
                            Asset->PopCommand();
                        }
                    }
                    else
                    {
                        /*
                        CPosition DeltaPosition(ClosestTargetPosition.X() -
                        Asset->PositionX(), ClosestTargetPosition.Y() -
                        Asset->PositionY()); int DivX = DeltaPosition.X() /
                        CPosition::HalfTileWidth(); int DivY = DeltaPosition.Y()
                        / CPosition::HalfTileHeight(); int Div; EDirection
                        AttackDirection; DivX = 0 > DivX ? -DivX : DivX; DivY =
                        0 > DivY ? -DivY : DivY; Div = DivX > DivY ? DivX :
                        DivY;
                        if(Div){
                            DeltaPosition.X(DeltaPosition.X() / Div);
                            DeltaPosition.Y(DeltaPosition.Y() / Div);
                        }
                        DeltaPosition.IncrementX(CPosition::HalfTileWidth());
                        DeltaPosition.IncrementY(CPosition::HalfTileHeight());
                        if(0 > DeltaPosition.X()){
                            DeltaPosition.X(0);
                        }
                        if(0 > DeltaPosition.Y()){
                            DeltaPosition.Y(0);
                        }
                        if(CPosition::TileWidth() <= DeltaPosition.X()){
                            DeltaPosition.X(CPosition::TileWidth() - 1);
                        }
                        if(CPosition::TileHeight() <= DeltaPosition.Y()){
                            DeltaPosition.Y(CPosition::TileHeight() - 1);
                        }
                        AttackDirection = DeltaPosition.TileOctant();
                        */
                        EDirection AttackDirection =
                            Asset->Position().DirectionTo(
                                ClosestTargetPosition);
                        Asset->Direction(AttackDirection);
                        Asset->IncrementStep();
                        if (Asset->Step() == Asset->AttackSteps())
                        {
                            SAssetCommand AttackCommand;  // Create missle
                            auto ArrowAsset =
                                DPlayers
                                    .at(to_underlying(EPlayerNumber::Neutral))
                                    ->CreateAsset("None");
                            int Damage =
                                Asset->EffectiveBasicDamage() -
                                CurrentCommand.DAssetTarget->EffectiveArmor();
                            Damage = 0 > Damage ? 0 : Damage;
                            Damage += Asset->EffectivePiercingDamage();
                            if (DRandomNumberGenerator.Random() & 0x1)
                            {  // 50% chance half damage
                                Damage /= 2;
                            }

                            TempEvent.DType = EEventType::MissleFire;
                            TempEvent.DAsset = Asset;
                            CurrentEvents.push_back(TempEvent);

                            ArrowAsset->HitPoints(Damage);
                            ArrowAsset->Position(Asset->Position());
                            if (ArrowAsset->PositionX() <
                                ClosestTargetPosition.X())
                            {
                                ArrowAsset->PositionX(
                                    ArrowAsset->PositionX() +
                                    CPosition::HalfTileWidth());
                            }
                            else if (ArrowAsset->PositionX() >
                                     ClosestTargetPosition.X())
                            {
                                ArrowAsset->PositionX(
                                    ArrowAsset->PositionX() -
                                    CPosition::HalfTileWidth());
                            }

                            if (ArrowAsset->PositionY() <
                                ClosestTargetPosition.Y())
                            {
                                ArrowAsset->PositionY(
                                    ArrowAsset->PositionY() +
                                    CPosition::HalfTileHeight());
                            }
                            else if (ArrowAsset->PositionY() >
                                     ClosestTargetPosition.Y())
                            {
                                ArrowAsset->PositionY(
                                    ArrowAsset->PositionY() -
                                    CPosition::HalfTileHeight());
                            }
                            ArrowAsset->Direction(AttackDirection);
                            AttackCommand.DAction = EAssetAction::Construct;
                            AttackCommand.DAssetTarget = Asset;
                            ArrowAsset->PushCommand(AttackCommand);
                            AttackCommand.DAction = EAssetAction::Attack;
                            AttackCommand.DAssetTarget =
                                CurrentCommand.DAssetTarget;
                            ArrowAsset->PushCommand(AttackCommand);
                        }
                        if (Asset->Step() >=
                            (Asset->AttackSteps() + Asset->ReloadSteps()))
                        {
                            Asset->ResetStep();
                        }
                    }
                }
            }
            else
            {
                SAssetCommand NextCommand = Asset->NextCommand();

                Asset->PopCommand();
                if (EAssetAction::StandGround != NextCommand.DAction)
                {
                    auto NewTarget =
                        DPlayers.at(to_underlying(Asset->Number()))
                            ->FindNearestEnemy(Asset->Position(),
                                               Asset->EffectiveSight());

                    if (!NewTarget.expired())
                    {
                        CurrentCommand.DAssetTarget = NewTarget.lock();
                        Asset->PushCommand(CurrentCommand);
                        Asset->ResetStep();
                    }
                }
            }
        }
        else if ((EAssetAction::ConveyLumber == Asset->Action()) ||
                 (EAssetAction::ConveyGold == Asset->Action()))
        {
            Asset->IncrementStep();
            if (DConveySteps <= Asset->Step())
            {
                SAssetCommand Command = Asset->CurrentCommand();
                CTilePosition NextTarget(
                    DPlayers.at(to_underlying(Asset->Number()))
                            ->PlayerMap()
                            ->Width() -
                        1,
                    DPlayers.at(to_underlying(Asset->Number()))
                            ->PlayerMap()
                            ->Height() -
                        1);

                DPlayers.at(to_underlying(Asset->Number()))
                    ->IncrementGold(Asset->Gold());
                DPlayers.at(to_underlying(Asset->Number()))
                    ->IncrementLumber(Asset->Lumber());
                Asset->Gold(0);
                Asset->Lumber(0);
                Asset->PopCommand();
                Asset->ResetStep();
                if (EAssetAction::None != Asset->Action())
                {
                    NextTarget =
                        Asset->CurrentCommand().DAssetTarget->TilePosition();
                }
                Asset->TilePosition(
                    DPlayers.at(to_underlying(Asset->Number()))
                        ->PlayerMap()
                        ->FindAssetPlacement(Asset, Command.DAssetTarget,
                                             NextTarget));
            }
        }
        else if (EAssetAction::Construct == Asset->Action())
        {
            SAssetCommand Command = Asset->CurrentCommand();
            if (Command.DActivatedCapability)
            {
                if (Command.DActivatedCapability->IncrementStep())
                {
                    // All Done
                }
            }
        }
        else if (EAssetAction::Death == Asset->Action())
        {
            Asset->IncrementStep();
            if (Asset->Step() > DDeathSteps)
            {
                if (Asset->Speed())
                {
                    SAssetCommand DecayCommand;
                    // Create corpse
                    auto CorpseAsset =
                        DPlayers.at(to_underlying(EPlayerNumber::Neutral))
                            ->CreateAsset("None");

                    DecayCommand.DAction = EAssetAction::Decay;
                    CorpseAsset->Position(Asset->Position());
                    CorpseAsset->Direction(Asset->Direction());
                    CorpseAsset->PushCommand(DecayCommand);
                }

                DPlayers.at(to_underlying(Asset->Number()))->DeleteAsset(Asset);
            }
        }
        else if (EAssetAction::Decay == Asset->Action())
        {
            Asset->IncrementStep();
            if (Asset->Step() > DDecaySteps)
            {
                DPlayers.at(to_underlying(Asset->Number()))->DeleteAsset(Asset);
            }
        }

        if (EAssetAction::Walk == Asset->Action())
        {
            if (Asset->TileAligned())
            {
                SAssetCommand Command = Asset->CurrentCommand();
                SAssetCommand NextCommand = Asset->NextCommand();
                EDirection TravelDirection;
                CPixelPosition MapTarget =
                    Command.DAssetTarget->ClosestPosition(Asset->Position());

                if (EAssetAction::Attack == NextCommand.DAction)
                {
                    // Check to see if can attack now
                    if (NextCommand.DAssetTarget
                            ->ClosestPosition(Asset->Position())
                            .DistanceSquared(Asset->Position()) <=
                        RangeToDistanceSquared(Asset->EffectiveRange()))
                    {
                        Asset->PopCommand();
                        Asset->ResetStep();
                        continue;
                    }
                }
                TravelDirection = DRouterMap.FindRoute(
                    *DPlayers.at(to_underlying(Asset->Number()))->PlayerMap(),
                    *Asset, MapTarget);
                if (EDirection::Max != TravelDirection)
                {
                    Asset->Direction(TravelDirection);
                }
                else
                {
                    CTilePosition TilePosition;
                    TilePosition.SetFromPixel(MapTarget);
                    if ((TilePosition == Asset->TilePosition()) ||
                        (EDirection::Max !=
                         Asset->TilePosition().AdjacentTileDirection(
                             TilePosition)))
                    {
                        Asset->PopCommand();
                        Asset->ResetStep();
                        continue;
                    }
                    else if (EAssetAction::HarvestLumber == NextCommand.DAction)
                    {
                        TilePosition =
                            DPlayers.at(to_underlying(Asset->Number()))
                                ->PlayerMap()
                                ->FindNearestReachableTileType(
                                    Asset->TilePosition(),
                                    CTerrainMap::ETileType::Forest);
                        // Find new lumber
                        Asset->PopCommand();
                        Asset->PopCommand();
                        if (0 <= TilePosition.X())
                        {
                            CPixelPosition NewPosition;
                            NewPosition.SetFromTile(TilePosition);
                            Command.DAction = EAssetAction::HarvestLumber;
                            Command.DAssetTarget =
                                DPlayers.at(to_underlying(Asset->Number()))
                                    ->CreateMarker(NewPosition, false);
                            Asset->PushCommand(Command);
                            Command.DAction = EAssetAction::Walk;
                            Asset->PushCommand(Command);
                            Asset->ResetStep();
                            continue;
                        }
                    }
                    else
                    {
                        Command.DAction = EAssetAction::None;
                        Asset->PushCommand(Command);
                        Asset->ResetStep();
                        continue;
                    }
                }
            }
            if (!Asset->MoveStep(DAssetOccupancyMap, DDiagonalOccupancyMap))
            {
                Asset->Direction(
                    DirectionOpposite(Asset->Position().TileOctant()));
            }
        }
    }

    ClearAssetsEvalList();

    DGameCycle++;
    for (int PlayerIndex = 0; PlayerIndex < to_underlying(EPlayerNumber::Max);
         PlayerIndex++)
    {
        DPlayers.at(PlayerIndex)->IncrementCycle();
        DPlayers.at(PlayerIndex)->AppendGameEvents(CurrentEvents);
    }
}

void CGameModel::ClearGameEvents()
{
    for (int PlayerIndex = 0; PlayerIndex < to_underlying(EPlayerNumber::Max);
         PlayerIndex++)
    {
        DPlayers.at(PlayerIndex)->ClearGameEvents();
    }
}
