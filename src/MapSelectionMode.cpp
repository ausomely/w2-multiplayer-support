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
#include "MapSelectionMode.h"
#include "ApplicationData.h"
#include "MainMenuMode.h"
#include "MemoryDataSource.h"
#include "MultiPlayerOptionsMenuMode.h"
#include "PlayerAIColorSelectMode.h"

std::shared_ptr<CMapSelectionMode> CMapSelectionMode::DMapSelectionModePointer;

CMapSelectionMode::CMapSelectionMode(const SPrivateConstructorType& key)
{
    DTitle = "Select Map";

    DButtonTexts.push_back("Select");
    DButtonFunctions.push_back(SelectMapButtonCallback);
    DButtonTexts.push_back("Cancel");
    DButtonFunctions.push_back(BackButtonCallback);
}

//! @brief Opens option for map selection
void CMapSelectionMode::SelectMapButtonCallback(
    std::shared_ptr<CApplicationData> context)
{
    for (int Index = 0; Index < to_underlying(EPlayerColor::Max); Index++)
    {
        context->DLoadingPlayerTypes[Index] = CApplicationData::ptNone;
        context->DPlayerNames[Index] = "None";
        context->DReadyPlayers[Index] = true;
        if (Index)
        {
            if (1 == Index)
            {
                context->DLoadingPlayerTypes[Index] = CApplicationData::ptHuman;
                context->DPlayerNames[Index] = context->DUsername;
                context->DReadyPlayers[Index] = false;
            }
            else if (Index <= context->DSelectedMap->PlayerCount())
            {
                context->DLoadingPlayerTypes[Index] =
                    CApplicationData::gstMultiPlayerHost ==
                            context->DGameSessionType
                        ? CApplicationData::ptHuman
                        : CApplicationData::ptAIEasy;
                context->DPlayerNames[Index] =
                    CApplicationData::gstMultiPlayerHost ==
                            context->DGameSessionType
                        ? "None"
                        : "AIEasy";
                context->DReadyPlayers[Index] =
                    CApplicationData::gstMultiPlayerHost ==
                            context->DGameSessionType
                        ? false
                        : true;
            }
        }
    }
    context->DPlayerNumber = EPlayerNumber::Player1;

    if (CApplicationData::gstMultiPlayerHost == context->DGameSessionType)
    {
        context->roomInfo.Clear();
        // configure room info
        context->roomInfo.set_host(context->DUsername);
        context->roomInfo.set_active(false);

        for(int Index = 0; Index < 3; Index++) {
            context->roomInfo.add_messages("");
        }

        for(auto &It: context->DLoadingPlayerTypes) {
            context->roomInfo.add_types(to_underlying(It));
        }

        for(auto &It: context->DLoadingPlayerColors) {
            context->roomInfo.add_colors(to_underlying(It));
        }

        for(auto &It: context->DPlayerNames) {
            context->roomInfo.add_players(It);
        }

        for(auto &It: context->DReadyPlayers) {
            context->roomInfo.add_ready(It);
        }

        context->roomInfo.set_map(context->DSelectedMap->MapName());
        context->roomInfo.set_size(1);
        context->roomInfo.set_capacity(context->DSelectedMap->PlayerCount());

        // send room info
        context->ClientPointer->SendRoomInfo(context);
    }
    context->ChangeApplicationMode(CPlayerAIColorSelectMode::Instance());
}

//! @brief Back to multi player options
void CMapSelectionMode::BackButtonCallback(
    std::shared_ptr<CApplicationData> context)
{
    if (CApplicationData::gstMultiPlayerHost == context->DGameSessionType)
    {
        context->ClientPointer->SendMessage("Back");
        context->ChangeApplicationMode(CMultiPlayerOptionsMenuMode::Instance());
    }
    else
    {
        context->ChangeApplicationMode(CMainMenuMode::Instance());
    }
}

//! @brief Loads and renders the map
void CMapSelectionMode::InitializeChange(
    std::shared_ptr<CApplicationData> context)
{
    DButtonHovered = false;
    DMapOffset = 0;

    context->ResetPlayerColors();
    context->DSelectedMapIndex = 0;
    context->DSelectedMap = CAssetDecoratedMap::DuplicateMap(0);
    context->DMapRenderer = std::make_shared<CMapRenderer>(
        std::make_shared<CMemoryDataSource>(
            context->DMapRendererConfigurationData),
        context->DTerrainTileset, context->DSelectedMap);
    context->DAssetRenderer = std::make_shared<CAssetRenderer>(
        context->DAssetRecolorMap, context->DAssetTilesets,
        context->DMarkerTileset, context->DCorpseTileset,
        context->DFireTilesets, context->DBuildingDeathTileset,
        context->DArrowTileset, nullptr, context->DSelectedMap);
    context->DMiniMapRenderer = std::make_shared<CMiniMapRenderer>(
        context->DMapRenderer, context->DAssetRenderer, nullptr, nullptr,
        context->DDoubleBufferSurface->Format());
}

void CMapSelectionMode::Input(std::shared_ptr<CApplicationData> context)
{
    int CurrentX, CurrentY;
    CurrentX = context->DCurrentX;
    CurrentY = context->DCurrentY;
    if (context->DLeftClick && !context->DLeftDown)
    {
        int ItemSelected = context->DMapSelectListViewRenderer->ItemAt(
            CurrentX - context->DMapSelectListViewXOffset,
            CurrentY - context->DMapSelectListViewYOffset);

        if (to_underlying(CListViewRenderer::EListViewObject::UpArrow) ==
            ItemSelected)
        {
            if (DMapOffset)
            {
                DMapOffset--;
            }
        }
        else if (to_underlying(CListViewRenderer::EListViewObject::DownArrow) ==
                 ItemSelected)
        {
            DMapOffset++;
        }
        else if (to_underlying(CListViewRenderer::EListViewObject::None) !=
                 ItemSelected)
        {
            if (context->DSelectedMapIndex != ItemSelected)
            {
                context->DSelectedMapIndex = ItemSelected;

                *context->DSelectedMap =
                    *CAssetDecoratedMap::GetMap(context->DSelectedMapIndex);
            }
        }
        else
        {
            for (int Index = 0; Index < DButtonLocations.size(); Index++)
            {
                if ((DButtonLocations[Index].DXPosition <= CurrentX) &&
                    ((DButtonLocations[Index].DXPosition +
                          DButtonLocations[Index].DWidth >
                      CurrentX)))
                {
                    if ((DButtonLocations[Index].DYPosition <= CurrentY) &&
                        ((DButtonLocations[Index].DYPosition +
                              DButtonLocations[Index].DHeight >
                          CurrentY)))
                    {
                        DButtonFunctions[Index](context);
                    }
                }
            }
        }
    }
}

void CMapSelectionMode::Calculate(std::shared_ptr<CApplicationData> context) {}

void CMapSelectionMode::Render(std::shared_ptr<CApplicationData> context)
{
    int CurrentX, CurrentY;
    int BufferWidth, BufferHeight;
    int MiniMapWidth, MiniMapHeight, MiniMapCenter, MiniMapLeft;
    int ListViewWidth = 0, ListViewHeight = 0;
    int TitleHeight;
    int TextWidth, TextHeight, TextTop;
    int TextColor, ShadowColor;
    std::string TempString;
    CButtonRenderer::EButtonState ButtonState =
        CButtonRenderer::EButtonState::None;
    bool ButtonXAlign = false, ButtonHovered = false;

    CurrentX = context->DCurrentX;
    CurrentY = context->DCurrentY;
    context->RenderMenuTitle(DTitle, TitleHeight, BufferWidth, BufferHeight);

    MiniMapWidth = context->DMiniMapSurface->Width();
    MiniMapHeight = context->DMiniMapSurface->Height();
    if (nullptr != context->DMapSelectListViewSurface)
    {
        ListViewWidth = context->DMapSelectListViewSurface->Width();
        ListViewHeight = context->DMapSelectListViewSurface->Height();
    }

    if ((ListViewHeight !=
         (BufferHeight - TitleHeight - context->DInnerBevel->Width() -
          context->DBorderWidth)) ||
        (ListViewWidth !=
         (BufferWidth - context->DViewportXOffset - context->DBorderWidth -
          context->DInnerBevel->Width() * 2)))
    {
        if (nullptr != context->DMapSelectListViewSurface)
        {
            context->DMapSelectListViewSurface = nullptr;
        }
        ListViewHeight = BufferHeight - TitleHeight -
                         context->DInnerBevel->Width() - context->DBorderWidth;
        ListViewWidth = BufferWidth - context->DViewportXOffset -
                        context->DBorderWidth -
                        context->DInnerBevel->Width() * 2;
        context->DMapSelectListViewSurface = CGraphicFactory::CreateSurface(
            ListViewWidth, ListViewHeight,
            context->DDoubleBufferSurface->Format());
    }

    context->DMapSelectListViewXOffset = context->DBorderWidth;
    context->DMapSelectListViewYOffset =
        TitleHeight + context->DInnerBevel->Width();

    context->DMapSelectListViewSurface->Draw(
        context->DWorkingBufferSurface, 0, 0, ListViewWidth, ListViewHeight,
        context->DMapSelectListViewXOffset, context->DMapSelectListViewYOffset);

    std::vector<std::string> MapNames;
    while (CAssetDecoratedMap::GetMap(MapNames.size()))
    {
        MapNames.push_back(
            CAssetDecoratedMap::GetMap(MapNames.size())->MapName());
    }

    context->DMapSelectListViewRenderer->DrawListView(
        context->DMapSelectListViewSurface, context->DSelectedMapIndex,
        DMapOffset, MapNames);
    context->DWorkingBufferSurface->Draw(context->DMapSelectListViewSurface,
                                         context->DMapSelectListViewXOffset,
                                         context->DMapSelectListViewYOffset,
                                         ListViewWidth, ListViewHeight, 0, 0);
    context->DInnerBevel->DrawBevel(
        context->DWorkingBufferSurface, context->DMapSelectListViewXOffset,
        context->DMapSelectListViewYOffset, ListViewWidth, ListViewHeight);

    context->DMiniMapRenderer->DrawMiniMap(context->DMiniMapSurface);
    MiniMapLeft = context->DMapSelectListViewXOffset + ListViewWidth +
                  context->DInnerBevel->Width() * 4;
    context->DWorkingBufferSurface->Draw(context->DMiniMapSurface, MiniMapLeft,
                                         context->DMapSelectListViewYOffset, -1,
                                         -1, 0, 0);
    context->DInnerBevel->DrawBevel(context->DWorkingBufferSurface, MiniMapLeft,
                                    context->DMapSelectListViewYOffset,
                                    MiniMapWidth, MiniMapHeight);

    TextTop = context->DMapSelectListViewYOffset + MiniMapHeight +
              context->DInnerBevel->Width() * 2;
    MiniMapCenter = MiniMapLeft + MiniMapWidth / 2;
    TextColor =
        context
            ->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]
            ->FindColor("white");
    ShadowColor =
        context
            ->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]
            ->FindColor("black");

    TempString =
        std::to_string(context->DSelectedMap->PlayerCount()) + " Players";
    context->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]
        ->MeasureText(TempString, TextWidth, TextHeight);
    context->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]
        ->DrawTextWithShadow(context->DWorkingBufferSurface,
                             MiniMapCenter - TextWidth / 2, TextTop, TextColor,
                             ShadowColor, 1, TempString);
    TextTop += TextHeight;
    TempString = std::to_string(context->DSelectedMap->Width()) + " x " +
                 std::to_string(context->DSelectedMap->Height());
    context->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]
        ->MeasureText(TempString, TextWidth, TextHeight);
    context->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]
        ->DrawTextWithShadow(context->DWorkingBufferSurface,
                             MiniMapCenter - TextWidth / 2, TextTop, TextColor,
                             ShadowColor, 1, TempString);
    TextTop += TextHeight * 3 / 2;

    DButtonLocations.resize(2);
    if ((MiniMapLeft <= CurrentX) && (MiniMapLeft + MiniMapWidth > CurrentX))
    {
        ButtonXAlign = true;
    }

    context->DButtonRenderer->Text(DButtonTexts[0], true);
    context->DButtonRenderer->Height(context->DButtonRenderer->Height() * 3 /
                                     2);
    context->DButtonRenderer->Width(MiniMapWidth);

    TextTop = BufferHeight - context->DBorderWidth -
              (context->DButtonRenderer->Height() * 9 / 4);
    if (ButtonXAlign)
    {
        if ((TextTop <= CurrentY) &&
            ((TextTop + context->DButtonRenderer->Height() > CurrentY)))
        {
            ButtonState = context->DLeftDown
                              ? CButtonRenderer::EButtonState::Pressed
                              : CButtonRenderer::EButtonState::Hover;
            ButtonHovered = true;
        }
    }
    context->DButtonRenderer->DrawButton(context->DWorkingBufferSurface,
                                         MiniMapLeft, TextTop, ButtonState);
    DButtonLocations[0] =
        SRectangle({MiniMapLeft, TextTop, context->DButtonRenderer->Width(),
                    context->DButtonRenderer->Height()});

    TextTop = BufferHeight - context->DBorderWidth -
              context->DButtonRenderer->Height();
    ButtonState = CButtonRenderer::EButtonState::None;
    if (ButtonXAlign)
    {
        if ((TextTop <= CurrentY) &&
            ((TextTop + context->DButtonRenderer->Height() > CurrentY)))
        {
            ButtonState = context->DLeftDown
                              ? CButtonRenderer::EButtonState::Pressed
                              : CButtonRenderer::EButtonState::Hover;
            ButtonHovered = true;
        }
    }
    context->DButtonRenderer->Text(DButtonTexts[1]);
    context->DButtonRenderer->DrawButton(context->DWorkingBufferSurface,
                                         MiniMapLeft, TextTop, ButtonState);
    DButtonLocations[1] =
        SRectangle({MiniMapLeft, TextTop, context->DButtonRenderer->Width(),
                    context->DButtonRenderer->Height()});

    if (!DButtonHovered && ButtonHovered)
    {
        context->DSoundLibraryMixer->PlayClip(
            context->DSoundLibraryMixer->FindClip("tick"),
            context->DSoundVolume, 0.0);
    }
    if (context->ModeIsChanging())
    {
        context->DSoundLibraryMixer->PlayClip(
            context->DSoundLibraryMixer->FindClip("place"),
            context->DSoundVolume, 0.0);
    }
    DButtonHovered = ButtonHovered;
}

std::shared_ptr<CApplicationMode> CMapSelectionMode::Instance()
{
    if (DMapSelectionModePointer == nullptr)
    {
        DMapSelectionModePointer =
            std::make_shared<CMapSelectionMode>(SPrivateConstructorType());
    }
    return DMapSelectionModePointer;
}
