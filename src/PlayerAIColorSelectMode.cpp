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
#include "PlayerAIColorSelectMode.h"
#include "ApplicationData.h"
#include "BattleMode.h"
#include "MainMenuMode.h"
#include "MapSelectionMode.h"
#include "MemoryDataSource.h"
#include "MultiPlayerOptionsMenuMode.h"

std::shared_ptr<CPlayerAIColorSelectMode>
    CPlayerAIColorSelectMode::DPlayerAIColorSelectModePointer;

CPlayerAIColorSelectMode::CPlayerAIColorSelectMode(
    const SPrivateConstructorType& key)
{
    DTitle = "Select Colors/Difficulty";

    DButtonTexts.push_back("Play Game");
    DButtonFunctions.push_back(PlayGameButtonCallback);
    DButtonTexts.push_back("Cancel");
    DButtonFunctions.push_back(BackButtonCallback);
}

void CPlayerAIColorSelectMode::PlayGameButtonCallback(
    std::shared_ptr<CApplicationData> context)
{
    context->ChangeApplicationMode(CBattleMode::Instance());
}

void CPlayerAIColorSelectMode::BackButtonCallback(
    std::shared_ptr<CApplicationData> context)
{
    if (CApplicationData::gstMultiPlayerClient == context->DGameSessionType)
    {
        context->ChangeApplicationMode(CMultiPlayerOptionsMenuMode::Instance());
    }
    else
    {
        context->ChangeApplicationMode(CMapSelectionMode::Instance());
    }
}

void CPlayerAIColorSelectMode::InitializeChange(
    std::shared_ptr<CApplicationData> context)
{
    DButtonHovered = false;
    DButtonLocations.clear();
    DColorButtonLocations.clear();
    DPlayerTypeButtonLocations.clear();
}

void CPlayerAIColorSelectMode::Input(std::shared_ptr<CApplicationData> context)
{
    int CurrentX, CurrentY;
    CurrentX = context->DCurrentX;
    CurrentY = context->DCurrentY;
    DPlayerNumberRequestingChange = EPlayerNumber::Neutral;
    DPlayerColorChangeRequest = EPlayerColor::None;
    DPlayerNumberRequesTypeChange = EPlayerNumber::Neutral;
    if (context->DLeftClick && !context->DLeftDown)
    {
        for (int Index = 0; Index < DColorButtonLocations.size(); Index++)
        {
            if (DColorButtonLocations[Index].PointInside(CurrentX, CurrentY))
            {
                int PlayerSelecting =
                    1 + (Index / (to_underlying(EPlayerNumber::Max) - 1));
                int ColorSelecting =
                    1 + (Index % (to_underlying(EPlayerColor::Max) - 1));
                if ((PlayerSelecting ==
                     to_underlying(context->DPlayerNumber)) ||
                    (CApplicationData::gstMultiPlayerClient !=
                     context->DGameSessionType))
                {
                    if ((PlayerSelecting ==
                         to_underlying(context->DPlayerNumber)) ||
                        (CApplicationData::ptHuman !=
                         context->DLoadingPlayerTypes[PlayerSelecting]))
                    {
                        DPlayerNumberRequestingChange =
                            static_cast<EPlayerNumber>(PlayerSelecting);
                        DPlayerColorChangeRequest =
                            static_cast<EPlayerColor>(ColorSelecting);
                    }
                }
            }
        }
        for (int Index = 0; Index < DButtonLocations.size(); Index++)
        {
            if (DButtonLocations[Index].PointInside(CurrentX, CurrentY))
            {
                DButtonFunctions[Index](context);
            }
        }
        for (int Index = 0; Index < DPlayerTypeButtonLocations.size(); Index++)
        {
            if (DPlayerTypeButtonLocations[Index].PointInside(CurrentX,
                                                              CurrentY))
            {
                DPlayerNumberRequesTypeChange =
                    static_cast<EPlayerNumber>(Index + 2);
                break;
            }
        }
    }
}

void CPlayerAIColorSelectMode::Calculate(
    std::shared_ptr<CApplicationData> context)
{
    if (EPlayerNumber::Neutral != DPlayerNumberRequestingChange)
    {
        EPlayerNumber NewColorInUseBy = EPlayerNumber::Neutral;
        for (int Index = 1; Index < to_underlying(EPlayerNumber::Max); Index++)
        {
            if (Index != to_underlying(DPlayerNumberRequestingChange))
            {
                if (CApplicationData::ptNone !=
                    context->DLoadingPlayerTypes[Index])
                {
                    if (context->DLoadingPlayerColors[Index] ==
                        DPlayerColorChangeRequest)
                    {
                        NewColorInUseBy = static_cast<EPlayerNumber>(Index);
                        break;
                    }
                }
            }
        }
        if (EPlayerNumber::Neutral != NewColorInUseBy)
        {
            context->DLoadingPlayerColors[to_underlying(NewColorInUseBy)] =
                context->DLoadingPlayerColors[to_underlying(
                    DPlayerNumberRequestingChange)];
        }
        context->DLoadingPlayerColors[to_underlying(
            DPlayerNumberRequestingChange)] = DPlayerColorChangeRequest;

        context->DAssetRenderer->UpdateLoadingPlayerColors(
            context->DLoadingPlayerColors);
    }
    if (EPlayerNumber::Neutral != DPlayerNumberRequesTypeChange)
    {
        if (CApplicationData::gstSinglePlayer == context->DGameSessionType)
        {
            switch (context->DLoadingPlayerTypes[to_underlying(
                DPlayerNumberRequesTypeChange)])
            {
                case CApplicationData::ptAIEasy:
                    context->DLoadingPlayerTypes[to_underlying(
                        DPlayerNumberRequesTypeChange)] =
                        CApplicationData::ptAIMedium;
                    break;
                case CApplicationData::ptAIMedium:
                    context->DLoadingPlayerTypes[to_underlying(
                        DPlayerNumberRequesTypeChange)] =
                        CApplicationData::ptAIHard;
                    break;
                default:
                    context->DLoadingPlayerTypes[to_underlying(
                        DPlayerNumberRequesTypeChange)] =
                        CApplicationData::ptAIEasy;
                    break;
            }
        }
        else if (CApplicationData::gstMultiPlayerHost ==
                 context->DGameSessionType)
        {
            switch (context->DLoadingPlayerTypes[to_underlying(
                DPlayerNumberRequesTypeChange)])
            {
                case CApplicationData::ptHuman:
                    context->DLoadingPlayerTypes[to_underlying(
                        DPlayerNumberRequesTypeChange)] =
                        CApplicationData::ptAIEasy;
                    break;
                case CApplicationData::ptAIEasy:
                    context->DLoadingPlayerTypes[to_underlying(
                        DPlayerNumberRequesTypeChange)] =
                        CApplicationData::ptAIMedium;
                    break;
                case CApplicationData::ptAIMedium:
                    context->DLoadingPlayerTypes[to_underlying(
                        DPlayerNumberRequesTypeChange)] =
                        CApplicationData::ptAIHard;
                    break;
                case CApplicationData::ptAIHard:
                    context->DLoadingPlayerTypes[to_underlying(
                        DPlayerNumberRequesTypeChange)] =
                        CApplicationData::ptNone;
                    break;
                default:
                    context->DLoadingPlayerTypes[to_underlying(
                        DPlayerNumberRequesTypeChange)] =
                        CApplicationData::ptHuman;
                    break;
            }
        }
    }
}

void CPlayerAIColorSelectMode::Render(std::shared_ptr<CApplicationData> context)
{
    int CurrentX, CurrentY;
    int BufferWidth, BufferHeight;
    int TitleHeight;
    int TextWidth, TextHeight, MaxTextWidth;
    int ColumnWidth, RowHeight;
    int MiniMapWidth, MiniMapHeight, MiniMapCenter, MiniMapLeft;
    int TextTop, ButtonLeft, ButtonTop, AIButtonLeft, ColorButtonHeight;
    int GoldColor, WhiteColor, ShadowColor;
    std::string TempString;
    std::array<std::string, to_underlying(EPlayerNumber::Max)> PlayerNames;
    CButtonRenderer::EButtonState ButtonState =
        CButtonRenderer::EButtonState::None;
    bool ButtonXAlign = false, ButtonHovered = false;

    CurrentX = context->DCurrentX;
    CurrentY = context->DCurrentY;

    context->RenderMenuTitle(DTitle, TitleHeight, BufferWidth, BufferHeight);

    GoldColor =
        context
            ->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]
            ->FindColor("gold");
    WhiteColor =
        context
            ->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]
            ->FindColor("white");
    ShadowColor =
        context
            ->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]
            ->FindColor("black");

    MiniMapWidth = context->DMiniMapSurface->Width();
    MiniMapHeight = context->DMiniMapSurface->Height();

    context->DMiniMapRenderer->DrawMiniMap(context->DMiniMapSurface);
    MiniMapLeft = BufferWidth - MiniMapWidth - context->DBorderWidth;
    context->DWorkingBufferSurface->Draw(
        context->DMiniMapSurface, MiniMapLeft,
        TitleHeight + context->DInnerBevel->Width(), -1, -1, 0, 0);
    context->DInnerBevel->DrawBevel(context->DWorkingBufferSurface, MiniMapLeft,
                                    TitleHeight + context->DInnerBevel->Width(),
                                    MiniMapWidth, MiniMapHeight);

    TextTop = TitleHeight + MiniMapHeight + context->DInnerBevel->Width() * 3;
    MiniMapCenter = MiniMapLeft + MiniMapWidth / 2;

    TempString =
        std::to_string(context->DSelectedMap->PlayerCount()) + " Players";
    context->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]
        ->MeasureText(TempString, TextWidth, TextHeight);
    context->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]
        ->DrawTextWithShadow(context->DWorkingBufferSurface,
                             MiniMapCenter - TextWidth / 2, TextTop, WhiteColor,
                             ShadowColor, 1, TempString);
    TextTop += TextHeight;
    TempString = std::to_string(context->DSelectedMap->Width()) + " x " +
                 std::to_string(context->DSelectedMap->Height());
    context->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]
        ->MeasureText(TempString, TextWidth, TextHeight);
    context->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]
        ->DrawTextWithShadow(context->DWorkingBufferSurface,
                             MiniMapCenter - TextWidth / 2, TextTop, WhiteColor,
                             ShadowColor, 1, TempString);

    TextTop = TitleHeight;
    TempString = "Player";
    context->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]
        ->MeasureText(TempString, TextWidth, TextHeight);
    context->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]
        ->DrawTextWithShadow(context->DWorkingBufferSurface,
                             context->DBorderWidth, TextTop, WhiteColor,
                             ShadowColor, 1, TempString);
    TextTop += TextHeight;

    context->DButtonRenderer->Text("AI Easy", true);
    ColorButtonHeight = context->DButtonRenderer->Height();
    RowHeight =
        context->DButtonRenderer->Height() + context->DInnerBevel->Width() * 2;
    if (RowHeight < TextHeight)
    {
        RowHeight = TextHeight;
    }
    context->DButtonRenderer->Text("X", true);
    context->DButtonRenderer->Height(ColorButtonHeight);
    ColumnWidth =
        context->DButtonRenderer->Width() + context->DInnerBevel->Width() * 2;
    MaxTextWidth = 0;
    for (int Index = 1; Index <= context->DSelectedMap->PlayerCount(); Index++)
    {
        if (Index == to_underlying(context->DPlayerNumber))
        {
            PlayerNames[Index] = TempString = std::to_string(Index) + ". You";
        }
        else if (CApplicationData::ptHuman !=
                 context->DLoadingPlayerTypes[Index])
        {
            PlayerNames[Index] = TempString =
                std::to_string(Index) + ". Player " + std::to_string(Index);
        }
        context
            ->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]
            ->MeasureText(TempString, TextWidth, TextHeight);
        if (MaxTextWidth < TextWidth)
        {
            MaxTextWidth = TextWidth;
        }
    }
    TempString = "Color";
    context->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]
        ->MeasureText(TempString, TextWidth, TextHeight);
    context->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]
        ->DrawTextWithShadow(
            context->DWorkingBufferSurface,
            context->DBorderWidth + MaxTextWidth +
                (ColumnWidth * (to_underlying(EPlayerColor::Max) + 1)) / 2 -
                TextWidth / 2,
            TitleHeight, WhiteColor, ShadowColor, 1, TempString);
    DColorButtonLocations.clear();
    for (int Index = 1; Index <= context->DSelectedMap->PlayerCount(); Index++)
    {
        TempString = PlayerNames[Index];
        context
            ->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]
            ->MeasureText(TempString, TextWidth, TextHeight);
        context
            ->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]
            ->DrawTextWithShadow(
                context->DWorkingBufferSurface, context->DBorderWidth, TextTop,
                Index == to_underlying(context->DPlayerNumber) ? GoldColor
                                                               : WhiteColor,
                ShadowColor, 1, TempString);
        for (int ColorIndex = 1; ColorIndex < to_underlying(EPlayerColor::Max);
             ColorIndex++)
        {
            int ButtonLeft =
                context->DBorderWidth + MaxTextWidth + ColorIndex * ColumnWidth;
            context->DButtonRenderer->Text(
                to_underlying(context->DLoadingPlayerColors[Index]) ==
                        ColorIndex
                    ? "X"
                    : "");
            context->DButtonRenderer->ButtonColor(
                static_cast<EPlayerColor>(ColorIndex));
            context->DButtonRenderer->DrawButton(
                context->DWorkingBufferSurface, ButtonLeft, TextTop,
                CButtonRenderer::EButtonState::None);
            DColorButtonLocations.push_back(SRectangle(
                {ButtonLeft, TextTop, context->DButtonRenderer->Width(),
                 context->DButtonRenderer->Height()}));
            AIButtonLeft = ButtonLeft + ColumnWidth;
        }
        TextTop += RowHeight;
    }

    context->DButtonRenderer->ButtonColor(EPlayerColor::None);
    TempString = "AI Easy";
    context->DButtonRenderer->Text(TempString);
    context->DButtonRenderer->Width(context->DButtonRenderer->Width() * 3 / 2);

    TextTop = TitleHeight;
    TempString = "Difficulty";
    context->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]
        ->MeasureText(TempString, TextWidth, TextHeight);
    context->DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]
        ->DrawTextWithShadow(
            context->DWorkingBufferSurface,
            AIButtonLeft + (context->DButtonRenderer->Width() - TextWidth) / 2,
            TextTop, WhiteColor, ShadowColor, 1, TempString);

    ButtonXAlign = false;
    if ((AIButtonLeft <= CurrentX) &&
        (AIButtonLeft + context->DButtonRenderer->Width() > CurrentX))
    {
        ButtonXAlign = true;
    }
    TextTop += RowHeight + TextHeight;
    DPlayerTypeButtonLocations.clear();
    for (int Index = 2; Index <= context->DSelectedMap->PlayerCount(); Index++)
    {
        switch (context->DLoadingPlayerTypes[Index])
        {
            case CApplicationData::ptHuman:
                context->DButtonRenderer->Text("Human");
                break;
            case CApplicationData::ptAIEasy:
                context->DButtonRenderer->Text("AI Easy");
                break;
            case CApplicationData::ptAIMedium:
                context->DButtonRenderer->Text("AI Medium");
                break;
            case CApplicationData::ptAIHard:
                context->DButtonRenderer->Text("AI Hard");
                break;
            default:
                context->DButtonRenderer->Text("Closed");
                break;
        }
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
        context->DButtonRenderer->DrawButton(
            context->DWorkingBufferSurface, AIButtonLeft, TextTop, ButtonState);
        DPlayerTypeButtonLocations.push_back(SRectangle(
            {AIButtonLeft, TextTop, context->DButtonRenderer->Width(),
             context->DButtonRenderer->Height()}));

        TextTop += RowHeight;
    }

    DButtonLocations.clear();

    context->DButtonRenderer->ButtonColor(EPlayerColor::None);

    context->DButtonRenderer->Text(DButtonTexts[0], true);
    context->DButtonRenderer->Height(context->DButtonRenderer->Height() * 3 /
                                     2);
    context->DButtonRenderer->Width(MiniMapWidth);
    ButtonLeft =
        BufferWidth - context->DButtonRenderer->Width() - context->DBorderWidth;
    ButtonTop = BufferHeight - (context->DButtonRenderer->Height() * 9 / 4) -
                context->DBorderWidth;
    ButtonState = CButtonRenderer::EButtonState::None;
    if ((ButtonLeft <= CurrentX) &&
        (ButtonLeft + context->DButtonRenderer->Width() > CurrentX))
    {
        ButtonXAlign = true;
    }
    if (ButtonXAlign)
    {
        if ((ButtonTop <= CurrentY) &&
            ((ButtonTop + context->DButtonRenderer->Height() > CurrentY)))
        {
            ButtonState = context->DLeftDown
                              ? CButtonRenderer::EButtonState::Pressed
                              : CButtonRenderer::EButtonState::Hover;
            ButtonHovered = true;
        }
    }
    context->DButtonRenderer->DrawButton(context->DWorkingBufferSurface,
                                         ButtonLeft, ButtonTop, ButtonState);
    DButtonLocations.push_back(
        SRectangle({ButtonLeft, ButtonTop, context->DButtonRenderer->Width(),
                    context->DButtonRenderer->Height()}));

    ButtonTop = BufferHeight - context->DButtonRenderer->Height() -
                context->DBorderWidth;
    ButtonState = CButtonRenderer::EButtonState::None;
    if (ButtonXAlign)
    {
        if ((ButtonTop <= CurrentY) &&
            ((ButtonTop + context->DButtonRenderer->Height() > CurrentY)))
        {
            ButtonState = context->DLeftDown
                              ? CButtonRenderer::EButtonState::Pressed
                              : CButtonRenderer::EButtonState::Hover;
            ButtonHovered = true;
        }
    }

    context->DButtonRenderer->Text(DButtonTexts[1], false);
    context->DButtonRenderer->DrawButton(context->DWorkingBufferSurface,
                                         ButtonLeft, ButtonTop, ButtonState);
    DButtonLocations.push_back(
        SRectangle({ButtonLeft, ButtonTop, context->DButtonRenderer->Width(),
                    context->DButtonRenderer->Height()}));

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

std::shared_ptr<CApplicationMode> CPlayerAIColorSelectMode::Instance()
{
    if (DPlayerAIColorSelectModePointer == nullptr)
    {
        DPlayerAIColorSelectModePointer =
            std::make_shared<CPlayerAIColorSelectMode>(
                SPrivateConstructorType());
    }
    return DPlayerAIColorSelectModePointer;
}
