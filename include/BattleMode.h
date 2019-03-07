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
#ifndef BATTLEMODE_H
#define BATTLEMODE_H

#include <string>
#include <vector>
#include "ChatOverlay.h"
#include "ApplicationMode.h"
#include "Rectangle.h"
#include "OverlayManagement.h"

// using TBattleModeCallbackFunction = void (*)(std::shared_ptr<
// CApplicationData >);

class CChatOverlay;

class CBattleMode : public CApplicationMode
{
  protected:
    struct SPrivateConstructorType
    {
    };
    static std::shared_ptr<CBattleMode> DBattleModePointer;
    std::shared_ptr<COverlayManagement> DOverlayManager;
    std::shared_ptr<CChatOverlay> DChatOverlay;

    CBattleMode(const CBattleMode &) = delete;
    const CBattleMode &operator=(const CBattleMode &) = delete;

    int DScenarioTimmer;

  public:
    explicit CBattleMode(const SPrivateConstructorType &key);
    virtual ~CBattleMode(){};
    virtual void InitializeChange(
        std::shared_ptr<CApplicationData> context) override;
    virtual void Input(std::shared_ptr<CApplicationData> context) override;
    virtual void Calculate(std::shared_ptr<CApplicationData> context) override;
    virtual void Render(std::shared_ptr<CApplicationData> context) override;
    static std::shared_ptr<CApplicationMode> Instance();
    void PixelPositionOnOverlay(int &CurrentX, int &CurrentY);
    void PixelPositionOnChat(int &CurrentX, int &CurrentY);
};

#endif
