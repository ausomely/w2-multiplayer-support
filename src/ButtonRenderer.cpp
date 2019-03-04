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
#include "ButtonRenderer.h"
#include "Debug.h"

CButtonRenderer::CButtonRenderer(std::shared_ptr<CGraphicRecolorMap> colors,
                                 std::shared_ptr<CBevel> innerbevel,
                                 std::shared_ptr<CBevel> outerbevel,
                                 std::shared_ptr<CFontTileset> font)
{
    DColorMap = colors;
    DOuterBevel = outerbevel;
    DInnerBevel = innerbevel;
    DFont = font;
    DButtonColor = EPlayerColor::None;
    DTextOffsetX = 0;
    DTextOffsetY = 0;

    SetBaseDimensions();

    DLightIndices.resize(to_underlying(EPlayerColor::Max));
    DDarkIndices.resize(to_underlying(EPlayerColor::Max));
    DDarkIndices[to_underlying(EPlayerColor::None)] =
        DDarkIndices[to_underlying(EPlayerColor::Blue)] =
            DColorMap->FindColor("blue-dark");
    DDarkIndices[to_underlying(EPlayerColor::Red)] =
        DColorMap->FindColor("red-dark");
    DDarkIndices[to_underlying(EPlayerColor::Green)] =
        DColorMap->FindColor("green-dark");
    DDarkIndices[to_underlying(EPlayerColor::Purple)] =
        DColorMap->FindColor("purple-dark");
    DDarkIndices[to_underlying(EPlayerColor::Orange)] =
        DColorMap->FindColor("orange-dark");
    DDarkIndices[to_underlying(EPlayerColor::Yellow)] =
        DColorMap->FindColor("yellow-dark");
    DDarkIndices[to_underlying(EPlayerColor::Black)] =
        DColorMap->FindColor("black-dark");
    DDarkIndices[to_underlying(EPlayerColor::White)] =
        DColorMap->FindColor("white-dark");

    DLightIndices[to_underlying(EPlayerColor::None)] =
        DLightIndices[to_underlying(EPlayerColor::Blue)] =
            DColorMap->FindColor("blue-light");
    DLightIndices[to_underlying(EPlayerColor::Red)] =
        DColorMap->FindColor("red-light");
    DLightIndices[to_underlying(EPlayerColor::Green)] =
        DColorMap->FindColor("green-light");
    DLightIndices[to_underlying(EPlayerColor::Purple)] =
        DColorMap->FindColor("purple-light");
    DLightIndices[to_underlying(EPlayerColor::Orange)] =
        DColorMap->FindColor("orange-light");
    DLightIndices[to_underlying(EPlayerColor::Yellow)] =
        DColorMap->FindColor("yellow-light");
    DLightIndices[to_underlying(EPlayerColor::Black)] =
        DColorMap->FindColor("black-light");
    DLightIndices[to_underlying(EPlayerColor::White)] =
        DColorMap->FindColor("white-light");

    DWhiteIndex = DFont->FindColor("white");
    DGoldIndex = DFont->FindColor("gold");
    DBlackIndex = DFont->FindColor("black");
    PrintDebug(DEBUG_HIGH, "CButtonRenderer w = %d, g = %d, b = %d\n",
               DWhiteIndex, DGoldIndex, DBlackIndex);
}

//! Set a initial values for button dimensions
void CButtonRenderer::SetBaseDimensions()
{
    DHeight = DWidth = DOuterBevel->Width() * 2;
}

//! Use when the button size is already set
void CButtonRenderer::SetButtonText(std::string &text)
{
    DText = text;
}

std::string CButtonRenderer::Text(const std::string &text, bool minimize)
{
    int TotalWidth, TotalHeight, Top, Bottom;
    DText = text;
    DFont->MeasureTextDetailed(DText, TotalWidth, TotalHeight, Top, Bottom);

    TotalHeight = Bottom - Top + 1;
    if (TotalHeight + DOuterBevel->Width() * 2 > DHeight)
    {
        DHeight = TotalHeight + DOuterBevel->Width() * 2;
    }
    else if (minimize)
    {
        DHeight = TotalHeight + DOuterBevel->Width() * 2;
    }
    if (TotalWidth + DOuterBevel->Width() * 2 > DWidth)
    {
        DWidth = TotalWidth + DOuterBevel->Width() * 2;
    }
    else if (minimize)
    {
        DWidth = TotalWidth + DOuterBevel->Width() * 2;
    }
    DTextOffsetX = DWidth / 2 - TotalWidth / 2;
    DTextOffsetY = DHeight / 2 - TotalHeight / 2 - Top;
    return DText;
}

int CButtonRenderer::Width(int width)
{
    if (width > DWidth)
    {
        int TotalWidth, TotalHeight, Top, Bottom;

        DFont->MeasureTextDetailed(DText, TotalWidth, TotalHeight, Top, Bottom);
        DWidth = width;
        DTextOffsetX = DWidth / 2 - TotalWidth / 2;
    }
    // ZF/MLH: hack to change button width
    //DWidth = width;

    return DWidth;
}

int CButtonRenderer::Height(int height)
{
    if (height > DHeight)
    {
        int TotalWidth, TotalHeight, Top, Bottom;

        DFont->MeasureTextDetailed(DText, TotalWidth, TotalHeight, Top, Bottom);
        TotalHeight = Bottom - Top + 1;
        DHeight = height;
        DTextOffsetY = DHeight / 2 - TotalHeight / 2 - Top;
    }
    // ZF/MLH: hack to change button height
    //DHeight = height;

    return DHeight;
}

void CButtonRenderer::DrawButton(std::shared_ptr<CGraphicSurface> surface,
                                 int x, int y, EButtonState state)
{
    auto ResourceContext = surface->CreateResourceContext();
    if (EButtonState::Pressed == state)
    {
        int BevelWidth = DInnerBevel->Width();

        ResourceContext->SetSourceRGBA(DColorMap->ColorValue(
            DDarkIndices[to_underlying(DButtonColor)], 0));
        ResourceContext->Rectangle(x, y, DWidth, DHeight);
        ResourceContext->Fill();
        // DColorMap->DrawTileRectangle(drawable, x, y, DWidth, DHeight,
        // DDarkIndices[DButtonColor]);
        DFont->DrawTextWithShadow(surface, x + DTextOffsetX, y + DTextOffsetY,
                                  DWhiteIndex, DBlackIndex, 1, DText);
        DInnerBevel->DrawBevel(surface, x + BevelWidth, y + BevelWidth,
                               DWidth - BevelWidth * 2,
                               DHeight - BevelWidth * 2);
    }
    else if (EButtonState::Inactive == state)
    {
        int BevelWidth = DOuterBevel->Width();

        ResourceContext->SetSourceRGBA(DColorMap->ColorValue(
            DDarkIndices[to_underlying(DButtonColor)], 0));
        ResourceContext->Rectangle(x, y, DWidth, DHeight);
        ResourceContext->Fill();
        // DColorMap->DrawTileRectangle(drawable, x, y, DWidth, DHeight,
        // DDarkIndices[pcBlack]);
        DFont->DrawTextWithShadow(surface, x + DTextOffsetX, y + DTextOffsetY,
                                  DBlackIndex, DWhiteIndex, 1, DText);
        DOuterBevel->DrawBevel(surface, x + BevelWidth, y + BevelWidth,
                               DWidth - BevelWidth * 2,
                               DHeight - BevelWidth * 2);
    }
    else
    {
        int BevelWidth = DOuterBevel->Width();

        ResourceContext->SetSourceRGBA(DColorMap->ColorValue(
            DLightIndices[to_underlying(DButtonColor)], 0));
        ResourceContext->Rectangle(x, y, DWidth, DHeight);
        ResourceContext->Fill();
        // DColorMap->DrawTileRectangle(drawable, x, y, DWidth, DHeight,
        // DLightIndices[DButtonColor]);
        DFont->DrawTextWithShadow(
            surface, x + DTextOffsetX, y + DTextOffsetY,
            EButtonState::Hover == state ? DWhiteIndex : DGoldIndex,
            DBlackIndex, 1, DText);
        DOuterBevel->DrawBevel(surface, x + BevelWidth, y + BevelWidth,
                               DWidth - BevelWidth * 2,
                               DHeight - BevelWidth * 2);
    }
}