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
#include "TriggerController.h"
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

        // Initialize overlay screen manager
        if (nullptr == DOverlayManager)
        {
            DOverlayManager = COverlayManagement::Initialize(context);
            DOverlayManager->SetMode(EOverlay::InGameMenu);
        }

        if (context->MultiPlayer())
        {
            if (nullptr == DChatOverlay)
            {
                DChatOverlay.reset(new CChatOverlay(context, EChatLocation::GameScreen));
            }
            DChatOverlay->InitializeChat();
            context->InGameText.clear();
        }
        else
        {
            context->DTriggerController = CTriggerController::Instance(context);
            DScenarioTimmer = 0;
        }
    }
    context->StartPlayingMusic("game1");
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

    SPlayerCommandRequest& request = context->DPlayerCommands[to_underlying(context->DPlayerNumber)];
    if (!context->OverlayActive() && !context->ChatOverlayActive())
    {
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
             // Toggle chat overlay
             // TODO: should probably add a check for multiplayer
            if ((SGUIKeyType::LeftAlt == Key) || (SGUIKeyType::RightAlt == Key))
            {
                if (context->MultiPlayer())
                {
                    context->ToggleChatOverlay();
                }
            }
            // Handle releases
            else if (context->DSelectedPlayerAssets.size())
            {
                bool CanMove = true;
                for (auto &Asset : context->DSelectedPlayerAssets)
                {
                    if (auto LockedAsset = Asset.lock())
                    {
                        if (0 == LockedAsset->EffectiveSpeed())
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
                                        request.DAction = KeyLookup->second;
                                        request.DActors = context->DSelectedPlayerAssets;
                                        request.DTargetNumber = EPlayerNumber::Neutral;
                                        request.DTargetType = EAssetType::None;
                                        request.DTargetLocation = ActorTarget->Position();
                                        context->DCurrentAssetCapability = EAssetCapabilityType::None;
                                    }
                                }
                                else
                                {
                                    context->DCurrentAssetCapability = KeyLookup->second;
                                }
                            }
                            else
                            {
                                context->DCurrentAssetCapability = KeyLookup->second;
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
                                        request.DAction = KeyLookup->second;
                                        request.DActors = context->DSelectedPlayerAssets;
                                        request.DTargetNumber = EPlayerNumber::Neutral;
                                        request.DTargetType = EAssetType::None;
                                        request.DTargetLocation = ActorTarget->Position();
                                        context->DCurrentAssetCapability = EAssetCapabilityType::None;
                                    }
                                }
                                else
                                {
                                    context->DCurrentAssetCapability = KeyLookup->second;
                                }
                            }
                            else
                            {
                                context->DCurrentAssetCapability = KeyLookup->second;
                            }
                        }
                    }
                }
            }
        }
        context->DReleasedKeys.clear();
    }

    context->DMenuButtonState = CButtonRenderer::EButtonState::None;
    CApplicationData::EUIComponentType ComponentType =
        context->FindUIComponentType(CPixelPosition(CurrentX, CurrentY));


    /*! Series of checks to see which UI element is selected
     * Check if UI elements such as buttons, player assets, overall game map,
     * menu button, etc. are selected. Set properties for that UI element's
     * state
     */
    if (CApplicationData::uictChat == ComponentType)
    {
        DChatOverlay->ProcessTextEntryFields(CurrentX, CurrentY, context->DLeftDown);
        DChatOverlay->ProcessKeyStrokes();
    }
    else if (CApplicationData::uictOverlay == ComponentType)
    {
        int X = CurrentX;
        int Y = CurrentY;

        // Given X,Y coordinates on working buffer surface, this function will
        // adjust the X,Y values with respect to the overlay surface.
        PixelPositionOnOverlay(X, Y);

        DOverlayManager->ProcessInput(X, Y, context->DLeftDown);
    }
    else if (CApplicationData::uictViewport == ComponentType)
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
                    if (0 == LockedAsset->EffectiveSpeed())
                    {
                        CanMove = false;
                        break;
                    }
                }
            }
            if (CanMove)
            {
                if (EPlayerNumber::Neutral != PixelType.Number() && CPixelType::EAssetTerrainType::GoldMine != PixelType.Type())
                {
                    //! Command is either walk/deliver, repair, or attack
                    request.DAction = EAssetCapabilityType::Move;
                    request.DTargetNumber = PixelType.Number();
                    request.DTargetType = PixelType.AssetType();
                    request.DActors = context->DSelectedPlayerAssets;
                    request.DTargetLocation = TempPosition;
                    if (PixelType.Number() == context->DPlayerNumber)
                    {
                        bool HaveLumber = false;
                        bool HaveGold = false;
                        bool HaveStone = false;

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
                                if (LockedAsset->Stone())
                                {
                                    HaveStone = true;
                                }
                            }
                        }
                        if (HaveGold)
                        {
                          if ((EAssetType::TownHall == request.DTargetType) ||
                              (EAssetType::Keep == request.DTargetType) ||
                              (EAssetType::Castle == request.DTargetType))
                          {
                              request.DAction = EAssetCapabilityType::Convey;
                          }
                        }
                        else if (HaveLumber)
                        {
                          if ((EAssetType::TownHall == request.DTargetType) ||
                              (EAssetType::Keep == request.DTargetType) ||
                              (EAssetType::Castle == request.DTargetType) ||
                              (EAssetType::LumberMill == request.DTargetType))
                          {
                              request.DAction = EAssetCapabilityType::Convey;
                          }
                        }
                        else if (HaveStone)
                        {
                          if ((EAssetType::TownHall == request.DTargetType) ||
                              (EAssetType::Keep == request.DTargetType) ||
                              (EAssetType::Castle == request.DTargetType) ||
                              (EAssetType::LumberMill == request.DTargetType))
                          {
                              request.DAction = EAssetCapabilityType::Convey;
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
                            if ((0 == TargetAsset->EffectiveSpeed()) &&
                                (TargetAsset->MaxHitPoints() >
                                 TargetAsset->HitPoints()))
                            {
                                request.DAction = EAssetCapabilityType::Repair;
                            }
                        }
                    }
                    else
                    {
                        request.DAction = EAssetCapabilityType::Attack;
                    }
                    context->DCurrentAssetCapability = EAssetCapabilityType::None;
                }
                else if(EPlayerNumber::Neutral == PixelType.Number() || CPixelType::EAssetTerrainType::GoldMine == PixelType.Type())
                {
                    // Command is either walk, mine, harvest
                    CPixelPosition TempPosition(context->ScreenToDetailedMap(
                        CPixelPosition(CurrentX, CurrentY)));
                    bool CanHarvest = true;

                    request.DAction = EAssetCapabilityType::Move;
                    request.DTargetNumber = EPlayerNumber::Neutral;
                    request.DTargetType = EAssetType::None;
                    request.DActors = context->DSelectedPlayerAssets;
                    request.DTargetLocation = TempPosition;

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

                            context->DGameModel->IsRock = false;
                            request.DAction = EAssetCapabilityType::Mine;
                            TempTilePosition.SetFromPixel(request.DTargetLocation);
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
                                    // Still allow harvesting of lumber
                                    context->DGameModel->IsRock = false;
                                    request.DTargetLocation.SetFromTile(TempTilePosition);
                                }
                            }
                        }
                        else if (CPixelType::EAssetTerrainType::Rock ==
                            PixelType.Type())
                        {
                            CTilePosition TempTilePosition;
                            context->DGameModel->IsRock = true;
                            request.DAction = EAssetCapabilityType::Mine;
                            TempTilePosition.SetFromPixel(request.DTargetLocation);
                            if (CTerrainMap::ETileType::Rock !=
                                context->DGameModel
                                        ->Player(context->DPlayerNumber)
                                        ->PlayerMap()
                                        ->TileType(TempTilePosition))
                            {
                                // Could be rock pixel, but tops of next row
                                TempTilePosition.IncrementY(1);
                                if (CTerrainMap::ETileType::Rock ==
                                    context->DGameModel
                                            ->Player(context->DPlayerNumber)
                                            ->PlayerMap()
                                            ->TileType(TempTilePosition))
                                {
                                    // Still allow mining of rock
                                    context->DGameModel->IsRock = true;
                                    request.DTargetLocation.SetFromTile(TempTilePosition);
                                }
                            }
                        }
                        else if (CPixelType::EAssetTerrainType::GoldMine ==
                                 PixelType.Type())
                        {

                            request.DTargetNumber = PixelType.Number();
                            request.DAction = EAssetCapabilityType::Mine;
                            request.DTargetType = EAssetType::GoldMine;
                        }
                    }
                    context->DCurrentAssetCapability = EAssetCapabilityType::None;
                }
            }
        }
        else if (context->DLeftClick)
        {
            if ((EAssetCapabilityType::None == context->DCurrentAssetCapability) ||
                (EAssetCapabilityType::BuildSimple == context->DCurrentAssetCapability))
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

                            request.DAction = context->DCurrentAssetCapability;
                            request.DActors = context->DSelectedPlayerAssets;
                            request.DTargetNumber = PixelType.Number();
                            request.DTargetType = PixelType.AssetType();
                            request.DTargetLocation = TempPosition;
                            context->DCurrentAssetCapability = EAssetCapabilityType::None;
                        }
                    }
                    //This checks for whether the clikced thing is terrain
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
                        //This part calls canapply to check whether, in this case, the clicked area can be mined
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

                            request.DAction = context->DCurrentAssetCapability;
                            request.DActors = context->DSelectedPlayerAssets;
                            request.DTargetNumber = EPlayerNumber::Neutral;
                            request.DTargetType = EAssetType::None;
                            request.DTargetLocation = TempPosition;
                            context->DCurrentAssetCapability = EAssetCapabilityType::None;

                            // Differentiate between rock and tree
                            if (PixelType.Type() == CPixelType::EAssetTerrainType::Rock)
                            {
                                context->DGameModel->IsRock = true;
                            }
                            else
                            {
                                context->DGameModel->IsRock = false;
                            }
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
                        request.DAction = CapabilityType;
                        request.DActors = context->DSelectedPlayerAssets;
                        request.DTargetNumber = EPlayerNumber::Neutral;
                        request.DTargetType = EAssetType::None;
                        request.DTargetLocation = ActorTarget->Position();
                        context->DCurrentAssetCapability = EAssetCapabilityType::None;
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

    //Handles Unit Grouping
    //Key 0 - 6 will add to groups 1 - 5 respectively and keys 1-5 will select groups 1-5
    //I tried doing shift 0-9 to add and just 0-9 to select but for some reason it hardly ever works
        for(auto key : context->DPressedKeys)
        {
            /*Add to groups*/
                if(SGUIKeyType::Key6 == key){ //Group 1
                    context->DUnitGroups->AddUnits(context->DSelectedPlayerAssets,1);
                    break;
                }else if(SGUIKeyType::Key7 == key){ //Group 2
                    context->DUnitGroups->AddUnits(context->DSelectedPlayerAssets,2);
                    break;
                }else if(SGUIKeyType::Key8 == key){ //Group 3
                    context->DUnitGroups->AddUnits(context->DSelectedPlayerAssets,3);
                    break;
                }else if(SGUIKeyType::Key9 == key){//Group 4
                    context->DUnitGroups->AddUnits(context->DSelectedPlayerAssets,4);
                    break;
                }else if(SGUIKeyType::Key0 == key){//Group 5
                    context->DUnitGroups->AddUnits(context->DSelectedPlayerAssets,5);
                    break;
                }
            /*Select Groups*/
                 if(SGUIKeyType::Key1 == key){
                    context->DSelectedPlayerAssets = context->DUnitGroups->GetGroup(1);
                    break;
                }else if(SGUIKeyType::Key2 == key){
                    context->DSelectedPlayerAssets = context->DUnitGroups->GetGroup(2);
                    break;
                }else if(SGUIKeyType::Key3 == key){
                    context->DSelectedPlayerAssets = context->DUnitGroups->GetGroup(3);
                    break;
                }else if(SGUIKeyType::Key4 == key){
                    context->DSelectedPlayerAssets = context->DUnitGroups->GetGroup(4);
                    break;
                }else if(SGUIKeyType::Key5 == key){
                    context->DSelectedPlayerAssets = context->DUnitGroups->GetGroup(5);
                    break;
                }
            /*Remove selected from groups*/
                if(SGUIKeyType::BackSpace == key){
                    context->DUnitGroups->RemoveUnits(context->DSelectedPlayerAssets);
                }
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

    // communicate with server if this is a multiplayer game
    if (context->MultiPlayer() && context->DActiveGame) {
        context->ClientPointer->SendGameInfo(context);
        context->ClientPointer->GetGameInfo(context);
    }

    // Check leave game flag. If set, change application mode according to
    // settings in CApplicationData::LeaveGame()
    if (context->CheckLeaveGameFlag())
    {
        if(context->MultiPlayer()) {
            boost::asio::ip::tcp::no_delay option(false);
            context->ClientPointer->socket.set_option(option);
        }
        context->LeaveGame();
    }

}

void CBattleMode::Calculate(std::shared_ptr<CApplicationData> context)
{
    int AIAlive = 0;             //!< AI player(s)
    int PlayersAlive = 0;  //players in total

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

        //! Check if human player is alive
        if (context->DGameModel->Player(static_cast<EPlayerNumber>(Index))
                ->IsAlive() &&
            !(context->DGameModel->Player(static_cast<EPlayerNumber>(Index))
                  ->IsAI()))
        {
            PlayersAlive++;
        }
    }

    //! Game is over when human player is dead or no AI players alive
    if (context->DGameSessionType == CApplicationData::gstSinglePlayer &&
        (0 == PlayersAlive || 0 == AIAlive))
    {
        int game_over_song;
        context->DSoundLibraryMixer->StopSong();

        if (0 == PlayersAlive)
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

    // multiplaye game end game condition
    else if(context->DGameSessionType != CApplicationData::gstSinglePlayer &&
        (1 == PlayersAlive && 0 == AIAlive)) {
          int game_over_song;
          context->DSoundLibraryMixer->StopSong();

          if (!context->DGameModel->Player(context->DPlayerNumber)->IsAlive())
          {
              game_over_song = context->DSoundLibraryMixer->FindSong("lose");
              // send lose to server
              context->ClientPointer->SendMessage("Lose");
          }
          else
          {
              game_over_song = context->DSoundLibraryMixer->FindSong("win");
              // send win to server
              context->ClientPointer->SendMessage("Win");
          }

          context->DSoundLibraryMixer->PlaySong(game_over_song,
                                                context->DMusicVolume);
          context->DActiveGame = false;

          boost::asio::ip::tcp::no_delay option(false);
          context->ClientPointer->socket.set_option(option);
          context->ChangeApplicationMode(CGameOverMenuMode::Instance());
    }

    /*! Check every player to enact its moves
     * Set up the game play state based on the actions input by the player.
     * Even though every player is checked, only the ones still alive will
     * have commands to process.
     */
    for (int Index = 1; Index < to_underlying(EPlayerColor::Max); Index++) {
        if (EAssetCapabilityType::None !=
            context->DPlayerCommands[Index].DAction) {
            auto PlayerCapability = CPlayerCapability::FindCapability(
            context->DPlayerCommands[Index].DAction);
            if (PlayerCapability) {
                std::shared_ptr<CPlayerAsset> NewTarget;

                if ((CPlayerCapability::ETargetType::None !=
                     PlayerCapability->TargetType()) &&
                    (CPlayerCapability::ETargetType::Player !=
                     PlayerCapability->TargetType())) {
                    if (EAssetType::None ==
                        context->DPlayerCommands[Index].DTargetType) {
                        NewTarget =
                        context->DGameModel
                        ->Player(static_cast<EPlayerNumber>(Index))
                        ->CreateMarker(context->DPlayerCommands[Index]
                                       .DTargetLocation,
                                       true);
                    } else {
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

                for (auto &WeakActor : context->DPlayerCommands[Index].DActors) {
                    if (auto Actor = WeakActor.lock()) {
                        if (PlayerCapability->CanApply(
                        Actor,
                        context->DGameModel->Player(
                        static_cast<EPlayerNumber>(Index)),
                        NewTarget) &&
                            (Actor->Interruptible() ||
                             (EAssetCapabilityType::Cancel ==
                              context->DPlayerCommands[Index].DAction))) {
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
    while (WeakAsset != context->DSelectedPlayerAssets.end()) {
        if (auto Asset = WeakAsset->lock()) {
            if (context->DGameModel->ValidAsset(Asset) && Asset->Alive()) {
                if (Asset->EffectiveSpeed() &&
                    (EAssetAction::Capability == Asset->Action())) {
                    auto Command = Asset->CurrentCommand();

                    if (Command.DAssetTarget &&
                        (EAssetAction::Construct ==
                         Command.DAssetTarget->Action())) {
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
            } else {
                WeakAsset = context->DSelectedPlayerAssets.erase(WeakAsset);
            }
        } else {
            WeakAsset = context->DSelectedPlayerAssets.erase(WeakAsset);
        }
    }

    // Check and Initiate Scenario Triggers
    if (DScenarioTimmer % 75 == 0 &&
        context->EGameSessionType::gstSinglePlayer == context->DGameSessionType) {
        context->DTriggerController->CheckTriggers();
        DScenarioTimmer = 0;

    }
    DScenarioTimmer++;
    //Change who owns gold mine

    //context->DGameModel->ChangeGoldMineOwner();

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
    int ButtonDescriptionWidth, ButtonDescriptionHeight;
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
    ButtonDescriptionWidth = context->DButtonDescriptionSurface->Width();
    ButtonDescriptionHeight = context->DButtonDescriptionSurface->Height();

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

    // Draws resource surface with resources on the top
    context->DResourceSurface->Draw(context->DWorkingBufferSurface, 0, 0,
                                    ResourceWidth, ResourceHeight,
                                    context->DViewportXOffset, 0);
    context->DResourceRenderer->DrawResources(context->DResourceSurface);
    context->DWorkingBufferSurface->Draw(
        context->DResourceSurface, context->DViewportXOffset, 0, -1, -1, 0, 0);

    // Draws the unit description box on the left
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

    // Action buttons of the unit box on the bottom left of the screen
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

    //These lines handle notification renderering. DO NOT CHANGE ANYTHING if you don't know exactly what you are doing.
    context->DNotificationRenderer->AddNotification(std::to_string(context->DNotificationRenderer->IncTime()));

    context->DNotificationRendererSurface->Draw(context->DWorkingBufferSurface,0,0,context->DNotificationRendererSurface->Width(),
         context->DNotificationRendererSurface->Height(),context->DViewportXOffset, BufferHeight - (ViewHeight / 4));

    context->DNotificationRenderer->DrawNotifications(context->DNotificationRendererSurface);

    context->DWorkingBufferSurface->Draw(context->DNotificationRendererSurface,context->DViewportXOffset,BufferHeight - (ViewHeight / 4),
        -1,-1,0,0);


    // Activate the the overlay screen when the in-game menu button is pressed
    if (CButtonRenderer::EButtonState::Pressed == context->DMenuButtonState)
    {
        // Activate overlay if it's not already active
        if(!context->OverlayActive())
        {
            context->ActivateOverlay();
        }
    }

    switch (context->FindUIComponentType(CPixelPosition(CurrentX, CurrentY)))
    {
        case CApplicationData::uictChat:
        {
            DChatOverlay->DrawChatText();
            DChatOverlay->DrawTextEntryField(context->DLoadingPlayerColors[to_underlying(context->DPlayerNumber)]);
            context->DWorkingBufferSurface->Draw(DChatOverlay->Surface(),
                DChatOverlay->Xoffset(), DChatOverlay->Yoffset(), -1, -1, 0, 0);
            break;
        }
        case CApplicationData::uictOverlay:
        {
            int X = CurrentX;
            int Y = CurrentY;
            auto Surface = DOverlayManager->Surface();
//            int Width = Surface->Width();
//            int Height = Surface->Height();
            int Xoffset = DOverlayManager->Xoffset();
            int Yoffset = DOverlayManager->Yoffset();

//            context->DWorkingBufferSurface->Draw(Surface, Xoffset,
//                Yoffset, -1, -1, 0, 0);

            PixelPositionOnOverlay(X, Y);

            // Pass input to set state and draw onto the overlay surface
            DOverlayManager->Draw(X, Y, context->DLeftDown);

            // Draw the overlay surface onto the main screen's surface
            context->DWorkingBufferSurface->Draw(Surface, Xoffset,
                Yoffset, -1, -1, 0, 0);

            break;
        }
        case CApplicationData::uictUserAction:
        {
            EAssetCapabilityType CapabilityType =
                context->DUnitActionRenderer->Selection(
                    context->ScreenToUnitAction(
                        CPixelPosition(CurrentX, CurrentY)));

            if (EAssetCapabilityType::None != CapabilityType)
            {
                // Action button description at the bottom of the screen
                auto TempCapName = CUnitDescriptionRenderer::AddAssetNameSpaces(
                    CPlayerCapability::TypeToName(CapabilityType));
                context->DButtonDescriptionSurface->Draw(
                    context->DWorkingBufferSurface, 0, 0, ButtonDescriptionWidth,
                    ButtonDescriptionHeight, context->DViewportXOffset,
                    BufferHeight - ButtonDescriptionHeight);
                context->DButtonDescriptionRenderer->DrawButtonDescription(
                    context->DButtonDescriptionSurface,
                    TempCapName);
                context->DWorkingBufferSurface->Draw(
                    context->DButtonDescriptionSurface, context->DViewportXOffset,
                    BufferHeight - ButtonDescriptionHeight, -1, -1, 0, 0);


            }
        }
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

// Set pixel position of pointer with respect to the overlay
void CBattleMode::PixelPositionOnOverlay(int &CurrentX, int &CurrentY)
{
    int Width = DOverlayManager->Width();
    int Height = DOverlayManager->Height();
    int Xoffset = DOverlayManager->Xoffset();
    int Yoffset = DOverlayManager->Yoffset();

    // Pixel position is within the overlay window
    if ((Xoffset <= CurrentX) && (Xoffset + Width > CurrentX) &&
        (Yoffset <= CurrentY) && (Yoffset + Height > CurrentY))
    {
        // Pass in coordinates centered on overlay
        CurrentX = CurrentX - Xoffset;
        CurrentY = CurrentY - Yoffset;
    }
    else
    {
        // Otherwise pass in coordinates outside overlay
        CurrentX = 0;
        CurrentY = 0;
    }

}
