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
    purposes only and this copyright notice does not EAssetType::tempt to claim
   any ownership of this material.
*/
#include "AIPlayer.h"
#include "Debug.h"

CAIPlayer::CAIPlayer(std::shared_ptr<CPlayerData> playerdata, int downsample)
{
    DPlayerData = playerdata;
    DCycle = 0;
    DDownSample = downsample;
}

bool CAIPlayer::SearchMap(SPlayerCommandRequest &command)
{
    auto IdleAssets = DPlayerData->IdleAssets();
    std::shared_ptr<CPlayerAsset> MovableAsset;

    for (auto WeakAsset : IdleAssets)
    {
        if (auto Asset = WeakAsset.lock())
        {
            if (Asset->EffectiveSpeed())
            {
                MovableAsset = Asset;
                break;
            }
        }
    }
    if (MovableAsset)
    {
        CTilePosition UnknownPosition =
            DPlayerData->PlayerMap()->FindNearestReachableTileType(
                MovableAsset->TilePosition(), CTerrainMap::ETileType::None);
        if (0 <= UnknownPosition.X())
        {
            // printf("Unkown Position (%d, %d)\n", UnknownPosition.X(),
            // UnknownPosition.Y());
            command.DAction = EAssetCapabilityType::Move;
            command.DActors.push_back(MovableAsset);
            command.DTargetLocation.SetFromTile(UnknownPosition);
            return true;
        }
    }
    return false;
}

bool CAIPlayer::FindEnemies(SPlayerCommandRequest &command)
{
    std::shared_ptr<CPlayerAsset> TownHallAsset;

    for (auto WeakAsset : DPlayerData->Assets())
    {
        if (auto Asset = WeakAsset.lock())
        {
            if (Asset->HasCapability(EAssetCapabilityType::BuildPeasant))
            {
                TownHallAsset = Asset;
                break;
            }
        }
    }
    if (DPlayerData->FindNearestEnemy(TownHallAsset->Position(), -1).expired())
    {
        return SearchMap(command);
    }
    return false;
}

bool CAIPlayer::AttackEnemies(SPlayerCommandRequest &command)
{
    CPixelPosition AverageLocation(0, 0);

    for (auto WeakAsset : DPlayerData->Assets())
    {
        if (auto Asset = WeakAsset.lock())
        {
            if ((EAssetType::Footman == Asset->Type()) ||
                (EAssetType::Archer == Asset->Type()) ||
                (EAssetType::Ranger == Asset->Type()))
            {
                if (!Asset->HasAction(EAssetAction::Attack))
                {
                    command.DActors.push_back(Asset);
                    AverageLocation.IncrementX(Asset->PositionX());
                    AverageLocation.IncrementY(Asset->PositionY());
                }
            }
        }
    }
    if (command.DActors.size())
    {
        AverageLocation.X(AverageLocation.X() / command.DActors.size());
        AverageLocation.Y(AverageLocation.Y() / command.DActors.size());

        auto TargetEnemy =
            DPlayerData->FindNearestEnemy(AverageLocation, -1).lock();
        if (!TargetEnemy)
        {
            command.DActors.clear();
            return SearchMap(command);
        }
        command.DAction = EAssetCapabilityType::Attack;
        command.DTargetLocation = TargetEnemy->Position();
        command.DTargetNumber = TargetEnemy->Number();
        command.DTargetType = TargetEnemy->Type();
        return true;
    }
    return false;
}

bool CAIPlayer::BuildTownHall(SPlayerCommandRequest &command)
{
    // Build Town Hall
    auto IdleAssets = DPlayerData->IdleAssets();
    std::shared_ptr<CPlayerAsset> BuilderAsset;

    for (auto WeakAsset : IdleAssets)
    {
        if (auto Asset = WeakAsset.lock())
        {
            if (Asset->HasCapability(EAssetCapabilityType::BuildTownHall))
            {
                BuilderAsset = Asset;
                break;
            }
        }
    }
    if (BuilderAsset)
    {
        auto GoldMineAsset = DPlayerData->FindNearestAsset(
            BuilderAsset->Position(), EAssetType::GoldMine);
        CTilePosition Placement = DPlayerData->FindBestAssetPlacement(
            GoldMineAsset->TilePosition(), BuilderAsset, EAssetType::TownHall,
            1);
        if (0 <= Placement.X())
        {
            command.DAction = EAssetCapabilityType::BuildTownHall;
            command.DActors.push_back(BuilderAsset);
            command.DTargetLocation.SetFromTile(Placement);
            return true;
        }
        else
        {
            return SearchMap(command);
        }
    }
    return false;
}

bool CAIPlayer::BuildBuilding(SPlayerCommandRequest &command,
                              EAssetType buildingtype, EAssetType neartype)
{
    std::shared_ptr<CPlayerAsset> BuilderAsset;
    std::shared_ptr<CPlayerAsset> TownHallAsset;
    std::shared_ptr<CPlayerAsset> NearAsset;
    EAssetCapabilityType BuildAction;
    bool AssetIsIdle = false;

    switch (buildingtype)
    {
        case EAssetType::Barracks:
            BuildAction = EAssetCapabilityType::BuildBarracks;
            break;
        case EAssetType::LumberMill:
            BuildAction = EAssetCapabilityType::BuildLumberMill;
            break;
        case EAssetType::Blacksmith:
            BuildAction = EAssetCapabilityType::BuildBlacksmith;
            break;
        default:
            BuildAction = EAssetCapabilityType::BuildFarm;
            break;
    }

    for (auto WeakAsset : DPlayerData->Assets())
    {
        if (auto Asset = WeakAsset.lock())
        {
            if (Asset->HasCapability(BuildAction) && Asset->Interruptible())
            {
                if (!BuilderAsset ||
                    (!AssetIsIdle && (EAssetAction::None == Asset->Action())))
                {
                    BuilderAsset = Asset;
                    AssetIsIdle = EAssetAction::None == Asset->Action();
                }
            }
            if (Asset->HasCapability(EAssetCapabilityType::BuildPeasant))
            {
                TownHallAsset = Asset;
            }
            if (Asset->HasActiveCapability(BuildAction))
            {
                return false;
            }
            if ((neartype == Asset->Type()) &&
                (EAssetAction::Construct != Asset->Action()))
            {
                NearAsset = Asset;
            }
            if (buildingtype == Asset->Type())
            {
                if (EAssetAction::Construct == Asset->Action())
                {
                    return false;
                }
            }
        }
    }
    if ((buildingtype != neartype) && !NearAsset)
    {
        return false;
    }
    if (BuilderAsset)
    {
        auto PlayerCapability = CPlayerCapability::FindCapability(BuildAction);
        CTilePosition SourcePosition = TownHallAsset->TilePosition();
        CTilePosition MapCenter(DPlayerData->PlayerMap()->Width() / 2,
                                DPlayerData->PlayerMap()->Height() / 2);

        if (NearAsset)
        {
            SourcePosition = NearAsset->TilePosition();
        }
        if (MapCenter.X() < SourcePosition.X())
        {
            SourcePosition.DecrementX(TownHallAsset->Size() / 2);
        }
        else if (MapCenter.X() > SourcePosition.X())
        {
            SourcePosition.IncrementX(TownHallAsset->Size() / 2);
        }
        if (MapCenter.Y() < SourcePosition.Y())
        {
            SourcePosition.DecrementY(TownHallAsset->Size() / 2);
        }
        else if (MapCenter.Y() > SourcePosition.Y())
        {
            SourcePosition.IncrementY(TownHallAsset->Size() / 2);
        }

        CTilePosition Placement = DPlayerData->FindBestAssetPlacement(
            SourcePosition, BuilderAsset, buildingtype, 1);
        if (0 > Placement.X())
        {
            return SearchMap(command);
        }
        if (PlayerCapability)
        {
            if (PlayerCapability->CanInitiate(BuilderAsset, DPlayerData))
            {
                if (0 <= Placement.X())
                {
                    command.DAction = BuildAction;
                    command.DActors.push_back(BuilderAsset);
                    command.DTargetLocation.SetFromTile(Placement);
                    return true;
                }
            }
        }
    }

    return false;
}

bool CAIPlayer::ActivatePeasants(SPlayerCommandRequest &command, bool trainmore)
{
    // Mine and build peasants
    // auto IdleAssets = DPlayerData->IdleAssets();
    std::shared_ptr<CPlayerAsset> MiningAsset;
    std::shared_ptr<CPlayerAsset> InterruptibleAsset;
    std::shared_ptr<CPlayerAsset> TownHallAsset;
    int GoldMiners = 0;
    int LumberHarvesters = 0;
    bool SwitchToGold = false;
    bool SwitchToLumber = false;

    for (auto WeakAsset : DPlayerData->Assets())
    {
        if (auto Asset = WeakAsset.lock())
        {
            if (Asset->HasCapability(EAssetCapabilityType::Mine))
            {
                if (!MiningAsset && (EAssetAction::None == Asset->Action()))
                {
                    MiningAsset = Asset;
                }

                if (Asset->HasAction(EAssetAction::MineGold))
                {
                    GoldMiners++;
                    if (Asset->Interruptible() &&
                        (EAssetAction::None != Asset->Action()))
                    {
                        InterruptibleAsset = Asset;
                    }
                }
                else if (Asset->HasAction(EAssetAction::HarvestLumber))
                {
                    LumberHarvesters++;
                    if (Asset->Interruptible() &&
                        (EAssetAction::None != Asset->Action()))
                    {
                        InterruptibleAsset = Asset;
                    }
                }
            }
            if (Asset->HasCapability(EAssetCapabilityType::BuildPeasant) &&
                (EAssetAction::None == Asset->Action()))
            {
                TownHallAsset = Asset;
            }
        }
    }
    if ((2 <= GoldMiners) && (0 == LumberHarvesters))
    {
        SwitchToLumber = true;
    }
    else if ((2 <= LumberHarvesters) && (0 == GoldMiners))
    {
        SwitchToGold = true;
    }

    if (TownHallAsset && (MiningAsset || (InterruptibleAsset &&
                                          (SwitchToLumber || SwitchToGold))))
    {
        if (MiningAsset && (MiningAsset->Lumber() || MiningAsset->Gold()))
        {
            command.DAction = EAssetCapabilityType::Convey;
            command.DTargetNumber = TownHallAsset->Number();
            command.DActors.push_back(MiningAsset);
            command.DTargetType = TownHallAsset->Type();
            command.DTargetLocation = TownHallAsset->Position();
        }
        else
        {
            if (!MiningAsset)
            {
                MiningAsset = InterruptibleAsset;
            }
            auto GoldMineAsset = DPlayerData->FindNearestAsset(
                MiningAsset->Position(), EAssetType::GoldMine);
            if (GoldMiners &&
                ((DPlayerData->Gold() > DPlayerData->Lumber() * 3) ||
                 SwitchToLumber))
            {
                CTilePosition LumberLocation =
                    DPlayerData->PlayerMap()->FindNearestReachableTileType(
                        MiningAsset->TilePosition(),
                        CTerrainMap::ETileType::Forest);
                if (0 <= LumberLocation.X())
                {
                    command.DAction = EAssetCapabilityType::Mine;
                    command.DActors.push_back(MiningAsset);
                    command.DTargetLocation.SetFromTile(LumberLocation);
                }
                else
                {
                    return SearchMap(command);
                }
            }
            else
            {
                command.DAction = EAssetCapabilityType::Mine;
                command.DActors.push_back(MiningAsset);
                command.DTargetType = EAssetType::GoldMine;
                command.DTargetLocation = GoldMineAsset->Position();
            }
        }
        return true;
    }
    else if (TownHallAsset && trainmore)
    {
        auto PlayerCapability = CPlayerCapability::FindCapability(
            EAssetCapabilityType::BuildPeasant);

        if (PlayerCapability)
        {
            if (PlayerCapability->CanApply(TownHallAsset, DPlayerData,
                                           TownHallAsset))
            {
                command.DAction = EAssetCapabilityType::BuildPeasant;
                command.DActors.push_back(TownHallAsset);
                command.DTargetLocation = TownHallAsset->Position();
                return true;
            }
        }
    }
    return false;
}

bool CAIPlayer::ActivateFighters(SPlayerCommandRequest &command)
{
    auto IdleAssets = DPlayerData->IdleAssets();

    for (auto WeakAsset : IdleAssets)
    {
        if (auto Asset = WeakAsset.lock())
        {
            if (Asset->EffectiveSpeed() && (EAssetType::Peasant != Asset->Type()))
            {
                if (!Asset->HasAction(EAssetAction::StandGround) &&
                    !Asset->HasActiveCapability(
                        EAssetCapabilityType::StandGround))
                {
                    command.DActors.push_back(Asset);
                }
            }
        }
    }
    if (command.DActors.size())
    {
        command.DAction = EAssetCapabilityType::StandGround;
        return true;
    }
    return false;
}

bool CAIPlayer::TrainFootman(SPlayerCommandRequest &command)
{
    auto IdleAssets = DPlayerData->IdleAssets();
    std::shared_ptr<CPlayerAsset> TrainingAsset;

    for (auto WeakAsset : IdleAssets)
    {
        if (auto Asset = WeakAsset.lock())
        {
            if (Asset->HasCapability(EAssetCapabilityType::BuildFootman))
            {
                TrainingAsset = Asset;
                break;
            }
        }
    }
    if (TrainingAsset)
    {
        auto PlayerCapability = CPlayerCapability::FindCapability(
            EAssetCapabilityType::BuildFootman);

        if (PlayerCapability)
        {
            if (PlayerCapability->CanApply(TrainingAsset, DPlayerData,
                                           TrainingAsset))
            {
                command.DAction = EAssetCapabilityType::BuildFootman;
                command.DActors.push_back(TrainingAsset);
                command.DTargetLocation = TrainingAsset->Position();
                return true;
            }
        }
    }
    return false;
}

bool CAIPlayer::TrainArcher(SPlayerCommandRequest &command)
{
    auto IdleAssets = DPlayerData->IdleAssets();
    std::shared_ptr<CPlayerAsset> TrainingAsset;
    EAssetCapabilityType BuildType = EAssetCapabilityType::BuildArcher;
    for (auto WeakAsset : IdleAssets)
    {
        if (auto Asset = WeakAsset.lock())
        {
            if (Asset->HasCapability(EAssetCapabilityType::BuildArcher))
            {
                TrainingAsset = Asset;
                BuildType = EAssetCapabilityType::BuildArcher;
                break;
            }
            if (Asset->HasCapability(EAssetCapabilityType::BuildRanger))
            {
                TrainingAsset = Asset;
                BuildType = EAssetCapabilityType::BuildRanger;
                break;
            }
        }
    }
    if (TrainingAsset)
    {
        auto PlayerCapability = CPlayerCapability::FindCapability(BuildType);
        if (PlayerCapability)
        {
            if (PlayerCapability->CanApply(TrainingAsset, DPlayerData,
                                           TrainingAsset))
            {
                command.DAction = BuildType;
                command.DActors.push_back(TrainingAsset);
                command.DTargetLocation = TrainingAsset->Position();
                return true;
            }
        }
    }
    return false;
}

void CAIPlayer::CalculateCommand(SPlayerCommandRequest &command)
{

    command.DAction = EAssetCapabilityType::None;
    command.DActors.clear();
    command.DTargetNumber = EPlayerNumber::Neutral;
    command.DTargetType = EAssetType::None;
    /*
    if ((DCycle % DDownSample) == 0)
    {
        // Do decision

        // Check if there are any units left and return to main menu if no town
        // hall or peasants
        if (0 == DPlayerData->PlayerAssetCount(EAssetType::TownHall) &&
            0 == DPlayerData->PlayerAssetCount(EAssetType::Keep) &&
            0 == DPlayerData->PlayerAssetCount(EAssetType::Castle))
        {
            // TODO: Return to menu
        }

        if (0 == DPlayerData->FoundAssetCount(EAssetType::GoldMine))
        {
            // Search for gold mine
            SearchMap(command);
        }
        else if ((0 == DPlayerData->PlayerAssetCount(EAssetType::TownHall)) &&
                 (0 == DPlayerData->PlayerAssetCount(EAssetType::Keep)) &&
                 (0 == DPlayerData->PlayerAssetCount(EAssetType::Castle)))
        {
            BuildTownHall(command);
        }
        else if (5 > DPlayerData->PlayerAssetCount(EAssetType::Peasant))
        {
            ActivatePeasants(command, true);
        }
        else if (12 > DPlayerData->VisibilityMap()->SeenPercent(100))
        {
            SearchMap(command);
        }
        else
        {
            bool CompletedAction = false;
            int BarracksCount = 0;
            int FootmanCount =
                DPlayerData->PlayerAssetCount(EAssetType::Footman);
            int ArcherCount =
                DPlayerData->PlayerAssetCount(EAssetType::Archer) +
                DPlayerData->PlayerAssetCount(EAssetType::Ranger);

            if (!CompletedAction && (DPlayerData->FoodConsumption() >=
                                     DPlayerData->FoodProduction()))
            {
                CompletedAction =
                    BuildBuilding(command, EAssetType::Farm, EAssetType::Farm);
            }
            if (!CompletedAction)
            {
                CompletedAction = ActivatePeasants(command, false);
            }
            if (!CompletedAction &&
                (0 == (BarracksCount = DPlayerData->PlayerAssetCount(
                           EAssetType::Barracks))))
            {
                CompletedAction = BuildBuilding(command, EAssetType::Barracks,
                                                EAssetType::Farm);
            }
            if (!CompletedAction && (5 > FootmanCount))
            {
                CompletedAction = TrainFootman(command);
            }
            if (!CompletedAction &&
                (0 == DPlayerData->PlayerAssetCount(EAssetType::LumberMill)))
            {
                CompletedAction = BuildBuilding(command, EAssetType::LumberMill,
                                                EAssetType::Barracks);
            }
            if (!CompletedAction && (5 > ArcherCount))
            {
                CompletedAction = TrainArcher(command);
            }
            if (!CompletedAction &&
                DPlayerData->PlayerAssetCount(EAssetType::Footman))
            {
                CompletedAction = FindEnemies(command);
            }
            if (!CompletedAction)
            {
                CompletedAction = ActivateFighters(command);
            }
            if (!CompletedAction && ((5 <= FootmanCount) && (5 <= ArcherCount)))
            {
                CompletedAction = AttackEnemies(command);
            }
        }
    }
    */
    DCycle++;
}
