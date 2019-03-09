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
#ifndef SOUNDOPTIONSOVERLAY_H
#define SOUNDOPTIONSOVERLAY_H

#include <string>
#include <vector>
#include "OverlayMode.h"
#include "Rectangle.h"
#include "HorizontalButtonAlignment.h"
#include "EditRenderer.h"

using TSoundCallbackFunction = void (*)(std::shared_ptr<CApplicationData>);
using TSoundValidTextCallbackFunction = bool (*)(const std::string &);


class COverlayManagement;

class CSoundOptionsOverlay : public COverlayMode
{
    friend class CButton;

  protected:
    std::shared_ptr<COverlayManagement> DOverlayManager;
    std::shared_ptr<CApplicationData> DContext;

    struct SPrivateConstructorType
    {
    };

    int DGoldColor;
    int DWhiteColor;
    int DShadowColor;

    int DTextXoffset;
    int DTextYoffset;
    int DTextMaxWidth;
    int DTextMaxHeight;
    int DFontID;

    int DEditSelected;
    int DEditSelectedCharacter;
    std::vector<std::string> DButtonTexts;
    std::vector<std::string> DEditTitles;
    std::vector<std::string> DEditText;
    std::vector<SRectangle> DEditLocations;

    std::shared_ptr<CEditRenderer> DEditRenderer;
    std::shared_ptr<CHorizontalButtonAlignment> DButtonStack;

  public:
    ~CSoundOptionsOverlay()
    {
    };
    explicit CSoundOptionsOverlay(std::shared_ptr<COverlayManagement> manager);
    void Input(int x, int y, bool clicked) override;
    void Draw(int x, int y, bool clicked) override;
    void DrawTextFields();
    void ProcessButtonStack();
    void ProcessTextEntryFields(int x, int y, bool clicked);
    void ProcessKeyStrokes();
    bool ValidateSoundLevel(const std::string &str);
    void SaveSettings();
    void SetTextColors();

};

#endif
