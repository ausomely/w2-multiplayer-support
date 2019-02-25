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
#ifndef INGAMEMENUOVERLAY_H
#define INGAMEMENUOVERLAY_H

#include <string>
#include <vector>
#include "OverlayMode.h"
#include "VerticalButtonAlignment.h"


class COverlayManagement;

class CInGameMenuOverlay : public COverlayMode
{
    friend class CButton;

  protected:
    struct SPrivateConstructorType {};

    static std::shared_ptr<CInGameMenuOverlay> DInGameMenuOverlayPointer;
    std::string DTitle;
    std::vector<std::string> DButtonTexts;
//    std::vector<TOverlayCallbackFunction> DButtonFunctions;
    std::shared_ptr<CVerticalButtonAlignment> DButtonStack;

    static std::shared_ptr<COverlayManagement> DOverlayManager;
    static std::shared_ptr<CApplicationData> DContext;

    CInGameMenuOverlay(const CInGameMenuOverlay &) = delete;
    const CInGameMenuOverlay &operator=(const CInGameMenuOverlay &) = delete;

  public:
    ~CInGameMenuOverlay(){};
    explicit CInGameMenuOverlay(const SPrivateConstructorType &key);
    virtual void Input() override;
    virtual void Draw(int x, int y, bool clicked) override;
    static std::shared_ptr<COverlayMode> Initialize();
};

#endif
