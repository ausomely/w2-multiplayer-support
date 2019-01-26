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
#include "BattleMode.h"
#include "ApplicationData.h"
#include "Debug.h"
#include "GameOverMenuMode.h"
#include "InGameMenuMode.h"
#include "PixelType.h"
#include "Client.h"

#define PAN_SPEED_MAX 0x100
#define PAN_SPEED_SHIFT 1

template <typename T>
inline bool WeakPtrEquals(const std::weak_ptr<T> &t, const std::weak_ptr<T> &u)
{
    return !t.expired() && t.lock() == u.lock();
}

template <typename T>
inline bool WeakPtrCompare(const std::weak_ptr<T> &t, const std::weak_ptr<T> &u)
{
    return !t.expired() && t.lock() <= u.lock();
}

std::shared_ptr<CBattleMode> CBattleMode::DBattleModePointer;

CBattleMode::CBattleMode(const SPrivateConstructorType &key) {}

//! @brief Loads the map and starts song
void CBattleMode::InitializeChange(std::shared_ptr<CApplicationData> context)
{
    if (context->DActiveGame == false)
    {
        context->LoadGameMap(context->DSelectedMapIndex);
        context->DActiveGame = true;
    }
    context->DSoundLibraryMixer->StopSong();
    context->DSoundLibraryMixer->PlaySong(
        context->DSoundLibraryMixer->FindSong("game1"), context->DMusicVolume);
}

//! @brief Handles keyboard and mouse input while the game is running
void CBattleMode::Input(std::shared_ptr<CApplicationData> context)
{
    int CurrentX, CurrentY;
    bool Panning = false;
    bool ShiftPressed = false;
    EDirection PanningDirection = EDirection::Max;

    CurrentX = context->DCurrentX;
    CurrentY = context->DCurrentY;

    //! Does things based on current keyboard input
    context->DGameModel->ClearGameEvents();
    for (auto Key : context->DPressedKeys)
    {
        if (SGUIKeyType::UpArrow == Key)
        {
            PanningDirection = EDirection::North;
            Panning = true;
        }
        else if (SGUIKeyType::DownArrow == Key)
        {
            PanningDirection = EDirection::South;
            Panning = true;
        }
        else if (SGUIKeyType::LeftArrow == Key)
        {
            PanningDirection = EDirection::West;
            Panning = true;
        }
        else if (SGUIKeyType::RightArrow == Key)
        {
            PanningDirection = EDirection::East;
            Panning = true;
        }
        else if ((SGUIKeyType::LeftShift == Key) ||
                 (SGUIKeyType::RightShift == Key))
        {
            ShiftPressed = true;
        }
    }

    for (auto Key : context->DReleasedKeys)
    {
        // Handle releases
        if (context->DSelectedPlayerAssets.size())
        {
            bool CanMove = true;
            for (auto &Asset : context->DSelectedPlayerAssets)
            {
                if (auto LockedAsset = Asset.lock())
                {
                    if (0 == LockedAsset->Speed())
                    {
                        CanMove = false;
                        break;
                    }
                }
            }

            if (SGUIKeyType::Escape == Key)
            {
                context->DCurrentAssetCapability = EAssetCapabilityType::None;
            }

            if (EAssetCapabilityType::BuildSimple ==
                context->DCurrentAssetCapability)
            {
                auto KeyLookup = context->DBuildHotKeyMap.find(Key);
                // check build
                if (KeyLookup != context->DBuildHotKeyMap.end())
                {
                    auto PlayerCapability =
                        CPlayerCapability::FindCapability(KeyLookup->second);

                    if (PlayerCapability)
                    {
                        auto ActorTarget =
                            context->DSelectedPlayerAssets.front().lock();

                        if (PlayerCapability->CanInitiate(
                                ActorTarget, context->DGameModel->Player(
                                                 context->DPlayerNumber)))
                        {
                            SGameEvent TempEvent;
                            TempEvent.DType = EEventType::ButtonTick;
                            context->DGameModel->Player(context->DPlayerNumber)
                                ->AddGameEvent(TempEvent);

                            context->DCurrentAssetCapability =
                                KeyLookup->second;
                        }
                    }
                }
            }
            else if (CanMove)
            {
                auto KeyLookup = context->DUnitHotKeyMap.find(Key);

                if (KeyLookup != context->DUnitHotKeyMap.end())
                {
                    bool HasCapability = true;
                    for (auto &Asset : context->DSelectedPlayerAssets)
                    {
                        if (auto LockedAsset = Asset.lock())
                        {
                            if (!LockedAsset->HasCapability(KeyLookup->second))
                            {
                                HasCapability = false;
                                break;
                            }
                        }
                    }
                    if (HasCapability)
                    {
                        auto PlayerCapability =
                            CPlayerCapability::FindCapability(
                                KeyLookup->second);
                        SGameEvent TempEvent;
                        TempEvent.DType = EEventType::ButtonTick;
                        context->DGameModel->Player(context->DPlayerNumber)
                            ->AddGameEvent(TempEvent);

                        if (PlayerCapability)
                        {
                            if ((CPlayerCapability::ETargetType::None ==
                                 PlayerCapability->TargetType()) ||
                                (CPlayerCapability::ETargetType::Player ==
                                 PlayerCapability->TargetType()))
                            {
                                auto ActorTarget =
                                    context->DSelectedPlayerAssets.front()
                                        .lock();

                                if (PlayerCapability->CanApply(
                                        ActorTarget,
                                        context->DGameModel->Player(
                                            context->DPlayerNumber),
                                        ActorTarget))
                                {
                                    context
                                        ->DPlayerCommands[to_underlying(
                                            context->DPlayerNumber)]
                                        .DAction = KeyLookup->second;
                                    context
                                        ->DPlayerCommands[to_underlying(
                                            context->DPlayerNumber)]
                                        .DActors =
                                        context->DSelectedPlayerAssets;
                                    context
                                        ->DPlayerCommands[to_underlying(
                                            context->DPlayerNumber)]
                                        .DTargetNumber = EPlayerNumber::Neutral;
                                    context
                                        ->DPlayerCommands[to_underlying(
                                            context->DPlayerNumber)]
                                        .DTargetType = EAssetType::None;
                                    context
                                        ->DPlayerCommands[to_underlying(
                                            context->DPlayerNumber)]
                                        .DTargetLocation =
                                        ActorTarget->Position();
                                    context->DCurrentAssetCapability =
                                        EAssetCapabilityType::None;
                                }
                            }
                            else
                            {
                                context->DCurrentAssetCapability =
                                    KeyLookup->second;
                            }
                        }
                        else
                        {
                            context->DCurrentAssetCapability =
                                KeyLookup->second;
                        }
                    }
                }
            }
            else
            {
                auto KeyLookup = context->DTrainHotKeyMap.find(Key);

                if (KeyLookup != context->DTrainHotKeyMap.end())
                {
                    bool HasCapability = true;
                    for (auto &Asset : context->DSelectedPlayerAssets)
                    {
                        if (auto LockedAsset = Asset.lock())
                        {
                            if (!LockedAsset->HasCapability(KeyLookup->second))
                            {
                                HasCapability = false;
                                break;
                            }
                        }
                    }
                    if (HasCapability)
                    {
                        auto PlayerCapability =
                            CPlayerCapability::FindCapability(
                                KeyLookup->second);
                        SGameEvent TempEvent;
                        TempEvent.DType = EEventType::ButtonTick;
                        context->DGameModel->Player(context->DPlayerNumber)
                            ->AddGameEvent(TempEvent);

                        if (PlayerCapability)
                        {
                            if ((CPlayerCapability::ETargetType::None ==
                                 PlayerCapability->TargetType()) ||
                                (CPlayerCapability::ETargetType::Player ==
                                 PlayerCapability->TargetType()))
                            {
                                auto ActorTarget =
                                    context->DSelectedPlayerAssets.front()
                                        .lock();

                                if (PlayerCapability->CanApply(
                                        ActorTarget,
                                        context->DGameModel->Player(
                                            context->DPlayerNumber),
                                        ActorTarget))
                                {
                                    context
                                        ->DPlayerCommands[to_underlying(
                                            context->DPlayerNumber)]
                                        .DAction = KeyLookup->second;
                                    context
                                        ->DPlayerCommands[to_underlying(
                                            context->DPlayerNumber)]
                                        .DActors =
                                        context->DSelectedPlayerAssets;
                                    context
                                        ->DPlayerCommands[to_underlying(
                                            context->DPlayerNumber)]
                                        .DTargetNumber = EPlayerNumber::Neutral;
                                    context
                                        ->DPlayerCommands[to_underlying(
                                            context->DPlayerNumber)]
                                        .DTargetType = EAssetType::None;
                                    context
                                        ->DPlayerCommands[to_underlying(
                                            context->DPlayerNumber)]
                                        .DTargetLocation =
                                        ActorTarget->Position();
                                    context->DCurrentAssetCapability =
                                        EAssetCapabilityType::None;
                                }
                            }
                            else
                            {
                                context->DCurrentAssetCapability =
                                    KeyLookup->second;
                            }
                        }
                        else
                        {
                            context->DCurrentAssetCapability =
                                KeyLookup->second;
                        }
                    }
                }
            }
        }
    }
    context->DReleasedKeys.clear();

    context->DMenuButtonState = CButtonRenderer::EButtonState::None;
    CApplicationData::EUIComponentType ComponentType =
        context->FindUIComponentType(CPixelPosition(CurrentX, CurrentY));

    /*! Series of checks to see which UI element is selected
     * Check if UI elements such as buttons, player assets, overall game map,
     * menu button, etc. are selected. Set properties for that UI element's
     * state
     */
    if (CApplicationData::uictViewport == ComponentType)
    {
        CPixelPosition TempPosition(
            context->ScreenToDetailedMap(CPixelPosition(CurrentX, CurrentY)));
        CPixelPosition ViewPortPosition =
            context->ScreenToViewport(CPixelPosition(CurrentX, CurrentY));
        CPixelType PixelType = CPixelType::GetPixelType(
            context->DViewportTypeSurface, ViewPortPosition);

        if (context->DRightClick && !context->DRightDown &&
            context->DSelectedPlayerAssets.size())
        {
            bool CanMove = true;

            for (auto &Asset : context->DSelectedPlayerAssets)
            {
                if (auto LockedAsset = Asset.lock())
                {
                    if (0 == LockedAsset->Speed())
                    {
                        CanMove = false;
                        break;
                    }
                }
            }
            if (CanMove)
            {
                if (EPlayerNumber::Neutral != PixelType.Number())
                {
                    //! Command is either walk/deliver, repair, or attack

                    context
                        ->DPlayerCommands[to_underlying(context->DPlayerNumber)]
                        .DAction = EAssetCapabilityType::Move;
                    context
                        ->DPlayerCommands[to_underlying(context->DPlayerNumber)]
                        .DTargetNumber = PixelType.Number();
                    context
                        ->DPlayerCommands[to_underlying(context->DPlayerNumber)]
                        .DTargetType = PixelType.AssetType();
                    context
                        ->DPlayerCommands[to_underlying(context->DPlayerNumber)]
                        .DActors = context->DSelectedPlayerAssets;
                    context
                        ->DPlayerCommands[to_underlying(context->DPlayerNumber)]
                        .DTargetLocation = TempPosition;
                    if (PixelType.Number() == context->DPlayerNumber)
                    {
                        bool HaveLumber = false;
                        bool HaveGold = false;

                        for (auto &Asset : context->DSelectedPlayerAssets)
                        {
                            if (auto LockedAsset = Asset.lock())
                            {
                                if (LockedAsset->Lumber())
                                {
                                    HaveLumber = true;
                                }
                                if (LockedAsset->Gold())
                                {
                                    HaveGold = true;
                                }
                            }
                        }
                        if (HaveGold)
                        {
                            if ((EAssetType::TownHall ==
                                 context
                                     ->DPlayerCommands[to_underlying(
                                         context->DPlayerNumber)]
                                     .DTargetType) ||
                                (EAssetType::Keep ==
                                 context
                                     ->DPlayerCommands[to_underlying(
                                         context->DPlayerNumber)]
                                     .DTargetType) ||
                                (EAssetType::Castle ==
                                 context
                                     ->DPlayerCommands[to_underlying(
                                         context->DPlayerNumber)]
                                     .DTargetType))
                            {
                                context
                                    ->DPlayerCommands[to_underlying(
                                        context->DPlayerNumber)]
                                    .DAction = EAssetCapabilityType::Convey;
                            }
                        }
                        else if (HaveLumber)
                        {
                            if ((EAssetType::TownHall ==
                                 context
                                     ->DPlayerCommands[to_underlying(
                                         context->DPlayerNumber)]
                                     .DTargetType) ||
                                (EAssetType::Keep ==
                                 context
                                     ->DPlayerCommands[to_underlying(
                                         context->DPlayerNumber)]
                                     .DTargetType) ||
                                (EAssetType::Castle ==
                                 context
                                     ->DPlayerCommands[to_underlying(
                                         context->DPlayerNumber)]
                                     .DTargetType) ||
                                (EAssetType::LumberMill ==
                                 context
                                     ->DPlayerCommands[to_underlying(
                                         context->DPlayerNumber)]
                                     .DTargetType))
                            {
                                context
                                    ->DPlayerCommands[to_underlying(
                                        context->DPlayerNumber)]
                                    .DAction = EAssetCapabilityType::Convey;
                            }
                        }
                        else
                        {
                            auto TargetAsset =
                                context->DGameModel
                                    ->Player(context->DPlayerNumber)
                                    ->SelectAsset(TempPosition,
                                                  PixelType.AssetType())
                                    .lock();
                            if ((0 == TargetAsset->Speed()) &&
                                (TargetAsset->MaxHitPoints() >
                                 TargetAsset->HitPoints()))
                            {
                                context
                                    ->DPlayerCommands[to_underlying(
                                        context->DPlayerNumber)]
                                    .DAction = EAssetCapabilityType::Repair;
                            }
                        }
                    }
                    else
                    {
                        context
                            ->DPlayerCommands[to_underlying(
                                context->DPlayerNumber)]
                            .DAction = EAssetCapabilityType::Attack;
                    }
                    context->DCurrentAssetCapability =
                        EAssetCapabilityType::None;
                }
                else
                {
                    // Command is either walk, mine, harvest
                    CPixelPosition TempPosition(context->ScreenToDetailedMap(
                        CPixelPosition(CurrentX, CurrentY)));
                    bool CanHarvest = true;

                    context
                        ->DPlayerCommands[to_underlying(context->DPlayerNumber)]
                        .DAction = EAssetCapabilityType::Move;
                    context
                        ->DPlayerCommands[to_underlying(context->DPlayerNumber)]
                        .DTargetNumber = EPlayerNumber::Neutral;
                    context
                        ->DPlayerCommands[to_underlying(context->DPlayerNumber)]
                        .DTargetType = EAssetType::None;
                    context
                        ->DPlayerCommands[to_underlying(context->DPlayerNumber)]
                        .DActors = context->DSelectedPlayerAssets;
                    context
                        ->DPlayerCommands[to_underlying(context->DPlayerNumber)]
                        .DTargetLocation = TempPosition;

                    for (auto &Asset : context->DSelectedPlayerAssets)
                    {
                        if (auto LockedAsset = Asset.lock())
                        {
                            if (!LockedAsset->HasCapability(
                                    EAssetCapabilityType::Mine))
                            {
                                CanHarvest = false;
                                break;
                            }
                        }
                    }
                    if (CanHarvest)
                    {
                        if (CPixelType::EAssetTerrainType::Tree ==
                            PixelType.Type())
                        {
                            CTilePosition TempTilePosition;

                            context
                                ->DPlayerCommands[to_underlying(
                                    context->DPlayerNumber)]
                                .DAction = EAssetCapabilityType::Mine;
                            TempTilePosition.SetFromPixel(
                                context
                                    ->DPlayerCommands[to_underlying(
                                        context->DPlayerNumber)]
                                    .DTargetLocation);
                            if (CTerrainMap::ETileType::Forest !=
                                context->DGameModel
                                    ->Player(context->DPlayerNumber)
                                    ->PlayerMap()
                                    ->TileType(TempTilePosition))
                            {
                                // Could be tree pixel, but tops of next row
                                TempTilePosition.IncrementY(1);
                                if (CTerrainMap::ETileType::Forest ==
                                    context->DGameModel
                                        ->Player(context->DPlayerNumber)
                                        ->PlayerMap()
                                        ->TileType(TempTilePosition))
                                {
                                    context
                                        ->DPlayerCommands[to_underlying(
                                            context->DPlayerNumber)]
                                        .DTargetLocation.SetFromTile(
                                            TempTilePosition);
                                }
                            }
                        }
                        else if (CPixelType::EAssetTerrainType::GoldMine ==
                                 PixelType.Type())
                        {
                            context
                                ->DPlayerCommands[to_underlying(
                                    context->DPlayerNumber)]
                                .DAction = EAssetCapabilityType::Mine;
                            context
                                ->DPlayerCommands[to_underlying(
                                    context->DPlayerNumber)]
                                .DTargetType = EAssetType::GoldMine;
                        }
                    }
                    context->DCurrentAssetCapability =
                        EAssetCapabilityType::None;
                }
            }
        }
        else if (context->DLeftClick)
        {
            if ((EAssetCapabilityType::None ==
                 context->DCurrentAssetCapability) ||
                (EAssetCapabilityType::BuildSimple ==
                 context->DCurrentAssetCapability))
            {
                if (context->DLeftDown)
                {
                    context->DMouseDown = TempPosition;
                }
                else
                {
                    SRectangle TempRectangle;
                    EPlayerNumber SearchNumber = context->DPlayerNumber;
                    std::list<std::shared_ptr<CPlayerAsset> > PreviousSelections;

                    for (auto WeakAsset : context->DSelectedPlayerAssets)
                    {
                        if (auto LockedAsset = WeakAsset.lock())
                        {
                            PreviousSelections.push_back(LockedAsset);
                        }
                    }

                    TempRectangle.DXPosition =
                        std::min(context->DMouseDown.X(), TempPosition.X());
                    TempRectangle.DYPosition =
                        std::min(context->DMouseDown.Y(), TempPosition.Y());
                    TempRectangle.DWidth =
                        std::max(context->DMouseDown.X(), TempPosition.X()) -
                        TempRectangle.DXPosition;
                    TempRectangle.DHeight =
                        std::max(context->DMouseDown.Y(), TempPosition.Y()) -
                        TempRectangle.DYPosition;

                    if ((TempRectangle.DWidth < CPosition::TileWidth()) ||
                        (TempRectangle.DHeight < CPosition::TileHeight()) ||
                        (2 == context->DLeftClick))
                    {
                        TempRectangle.DXPosition = TempPosition.X();
                        TempRectangle.DYPosition = TempPosition.Y();
                        TempRectangle.DWidth = 0;
                        TempRectangle.DHeight = 0;
                        SearchNumber = PixelType.Number();
                    }
                    if (SearchNumber != context->DPlayerNumber)
                    {
                        context->DSelectedPlayerAssets.clear();
                    }
                    if (ShiftPressed)
                    {
                        if (!context->DSelectedPlayerAssets.empty())
                        {
                            if (auto TempAsset =
                                    context->DSelectedPlayerAssets.front()
                                        .lock())
                            {
                                if (TempAsset->Number() !=
                                    context->DPlayerNumber)
                                {
                                    context->DSelectedPlayerAssets.clear();
                                }
                            }
                        }
                        context->DSelectedPlayerAssets.splice(
                            context->DSelectedPlayerAssets.end(),
                            context->DGameModel->Player(SearchNumber)
                                ->SelectAssets(TempRectangle,
                                               PixelType.AssetType(),
                                               2 == context->DLeftClick));
                        context->DSelectedPlayerAssets.sort(
                            WeakPtrCompare<CPlayerAsset>);
                        context->DSelectedPlayerAssets.unique(
                            WeakPtrEquals<CPlayerAsset>);
                    }
                    else
                    {
                        PreviousSelections.clear();
                        context->DSelectedPlayerAssets =
                            context->DGameModel->Player(SearchNumber)
                                ->SelectAssets(TempRectangle,
                                               PixelType.AssetType(),
                                               2 == context->DLeftClick);
                    }
                    for (auto WeakAsset : context->DSelectedPlayerAssets)
                    {
                        if (auto LockedAsset = WeakAsset.lock())
                        {
                            bool FoundPrevious = false;
                            for (auto PrevAsset : PreviousSelections)
                            {
                                if (PrevAsset == LockedAsset)
                                {
                                    FoundPrevious = true;
                                    break;
                                }
                            }
                            if (!FoundPrevious)
                            {
                                SGameEvent TempEvent;
                                TempEvent.DType = EEventType::Selection;
                                TempEvent.DAsset = LockedAsset;
                                context->DGameModel
                                    ->Player(context->DPlayerNumber)
                                    ->AddGameEvent(TempEvent);
                            }
                        }
                    }

                    context->DMouseDown = CPixelPosition(-1, -1);
                }
                context->DCurrentAssetCapability = EAssetCapabilityType::None;
            }
            else
            {
                auto PlayerCapability = CPlayerCapability::FindCapability(
                    context->DCurrentAssetCapability);

                if (PlayerCapability && !context->DLeftDown)
                {
                    if (((CPlayerCapability::ETargetType::Asset ==
                          PlayerCapability->TargetType()) ||
                         (CPlayerCapability::ETargetType::TerrainOrAsset ==
                          PlayerCapability->TargetType())) &&
                        (EAssetType::None != PixelType.AssetType()))
                    {
                        auto NewTarget =
                            context->DGameModel->Player(PixelType.Number())
                                ->SelectAsset(TempPosition,
                                              PixelType.AssetType())
                                .lock();

                        if (PlayerCapability->CanApply(
                                context->DSelectedPlayerAssets.front().lock(),
                                context->DGameModel->Player(
                                    context->DPlayerNumber),
                                NewTarget))
                        {
                            SGameEvent TempEvent;
                            TempEvent.DType = EEventType::PlaceAction;
                            TempEvent.DAsset = NewTarget;
                            context->DGameModel->Player(context->DPlayerNumber)
                                ->AddGameEvent(TempEvent);

                            context
                                ->DPlayerCommands[to_underlying(
                                    context->DPlayerNumber)]
                                .DAction = context->DCurrentAssetCapability;
                            context
                                ->DPlayerCommands[to_underlying(
                                    context->DPlayerNumber)]
                                .DActors = context->DSelectedPlayerAssets;
                            context
                                ->DPlayerCommands[to_underlying(
                                    context->DPlayerNumber)]
                                .DTargetNumber = PixelType.Number();
                            context
                                ->DPlayerCommands[to_underlying(
                                    context->DPlayerNumber)]
                                .DTargetType = PixelType.AssetType();
                            context
                                ->DPlayerCommands[to_underlying(
                                    context->DPlayerNumber)]
                                .DTargetLocation = TempPosition;
                            context->DCurrentAssetCapability =
                                EAssetCapabilityType::None;
                        }
                    }
                    else if (((CPlayerCapability::ETargetType::Terrain ==
                               PlayerCapability->TargetType()) ||
                              (CPlayerCapability::ETargetType::TerrainOrAsset ==
                               PlayerCapability->TargetType())) &&
                             ((EAssetType::None == PixelType.AssetType()) &&
                              (EPlayerNumber::Neutral == PixelType.Number())))
                    {
                        auto NewTarget =
                            context->DGameModel->Player(context->DPlayerNumber)
                                ->CreateMarker(TempPosition, false);

                        if (PlayerCapability->CanApply(
                                context->DSelectedPlayerAssets.front().lock(),
                                context->DGameModel->Player(
                                    context->DPlayerNumber),
                                NewTarget))
                        {
                            SGameEvent TempEvent;
                            TempEvent.DType = EEventType::PlaceAction;
                            TempEvent.DAsset = NewTarget;
                            context->DGameModel->Player(context->DPlayerNumber)
                                ->AddGameEvent(TempEvent);

                            context
                                ->DPlayerCommands[to_underlying(
                                    context->DPlayerNumber)]
                                .DAction = context->DCurrentAssetCapability;
                            context
                                ->DPlayerCommands[to_underlying(
                                    context->DPlayerNumber)]
                                .DActors = context->DSelectedPlayerAssets;
                            context
                                ->DPlayerCommands[to_underlying(
                                    context->DPlayerNumber)]
                                .DTargetNumber = EPlayerNumber::Neutral;
                            context
                                ->DPlayerCommands[to_underlying(
                                    context->DPlayerNumber)]
                                .DTargetType = EAssetType::None;
                            context
                                ->DPlayerCommands[to_underlying(
                                    context->DPlayerNumber)]
                                .DTargetLocation = TempPosition;
                            context->DCurrentAssetCapability =
                                EAssetCapabilityType::None;
                        }
                    }
                    else
                    {
                    }
                }
            }
        }
    }
    else if (CApplicationData::uictViewportBevelN == ComponentType)
    {
        PanningDirection = EDirection::North;
        Panning = true;
    }
    else if (CApplicationData::uictViewportBevelE == ComponentType)
    {
        PanningDirection = EDirection::East;
        Panning = true;
    }
    else if (CApplicationData::uictViewportBevelS == ComponentType)
    {
        PanningDirection = EDirection::South;
        Panning = true;
    }
    else if (CApplicationData::uictViewportBevelW == ComponentType)
    {
        PanningDirection = EDirection::West;
        Panning = true;
    }
    else if (CApplicationData::uictMiniMap == ComponentType)
    {
        if (context->DLeftClick && !context->DLeftDown)
        {
            CPixelPosition TempPosition(
                context->ScreenToMiniMap(CPixelPosition(CurrentX, CurrentY)));
            TempPosition = context->MiniMapToDetailedMap(TempPosition);

            context->DViewportRenderer->CenterViewport(TempPosition);
        }
    }
    else if (CApplicationData::uictUserDescription == ComponentType)
    {
        if (context->DLeftClick && !context->DLeftDown)
        {
            int IconPressed = context->DUnitDescriptionRenderer->Selection(
                context->ScreenToUnitDescription(
                    CPixelPosition(CurrentX, CurrentY)));

            if (1 == context->DSelectedPlayerAssets.size())
            {
                if (0 == IconPressed)
                {
                    if (auto Asset =
                            context->DSelectedPlayerAssets.front().lock())
                    {
                        context->DViewportRenderer->CenterViewport(
                            Asset->Position());
                    }
                }
            }
            else if (0 <= IconPressed)
            {
                while (IconPressed)
                {
                    IconPressed--;
                    context->DSelectedPlayerAssets.pop_front();
                }
                while (1 < context->DSelectedPlayerAssets.size())
                {
                    context->DSelectedPlayerAssets.pop_back();
                }
                SGameEvent TempEvent;
                TempEvent.DType = EEventType::Selection;
                TempEvent.DAsset =
                    context->DSelectedPlayerAssets.front().lock();
                context->DGameModel->Player(context->DPlayerNumber)
                    ->AddGameEvent(TempEvent);
            }
        }
    }
    else if (CApplicationData::uictUserAction == ComponentType)
    {
        if (context->DLeftClick && !context->DLeftDown)
        {
            EAssetCapabilityType CapabilityType =
                context->DUnitActionRenderer->Selection(
                    context->ScreenToUnitAction(
                        CPixelPosition(CurrentX, CurrentY)));
            auto PlayerCapability =
                CPlayerCapability::FindCapability(CapabilityType);

            if (EAssetCapabilityType::None != CapabilityType)
            {
                SGameEvent TempEvent;
                TempEvent.DType = EEventType::ButtonTick;
                context->DGameModel->Player(context->DPlayerNumber)
                    ->AddGameEvent(TempEvent);
            }
            if (PlayerCapability)
            {
                if ((CPlayerCapability::ETargetType::None ==
                     PlayerCapability->TargetType()) ||
                    (CPlayerCapability::ETargetType::Player ==
                     PlayerCapability->TargetType()))
                {
                    auto ActorTarget =
                        context->DSelectedPlayerAssets.front().lock();

                    if (PlayerCapability->CanApply(
                            ActorTarget,
                            context->DGameModel->Player(context->DPlayerNumber),
                            ActorTarget))
                    {
                        context
                            ->DPlayerCommands[to_underlying(
                                context->DPlayerNumber)]
                            .DAction = CapabilityType;
                        context
                            ->DPlayerCommands[to_underlying(
                                context->DPlayerNumber)]
                            .DActors = context->DSelectedPlayerAssets;
                        context
                            ->DPlayerCommands[to_underlying(
                                context->DPlayerNumber)]
                            .DTargetNumber = EPlayerNumber::Neutral;
                        context
                            ->DPlayerCommands[to_underlying(
                                context->DPlayerNumber)]
                            .DTargetType = EAssetType::None;
                        context
                            ->DPlayerCommands[to_underlying(
                                context->DPlayerNumber)]
                            .DTargetLocation = ActorTarget->Position();
                        context->DCurrentAssetCapability =
                            EAssetCapabilityType::None;
                    }
                }
                else
                {
                    context->DCurrentAssetCapability = CapabilityType;
                }
            }
            else
            {
                context->DCurrentAssetCapability = CapabilityType;
            }
        }
    }
    else if (CApplicationData::uictMenuButton == ComponentType)
    {
        context->DMenuButtonState = context->DLeftDown
                                        ? CButtonRenderer::EButtonState::Pressed
                                        : CButtonRenderer::EButtonState::Hover;
    }

    if (!Panning)
    {
        context->DPanningSpeed = 0;
    }
    else
    {
        if (EDirection::North == PanningDirection)
        {
            context->DViewportRenderer->PanNorth(context->DPanningSpeed >>
                                                 PAN_SPEED_SHIFT);
        }
        else if (EDirection::East == PanningDirection)
        {
            context->DViewportRenderer->PanEast(context->DPanningSpeed >>
                                                PAN_SPEED_SHIFT);
        }
        else if (EDirection::South == PanningDirection)
        {
            context->DViewportRenderer->PanSouth(context->DPanningSpeed >>
                                                 PAN_SPEED_SHIFT);
        }
        else if (EDirection::West == PanningDirection)
        {
            context->DViewportRenderer->PanWest(context->DPanningSpeed >>
                                                PAN_SPEED_SHIFT);
        }
        if (context->DPanningSpeed)
        {
            context->DPanningSpeed++;
            if (PAN_SPEED_MAX < context->DPanningSpeed)
            {
                context->DPanningSpeed = PAN_SPEED_MAX;
            }
        }
        else
        {
            context->DPanningSpeed = 1 << PAN_SPEED_SHIFT;
        }
    }

    // serialize packages and send it to server
    context->ClientPointer->SendGameInfo(context);
}

void CBattleMode::Calculate(std::shared_ptr<CApplicationData> context)
{


    //! If Menu button pressed switch to in-game options screen
    if (CButtonRenderer::EButtonState::Pressed == context->DMenuButtonState)
    {
        //! Clear the button state before changing the application mode
        context->DMenuButtonState = CButtonRenderer::EButtonState::None;
        context->ChangeApplicationMode(CInGameMenuMode::Instance());
    }

    bool PlayerAlive = false;    //!< Human player
    int AIAlive = 0;             //!< AI player(s)
    int RemotePlayersAlive = 0;  //!< Remote players on the network

    /*! Check if every potentially possible player is alive
     * Every round of game play iterates over the whole enum EPlayerColor to
     * see which players are alive.
     */
    for (int Index = 1; Index < to_underlying(EPlayerColor::Max); Index++)
    {
        //! Check if AI player is alive
        if (context->DGameModel->Player(static_cast<EPlayerNumber>(Index))
                ->IsAlive() &&
            context->DGameModel->Player(static_cast<EPlayerNumber>(Index))
                ->IsAI())
        {
            AIAlive++;  //! This AI is alive

            //! Ask AI to calculate its moves
            context->DAIPlayers[Index]->CalculateCommand(
                context->DPlayerCommands[Index]);
        }

        //! TODO: Check for remote players that are alive

        //! Check if human player is alive
        if (context->DGameModel->Player(static_cast<EPlayerNumber>(Index))
                ->IsAlive() &&
            !(context->DGameModel->Player(static_cast<EPlayerNumber>(Index))
                  ->IsAI()))
        {
            PlayerAlive = true;  //! Human player is alive
        }
    }

    //! Game is over when human player is dead or no AI players alive
    if (!PlayerAlive || 0 == AIAlive)
    {
        int game_over_song;
        context->DSoundLibraryMixer->StopSong();

        if (!PlayerAlive)
        {
            game_over_song = context->DSoundLibraryMixer->FindSong("lose");
        }
        else
        {
            game_over_song = context->DSoundLibraryMixer->FindSong("win");
        }

        context->DSoundLibraryMixer->PlaySong(game_over_song,
                                              context->DMusicVolume);
        context->DActiveGame = false;
        context->ChangeApplicationMode(CGameOverMenuMode::Instance());
    }

    /*! Check every player to enact its moves
     * Set up the game play state based on the actions input by the player.
     * Even though every player is checked, only the ones still alive will
     * have commands to process.
     */
    for (int Index = 1; Index < to_underlying(EPlayerColor::Max); Index++)
    {
        if (EAssetCapabilityType::None !=
            context->DPlayerCommands[Index].DAction)
        {
            auto PlayerCapability = CPlayerCapability::FindCapability(
                context->DPlayerCommands[Index].DAction);
            if (PlayerCapability)
            {
                std::shared_ptr<CPlayerAsset> NewTarget;

                if ((CPlayerCapability::ETargetType::None !=
                     PlayerCapability->TargetType()) &&
                    (CPlayerCapability::ETargetType::Player !=
                     PlayerCapability->TargetType()))
                {
                    if (EAssetType::None ==
                        context->DPlayerCommands[Index].DTargetType)
                    {
                        NewTarget =
                            context->DGameModel
                                ->Player(static_cast<EPlayerNumber>(Index))
                                ->CreateMarker(context->DPlayerCommands[Index]
                                                   .DTargetLocation,
                                               true);
                    }
                    else
                    {
                        NewTarget =
                            context->DGameModel
                                ->Player(context->DPlayerCommands[Index]
                                             .DTargetNumber)
                                ->SelectAsset(
                                    context->DPlayerCommands[Index]
                                        .DTargetLocation,
                                    context->DPlayerCommands[Index].DTargetType)
                                .lock();
                    }
                }

                for (auto &WeakActor : context->DPlayerCommands[Index].DActors)
                {
                    if (auto Actor = WeakActor.lock())
                    {
                        if (PlayerCapability->CanApply(
                                Actor,
                                context->DGameModel->Player(
                                    static_cast<EPlayerNumber>(Index)),
                                NewTarget) &&
                            (Actor->Interruptible() ||
                             (EAssetCapabilityType::Cancel ==
                              context->DPlayerCommands[Index].DAction)))
                        {
                            PlayerCapability->ApplyCapability(
                                Actor,
                                context->DGameModel->Player(
                                    static_cast<EPlayerNumber>(Index)),
                                NewTarget);
                        }
                    }
                }
            }
            context->DPlayerCommands[Index].DAction =
                EAssetCapabilityType::None;
        }
    }

    //! GameModel puts the human/AI(s) player moves into action
    context->DGameModel->Timestep();

    auto WeakAsset = context->DSelectedPlayerAssets.begin();
    while (WeakAsset != context->DSelectedPlayerAssets.end())
    {
        if (auto Asset = WeakAsset->lock())
        {
            if (context->DGameModel->ValidAsset(Asset) && Asset->Alive())
            {
                if (Asset->Speed() &&
                    (EAssetAction::Capability == Asset->Action()))
                {
                    auto Command = Asset->CurrentCommand();

                    if (Command.DAssetTarget &&
                        (EAssetAction::Construct ==
                         Command.DAssetTarget->Action()))
                    {
                        SGameEvent TempEvent;

                        context->DSelectedPlayerAssets.clear();
                        context->DSelectedPlayerAssets.push_back(
                            Command.DAssetTarget);

                        TempEvent.DType = EEventType::Selection;
                        TempEvent.DAsset = Command.DAssetTarget;
                        context->DGameModel->Player(context->DPlayerNumber)
                            ->AddGameEvent(TempEvent);

                        break;
                    }
                }
                WeakAsset++;
            }
            else
            {
                WeakAsset = context->DSelectedPlayerAssets.erase(WeakAsset);
            }
        }
        else
        {
            WeakAsset = context->DSelectedPlayerAssets.erase(WeakAsset);
        }
    }
}

//! @brief Handles displaying of the buttons
void CBattleMode::Render(std::shared_ptr<CApplicationData> context)
{
    SRectangle TempRectangle({0, 0, 0, 0});
    int CurrentX, CurrentY;
    int BufferWidth, BufferHeight;
    int ViewWidth, ViewHeight;
    int MiniMapWidth, MiniMapHeight;
    int DescriptionWidth, DescriptionHeight;
    int ActionWidth, ActionHeight;
    int ResourceWidth, ResourceHeight;
    std::list<std::weak_ptr<CPlayerAsset> > SelectedAndMarkerAssets =
        context->DSelectedPlayerAssets;

    CurrentX = context->DCurrentX;
    CurrentY = context->DCurrentY;
    BufferWidth = context->DWorkingBufferSurface->Width();
    BufferHeight = context->DWorkingBufferSurface->Height();
    ViewWidth = context->DViewportSurface->Width();
    ViewHeight = context->DViewportSurface->Height();
    MiniMapWidth = context->DMiniMapSurface->Width();
    MiniMapHeight = context->DMiniMapSurface->Height();
    DescriptionWidth = context->DUnitDescriptionSurface->Width();
    DescriptionHeight = context->DUnitDescriptionSurface->Height();
    ActionWidth = context->DUnitActionSurface->Width();
    ActionHeight = context->DUnitActionSurface->Height();
    ResourceWidth = context->DResourceSurface->Width();
    ResourceHeight = context->DResourceSurface->Height();

    if (context->DLeftDown && 0 < context->DMouseDown.X())
    {
        CPixelPosition TempPosition(
            context->ScreenToDetailedMap(CPixelPosition(CurrentX, CurrentY)));
        TempRectangle.DXPosition =
            std::min(context->DMouseDown.X(), TempPosition.X());
        TempRectangle.DYPosition =
            std::min(context->DMouseDown.Y(), TempPosition.Y());
        TempRectangle.DWidth =
            std::max(context->DMouseDown.X(), TempPosition.X()) -
            TempRectangle.DXPosition;
        TempRectangle.DHeight =
            std::max(context->DMouseDown.Y(), TempPosition.Y()) -
            TempRectangle.DYPosition;
    }
    else
    {
        CPixelPosition TempPosition(
            context->ScreenToDetailedMap(CPixelPosition(CurrentX, CurrentY)));
        TempRectangle.DXPosition = TempPosition.X();
        TempRectangle.DYPosition = TempPosition.Y();
    }
    for (int YPos = 0; YPos < BufferHeight;
         YPos += context->DBackgroundTileset->TileHeight())
    {
        for (int XPos = 0; XPos < BufferWidth;
             XPos += context->DBackgroundTileset->TileWidth())
        {
            context->DBackgroundTileset->DrawTile(
                context->DWorkingBufferSurface, XPos, YPos, 0);
        }
    }

    context->DInnerBevel->DrawBevel(
        context->DWorkingBufferSurface, context->DViewportXOffset,
        context->DViewportYOffset, ViewWidth, ViewHeight);
    context->DInnerBevel->DrawBevel(
        context->DWorkingBufferSurface, context->DMiniMapXOffset,
        context->DMiniMapYOffset, MiniMapWidth, MiniMapHeight);

    context->DResourceSurface->Draw(context->DWorkingBufferSurface, 0, 0,
                                    ResourceWidth, ResourceHeight,
                                    context->DViewportXOffset, 0);
    context->DResourceRenderer->DrawResources(context->DResourceSurface);
    context->DWorkingBufferSurface->Draw(
        context->DResourceSurface, context->DViewportXOffset, 0, -1, -1, 0, 0);

    context->DOuterBevel->DrawBevel(
        context->DWorkingBufferSurface, context->DUnitDescriptionXOffset,
        context->DUnitDescriptionYOffset, DescriptionWidth, DescriptionHeight);

    context->DUnitDescriptionSurface->Draw(context->DWorkingBufferSurface, 0, 0,
                                           DescriptionWidth, DescriptionHeight,
                                           context->DUnitDescriptionXOffset,
                                           context->DUnitDescriptionYOffset);
    context->DUnitDescriptionRenderer->DrawUnitDescription(
        context->DUnitDescriptionSurface, context->DSelectedPlayerAssets);
    context->DWorkingBufferSurface->Draw(
        context->DUnitDescriptionSurface, context->DUnitDescriptionXOffset,
        context->DUnitDescriptionYOffset, -1, -1, 0, 0);

    context->DOuterBevel->DrawBevel(
        context->DWorkingBufferSurface, context->DUnitActionXOffset,
        context->DUnitActionYOffset, ActionWidth, ActionHeight);
    context->DUnitActionSurface->Draw(
        context->DWorkingBufferSurface, 0, 0, ActionWidth, ActionHeight,
        context->DUnitActionXOffset, context->DUnitActionYOffset);
    context->DUnitActionRenderer->DrawUnitAction(
        context->DUnitActionSurface, context->DSelectedPlayerAssets,
        context->DCurrentAssetCapability);
    context->DWorkingBufferSurface->Draw(
        context->DUnitActionSurface, context->DUnitActionXOffset,
        context->DUnitActionYOffset, -1, -1, 0, 0);

    for (auto Asset : context->DGameModel->Player(context->DPlayerNumber)
                          ->PlayerMap()
                          ->Assets())
    {
        if (EAssetType::None == Asset->Type())
        {
            SelectedAndMarkerAssets.push_back(Asset);
        }
    }
    context->DViewportRenderer->DrawViewport(
        context->DViewportSurface, context->DViewportTypeSurface,
        SelectedAndMarkerAssets, TempRectangle,
        context->DCurrentAssetCapability);
    context->DMiniMapRenderer->DrawMiniMap(context->DMiniMapSurface);

    context->DWorkingBufferSurface->Draw(
        context->DMiniMapSurface, context->DMiniMapXOffset,
        context->DMiniMapYOffset, -1, -1, 0, 0);
    context->DWorkingBufferSurface->Draw(
        context->DViewportSurface, context->DViewportXOffset,
        context->DViewportYOffset, -1, -1, 0, 0);

    context->DMenuButtonRenderer->DrawButton(
        context->DWorkingBufferSurface, context->DMenuButtonXOffset,
        context->DMenuButtonYOffset, context->DMenuButtonState);

    switch (context->FindUIComponentType(CPixelPosition(CurrentX, CurrentY)))
    {
        case CApplicationData::uictViewport:
        {
            CPixelPosition ViewportCursorLocation =
                context->ScreenToViewport(CPixelPosition(CurrentX, CurrentY));
            CPixelType PixelType = CPixelType::GetPixelType(
                context->DViewportTypeSurface, ViewportCursorLocation.X(),
                ViewportCursorLocation.Y());
            context->DCursorType = CApplicationData::ctPointer;
            if (EAssetCapabilityType::None == context->DCurrentAssetCapability)
            {
                if (PixelType.Number() == context->DPlayerNumber)
                {
                    context->DCursorType = CApplicationData::ctInspect;
                }
            }
            else
            {
                auto PlayerCapability = CPlayerCapability::FindCapability(
                    context->DCurrentAssetCapability);

                if (PlayerCapability)
                {
                    bool CanApply = false;

                    if (EAssetType::None == PixelType.AssetType())
                    {
                        if ((CPlayerCapability::ETargetType::Terrain ==
                             PlayerCapability->TargetType()) ||
                            (CPlayerCapability::ETargetType::TerrainOrAsset ==
                             PlayerCapability->TargetType()))
                        {
                            auto NewTarget =
                                context->DGameModel
                                    ->Player(context->DPlayerNumber)
                                    ->CreateMarker(
                                        context->ViewportToDetailedMap(
                                            ViewportCursorLocation),
                                        false);

                            CanApply = PlayerCapability->CanApply(
                                context->DSelectedPlayerAssets.front().lock(),
                                context->DGameModel->Player(
                                    context->DPlayerNumber),
                                NewTarget);
                        }
                    }
                    else
                    {
                        if ((CPlayerCapability::ETargetType::Asset ==
                             PlayerCapability->TargetType()) ||
                            (CPlayerCapability::ETargetType::TerrainOrAsset ==
                             PlayerCapability->TargetType()))
                        {
                            auto NewTarget =
                                context->DGameModel->Player(PixelType.Number())
                                    ->SelectAsset(
                                        context->ViewportToDetailedMap(
                                            ViewportCursorLocation),
                                        PixelType.AssetType())
                                    .lock();

                            CanApply = PlayerCapability->CanApply(
                                context->DSelectedPlayerAssets.front().lock(),
                                context->DGameModel->Player(
                                    context->DPlayerNumber),
                                NewTarget);
                        }
                    }

                    context->DCursorType = CanApply
                                               ? CApplicationData::ctTargetOn
                                               : CApplicationData::ctTargetOff;
                }
            }
        }
        break;
        case CApplicationData::uictViewportBevelN:
            context->DCursorType = CApplicationData::ctArrowN;
            break;
        case CApplicationData::uictViewportBevelE:
            context->DCursorType = CApplicationData::ctArrowE;
            break;
        case CApplicationData::uictViewportBevelS:
            context->DCursorType = CApplicationData::ctArrowS;
            break;
        case CApplicationData::uictViewportBevelW:
            context->DCursorType = CApplicationData::ctArrowW;
            break;
        default:
            context->DCursorType = CApplicationData::ctPointer;
            break;
    }
    SRectangle ViewportRectangle(
        {context->DViewportRenderer->ViewportX(),
         context->DViewportRenderer->ViewportY(),
         context->DViewportRenderer->LastViewportWidth(),
         context->DViewportRenderer->LastViewportHeight()});

    context->DSoundEventRenderer->RenderEvents(ViewportRectangle);
}

std::shared_ptr<CApplicationMode> CBattleMode::Instance()
{
    if (DBattleModePointer == nullptr)
    {
        DBattleModePointer =
            std::make_shared<CBattleMode>(SPrivateConstructorType{});
    }
    return DBattleModePointer;
}
