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
#include "AssetRenderer.h"
#include <algorithm>
#include "Debug.h"
#include "PixelType.h"

#define TARGET_FREQUENCY 10
int CAssetRenderer::DAnimationDownsample = 1;

CAssetRenderer::CAssetRenderer(
    std::shared_ptr<CGraphicRecolorMap> colors,
    std::vector<std::shared_ptr<CGraphicMulticolorTileset> > tilesets,
    std::shared_ptr<CGraphicTileset> markertileset,
    std::shared_ptr<CGraphicTileset> corpsetileset,
    std::vector<std::shared_ptr<CGraphicTileset> > firetileset,
    std::shared_ptr<CGraphicTileset> buildingdeath,
    std::shared_ptr<CGraphicTileset> arrowtileset,
    std::shared_ptr<CPlayerData> player,
    std::shared_ptr<CAssetDecoratedMap> map)
{
    int TypeIndex = 0, MarkerIndex = 0;
    DTilesets = tilesets;
    DMarkerTileset = markertileset;
    DFireTilesets = firetileset;
    DBuildingDeathTileset = buildingdeath;
    DCorpseTileset = corpsetileset;
    DArrowTileset = arrowtileset;
    DPlayerData = player;
    DPlayerMap = map;

    for (int Index = 0; Index < to_underlying(EPlayerNumber::Max); Index++)
    {
        DLoadingPlayerColors[Index] = static_cast<EPlayerColor>(Index);
    }

    DPixelColors.resize(to_underlying(EPlayerColor::Max) + 3);
    DPixelColors[to_underlying(EPlayerColor::None)] =
        colors->ColorValue(colors->FindColor("none"), 0);
    DPixelColors[to_underlying(EPlayerColor::Blue)] =
        colors->ColorValue(colors->FindColor("blue"), 0);
    DPixelColors[to_underlying(EPlayerColor::Red)] =
        colors->ColorValue(colors->FindColor("red"), 0);
    DPixelColors[to_underlying(EPlayerColor::Green)] =
        colors->ColorValue(colors->FindColor("green"), 0);
    DPixelColors[to_underlying(EPlayerColor::Purple)] =
        colors->ColorValue(colors->FindColor("purple"), 0);
    DPixelColors[to_underlying(EPlayerColor::Orange)] =
        colors->ColorValue(colors->FindColor("orange"), 0);
    DPixelColors[to_underlying(EPlayerColor::Yellow)] =
        colors->ColorValue(colors->FindColor("yellow"), 0);
    DPixelColors[to_underlying(EPlayerColor::Black)] =
        colors->ColorValue(colors->FindColor("black"), 0);
    DPixelColors[to_underlying(EPlayerColor::White)] =
        colors->ColorValue(colors->FindColor("white"), 0);
    DPixelColors[to_underlying(EPlayerColor::Max)] =
        colors->ColorValue(colors->FindColor("self"), 0);
    DPixelColors[to_underlying(EPlayerColor::Max) + 1] =
        colors->ColorValue(colors->FindColor("enemy"), 0);
    DPixelColors[to_underlying(EPlayerColor::Max) + 2] =
        colors->ColorValue(colors->FindColor("building"), 0);

    while (true)
    {
        int Index = DMarkerTileset->FindTile(std::string("marker-") +
                                             std::to_string(MarkerIndex));
        if (0 > Index)
        {
            break;
        }
        DMarkerIndices.push_back(Index);
        MarkerIndex++;
    }
    DPlaceGoodIndex = DMarkerTileset->FindTile("place-good");
    DPlaceBadIndex = DMarkerTileset->FindTile("place-bad");

    {
        std::string LastDirectionName = "decay-nw-";
        for (auto &DirectionName :
             {"decay-n-", "decay-ne-", "decay-e-", "decay-se-", "decay-s-",
              "decay-sw-", "decay-w-", "decay-nw-"})
        {
            int StepIndex = 0, TileIndex;
            while (true)
            {
                TileIndex = DCorpseTileset->FindTile(
                    std::string(DirectionName) + std::to_string(StepIndex));
                if (0 <= TileIndex)
                {
                    DCorpseIndices.push_back(TileIndex);
                }
                else
                {
                    TileIndex = DCorpseTileset->FindTile(
                        std::string(LastDirectionName) +
                        std::to_string(StepIndex));
                    if (0 <= TileIndex)
                    {
                        DCorpseIndices.push_back(TileIndex);
                    }
                    else
                    {
                        break;
                    }
                }
                StepIndex++;
            }
            LastDirectionName = DirectionName;
        }
    }

    for (auto &DirectionName :
         {"attack-n-", "attack-ne-", "attack-e-", "attack-se-", "attack-s-",
          "attack-sw-", "attack-w-", "attack-nw-"})
    {
        int StepIndex = 0, TileIndex;
        while (true)
        {
            TileIndex = DArrowTileset->FindTile(std::string(DirectionName) +
                                                std::to_string(StepIndex));
            if (0 <= TileIndex)
            {
                DArrowIndices.push_back(TileIndex);
            }
            else
            {
                break;
            }
            StepIndex++;
        }
    }

    DConstructIndices.resize(DTilesets.size());
    DBuildIndices.resize(DTilesets.size());
    DWalkIndices.resize(DTilesets.size());
    DNoneIndices.resize(DTilesets.size());
    DCarryGoldIndices.resize(DTilesets.size());
    DCarryLumberIndices.resize(DTilesets.size());
    DAttackIndices.resize(DTilesets.size());
    DDeathIndices.resize(DTilesets.size());
    DPlaceIndices.resize(DTilesets.size());
    for (auto &Tileset : DTilesets)
    {
        if (Tileset)
        {
            PrintDebug(DEBUG_LOW, "Checking Walk on %d\n", TypeIndex);
            for (auto &DirectionName :
                 {"walk-n-", "walk-ne-", "walk-e-", "walk-se-", "walk-s-",
                  "walk-sw-", "walk-w-", "walk-nw-"})
            {
                int StepIndex = 0, TileIndex;
                while (true)
                {
                    TileIndex = Tileset->FindTile(std::string(DirectionName) +
                                                  std::to_string(StepIndex));
                    if (0 <= TileIndex)
                    {
                        DWalkIndices[TypeIndex].push_back(TileIndex);
                    }
                    else
                    {
                        break;
                    }
                    StepIndex++;
                }
            }
            PrintDebug(DEBUG_LOW, "Checking Construct on %d\n", TypeIndex);
            {
                int StepIndex = 0, TileIndex;
                while (true)
                {
                    TileIndex = Tileset->FindTile(std::string("construct-") +
                                                  std::to_string(StepIndex));
                    if (0 <= TileIndex)
                    {
                        DConstructIndices[TypeIndex].push_back(TileIndex);
                    }
                    else
                    {
                        if (!StepIndex)
                        {
                            DConstructIndices[TypeIndex].push_back(-1);
                        }
                        break;
                    }
                    StepIndex++;
                }
            }
            PrintDebug(DEBUG_LOW, "Checking Gold on %d\n", TypeIndex);
            for (auto &DirectionName :
                 {"gold-n-", "gold-ne-", "gold-e-", "gold-se-", "gold-s-",
                  "gold-sw-", "gold-w-", "gold-nw-"})
            {
                int StepIndex = 0, TileIndex;
                while (true)
                {
                    TileIndex = Tileset->FindTile(std::string(DirectionName) +
                                                  std::to_string(StepIndex));
                    if (0 <= TileIndex)
                    {
                        DCarryGoldIndices[TypeIndex].push_back(TileIndex);
                    }
                    else
                    {
                        break;
                    }
                    StepIndex++;
                }
            }
            PrintDebug(DEBUG_LOW, "Checking Lumber on %d\n", TypeIndex);
            for (auto &DirectionName :
                 {"lumber-n-", "lumber-ne-", "lumber-e-", "lumber-se-",
                  "lumber-s-", "lumber-sw-", "lumber-w-", "lumber-nw-"})
            {
                int StepIndex = 0, TileIndex;
                while (true)
                {
                    TileIndex = Tileset->FindTile(std::string(DirectionName) +
                                                  std::to_string(StepIndex));
                    if (0 <= TileIndex)
                    {
                        DCarryLumberIndices[TypeIndex].push_back(TileIndex);
                    }
                    else
                    {
                        break;
                    }
                    StepIndex++;
                }
            }
            PrintDebug(DEBUG_LOW, "Checking Attack on %d\n", TypeIndex);
            for (auto &DirectionName :
                 {"attack-n-", "attack-ne-", "attack-e-", "attack-se-",
                  "attack-s-", "attack-sw-", "attack-w-", "attack-nw-"})
            {
                int StepIndex = 0, TileIndex;
                while (true)
                {
                    TileIndex = Tileset->FindTile(std::string(DirectionName) +
                                                  std::to_string(StepIndex));
                    if (0 <= TileIndex)
                    {
                        DAttackIndices[TypeIndex].push_back(TileIndex);
                    }
                    else
                    {
                        break;
                    }
                    StepIndex++;
                }
            }
            if (0 == DAttackIndices[TypeIndex].size())
            {
                int TileIndex;
                for (int Index = 0; Index < to_underlying(EDirection::Max);
                     Index++)
                {
                    if (0 <= (TileIndex = Tileset->FindTile("active")))
                    {
                        DAttackIndices[TypeIndex].push_back(TileIndex);
                    }
                    else if (0 <= (TileIndex = Tileset->FindTile("inactive")))
                    {
                        DAttackIndices[TypeIndex].push_back(TileIndex);
                    }
                }
            }
            PrintDebug(DEBUG_LOW, "Checking Death on %d\n", TypeIndex);
            std::string LastDirectionName = "death-nw";
            for (auto &DirectionName :
                 {"death-n-", "death-ne-", "death-e-", "death-se-", "death-s-",
                  "death-sw-", "death-w-", "death-nw-"})
            {
                int StepIndex = 0, TileIndex;
                while (true)
                {
                    TileIndex = Tileset->FindTile(std::string(DirectionName) +
                                                  std::to_string(StepIndex));
                    if (0 <= TileIndex)
                    {
                        DDeathIndices[TypeIndex].push_back(TileIndex);
                    }
                    else
                    {
                        TileIndex =
                            Tileset->FindTile(std::string(LastDirectionName) +
                                              std::to_string(StepIndex));
                        if (0 <= TileIndex)
                        {
                            DDeathIndices[TypeIndex].push_back(TileIndex);
                        }
                        else
                        {
                            break;
                        }
                    }
                    StepIndex++;
                }
                LastDirectionName = DirectionName;
            }
            if (DDeathIndices[TypeIndex].size())
            {
            }
            PrintDebug(DEBUG_LOW, "Checking None on %d\n", TypeIndex);
            for (auto &DirectionName :
                 {"none-n-", "none-ne-", "none-e-", "none-se-", "none-s-",
                  "none-sw-", "none-w-", "none-nw-"})
            {
                int TileIndex = Tileset->FindTile(std::string(DirectionName));
                if (0 <= TileIndex)
                {
                    DNoneIndices[TypeIndex].push_back(TileIndex);
                }
                else if (DWalkIndices[TypeIndex].size())
                {
                    DNoneIndices[TypeIndex].push_back(
                        DWalkIndices[TypeIndex]
                                    [DNoneIndices[TypeIndex].size() *
                                     (DWalkIndices[TypeIndex].size() /
                                      to_underlying(EDirection::Max))]);
                }
                else if (0 <= (TileIndex = Tileset->FindTile("inactive")))
                {
                    DNoneIndices[TypeIndex].push_back(TileIndex);
                }
            }
            PrintDebug(DEBUG_LOW, "Checking Build on %d\n", TypeIndex);
            for (auto &DirectionName :
                 {"build-n-", "build-ne-", "build-e-", "build-se-", "build-s-",
                  "build-sw-", "build-w-", "build-nw-"})
            {
                int StepIndex = 0, TileIndex;
                while (true)
                {
                    TileIndex = Tileset->FindTile(std::string(DirectionName) +
                                                  std::to_string(StepIndex));
                    if (0 <= TileIndex)
                    {
                        DBuildIndices[TypeIndex].push_back(TileIndex);
                    }
                    else
                    {
                        if (!StepIndex)
                        {
                            if (0 <= (TileIndex = Tileset->FindTile("active")))
                            {
                                DBuildIndices[TypeIndex].push_back(TileIndex);
                            }
                            else if (0 <= (TileIndex =
                                               Tileset->FindTile("inactive")))
                            {
                                DBuildIndices[TypeIndex].push_back(TileIndex);
                            }
                        }
                        break;
                    }
                    StepIndex++;
                }
            }
            PrintDebug(DEBUG_LOW, "Checking Place on %d\n", TypeIndex);
            {
                DPlaceIndices[TypeIndex].push_back(Tileset->FindTile("place"));
            }

            PrintDebug(DEBUG_LOW, "Done checking type %d\n", TypeIndex);
        }
        TypeIndex++;
    }
}

void CAssetRenderer::UpdateLoadingPlayerColors(
    const std::array<EPlayerColor, to_underlying(EPlayerNumber::Max)> &newcolors)
{
    DLoadingPlayerColors = newcolors;
}

int CAssetRenderer::UpdateFrequency(int freq)
{
    if (TARGET_FREQUENCY >= freq)
    {
        DAnimationDownsample = 1;
        return TARGET_FREQUENCY;
    }
    DAnimationDownsample = freq / TARGET_FREQUENCY;
    return freq;
}

using SAssetRenderData = struct ASSETRENDERERDATA_TAG
{
    EAssetType DType;
    int DX;
    int DY;
    int DBottomY;
    int DTileIndex;
    int DColorIndex;
    uint32_t DPixelColor;
};

bool CompareRenderData(const SAssetRenderData &first,
                       const SAssetRenderData &second)
{
    if (first.DBottomY < second.DBottomY)
    {
        return true;
    }
    if (first.DBottomY > second.DBottomY)
    {
        return false;
    }

    return first.DX <= second.DX;
}

void CAssetRenderer::DrawAssets(std::shared_ptr<CGraphicSurface> surface,
                                std::shared_ptr<CGraphicSurface> typesurface,
                                const SRectangle &rect)
{
    int ScreenRightX = rect.DXPosition + rect.DWidth - 1;
    int ScreenBottomY = rect.DYPosition + rect.DHeight - 1;
    std::list<SAssetRenderData> FinalRenderList;

    for (auto &AssetIterator : DPlayerMap->Assets())
    {
        SAssetRenderData TempRenderData;
        TempRenderData.DType = AssetIterator->Type();
        if (EAssetType::None == TempRenderData.DType)
        {
            continue;
        }
        if ((0 <= to_underlying(TempRenderData.DType)) &&
            (to_underlying(TempRenderData.DType) <
             static_cast<int>(DTilesets.size())))
        {
            CPixelType PixelType(*AssetIterator);
            int RightX;
            TempRenderData.DX =
                AssetIterator->PositionX() +
                (AssetIterator->Size() - 1) * CPosition::HalfTileWidth() -
                DTilesets[to_underlying(TempRenderData.DType)]->TileHalfWidth();
            TempRenderData.DY =
                AssetIterator->PositionY() +
                (AssetIterator->Size() - 1) * CPosition::HalfTileHeight() -
                DTilesets[to_underlying(TempRenderData.DType)]->TileHalfHeight();
            TempRenderData.DPixelColor = PixelType.ToPixelColor();

            RightX =
                TempRenderData.DX +
                DTilesets[to_underlying(TempRenderData.DType)]->TileWidth() - 1;
            TempRenderData.DBottomY =
                TempRenderData.DY +
                DTilesets[to_underlying(TempRenderData.DType)]->TileHeight() -
                1;
            bool OnScreen = true;
            if ((RightX < rect.DXPosition) ||
                (TempRenderData.DX > ScreenRightX))
            {
                OnScreen = false;
            }
            else if ((TempRenderData.DBottomY < rect.DYPosition) ||
                     (TempRenderData.DY > ScreenBottomY))
            {
                OnScreen = false;
            }
            TempRenderData.DX -= rect.DXPosition;
            TempRenderData.DY -= rect.DYPosition;
            TempRenderData.DColorIndex =
                to_underlying(AssetIterator->Color())
                    ? to_underlying(AssetIterator->Color()) - 1
                    : to_underlying(AssetIterator->Color());
            TempRenderData.DTileIndex = -1;
            if (OnScreen)
            {
                int ActionSteps, CurrentStep, TileIndex;
                switch (AssetIterator->Action())
                {
                    case EAssetAction::Build:
                        ActionSteps =
                            DBuildIndices[to_underlying(TempRenderData.DType)]
                                .size();
                        ActionSteps /= to_underlying(EDirection::Max);
                        if (ActionSteps)
                        {
                            TileIndex =
                                to_underlying(AssetIterator->Direction()) *
                                    ActionSteps +
                                ((AssetIterator->Step() /
                                  DAnimationDownsample) %
                                 ActionSteps);
                            TempRenderData.DTileIndex =
                                DBuildIndices[to_underlying(
                                    TempRenderData.DType)][TileIndex];
                        }
                        break;
                    case EAssetAction::Construct:
                        ActionSteps =
                            DConstructIndices[to_underlying(
                                                  TempRenderData.DType)]
                                .size();
                        if (ActionSteps)
                        {
                            int TotalSteps = AssetIterator->BuildTime() *
                                             CPlayerAsset::UpdateFrequency();
                            int CurrentStep = AssetIterator->Step() *
                                              ActionSteps / TotalSteps;
                            if (CurrentStep ==
                                DConstructIndices[to_underlying(
                                                      TempRenderData.DType)]
                                    .size())
                            {
                                CurrentStep--;
                            }
                            TempRenderData.DTileIndex =
                                DConstructIndices[to_underlying(
                                    TempRenderData.DType)][CurrentStep];
                        }
                        break;
                    case EAssetAction::Walk:
                        if (AssetIterator->Lumber())
                        {
                            ActionSteps =
                                DCarryLumberIndices[to_underlying(
                                                        TempRenderData.DType)]
                                    .size();
                            ActionSteps /= to_underlying(EDirection::Max);
                            TileIndex =
                                to_underlying(AssetIterator->Direction()) *
                                    ActionSteps +
                                ((AssetIterator->Step() /
                                  DAnimationDownsample) %
                                 ActionSteps);
                            TempRenderData.DTileIndex =
                                DCarryLumberIndices[to_underlying(
                                    TempRenderData.DType)][TileIndex];
                        }
                        else if (AssetIterator->Gold())
                        {
                            ActionSteps =
                                DCarryGoldIndices[to_underlying(
                                                      TempRenderData.DType)]
                                    .size();
                            ActionSteps /= to_underlying(EDirection::Max);
                            TileIndex =
                                to_underlying(AssetIterator->Direction()) *
                                    ActionSteps +
                                ((AssetIterator->Step() /
                                  DAnimationDownsample) %
                                 ActionSteps);
                            TempRenderData.DTileIndex =
                                DCarryGoldIndices[to_underlying(
                                    TempRenderData.DType)][TileIndex];
                        }
                        else
                        {
                            ActionSteps =
                                DWalkIndices[to_underlying(TempRenderData.DType)]
                                    .size();
                            ActionSteps /= to_underlying(EDirection::Max);
                            TileIndex =
                                to_underlying(AssetIterator->Direction()) *
                                    ActionSteps +
                                ((AssetIterator->Step() /
                                  DAnimationDownsample) %
                                 ActionSteps);
                            TempRenderData.DTileIndex =
                                DWalkIndices[to_underlying(
                                    TempRenderData.DType)][TileIndex];
                        }
                        break;
                    case EAssetAction::Attack:
                        CurrentStep = AssetIterator->Step() %
                                      (AssetIterator->AttackSteps() +
                                       AssetIterator->ReloadSteps());
                        if (CurrentStep < AssetIterator->AttackSteps())
                        {
                            ActionSteps =
                                DAttackIndices[to_underlying(
                                                   TempRenderData.DType)]
                                    .size();
                            ActionSteps /= to_underlying(EDirection::Max);
                            TileIndex =
                                to_underlying(AssetIterator->Direction()) *
                                    ActionSteps +
                                (CurrentStep * ActionSteps /
                                 AssetIterator->AttackSteps());
                            TempRenderData.DTileIndex =
                                DAttackIndices[to_underlying(
                                    TempRenderData.DType)][TileIndex];
                        }
                        else
                        {
                            TempRenderData.DTileIndex =
                                DNoneIndices[to_underlying(TempRenderData.DType)]
                                            [to_underlying(
                                                AssetIterator->Direction())];
                        }
                        break;
                    case EAssetAction::Repair:
                    case EAssetAction::HarvestLumber:
                        ActionSteps =
                            DAttackIndices[to_underlying(TempRenderData.DType)]
                                .size();
                        ActionSteps /= to_underlying(EDirection::Max);
                        TileIndex =
                            to_underlying(AssetIterator->Direction()) *
                                ActionSteps +
                            ((AssetIterator->Step() / DAnimationDownsample) %
                             ActionSteps);
                        TempRenderData.DTileIndex =
                            DAttackIndices[to_underlying(TempRenderData.DType)]
                                          [TileIndex];
                        break;
                    case EAssetAction::MineGold:
                        break;
                    case EAssetAction::StandGround:
                    case EAssetAction::None:
                        TempRenderData.DTileIndex =
                            DNoneIndices[to_underlying(TempRenderData.DType)]
                                        [to_underlying(
                                            AssetIterator->Direction())];
                        if (AssetIterator->Speed())
                        {
                            if (AssetIterator->Lumber())
                            {
                                ActionSteps =
                                    DCarryLumberIndices
                                        [to_underlying(TempRenderData.DType)]
                                            .size();
                                ActionSteps /= to_underlying(EDirection::Max);
                                TempRenderData.DTileIndex = DCarryLumberIndices
                                    [to_underlying(TempRenderData.DType)]
                                    [to_underlying(AssetIterator->Direction()) *
                                     ActionSteps];
                            }
                            else if (AssetIterator->Gold())
                            {
                                ActionSteps =
                                    DCarryGoldIndices[to_underlying(
                                                          TempRenderData.DType)]
                                        .size();
                                ActionSteps /= to_underlying(EDirection::Max);
                                TempRenderData.DTileIndex = DCarryGoldIndices
                                    [to_underlying(TempRenderData.DType)]
                                    [to_underlying(AssetIterator->Direction()) *
                                     ActionSteps];
                            }
                        }
                        break;
                    case EAssetAction::Capability:
                        if (AssetIterator->Speed())
                        {
                            if ((EAssetCapabilityType::Patrol ==
                                 AssetIterator->CurrentCommand().DCapability) ||
                                (EAssetCapabilityType::StandGround ==
                                 AssetIterator->CurrentCommand().DCapability))
                            {
                                TempRenderData.DTileIndex = DNoneIndices
                                    [to_underlying(TempRenderData.DType)]
                                    [to_underlying(AssetIterator->Direction())];
                            }
                        }
                        else
                        {
                            // Buildings
                            TempRenderData.DTileIndex =
                                DNoneIndices[to_underlying(TempRenderData.DType)]
                                            [to_underlying(
                                                AssetIterator->Direction())];
                        }
                        break;
                    case EAssetAction::Death:
                        ActionSteps =
                            DDeathIndices[to_underlying(TempRenderData.DType)]
                                .size();
                        if (AssetIterator->Speed())
                        {
                            ActionSteps /= to_underlying(EDirection::Max);
                            if (ActionSteps)
                            {
                                CurrentStep = AssetIterator->Step() /
                                              DAnimationDownsample;
                                if (CurrentStep >= ActionSteps)
                                {
                                    CurrentStep = ActionSteps - 1;
                                }
                                TempRenderData.DTileIndex = DDeathIndices
                                    [to_underlying(TempRenderData.DType)]
                                    [to_underlying(AssetIterator->Direction()) *
                                         ActionSteps +
                                     CurrentStep];
                            }
                        }
                        else
                        {
                            if (AssetIterator->Step() <
                                DBuildingDeathTileset->TileCount())
                            {
                                TempRenderData.DTileIndex =
                                    DTilesets[to_underlying(
                                                  TempRenderData.DType)]
                                        ->TileCount() +
                                    AssetIterator->Step();
                                TempRenderData.DX +=
                                    DTilesets[to_underlying(
                                                  TempRenderData.DType)]
                                        ->TileHalfWidth() -
                                    DBuildingDeathTileset->TileHalfWidth();
                                TempRenderData.DY +=
                                    DTilesets[to_underlying(
                                                  TempRenderData.DType)]
                                        ->TileHalfHeight() -
                                    DBuildingDeathTileset->TileHalfHeight();
                            }
                        }
                    default:
                        break;
                }
                if (0 <= TempRenderData.DTileIndex)
                {
                    FinalRenderList.push_back(TempRenderData);
                }
            }
        }
    }
    FinalRenderList.sort(CompareRenderData);
    for (auto &RenderIterator : FinalRenderList)
    {
        if (RenderIterator.DTileIndex <
            DTilesets[to_underlying(RenderIterator.DType)]->TileCount())
        {
            DTilesets[to_underlying(RenderIterator.DType)]->DrawTile(
                surface, RenderIterator.DX, RenderIterator.DY,
                RenderIterator.DTileIndex, RenderIterator.DColorIndex);
            DTilesets[to_underlying(RenderIterator.DType)]->DrawClipped(
                typesurface, RenderIterator.DX, RenderIterator.DY,
                RenderIterator.DTileIndex, RenderIterator.DPixelColor);
        }
        else
        {
            DBuildingDeathTileset->DrawTile(surface, RenderIterator.DX,
                                            RenderIterator.DY,
                                            RenderIterator.DTileIndex);
        }
    }
}

void CAssetRenderer::DrawSelections(
    std::shared_ptr<CGraphicSurface> surface, const SRectangle &rect,
    const std::list<std::weak_ptr<CPlayerAsset> > &selectionlist,
    const SRectangle &selectrect, bool highlightbuilding)
{
    auto ResourceContext = surface->CreateResourceContext();
    uint32_t RectangleColor = DPixelColors[to_underlying(EPlayerColor::Max)];
    int ScreenRightX = rect.DXPosition + rect.DWidth - 1;
    int ScreenBottomY = rect.DYPosition + rect.DHeight - 1;
    int SelectionX, SelectionY;

    if (highlightbuilding)
    {
        RectangleColor = DPixelColors[to_underlying(EPlayerColor::Max) + 2];

        ResourceContext->SetSourceRGB(RectangleColor);
        for (auto &AssetIterator : DPlayerMap->Assets())
        {
            SAssetRenderData TempRenderData;
            TempRenderData.DType = AssetIterator->Type();
            if (EAssetType::None == TempRenderData.DType)
            {
                continue;
            }
            if ((0 <= to_underlying(TempRenderData.DType)) &&
                (to_underlying(TempRenderData.DType) < DTilesets.size()))
            {
                if (0 == AssetIterator->Speed())
                {
                    int RightX;
                    int Offset =
                        EAssetType::GoldMine == TempRenderData.DType ? 1 : 0;

                    TempRenderData.DX =
                        AssetIterator->PositionX() +
                        (AssetIterator->Size() - 1) *
                            CPosition::HalfTileWidth() -
                        DTilesets[to_underlying(TempRenderData.DType)]
                            ->TileHalfWidth();
                    TempRenderData.DY =
                        AssetIterator->PositionY() +
                        (AssetIterator->Size() - 1) *
                            CPosition::HalfTileHeight() -
                        DTilesets[to_underlying(TempRenderData.DType)]
                            ->TileHalfHeight();
                    TempRenderData.DX -= Offset * CPosition::TileWidth();
                    TempRenderData.DY -= Offset * CPosition::TileHeight();

                    RightX = TempRenderData.DX +
                             DTilesets[to_underlying(TempRenderData.DType)]
                                 ->TileWidth() +
                             (2 * Offset * CPosition::TileWidth()) - 1;
                    TempRenderData.DBottomY =
                        TempRenderData.DY +
                        DTilesets[to_underlying(TempRenderData.DType)]
                            ->TileHeight() +
                        (2 * Offset * CPosition::TileHeight()) - 1;
                    bool OnScreen = true;
                    if ((RightX < rect.DXPosition) ||
                        (TempRenderData.DX > ScreenRightX))
                    {
                        OnScreen = false;
                    }
                    else if ((TempRenderData.DBottomY < rect.DYPosition) ||
                             (TempRenderData.DY > ScreenBottomY))
                    {
                        OnScreen = false;
                    }
                    TempRenderData.DX -= rect.DXPosition;
                    TempRenderData.DY -= rect.DYPosition;
                    if (OnScreen)
                    {
                        ResourceContext->Rectangle(
                            TempRenderData.DX, TempRenderData.DY,
                            DTilesets[to_underlying(TempRenderData.DType)]
                                    ->TileWidth() +
                                (2 * Offset * CPosition::TileWidth()),
                            DTilesets[to_underlying(TempRenderData.DType)]
                                    ->TileHeight() +
                                (2 * Offset * CPosition::TileHeight()));
                        ResourceContext->Stroke();
                    }
                }
            }
        }

        RectangleColor = DPixelColors[to_underlying(EPlayerColor::Max)];
    }

    ResourceContext->SetSourceRGB(RectangleColor);

    if (selectrect.DWidth && selectrect.DHeight)
    {
        SelectionX = selectrect.DXPosition - rect.DXPosition;
        SelectionY = selectrect.DYPosition - rect.DYPosition;

        ResourceContext->Rectangle(SelectionX, SelectionY, selectrect.DWidth,
                                   selectrect.DHeight);
        ResourceContext->Stroke();
    }

    if (selectionlist.size())
    {
        if (auto Asset = selectionlist.front().lock())
        {
            if (EPlayerColor::None == Asset->Color())
            {
                RectangleColor =
                    DPixelColors[to_underlying(EPlayerColor::None)];
            }
            else if (DPlayerData->Color() != Asset->Color())
            {
                RectangleColor =
                    DPixelColors[to_underlying(EPlayerColor::Max) + 1];
            }
            ResourceContext->SetSourceRGB(RectangleColor);
        }
    }

    for (auto &AssetIterator : selectionlist)
    {
        if (auto LockedAsset = AssetIterator.lock())
        {
            SAssetRenderData TempRenderData;
            TempRenderData.DType = LockedAsset->Type();
            if (EAssetType::None == TempRenderData.DType)
            {
                if (EAssetAction::Decay == LockedAsset->Action())
                {
                    int RightX;
                    bool OnScreen = true;

                    TempRenderData.DX = LockedAsset->PositionX() -
                                        DCorpseTileset->TileWidth() / 2;
                    TempRenderData.DY = LockedAsset->PositionY() -
                                        DCorpseTileset->TileHeight() / 2;
                    RightX = TempRenderData.DX + DCorpseTileset->TileWidth();
                    TempRenderData.DBottomY =
                        TempRenderData.DY + DCorpseTileset->TileHeight();

                    if ((RightX < rect.DXPosition) ||
                        (TempRenderData.DX > ScreenRightX))
                    {
                        OnScreen = false;
                    }
                    else if ((TempRenderData.DBottomY < rect.DYPosition) ||
                             (TempRenderData.DY > ScreenBottomY))
                    {
                        OnScreen = false;
                    }
                    TempRenderData.DX -= rect.DXPosition;
                    TempRenderData.DY -= rect.DYPosition;
                    if (OnScreen)
                    {
                        int ActionSteps = DCorpseIndices.size();
                        ActionSteps /= to_underlying(EDirection::Max);
                        if (ActionSteps)
                        {
                            int CurrentStep =
                                LockedAsset->Step() /
                                (DAnimationDownsample * TARGET_FREQUENCY);
                            if (CurrentStep >= ActionSteps)
                            {
                                CurrentStep = ActionSteps - 1;
                            }
                            TempRenderData.DTileIndex =
                                DCorpseIndices[to_underlying(
                                                   LockedAsset->Direction()) *
                                                   ActionSteps +
                                               CurrentStep];
                        }

                        DCorpseTileset->DrawTile(surface, TempRenderData.DX,
                                                 TempRenderData.DY,
                                                 TempRenderData.DTileIndex);
                    }
                }
                else if (EAssetAction::Attack != LockedAsset->Action())
                {
                    int RightX;
                    bool OnScreen = true;

                    TempRenderData.DX = LockedAsset->PositionX() -
                                        DMarkerTileset->TileWidth() / 2;
                    TempRenderData.DY = LockedAsset->PositionY() -
                                        DMarkerTileset->TileHeight() / 2;
                    RightX = TempRenderData.DX + DMarkerTileset->TileWidth();
                    TempRenderData.DBottomY =
                        TempRenderData.DY + DMarkerTileset->TileHeight();

                    if ((RightX < rect.DXPosition) ||
                        (TempRenderData.DX > ScreenRightX))
                    {
                        OnScreen = false;
                    }
                    else if ((TempRenderData.DBottomY < rect.DYPosition) ||
                             (TempRenderData.DY > ScreenBottomY))
                    {
                        OnScreen = false;
                    }
                    TempRenderData.DX -= rect.DXPosition;
                    TempRenderData.DY -= rect.DYPosition;
                    if (OnScreen)
                    {
                        int MarkerIndex =
                            LockedAsset->Step() / DAnimationDownsample;
                        if (MarkerIndex < DMarkerIndices.size())
                        {
                            DMarkerTileset->DrawTile(
                                surface, TempRenderData.DX, TempRenderData.DY,
                                DMarkerIndices[MarkerIndex]);
                        }
                    }
                }
            }
            else if ((0 <= to_underlying(TempRenderData.DType)) &&
                     (to_underlying(TempRenderData.DType) < DTilesets.size()))
            {
                int RightX, RectWidth, RectHeight;
                bool OnScreen = true;

                TempRenderData.DX =
                    LockedAsset->PositionX() - CPosition::HalfTileWidth();
                TempRenderData.DY =
                    LockedAsset->PositionY() - CPosition::HalfTileHeight();
                RectWidth = CPosition::TileWidth() * LockedAsset->Size();
                RectHeight = CPosition::TileHeight() * LockedAsset->Size();
                RightX = TempRenderData.DX + RectWidth;
                TempRenderData.DBottomY = TempRenderData.DY + RectHeight;
                if ((RightX < rect.DXPosition) ||
                    (TempRenderData.DX > ScreenRightX))
                {
                    OnScreen = false;
                }
                else if ((TempRenderData.DBottomY < rect.DYPosition) ||
                         (TempRenderData.DY > ScreenBottomY))
                {
                    OnScreen = false;
                }
                else if ((EAssetAction::MineGold == LockedAsset->Action()) ||
                         (EAssetAction::ConveyLumber ==
                          LockedAsset->Action()) ||
                         (EAssetAction::ConveyGold == LockedAsset->Action()))
                {
                    OnScreen = false;
                }
                TempRenderData.DX -= rect.DXPosition;
                TempRenderData.DY -= rect.DYPosition;
                if (OnScreen)
                {
                    ResourceContext->Rectangle(TempRenderData.DX,
                                               TempRenderData.DY, RectWidth,
                                               RectHeight);
                    ResourceContext->Stroke();
                }
            }
        }
    }
}

void CAssetRenderer::DrawOverlays(std::shared_ptr<CGraphicSurface> surface,
                                  const SRectangle &rect)
{
    int ScreenRightX = rect.DXPosition + rect.DWidth - 1;
    int ScreenBottomY = rect.DYPosition + rect.DHeight - 1;

    for (auto &AssetIterator : DPlayerMap->Assets())
    {
        SAssetRenderData TempRenderData;
        TempRenderData.DType = AssetIterator->Type();
        if (EAssetType::None == TempRenderData.DType)
        {
            if (EAssetAction::Attack == AssetIterator->Action())
            {
                int RightX;
                bool OnScreen = true;

                TempRenderData.DX =
                    AssetIterator->PositionX() - DArrowTileset->TileWidth() / 2;
                TempRenderData.DY = AssetIterator->PositionY() -
                                    DArrowTileset->TileHeight() / 2;
                RightX = TempRenderData.DX + DArrowTileset->TileWidth();
                TempRenderData.DBottomY =
                    TempRenderData.DY + DArrowTileset->TileHeight();

                if ((RightX < rect.DXPosition) ||
                    (TempRenderData.DX > ScreenRightX))
                {
                    OnScreen = false;
                }
                else if ((TempRenderData.DBottomY < rect.DYPosition) ||
                         (TempRenderData.DY > ScreenBottomY))
                {
                    OnScreen = false;
                }
                TempRenderData.DX -= rect.DXPosition;
                TempRenderData.DY -= rect.DYPosition;
                if (OnScreen)
                {
                    int ActionSteps = DArrowIndices.size();
                    ActionSteps /= to_underlying(EDirection::Max);

                    DArrowTileset->DrawTile(
                        surface, TempRenderData.DX, TempRenderData.DY,
                        DArrowIndices[to_underlying(AssetIterator->Direction()) *
                                          ActionSteps +
                                      ((DPlayerData->GameCycle() -
                                        AssetIterator->CreationCycle()) %
                                       ActionSteps)]);
                }
            }
        }
        else if (0 == AssetIterator->Speed())
        {
            EAssetAction CurrentAction = AssetIterator->Action();

            if (EAssetAction::Death != CurrentAction)
            {
                int HitRange = AssetIterator->HitPoints() *
                               DFireTilesets.size() * 2 /
                               AssetIterator->MaxHitPoints();

                if (EAssetAction::Construct == CurrentAction)
                {
                    auto Command = AssetIterator->CurrentCommand();

                    if (Command.DAssetTarget)
                    {
                        Command = Command.DAssetTarget->CurrentCommand();
                        if (Command.DActivatedCapability)
                        {
                            int Divisor =
                                Command.DActivatedCapability->PercentComplete(
                                    AssetIterator->MaxHitPoints());
                            Divisor = Divisor ? Divisor : 1;
                            HitRange = AssetIterator->HitPoints() *
                                       DFireTilesets.size() * 2 / Divisor;
                        }
                    }
                    else if (Command.DActivatedCapability)
                    {
                        int Divisor =
                            Command.DActivatedCapability->PercentComplete(
                                AssetIterator->MaxHitPoints());
                        Divisor = Divisor ? Divisor : 1;
                        HitRange = AssetIterator->HitPoints() *
                                   DFireTilesets.size() * 2 / Divisor;
                    }
                }

                if (HitRange < DFireTilesets.size())
                {
                    int TilesetIndex = DFireTilesets.size() - 1 - HitRange;
                    int RightX;

                    TempRenderData.DTileIndex =
                        (DPlayerData->GameCycle() -
                         AssetIterator->CreationCycle()) %
                        DFireTilesets[TilesetIndex]->TileCount();
                    TempRenderData.DX =
                        AssetIterator->PositionX() +
                        (AssetIterator->Size() - 1) *
                            CPosition::HalfTileWidth() -
                        DFireTilesets[TilesetIndex]->TileHalfWidth();
                    TempRenderData.DY =
                        AssetIterator->PositionY() +
                        (AssetIterator->Size() - 1) *
                            CPosition::HalfTileHeight() -
                        DFireTilesets[TilesetIndex]->TileHeight();

                    RightX = TempRenderData.DX +
                             DFireTilesets[TilesetIndex]->TileWidth() - 1;
                    TempRenderData.DBottomY =
                        TempRenderData.DY +
                        DFireTilesets[TilesetIndex]->TileHeight() - 1;
                    bool OnScreen = true;
                    if ((RightX < rect.DXPosition) ||
                        (TempRenderData.DX > ScreenRightX))
                    {
                        OnScreen = false;
                    }
                    else if ((TempRenderData.DBottomY < rect.DYPosition) ||
                             (TempRenderData.DY > ScreenBottomY))
                    {
                        OnScreen = false;
                    }
                    TempRenderData.DX -= rect.DXPosition;
                    TempRenderData.DY -= rect.DYPosition;
                    if (OnScreen)
                    {
                        DFireTilesets[TilesetIndex]->DrawTile(
                            surface, TempRenderData.DX, TempRenderData.DY,
                            TempRenderData.DTileIndex);
                    }
                }
            }
        }
    }
}

void CAssetRenderer::DrawPlacement(std::shared_ptr<CGraphicSurface> surface,
                                   const SRectangle &rect,
                                   const CPixelPosition &pos, EAssetType type,
                                   std::shared_ptr<CPlayerAsset> builder)
{
    int ScreenRightX = rect.DXPosition + rect.DWidth - 1;
    int ScreenBottomY = rect.DYPosition + rect.DHeight - 1;

    if (EAssetType::None != type)
    {
        CPixelPosition TempPosition;
        CTilePosition TempTilePosition;
        int PlacementRightX, PlacementBottomY;
        bool OnScreen = true;
        auto AssetType = CPlayerAssetType::FindDefaultFromType(type);
        std::vector<std::vector<int> > PlacementTiles;
        int XOff, YOff;

        TempTilePosition.SetFromPixel(pos);
        TempPosition.SetFromTile(TempTilePosition);

        TempPosition.IncrementX(
            (AssetType->Size() - 1) * CPosition::HalfTileWidth() -
            DTilesets[to_underlying(type)]->TileHalfWidth());
        TempPosition.IncrementY(
            (AssetType->Size() - 1) * CPosition::HalfTileHeight() -
            DTilesets[to_underlying(type)]->TileHalfHeight());
        PlacementRightX =
            TempPosition.X() + DTilesets[to_underlying(type)]->TileWidth();
        PlacementBottomY =
            TempPosition.Y() + DTilesets[to_underlying(type)]->TileHeight();

        TempTilePosition.SetFromPixel(TempPosition);
        XOff = 0;
        YOff = 0;
        PlacementTiles.resize(AssetType->Size());
        for (auto &Row : PlacementTiles)
        {
            Row.resize(AssetType->Size());
            for (auto &Cell : Row)
            {
                auto TileType = DPlayerMap->TileType(
                    TempTilePosition.X() + XOff, TempTilePosition.Y() + YOff);
                if (CTerrainMap::CanPlaceOn(TileType))
                {
                    Cell = 1;
                }
                else
                {
                    Cell = 0;
                }
                XOff++;
            }
            XOff = 0;
            YOff++;
        }
        XOff = TempTilePosition.X() + AssetType->Size();
        YOff = TempTilePosition.Y() + AssetType->Size();
        for (auto PlayerAsset : DPlayerMap->Assets())
        {
            int MinX, MaxX, MinY, MaxY;
            int Offset = EAssetType::GoldMine == PlayerAsset->Type() ? 1 : 0;

            if (builder == PlayerAsset)
            {
                continue;
            }
            if (XOff <= PlayerAsset->TilePositionX() - Offset)
            {
                continue;
            }
            if (TempTilePosition.X() >=
                (PlayerAsset->TilePositionX() + PlayerAsset->Size() + Offset))
            {
                continue;
            }
            if (YOff <= PlayerAsset->TilePositionY() - Offset)
            {
                continue;
            }
            if (TempTilePosition.Y() >=
                (PlayerAsset->TilePositionY() + PlayerAsset->Size() + Offset))
            {
                continue;
            }
            MinX = std::max(TempTilePosition.X(),
                            PlayerAsset->TilePositionX() - Offset);
            MaxX = std::min(XOff, PlayerAsset->TilePositionX() +
                                      PlayerAsset->Size() + Offset);
            MinY = std::max(TempTilePosition.Y(),
                            PlayerAsset->TilePositionY() - Offset);
            MaxY = std::min(YOff, PlayerAsset->TilePositionY() +
                                      PlayerAsset->Size() + Offset);
            for (int Y = MinY; Y < MaxY; Y++)
            {
                for (int X = MinX; X < MaxX; X++)
                {
                    PlacementTiles[Y - TempTilePosition.Y()]
                                  [X - TempTilePosition.X()] = 0;
                }
            }
        }

        if (PlacementRightX <= rect.DXPosition)
        {
            OnScreen = false;
        }
        else if (PlacementBottomY <= rect.DYPosition)
        {
            OnScreen = false;
        }
        else if (TempPosition.X() >= ScreenRightX)
        {
            OnScreen = false;
        }
        else if (TempPosition.Y() >= ScreenBottomY)
        {
            OnScreen = false;
        }
        if (OnScreen)
        {
            int XPos, YPos;
            TempPosition.X(TempPosition.X() - rect.DXPosition);
            TempPosition.Y(TempPosition.Y() - rect.DYPosition);
            DTilesets[to_underlying(type)]->DrawTile(
                surface, TempPosition.X(), TempPosition.Y(),
                DPlaceIndices[to_underlying(type)][0],
                to_underlying(DPlayerData->Color()) - 1);
            XPos = TempPosition.X();
            YPos = TempPosition.Y();
            for (auto &Row : PlacementTiles)
            {
                for (auto &Cell : Row)
                {
                    DMarkerTileset->DrawTile(
                        surface, XPos, YPos,
                        Cell ? DPlaceGoodIndex : DPlaceBadIndex);
                    XPos += DMarkerTileset->TileWidth();
                }
                YPos += DMarkerTileset->TileHeight();
                XPos = TempPosition.X();
            }
        }
    }
}

void CAssetRenderer::DrawMiniAssets(std::shared_ptr<CGraphicSurface> surface)
{
    auto ResourceContext = surface->CreateResourceContext();
    if (nullptr != DPlayerData)
    {
        for (auto &AssetIterator : DPlayerMap->Assets())
        {
            EPlayerColor AssetColor = AssetIterator->Color();
            int Size = AssetIterator->Size();
            if (AssetColor == DPlayerData->Color())
            {
                AssetColor = EPlayerColor::Max;
            }
            ResourceContext->SetSourceRGB(
                DPixelColors[to_underlying(AssetColor)]);
            ResourceContext->Rectangle(AssetIterator->TilePositionX(),
                                       AssetIterator->TilePositionY(), Size,
                                       Size);
            ResourceContext->Fill();
        }
    }
    else
    {
        for (auto &AssetIterator : DPlayerMap->AssetInitializationList())
        {
            EPlayerColor AssetColor =
                DLoadingPlayerColors[to_underlying(AssetIterator.DNumber)];
            int Size =
                CPlayerAssetType::FindDefaultFromName(AssetIterator.DType)
                    ->Size();

            ResourceContext->SetSourceRGB(
                DPixelColors[to_underlying(AssetColor)]);
            ResourceContext->Rectangle(AssetIterator.DTilePosition.X(),
                                       AssetIterator.DTilePosition.Y(), Size,
                                       Size);
            ResourceContext->Fill();
        }
    }
}
