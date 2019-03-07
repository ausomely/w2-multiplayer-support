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
#ifndef CHATOVERLAY_H
#define CHATOVERLAY_H

#include <memory>
#include "ApplicationData.h"
#include "EditRenderer.h"
#include "BattleMode.h"
#include "Rectangle.h"
#include "ButtonDescriptionRenderer.h"
#include "SurfaceBuilder.h"

class CApplicationData;
class CGraphicSurface;
class CEditRenderer;

enum class EChatLocation
{
    Lobby = 0,
    GameScreen
};

class CChatOverlay
{
    friend class CBattleMode;
    friend class CPlayerAIColorSelectMode;

  protected:
    std::shared_ptr<CApplicationData> DContext;
    std::shared_ptr<CGraphicSurface> DSurface;
    std::shared_ptr<CEditRenderer> DEditRenderer;
    EChatLocation DScreen;
    int DWidth;
    int DHeight;
    int DXoffset;
    int DYoffset;


    int DFontID;
    int DTextMaxWidth;
    int DTextMaxHeight;
    int DEditYoffset;

    int DMaxCharacters;

    uint32_t DSurfaceColor;
    int DGoldColor;
    int DWhiteColor;
    int DShadowColor;

    bool DEditSelected;
    int DEditSelectedCharacter;
    std::string DEditText;
    std::unique_ptr<SRectangle> DEditLocation;

    std::shared_ptr<CGraphicSurface> Surface()
    {
        return DSurface;
    }

    void SetDefaults();
    void ClearMouseButtonState();
    void SetupFontsAndRenderers();
    void SetupSurface();

    // Delete copy constructor
    CChatOverlay(const CChatOverlay &) = delete;
    const CChatOverlay &operator=(const CChatOverlay &) = delete;

  public:

    virtual ~CChatOverlay()
    {
    };

    explicit CChatOverlay(std::shared_ptr<CApplicationData> context,
        EChatLocation screen);

    void DrawBackground();
    void InitializeChat();
    void ProcessTextEntryFields(int x, int y, bool clicked);
    void ProcessKeyStrokes();
    void DrawChatText();
    void DrawText(std::string text, int xoffset, int yoffset, int fgcolor,
        int bgcolor);
    void DrawTextEntryField();
    void HideChat();
    void PixelPositionOnChat(int &CurrentX, int &CurrentY);
    void SetTextColors();
    void ClearChatTextArea();

    int Xoffset()
    {
        return DXoffset;
    }
    int Yoffset()
    {
        return DYoffset;
    }
    int Width()
    {
        return DWidth;
    }
    int Height()
    {
        return DHeight;
    }

};

#endif
