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
#include "ApplicationData.h"
#include "ButtonRenderer.h"
#include "OverlayMode.h"
#include "InGameMenuOverlay.h"
#include "SoundOptionsOverlay.h"
#include "BattleMode.h"

class CApplicationData;
class CInGameMenuOverlay;
class CGraphicSurface;
class CButtonRenderer;

enum class EOverlay
{
    None = 0,
    InGameMenu,
    SoundOptions
};

class COverlayManagement
{
    friend class CInGameMenuOverlay;
    friend class CSoundOptionsOverlay;
    friend class CBattleMode;

  protected:
    struct SPrivateConstructorType
    {
    };

    static std::shared_ptr<COverlayManagement> DOverlayManagementPointer;

    std::shared_ptr<CApplicationData> DContext;
    std::shared_ptr<CGraphicSurface> DSurface;
    std::shared_ptr<CButtonRenderer> DButtonRenderer;
    int DBorderWidth;
    int DWidth;
    int DHeight;
    int DXoffset;
    int DYoffset;

    bool DLeaveGameFlag;
    bool DExitOverlayManager;
    bool DChangeOverlay;
    EOverlay ENextOverlay;

    std::unique_ptr<COverlayMode> DOverlayMode;

    std::shared_ptr<COverlayManagement> Manager()
    {
        return DOverlayManagementPointer;
    }

    int CanvasWidth()
    {
        return DSurface->Width();
    }

    int CanvasHeight()
    {
        return DSurface->Height();
    }

    std::shared_ptr<CApplicationData> Context()
    {
        return DContext;
    }

    std::shared_ptr<CGraphicSurface> Surface()
    {
        return DSurface;
    }

    std::shared_ptr<CButtonRenderer> ButtonRenderer()
    {
        return DContext->DButtonRenderer;
    }

    int BorderWidth()
    {
        return DBorderWidth;
    }

    void BuildOverlay(EOverlay mode);
    void ChangeOverlay();
    void SetDefaults();
    void ClearMouseButtonState();

    // Delete copy constructor
    COverlayManagement(const COverlayManagement &) = delete;
    const COverlayManagement &operator=(const COverlayManagement &) = delete;

  public:

    virtual ~COverlayManagement()
    {
    };

    explicit COverlayManagement(const SPrivateConstructorType &key,
        std::shared_ptr<CApplicationData> context);

    static std::shared_ptr<COverlayManagement>
    Initialize(std::shared_ptr<CApplicationData> context);
    void DrawBackground();
    void LeaveGame();
    void ReturnToGame();
    void Draw(int x, int y, bool clicked);
    void ProcessInput(int x, int y, bool clicked);
//    void Input(int x, int y, bool clicked);
    void SetMode(EOverlay mode);

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

class COverlaySurfaceBuilder
{
    std::shared_ptr<CGraphicSurface> DSurface;
  public:
    int DXoffset, DYoffset;
    int DWidth, DHeight;
  private:
    float DXscale, DYscale;
    uint32_t DColor;

    int DVPXoffset, DVPYoffset;
    int DVPWidth, DVPHeight;

    void AllocateSurface()
    {
        DSurface = CGraphicFactory::CreateSurface(
            DWidth, DHeight, CGraphicSurface::ESurfaceFormat::ARGB32);

        auto ResourceContext = DSurface->CreateResourceContext();
        ResourceContext->SetSourceRGB(DColor);
        ResourceContext->Rectangle(0, 0, DWidth, DHeight);
        ResourceContext->Fill();
    }
  public:
    COverlaySurfaceBuilder(std::shared_ptr<CGraphicSurface> viewport,
        int vp_x_offset, int vp_y_offset)
    {
        // Get viewport's dimensions and X,Y offsets for positioning on
        // working buffer surface
        DVPXoffset = vp_x_offset;
        DVPYoffset = vp_y_offset;
        DVPWidth = viewport->Width();
        DVPHeight = viewport->Height();

        // Surface color
        DColor = 0x666666;

        // Scale size of viewport's dimensions
        DXscale = 0.7;
        DYscale = 0.7;

        // Set width and height
        DWidth = static_cast<int>(DXscale * DVPWidth);
        DHeight = static_cast<int>(DYscale * DVPHeight);

        // Set X,Y offsets
        DXoffset = static_cast<int>(DVPXoffset + 0.5*(DVPWidth-DWidth));
        DYoffset = 3 * DVPYoffset;
    }

    std::shared_ptr<CGraphicSurface> GetOverlaySurface()
    {
        if (nullptr == DSurface)
        {
            AllocateSurface();
        }

        return DSurface;
    }
};

#endif
