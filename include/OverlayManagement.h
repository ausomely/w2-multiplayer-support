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
#ifndef OVERLAYMANAGEMENT_H
#define OVERLAYMANAGEMENT_H

#include <memory>
#include "ButtonRenderer.h"
#include "OverlayMode.h"

class CApplicationData;
class CInGameMenuOverlay;

class COverlayManagement
{
    friend class CInGameMenuOverlay;
  protected:
    struct SPrivateConstructorType {};

    static std::shared_ptr<COverlayManagement> DOverlayManagementPointer;

    std::shared_ptr<CApplicationData> DContext;
    std::shared_ptr<CGraphicSurface> DSurface;
    int DBorderWidth;

    std::shared_ptr<COverlayMode> DOverlayMode;

    static std::shared_ptr<COverlayManagement> Manager()
    {
        return DOverlayManagementPointer;
    }

    std::shared_ptr<CApplicationData> &Context()
    {
        return DContext;
    }

    std::shared_ptr<CGraphicSurface> &Surface()
    {
        return DSurface;
    }

    // Delete copy constructor
    COverlayManagement(const COverlayManagement &) = delete;
    const COverlayManagement &operator=(const COverlayManagement &) = delete;

  public:
    virtual ~COverlayManagement(){};

    explicit COverlayManagement(const SPrivateConstructorType &key,
        std::shared_ptr<CApplicationData> &context);

    static std::shared_ptr<COverlayManagement> Initialize(
        std::shared_ptr<CApplicationData> context);

    void LeaveGameFunction();
    void ReturnToGameFunction();
    void Draw(int x, int y, bool clicked);
    void ProcessInput();
//    void Input(int x, int y, bool clicked);
    void Mode(std::shared_ptr<COverlayMode> mode)
    {
        DOverlayMode = mode;
    }

};

#endif
