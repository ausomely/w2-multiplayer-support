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
#include "ApplicationData.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <memory>
#include "ApplicationPath.h"
#include "Debug.h"
#include "FileDataContainer.h"
#include "MainMenuMode.h"
#include "MemoryDataSource.h"
#include "PixelType.h"
#include "TriggerController.h"
#include "ServerConnectMenuMode.h"
#include "json.hpp"

#define INITIAL_MAP_WIDTH 800
#define INITIAL_MAP_HEIGHT 600
#define TIMEOUT_INTERVAL 50
#define TIMEOUT_FREQUENCY (1000 / TIMEOUT_INTERVAL)
#define MINI_MAP_MIN_WIDTH 128
#define MINI_MAP_MIN_HEIGHT 128

std::shared_ptr<CApplicationData> CApplicationData::DApplicationDataPointer;

CApplicationData::CApplicationData(const std::string &appname,
                                   const SPrivateApplicationType &key)
{
    DApplication = CGUIFactory::ApplicationInstance(appname);
    DApplication->SetActivateCallback(this, ActivateCallback);

    DPlayerNumber = EPlayerNumber::Player1;
    DMiniMapViewportColor = 0xFFFFFF;
    DDeleted = false;
    DActiveGame = false;  //!< Flag for active game
    DOverlayActive = false;
    DChatOverlayActive = false;
    DLeaveGame = false;
    DLeftDown = false;
    DRightDown = false;


    DCursorType = ctPointer;

    DMapSelectListViewXOffset = 0;
    DMapSelectListViewYOffset = 0;
    DSelectedMapIndex = 0;
    DUsername = "mgcheng";
    DRemoteHostname = "localhost";
    DMultiplayerPort = 55107;  // Ascii WC = 0x5743 or'd with 0x8000
    DPassword = "11051996";
    DSoundSettingsFilename = "SoundSettings.json";
    DBorderWidth = 32;
    DPanningSpeed = 0;
    for (int Index = 0; Index < to_underlying(EPlayerNumber::Max); Index++)
    {
        DPlayerCommands[Index].DAction = EAssetCapabilityType::None;
        DLoadingPlayerColors[Index] = static_cast<EPlayerColor>(Index);
    }
    DCurrentX = 0;
    DCurrentY = 0;
    DMouseDown = CPixelPosition(-1, -1);
    DLeftClick = 0;
    DRightClick = 0;
    DLeftDown = false;
    DRightDown = false;

    DMenuButtonState = CButtonRenderer::EButtonState::None;
    DUnitHotKeyMap[SGUIKeyType::KeyA] = EAssetCapabilityType::Attack;
    DUnitHotKeyMap[SGUIKeyType::Keya] = EAssetCapabilityType::Attack;
    DUnitHotKeyMap[SGUIKeyType::KeyB] = EAssetCapabilityType::BuildSimple;
    DUnitHotKeyMap[SGUIKeyType::Keyb] = EAssetCapabilityType::BuildSimple;
    DUnitHotKeyMap[SGUIKeyType::KeyG] = EAssetCapabilityType::Convey;
    DUnitHotKeyMap[SGUIKeyType::Keyg] = EAssetCapabilityType::Convey;
    DUnitHotKeyMap[SGUIKeyType::KeyM] = EAssetCapabilityType::Move;
    DUnitHotKeyMap[SGUIKeyType::Keym] = EAssetCapabilityType::Move;
    DUnitHotKeyMap[SGUIKeyType::KeyP] = EAssetCapabilityType::Patrol;
    DUnitHotKeyMap[SGUIKeyType::Keyp] = EAssetCapabilityType::Patrol;
    DUnitHotKeyMap[SGUIKeyType::KeyR] = EAssetCapabilityType::Repair;
    DUnitHotKeyMap[SGUIKeyType::Keyr] = EAssetCapabilityType::Repair;
    DUnitHotKeyMap[SGUIKeyType::KeyT] = EAssetCapabilityType::StandGround;
    DUnitHotKeyMap[SGUIKeyType::Keyt] = EAssetCapabilityType::StandGround;

    DBuildHotKeyMap[SGUIKeyType::KeyB] = EAssetCapabilityType::BuildBarracks;
    DBuildHotKeyMap[SGUIKeyType::Keyb] = EAssetCapabilityType::BuildBarracks;
    DBuildHotKeyMap[SGUIKeyType::KeyF] = EAssetCapabilityType::BuildFarm;
    DBuildHotKeyMap[SGUIKeyType::Keyf] = EAssetCapabilityType::BuildFarm;
    DBuildHotKeyMap[SGUIKeyType::KeyH] = EAssetCapabilityType::BuildTownHall;
    DBuildHotKeyMap[SGUIKeyType::Keyh] = EAssetCapabilityType::BuildTownHall;
    DBuildHotKeyMap[SGUIKeyType::KeyL] = EAssetCapabilityType::BuildLumberMill;
    DBuildHotKeyMap[SGUIKeyType::Keyl] = EAssetCapabilityType::BuildLumberMill;
    DBuildHotKeyMap[SGUIKeyType::KeyS] = EAssetCapabilityType::BuildBlacksmith;
    DBuildHotKeyMap[SGUIKeyType::Keys] = EAssetCapabilityType::BuildBlacksmith;
    DBuildHotKeyMap[SGUIKeyType::KeyT] = EAssetCapabilityType::BuildScoutTower;
    DBuildHotKeyMap[SGUIKeyType::Keyt] = EAssetCapabilityType::BuildScoutTower;
   // DBuildHotKeyMap[SGUIKeyType::KeyG] = EAssetCapabilityType::BuildGoldMine;
   // DBuildHotKeyMap[SGUIKeyType::Keyg] = EAssetCapabilityType::BuildGoldMine;

    DTrainHotKeyMap[SGUIKeyType::KeyA] = EAssetCapabilityType::BuildArcher;
    DTrainHotKeyMap[SGUIKeyType::Keya] = EAssetCapabilityType::BuildArcher;
    DTrainHotKeyMap[SGUIKeyType::KeyF] = EAssetCapabilityType::BuildFootman;
    DTrainHotKeyMap[SGUIKeyType::Keyf] = EAssetCapabilityType::BuildFootman;
    DTrainHotKeyMap[SGUIKeyType::KeyP] = EAssetCapabilityType::BuildPeasant;
    DTrainHotKeyMap[SGUIKeyType::Keyp] = EAssetCapabilityType::BuildPeasant;
    DTrainHotKeyMap[SGUIKeyType::KeyR] = EAssetCapabilityType::BuildRanger;
    DTrainHotKeyMap[SGUIKeyType::Keyr] = EAssetCapabilityType::BuildRanger;
}

CApplicationData::~CApplicationData() {}

std::shared_ptr<CApplicationData> CApplicationData::Instance(
    const std::string &appname)
{
    if (!DApplicationDataPointer)
    {
        DApplicationDataPointer = std::make_shared<CApplicationData>(
            appname, SPrivateApplicationType{});
    }
    return DApplicationDataPointer;
}

void CApplicationData::ActivateCallback(TGUICalldata data)
{
    CApplicationData *AppData = static_cast<CApplicationData *>(data);
    AppData->Activate();
}

bool CApplicationData::TimeoutCallback(TGUICalldata data)
{
    CApplicationData *AppData = static_cast<CApplicationData *>(data);
    PrintDebug(DEBUG_HIGH, "CApplicationData::Timeout\n");
    if (AppData->DDeleted)
    {
        return false;
    }

    return AppData->Timeout();
}

// This is a callback function to signify that the delete windows has been
// called. If you return false in the "delete-event" signal handler, GTK will
// emit the "destroy" signal. Returning true means you don't want the window to
// be destroyed. This is useful for popping up 'are you sure you want to quit?'
// type dialogs.
bool CApplicationData::MainWindowDeleteEventCallback(
    std::shared_ptr<CGUIWidget> widget, TGUICalldata data)
{
    CApplicationData *AppData = static_cast<CApplicationData *>(data);
    PrintDebug(DEBUG_HIGH, "CApplicationData::Delete\n");
    return AppData->MainWindowDeleteEvent(widget);
}

// The destroy window callback
void CApplicationData::MainWindowDestroyCallback(
    std::shared_ptr<CGUIWidget> widget, TGUICalldata data)
{
    CApplicationData *AppData = static_cast<CApplicationData *>(data);
    PrintDebug(DEBUG_HIGH, "CApplicationData::Destroy\n");
    AppData->MainWindowDestroy(widget);
}

bool CApplicationData::MainWindowKeyPressEventCallback(
    std::shared_ptr<CGUIWidget> widget, SGUIKeyEvent &event, TGUICalldata data)
{
    CApplicationData *AppData = static_cast<CApplicationData *>(data);
    PrintDebug(DEBUG_HIGH, "CApplicationData::KeyPress\n");
    return AppData->MainWindowKeyPressEvent(widget, event);
}

bool CApplicationData::MainWindowKeyReleaseEventCallback(
    std::shared_ptr<CGUIWidget> widget, SGUIKeyEvent &event, TGUICalldata data)
{
    CApplicationData *AppData = static_cast<CApplicationData *>(data);
    PrintDebug(DEBUG_HIGH, "CApplicationData::KeyRelease\n");
    return AppData->MainWindowKeyReleaseEvent(widget, event);
}

bool CApplicationData::MainWindowConfigureEventCallback(
    std::shared_ptr<CGUIWidget> widget, SGUIConfigureEvent &event,
    TGUICalldata data)
{
    CApplicationData *AppData = static_cast<CApplicationData *>(data);
    PrintDebug(DEBUG_HIGH, "CApplicationData::Configure\n");
    return AppData->MainWindowConfigureEvent(widget, event);
}

bool CApplicationData::DrawingAreaDrawCallback(
    std::shared_ptr<CGUIWidget> widget,
    std::shared_ptr<CGraphicResourceContext> rc, TGUICalldata data)
{
    CApplicationData *AppData = static_cast<CApplicationData *>(data);
    PrintDebug(DEBUG_HIGH, "CApplicationData::Draw\n");
    return AppData->DrawingAreaDraw(widget, rc);
}

bool CApplicationData::DrawingAreaButtonPressEventCallback(
    std::shared_ptr<CGUIWidget> widget, SGUIButtonEvent &event,
    TGUICalldata data)
{
    CApplicationData *AppData = static_cast<CApplicationData *>(data);
    PrintDebug(DEBUG_HIGH, "CApplicationData::ButtonPress\n");
    return AppData->DrawingAreaButtonPressEvent(widget, event);
}

bool CApplicationData::DrawingAreaButtonReleaseEventCallback(
    std::shared_ptr<CGUIWidget> widget, SGUIButtonEvent &event,
    TGUICalldata data)
{
    CApplicationData *AppData = static_cast<CApplicationData *>(data);
    PrintDebug(DEBUG_HIGH, "CApplicationData::ButtonRelease\n");
    return AppData->DrawingAreaButtonReleaseEvent(widget, event);
}

bool CApplicationData::DrawingAreaMotionNotifyEventCallback(
    std::shared_ptr<CGUIWidget> widget, SGUIMotionEvent &event,
    TGUICalldata data)
{
    CApplicationData *AppData = static_cast<CApplicationData *>(data);
    PrintDebug(DEBUG_HIGH, "CApplicationData::Motion\n");
    return AppData->DrawingAreaMotionNotifyEvent(widget, event);
}

void CApplicationData::Activate()
{
    CPath AppPath = GetApplicationPath().Containing();

    // Set data directory path if not set from command line
    if (DDataPath.length() == 0)
    {
        DDataPath = AppPath.ToString() + "/data";
    }

    LoadVolumeSettings(DDataPath + "/snd/" + DSoundSettingsFilename);
    std::shared_ptr<CDataContainer> TempDataContainer =
        std::make_shared<CDirectoryDataContainer>(DDataPath);
    std::shared_ptr<CDataContainer> ImageDirectory =
        TempDataContainer->DataContainer("img");
    std::shared_ptr<CDataContainerIterator> FileIterator;
    std::shared_ptr<CDataSource> TempDataSource;

    // Create a new main window
    DMainWindow = DApplication->NewWindow();

    // When the window is given the "delete-event" signal (this is given by the
    // window manager, usually by the "close" option, or on the titlebar), we
    // ask it to call the delete_event () function as defined above. The data
    // passed to the callback function is nullptr and is ignored in the callback
    // function.
    DMainWindow->SetDeleteEventCallback(this, MainWindowDeleteEventCallback);
    // g_signal_connect(DMainWindow, "delete-event",
    // G_CALLBACK(MainWindowDeleteEventCallback), this);

    // Here we connect the "destroy" event to a signal handler. This event
    // occurs when we call gtk_widget_destroy() on the window, or if we return
    // false in the "delete-event" callback.
    DMainWindow->SetDestroyEventCallback(this, MainWindowDestroyCallback);

    DMainWindow->SetKeyPressEventCallback(this,
                                          MainWindowKeyPressEventCallback);
    DMainWindow->SetKeyReleaseEventCallback(this,
                                            MainWindowKeyReleaseEventCallback);

    // Sets the border width of the window.
    // DMainWindow->SetBorderWidth(2);

    // Creates a drawing surface
    DDrawingArea = CGUIFactory::NewDrawingArea();

    // Add drawing surface to main window
    DMainWindow->Add(DDrawingArea);

    DMainWindow->SetMinSize(DDrawingArea, INITIAL_MAP_WIDTH,
                            INITIAL_MAP_HEIGHT);

    DDrawingArea->SetDrawEventCallback(this, DrawingAreaDrawCallback);
    DDrawingArea->SetButtonPressEventCallback(
        this, DrawingAreaButtonPressEventCallback);
    DDrawingArea->SetButtonReleaseEventCallback(
        this, DrawingAreaButtonReleaseEventCallback);
    DDrawingArea->SetMotionEventCallback(this,
                                         DrawingAreaMotionNotifyEventCallback);
    DDrawingArea->SetConfigureEventCallback(this,
                                            MainWindowConfigureEventCallback);

    DDrawingArea->EnableEvent(EGUIEvent::Expose);
    DDrawingArea->EnableEvent(EGUIEvent::ButtonPress);
    DDrawingArea->EnableEvent(EGUIEvent::ButtonRelease);
    DDrawingArea->EnableEvent(EGUIEvent::Motion);

    auto DefaultDisplay = CGUIFactory::DefaultDisplay();

    DBlankCursor = DefaultDisplay->NewCursor(CGUICursor::ECursorType::Blank);
    // Show all widgets so they are displayed
    TempDataSource = ImageDirectory->DataSource("Splash.dat");
    DSplashTileset = std::make_shared<CGraphicTileset>();
    if (!DSplashTileset->LoadTileset(TempDataSource))
    {
        PrintError("Failed to splash screen.\n");
        return;
    }
    DDrawingArea->ShowAll();
    DMainWindow->ShowAll();
    DMainWindow->SetCursor(DBlankCursor);

    DDoubleBufferSurface = CGraphicFactory::CreateSurface(
        INITIAL_MAP_WIDTH, INITIAL_MAP_HEIGHT,
        CGraphicSurface::ESurfaceFormat::
            ARGB32);  // DDrawingArea->CreateSimilarSurface(INITIAL_MAP_WIDTH,
                      // INITIAL_MAP_HEIGHT);
    DWorkingBufferSurface = CGraphicFactory::CreateSurface(
        INITIAL_MAP_WIDTH, INITIAL_MAP_HEIGHT,
        CGraphicSurface::ESurfaceFormat::
            ARGB32);  // DDrawingArea->CreateSimilarSurface(INITIAL_MAP_WIDTH,
                      // INITIAL_MAP_HEIGHT);

    DefaultDisplay->Flush();
    DDrawingArea->Invalidate();
    DApplication->ProcessEvents(true);
    DTotalLoadingSteps = 128;
    DCurrentLoadingStep = 0;
    DLoadingResourceContext = DDrawingArea->CreateResourceContext();
    RenderSplashStep();

    DefaultDisplay->Flush();
    DApplication->ProcessEvents(true);
    DefaultDisplay->Flush();

    TempDataSource = ImageDirectory->DataSource("Cursors.dat");
    DCursorSet = std::make_shared<CCursorSet>();
    if (!DCursorSet->LoadCursors(TempDataSource))
    {
        PrintError("Failed to load cursors.\n");
        return;
    }
    DCursorIndices[ctPointer] = DCursorSet->FindCursor("pointer");
    DCursorIndices[ctInspect] = DCursorSet->FindCursor("magnifier");
    DCursorIndices[ctArrowN] = DCursorSet->FindCursor("arrow-n");
    DCursorIndices[ctArrowE] = DCursorSet->FindCursor("arrow-e");
    DCursorIndices[ctArrowS] = DCursorSet->FindCursor("arrow-s");
    DCursorIndices[ctArrowW] = DCursorSet->FindCursor("arrow-w");
    DCursorIndices[ctTargetOff] = DCursorSet->FindCursor("target-off");
    DCursorIndices[ctTargetOn] = DCursorSet->FindCursor("target-on");

    TempDataSource = TempDataContainer->DataSource("./snd/SoundClips.dat");
    DSoundLibraryMixer = std::make_shared<CSoundLibraryMixer>();
    if (!DSoundLibraryMixer->LoadLibrary(TempDataSource, SoundLoadingCallback,
                                         this))
    {
        PrintError("Failed to sound mixer.\n");
        return;
    }
    RenderSplashStep();
    DSoundLibraryMixer->PlaySong(DSoundLibraryMixer->FindSong("load"),
                                 DMusicVolume);

    TempDataSource = ImageDirectory->DataSource("ButtonColors.dat");
    DButtonRecolorMap = std::make_shared<CGraphicRecolorMap>();
    if (!DButtonRecolorMap->Load(TempDataSource))
    {
        PrintError("Failed to load button colors.\n");
        return;
    }

    RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("Marker.dat");
    DMarkerTileset = std::make_shared<CGraphicTileset>();
    if (!DMarkerTileset->LoadTileset(TempDataSource))
    {
        PrintError("Failed to load markers.\n");
        return;
    }
    DMarkerTileset->CreateClippingMasks();

    RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("Texture.dat");
    DBackgroundTileset = std::make_shared<CGraphicTileset>();
    if (!DBackgroundTileset->LoadTileset(TempDataSource))
    {
        PrintError("Failed to load background tileset.\n");
        return;
    }

    PrintDebug(DEBUG_LOW, "Loading FontColors\n");
    RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("FontColors.dat");
    DFontRecolorMap = std::make_shared<CGraphicRecolorMap>();
    if (!DFontRecolorMap->Load(TempDataSource))
    {
        PrintError("Failed to load font colors.\n");
        return;
    }

    PrintDebug(DEBUG_LOW, "Loading Font10\n");
    RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("FontKingthings10.dat");
    DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Small)] =
        std::make_shared<CFontTileset>();
    if (!DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Small)]
             ->LoadFont(DFontRecolorMap, TempDataSource))
    {
        PrintError("Failed to load font tileset.\n");
        return;
    }

    PrintDebug(DEBUG_LOW, "Loading Font12\n");
    RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("FontKingthings12.dat");
    DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Medium)] =
        std::make_shared<CFontTileset>();
    if (!DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Medium)]
             ->LoadFont(DFontRecolorMap, TempDataSource))
    {
        PrintError("Failed to load font tileset.\n");
        return;
    }
    PrintDebug(DEBUG_LOW, "Loading Font16\n");
    RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("FontKingthings16.dat");
    DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)] =
        std::make_shared<CFontTileset>();
    if (!DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]
             ->LoadFont(DFontRecolorMap, TempDataSource))
    {
        PrintError("Failed to load font tileset.\n");
        return;
    }
    PrintDebug(DEBUG_LOW, "Loading Font24\n");
    RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("FontKingthings24.dat");
    DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Giant)] =
        std::make_shared<CFontTileset>();
    if (!DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Giant)]
             ->LoadFont(DFontRecolorMap, TempDataSource))
    {
        PrintError("Failed to load font tileset.\n");
        return;
    }

    PrintDebug(DEBUG_LOW, "Loading MiniBevel\n");
    RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("MiniBevel.dat");
    DMiniBevelTileset = std::make_shared<CGraphicTileset>();
    if (!DMiniBevelTileset->LoadTileset(TempDataSource))
    {
        PrintError("Failed to load bevel tileset.\n");
        return;
    }
    DMiniBevel = std::make_shared<CBevel>(DMiniBevelTileset);

    PrintDebug(DEBUG_LOW, "Loading InnerBevel\n");
    RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("InnerBevel.dat");
    DInnerBevelTileset = std::make_shared<CGraphicTileset>();
    if (!DInnerBevelTileset->LoadTileset(TempDataSource))
    {
        PrintError("Failed to load bevel tileset.\n");
        return;
    }
    DInnerBevel = std::make_shared<CBevel>(DInnerBevelTileset);

    PrintDebug(DEBUG_LOW, "Loading OuterBevel\n");
    RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("OuterBevel.dat");
    DOuterBevelTileset = std::make_shared<CGraphicTileset>();
    if (!DOuterBevelTileset->LoadTileset(TempDataSource))
    {
        PrintError("Failed to load bevel tileset.\n");
        return;
    }
    DOuterBevel = std::make_shared<CBevel>(DOuterBevelTileset);

    PrintDebug(DEBUG_LOW, "Loading ListViewIcons\n");
    RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("ListViewIcons.dat");
    DListViewIconTileset = std::make_shared<CGraphicTileset>();
    if (!DListViewIconTileset->LoadTileset(TempDataSource))
    {
        PrintError("Failed to load list view tileset.\n");
        return;
    }

    PrintDebug(DEBUG_LOW, "Loading Rendering Configuration\n");
    RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("MapRendering.dat");
    char TempChar;
    while (1 == TempDataSource->Read(&TempChar, 1))
    {
        DMapRendererConfigurationData.push_back(TempChar);
    }

    PrintDebug(DEBUG_LOW, "Loading Terrain\n");
    RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("Terrain.dat");
    DTerrainTileset = std::make_shared<CGraphicTileset>();
    if (!DTerrainTileset->LoadTileset(TempDataSource))
    {
        PrintError("Failed to load tileset.\n");
        return;
    }
    DTerrainTileset->CreateClippingMasks();
    CPosition::SetTileDimensions(DTerrainTileset->TileWidth(),
                                 DTerrainTileset->TileHeight());

    PrintDebug(DEBUG_LOW, "Loading Fog\n");
    RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("Fog.dat");
    DFogTileset = std::make_shared<CGraphicTileset>();
    if (!DFogTileset->LoadTileset(TempDataSource))
    {
        PrintError("Failed to load tileset.\n");
        return;
    }

    PrintDebug(DEBUG_LOW, "Loading Player Colors\n");
    RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("Colors.dat");
    DPlayerRecolorMap = std::make_shared<CGraphicRecolorMap>();
    if (!DPlayerRecolorMap->Load(TempDataSource))
    {
        PrintError("Failed to load recolor map.\n");
        return;
    }

    DAssetTilesets.resize(to_underlying(EAssetType::Max));

    PrintDebug(DEBUG_LOW, "Loading Icons\n");
    RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("Icons.dat");
    DIconTileset = std::make_shared<CGraphicMulticolorTileset>();
    if (!DIconTileset->LoadTileset(DPlayerRecolorMap, TempDataSource))
    {
        PrintError("Failed to load icons.\n");
        return;
    }

    PrintDebug(DEBUG_LOW, "Loading Mini Icons\n");
    RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("MiniIcons.dat");
    DMiniIconTileset = std::make_shared<CGraphicTileset>();
    if (!DMiniIconTileset->LoadTileset(TempDataSource))
    {
        PrintError("Failed to load mini icons.\n");
        return;
    }

    PrintDebug(DEBUG_LOW, "Loading Coprse\n");
    RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("Corpse.dat");
    DCorpseTileset = std::make_shared<CGraphicTileset>();
    if (!DCorpseTileset->LoadTileset(TempDataSource))
    {
        PrintError("Failed to load corpse tileset.\n");
        return;
    }
    DCorpseTileset->CreateClippingMasks();

    PrintDebug(DEBUG_LOW, "Loading FireSmall\n");
    RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("FireSmall.dat");
    auto FireTileset = std::make_shared<CGraphicTileset>();
    if (!FireTileset->LoadTileset(TempDataSource))
    {
        PrintError("Failed to load fire small tileset.\n");
        return;
    }
    FireTileset->CreateClippingMasks();
    DFireTilesets.push_back(FireTileset);

    PrintDebug(DEBUG_LOW, "Loading FireLarge\n");
    RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("FireLarge.dat");
    FireTileset = std::make_shared<CGraphicTileset>();
    if (!FireTileset->LoadTileset(TempDataSource))
    {
        PrintError("Failed to load fire large tileset.\n");
        return;
    }
    FireTileset->CreateClippingMasks();
    DFireTilesets.push_back(FireTileset);

    PrintDebug(DEBUG_LOW, "Loading BuildingDeath\n");
    RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("BuildingDeath.dat");
    DBuildingDeathTileset = std::make_shared<CGraphicTileset>();
    if (!DBuildingDeathTileset->LoadTileset(TempDataSource))
    {
        PrintError("Failed to load building death tileset.\n");
        return;
    }
    DBuildingDeathTileset->CreateClippingMasks();

    PrintDebug(DEBUG_LOW, "Loading Arrow\n");
    TempDataSource = ImageDirectory->DataSource("Arrow.dat");
    DArrowTileset = std::make_shared<CGraphicTileset>();
    if (!DArrowTileset->LoadTileset(TempDataSource))
    {
        PrintError("Failed to load arrow tileset.\n");
        return;
    }
    DArrowTileset->CreateClippingMasks();

    PrintDebug(DEBUG_LOW, "Loading AssetColor\n");
    RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("AssetColor.dat");
    DAssetRecolorMap = std::make_shared<CGraphicRecolorMap>();
    if (!DAssetRecolorMap->Load(TempDataSource))
    {
        PrintError("Failed to load asset color map.\n");
        return;
    }
    DAssetTilesets[to_underlying(EAssetType::None)] = nullptr;

    PrintDebug(DEBUG_LOW, "Loading Peasant\n");
    RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("Peasant.dat");
    DAssetTilesets[to_underlying(EAssetType::Peasant)] =
        std::make_shared<CGraphicMulticolorTileset>();
    if (!DAssetTilesets[to_underlying(EAssetType::Peasant)]->LoadTileset(
            DPlayerRecolorMap, TempDataSource))
    {
        PrintError("Failed to load peasant tileset.\n");
        return;
    }
    PrintDebug(DEBUG_LOW, "Loading Footman\n");
    RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("Footman.dat");
    DAssetTilesets[to_underlying(EAssetType::Footman)] =
        std::make_shared<CGraphicMulticolorTileset>();
    if (!DAssetTilesets[to_underlying(EAssetType::Footman)]->LoadTileset(
            DPlayerRecolorMap, TempDataSource))
    {
        PrintError("Failed to load footman tileset.\n");
        return;
    }
    PrintDebug(DEBUG_LOW, "Loading Archer\n");
    RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("Archer.dat");
    DAssetTilesets[to_underlying(EAssetType::Archer)] =
        std::make_shared<CGraphicMulticolorTileset>();
    if (!DAssetTilesets[to_underlying(EAssetType::Archer)]->LoadTileset(
            DPlayerRecolorMap, TempDataSource))
    {
        PrintError("Failed to load archer tileset.\n");
        return;
    }
    PrintDebug(DEBUG_LOW, "Loading Ranger\n");
    RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("Ranger.dat");
    DAssetTilesets[to_underlying(EAssetType::Ranger)] =
        std::make_shared<CGraphicMulticolorTileset>();
    if (!DAssetTilesets[to_underlying(EAssetType::Ranger)]->LoadTileset(
            DPlayerRecolorMap, TempDataSource))
    {
        PrintError("Failed to load archer tileset.\n");
        return;
    }

    PrintDebug(DEBUG_LOW, "Loading GoldVein\n");
    RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("GoldVein.dat");
    DAssetTilesets[to_underlying(EAssetType::GoldVein)] =
        std::make_shared<CGraphicMulticolorTileset>();
    if (!DAssetTilesets[to_underlying(EAssetType::GoldVein)]->LoadTileset(
            DPlayerRecolorMap, TempDataSource))
    {
        PrintError("Failed to load gold vein tileset.\n");
        return;
    }

    PrintDebug(DEBUG_LOW, "Loading GoldMine\n");
    RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("GoldMine.dat");
    DAssetTilesets[to_underlying(EAssetType::GoldMine)] =
        std::make_shared<CGraphicMulticolorTileset>();
    if (!DAssetTilesets[to_underlying(EAssetType::GoldMine)]->LoadTileset(
            DPlayerRecolorMap, TempDataSource))
    {
        PrintError("Failed to load gold mine tileset.\n");
        return;
    }
    PrintDebug(DEBUG_LOW, "Loading TownHall\n");
    RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("TownHall.dat");
    DAssetTilesets[to_underlying(EAssetType::TownHall)] =
        std::make_shared<CGraphicMulticolorTileset>();
    if (!DAssetTilesets[to_underlying(EAssetType::TownHall)]->LoadTileset(
            DPlayerRecolorMap, TempDataSource))
    {
        PrintError("Failed to load town hall tileset.\n");
        return;
    }
    PrintDebug(DEBUG_LOW, "Loading Keep\n");
    RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("Keep.dat");
    DAssetTilesets[to_underlying(EAssetType::Keep)] =
        std::make_shared<CGraphicMulticolorTileset>();
    if (!DAssetTilesets[to_underlying(EAssetType::Keep)]->LoadTileset(
            DPlayerRecolorMap, TempDataSource))
    {
        PrintError("Failed to load keep tileset.\n");
        return;
    }
    PrintDebug(DEBUG_LOW, "Loading Castle\n");
    RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("Castle.dat");
    DAssetTilesets[to_underlying(EAssetType::Castle)] =
        std::make_shared<CGraphicMulticolorTileset>();
    if (!DAssetTilesets[to_underlying(EAssetType::Castle)]->LoadTileset(
            DPlayerRecolorMap, TempDataSource))
    {
        PrintError("Failed to load castle tileset.\n");
        return;
    }
    PrintDebug(DEBUG_LOW, "Loading Farm\n");
    RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("Farm.dat");
    DAssetTilesets[to_underlying(EAssetType::Farm)] =
        std::make_shared<CGraphicMulticolorTileset>();
    if (!DAssetTilesets[to_underlying(EAssetType::Farm)]->LoadTileset(
            DPlayerRecolorMap, TempDataSource))
    {
        PrintError("Failed to load farm tileset.\n");
        return;
    }
    PrintDebug(DEBUG_LOW, "Loading Barracks\n");
    RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("Barracks.dat");
    DAssetTilesets[to_underlying(EAssetType::Barracks)] =
        std::make_shared<CGraphicMulticolorTileset>();
    if (!DAssetTilesets[to_underlying(EAssetType::Barracks)]->LoadTileset(
            DPlayerRecolorMap, TempDataSource))
    {
        PrintError("Failed to load barracks tileset.\n");
        return;
    }
    PrintDebug(DEBUG_LOW, "Loading Blacksmith\n");
    RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("Blacksmith.dat");
    DAssetTilesets[to_underlying(EAssetType::Blacksmith)] =
        std::make_shared<CGraphicMulticolorTileset>();
    if (!DAssetTilesets[to_underlying(EAssetType::Blacksmith)]->LoadTileset(
            DPlayerRecolorMap, TempDataSource))
    {
        PrintError("Failed to load blacksmith tileset.\n");
        return;
    }
    PrintDebug(DEBUG_LOW, "Loading LumberMill\n");
    RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("LumberMill.dat");
    DAssetTilesets[to_underlying(EAssetType::LumberMill)] =
        std::make_shared<CGraphicMulticolorTileset>();
    if (!DAssetTilesets[to_underlying(EAssetType::LumberMill)]->LoadTileset(
            DPlayerRecolorMap, TempDataSource))
    {
        PrintError("Failed to load lumber mill tileset.\n");
        return;
    }
    PrintDebug(DEBUG_LOW, "Loading ScoutTower\n");
    RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("ScoutTower.dat");
    DAssetTilesets[to_underlying(EAssetType::ScoutTower)] =
        std::make_shared<CGraphicMulticolorTileset>();
    if (!DAssetTilesets[to_underlying(EAssetType::ScoutTower)]->LoadTileset(
            DPlayerRecolorMap, TempDataSource))
    {
        PrintError("Failed to load scout tower tileset.\n");
        return;
    }
    PrintDebug(DEBUG_LOW, "Loading GuardTower\n");
    RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("GuardTower.dat");
    DAssetTilesets[to_underlying(EAssetType::GuardTower)] =
        std::make_shared<CGraphicMulticolorTileset>();
    if (!DAssetTilesets[to_underlying(EAssetType::GuardTower)]->LoadTileset(
            DPlayerRecolorMap, TempDataSource))
    {
        PrintError("Failed to load guard tower tileset.\n");
        return;
    }
    PrintDebug(DEBUG_LOW, "Loading CannonTower\n");
    RenderSplashStep();
    TempDataSource = ImageDirectory->DataSource("CannonTower.dat");
    DAssetTilesets[to_underlying(EAssetType::CannonTower)] =
        std::make_shared<CGraphicMulticolorTileset>();
    if (!DAssetTilesets[to_underlying(EAssetType::CannonTower)]->LoadTileset(
            DPlayerRecolorMap, TempDataSource))
    {
        PrintError("Failed to load cannon tower tileset.\n");
        return;
    }
    RenderSplashStep();
    for (auto AssetTileset : DAssetTilesets)
    {
        if (AssetTileset != nullptr)
        {
            AssetTileset->CreateClippingMasks();
        }
    }
    PrintDebug(DEBUG_LOW, "Assets Loaded\n");

    PrintDebug(DEBUG_LOW, "Loading res directory\n");
    RenderSplashStep();
    std::shared_ptr<CDataContainer> AssetDirectory =
        TempDataContainer->DataContainer("res");
    if (!CPlayerAssetType::LoadTypes(AssetDirectory))
    {
        PrintError("Failed to load resources\n");
        return;
    }

    PrintDebug(DEBUG_LOW, "Loading upg directory\n");
    RenderSplashStep();
    std::shared_ptr<CDataContainer> UpgradeDirectory =
        TempDataContainer->DataContainer("upg");
    if (!CPlayerUpgrade::LoadUpgrades(UpgradeDirectory))
    {
        PrintError("Failed to load upgrades\n");
        return;
    }

    PrintDebug(DEBUG_LOW, "Loading Maps\n");
    RenderSplashStep();
    std::shared_ptr<CDataContainer> MapDirectory =
        TempDataContainer->DataContainer("map");
    if (!CAssetDecoratedMap::LoadMaps(MapDirectory))
    {
        PrintError("Failed to load maps\n");
        return;
    }

    CPlayerAsset::UpdateFrequency(TIMEOUT_FREQUENCY);
    CAssetRenderer::UpdateFrequency(TIMEOUT_FREQUENCY);

    PrintDebug(DEBUG_LOW, "Loading Game Map 0\n");

    LoadGameMap(0);

    PrintDebug(DEBUG_LOW, "Game Map 0 Loaded\n");

    DButtonRenderer = std::make_shared<CButtonRenderer>(
        DButtonRecolorMap, DInnerBevel, DOuterBevel,
        DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]);
    DMapSelectListViewRenderer = std::make_shared<CListViewRenderer>(
        DListViewIconTileset,
        DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]);
    DOptionsEditRenderer = std::make_shared<CEditRenderer>(
        DButtonRecolorMap, DInnerBevel,
        DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]);

    PrintDebug(DEBUG_LOW, "Changing Mode to MainMenu\n");
    DDoubleBufferSurface->Draw(DWorkingBufferSurface, 0, 0, -1, -1, 0, 0);
    DDrawingArea->Invalidate();

    DApplicationMode = DNextApplicationMode = CMainMenuMode::Instance();

    DApplication->SetTimer(TIMEOUT_INTERVAL, this, TimeoutCallback);

    DSoundLibraryMixer->StopSong();
    DSoundLibraryMixer->PlaySong(DSoundLibraryMixer->FindSong("menu"),
                                 DMusicVolume);
    DLoadingResourceContext = nullptr;
}

bool CApplicationData::Timeout()
{
    DApplicationMode->Input(shared_from_this());
    DApplicationMode->Calculate(shared_from_this());
    DApplicationMode->Render(shared_from_this());

    if (!DLeftDown || (2 != DLeftClick))
    {
        DLeftClick = 0;
    }
    DRightClick = 0;

    DCursorSet->DrawCursor(DWorkingBufferSurface, DCurrentX, DCurrentY,
                           DCursorIndices[DCursorType]);

    DDoubleBufferSurface->Draw(DWorkingBufferSurface, 0, 0, -1, -1, 0, 0);

    DDrawingArea->Invalidate();

    DApplicationMode = DNextApplicationMode;

    return true;
}

bool CApplicationData::MainWindowDeleteEvent(std::shared_ptr<CGUIWidget> widget)
{
    PrintDebug(DEBUG_LOW, "Delete event occurred\n");
    DDeleted = true;
    return false;
}

void CApplicationData::MainWindowDestroy(std::shared_ptr<CGUIWidget> widget)
{
    DMainWindow = nullptr;
}

bool CApplicationData::MainWindowKeyPressEvent(
    std::shared_ptr<CGUIWidget> widget, SGUIKeyEvent &event)
{
    bool Found = false;
    for (auto Key : DPressedKeys)
    {
        if (Key == event.DValue.DValue)
        {
            Found = true;
            break;
        }
    }
    if (!Found)
    {
        DPressedKeys.push_back(event.DValue.DValue);
    }
    return true;
}

bool CApplicationData::MainWindowKeyReleaseEvent(
    std::shared_ptr<CGUIWidget> widget, SGUIKeyEvent &event)
{
    bool Found = false;
    int Index = 0;
    for (auto Key : DPressedKeys)
    {
        if (Key == event.DValue.DValue)
        {
            Found = true;
            break;
        }
        Index++;
    }
    if (Found)
    {
        DPressedKeys.erase(DPressedKeys.begin() + Index);
    }
    Found = false;
    for (auto Key : DReleasedKeys)
    {
        if (Key == event.DValue.DValue)
        {
            Found = true;
            break;
        }
    }
    if (!Found)
    {
        DReleasedKeys.push_back(event.DValue.DValue);
    }
    return true;
}

bool CApplicationData::MainWindowConfigureEvent(
    std::shared_ptr<CGUIWidget> widget, SGUIConfigureEvent &event)
{
    ResizeCanvases();
    return true;
}

bool CApplicationData::DrawingAreaDraw(
    std::shared_ptr<CGUIWidget> widget,
    std::shared_ptr<CGraphicResourceContext> rc)
{
    rc->DrawSurface(DDoubleBufferSurface, 0, 0, -1, -1, 0, 0);
    return true;
}

bool CApplicationData::DrawingAreaButtonPressEvent(
    std::shared_ptr<CGUIWidget> widget, SGUIButtonEvent &event)
{
    if (1 == event.DButton)
    {
        if (event.DModifier.ModifierIsSet(SGUIModifierType::Control))
        {
            DRightClick = event.DType.IsDoubleButtonPress() ? 2 : 1;
            DRightDown = true;
        }
        else
        {
            DLeftClick = event.DType.IsDoubleButtonPress() ? 2 : 1;
            DLeftDown = true;
        }
    }
    if (3 == event.DButton)
    {
        DRightClick = event.DType.IsDoubleButtonPress() ? 2 : 1;
        DRightDown = true;
    }
    return true;
}

bool CApplicationData::DrawingAreaButtonReleaseEvent(
    std::shared_ptr<CGUIWidget> widget, SGUIButtonEvent &event)
{
    if (1 == event.DButton)
    {
        if (event.DModifier.ModifierIsSet(SGUIModifierType::Control))
        {
            DRightClick = event.DType.IsDoubleButtonPress() ? 2 : 1;
            DRightDown = false;
        }
        else
        {
            if (0 == DLeftClick)
            {
                DLeftClick = event.DType.IsDoubleButtonPress() ? 2 : 1;
            }
            DLeftDown = false;
        }
    }
    if (3 == event.DButton)
    {
        DRightClick = event.DType.IsDoubleButtonPress() ? 2 : 1;
        DRightDown = false;
    }
    return true;
}

bool CApplicationData::DrawingAreaMotionNotifyEvent(
    std::shared_ptr<CGUIWidget> widget, SGUIMotionEvent &event)
{
    int DrawingAreaAllocationWidth, DrawingAreaAllocationHeight;
    int EventX, EventY;
    EventX = event.DWindowX;
    EventY = event.DWindowY;

    DrawingAreaAllocationWidth = DDrawingArea->AllocatedWidth();
    DrawingAreaAllocationHeight = DDrawingArea->AllocatedHeight();

    if (EventX > DrawingAreaAllocationWidth)
    {
        EventX = DrawingAreaAllocationWidth - 1;
    }
    if (0 > EventX)
    {
        EventX = 0;
    }
    if (EventY > DrawingAreaAllocationHeight)
    {
        EventY = DrawingAreaAllocationHeight - 1;
    }
    if (0 > EventY)
    {
        EventY = 0;
    }

    DCurrentX = EventX;
    DCurrentY = EventY;
    return true;
}

CApplicationData::EUIComponentType CApplicationData::FindUIComponentType(
    const CPixelPosition &pos)
{
    int ViewWidth, ViewHeight;
    int MiniWidth, MiniHeight;
    int DescrWidth, DescrHeight;
    int ActWidth, ActHeight;

    // When overlay is active, it's the only valid UI component type
    if (OverlayActive())
    {
        return uictOverlay;
    }

    // When chat overlay is active, it's the only valid UI component type
    if (ChatOverlayActive())
    {
        return uictChat;
    }

    ViewWidth = DViewportSurface->Width();
    ViewHeight = DViewportSurface->Height();

    if ((DViewportXOffset > pos.X()) || (DViewportYOffset > pos.Y()) ||
        (DViewportXOffset + ViewWidth <= pos.X()) ||
        (DViewportYOffset + ViewHeight <= pos.Y()))
    {
        if ((DViewportXOffset - DInnerBevel->Width() <= pos.X()) &&
            (DViewportXOffset > pos.X()))
        {
            if ((DViewportYOffset <= pos.Y()) &&
                (pos.Y() < DViewportYOffset + ViewHeight))
            {
                return uictViewportBevelW;
            }
        }
        else if ((DViewportXOffset + ViewWidth <= pos.X()) &&
                 (DViewportXOffset + ViewWidth + DInnerBevel->Width() >
                  pos.X()))
        {
            if ((DViewportYOffset <= pos.Y()) &&
                (pos.Y() < DViewportYOffset + ViewHeight))
            {
                return uictViewportBevelE;
            }
        }
        else if ((DViewportXOffset <= pos.X()) &&
                 (pos.X() < DViewportXOffset + ViewWidth))
        {
            if ((DViewportYOffset - DInnerBevel->Width() <= pos.Y()) &&
                (DViewportYOffset > pos.Y()))
            {
                return uictViewportBevelN;
            }
            else if ((DViewportYOffset + ViewHeight <= pos.Y()) &&
                     (DViewportYOffset + ViewHeight + DInnerBevel->Width() >
                      pos.Y()))
            {
                return uictViewportBevelS;
            }
        }
    }
    else
    {
        return uictViewport;
    }
    MiniWidth = DMiniMapSurface->Width();
    MiniHeight = DMiniMapSurface->Height();
    if ((DMiniMapXOffset <= pos.X()) &&
        (DMiniMapXOffset + MiniWidth > pos.X()) &&
        (DMiniMapYOffset <= pos.Y()) &&
        (DMiniMapYOffset + MiniHeight > pos.Y()))
    {
        return uictMiniMap;
    }
    DescrWidth = DUnitDescriptionSurface->Width();
    DescrHeight = DUnitDescriptionSurface->Height();
    if ((DUnitDescriptionXOffset <= pos.X()) &&
        (DUnitDescriptionXOffset + DescrWidth > pos.X()) &&
        (DUnitDescriptionYOffset <= pos.Y()) &&
        (DUnitDescriptionYOffset + DescrHeight > pos.Y()))
    {
        return uictUserDescription;
    }
    ActWidth = DUnitActionSurface->Width();
    ActHeight = DUnitActionSurface->Height();
    if ((DUnitActionXOffset <= pos.X()) &&
        (DUnitActionXOffset + ActWidth > pos.X()) &&
        (DUnitActionYOffset <= pos.Y()) &&
        (DUnitActionYOffset + ActHeight > pos.Y()))
    {
        return uictUserAction;
    }

    if ((DMenuButtonXOffset <= pos.X()) &&
        (DMenuButtonXOffset + DMenuButtonRenderer->Width() > pos.X()) &&
        (DMenuButtonYOffset <= pos.Y()) &&
        (DMenuButtonYOffset + DMenuButtonRenderer->Height() > pos.Y()))
    {
        return uictMenuButton;
    }
    return uictNone;
}

CPixelPosition CApplicationData::ScreenToViewport(const CPixelPosition &pos)
{
    return CPixelPosition(pos.X() - DViewportXOffset,
                          pos.Y() - DViewportYOffset);
}

CPixelPosition CApplicationData::ScreenToMiniMap(const CPixelPosition &pos)
{
    return CPixelPosition(pos.X() - DMiniMapXOffset, pos.Y() - DMiniMapYOffset);
}

CPixelPosition CApplicationData::ScreenToDetailedMap(const CPixelPosition &pos)
{
    return ViewportToDetailedMap(ScreenToViewport(pos));
}

CPixelPosition CApplicationData::ScreenToUnitDescription(
    const CPixelPosition &pos)
{
    return CPixelPosition(pos.X() - DUnitDescriptionXOffset,
                          pos.Y() - DUnitDescriptionYOffset);
}

CPixelPosition CApplicationData::ScreenToUnitAction(const CPixelPosition &pos)
{
    return CPixelPosition(pos.X() - DUnitActionXOffset,
                          pos.Y() - DUnitActionYOffset);
}

CPixelPosition CApplicationData::ViewportToDetailedMap(const CPixelPosition &pos)
{
    return DViewportRenderer->DetailedPosition(pos);
}

CPixelPosition CApplicationData::MiniMapToDetailedMap(const CPixelPosition &pos)
{
    int X =
        pos.X() * DGameModel->Map()->Width() / DMiniMapRenderer->VisibleWidth();
    int Y = pos.Y() * DGameModel->Map()->Height() /
            DMiniMapRenderer->VisibleHeight();

    if (0 > X)
    {
        X = 0;
    }
    if (DGameModel->Map()->Width() <= X)
    {
        X = DGameModel->Map()->Width() - 1;
    }
    if (0 > Y)
    {
        Y = 0;
    }
    if (DGameModel->Map()->Height() <= Y)
    {
        Y = DGameModel->Map()->Height() - 1;
    }
    CPixelPosition Temp;
    Temp.SetXFromTile(X);
    Temp.SetYFromTile(Y);
    return Temp;
}

void CApplicationData::DrawBackground(
    std::shared_ptr<CGraphicSurface> surface, int &pagewidth, int &pageheight)
{
    for (int YPos = 0; YPos < pageheight;
         YPos += DBackgroundTileset->TileHeight())
    {
        for (int XPos = 0; XPos < pagewidth;
             XPos += DBackgroundTileset->TileWidth())
        {
            DBackgroundTileset->DrawTile(surface, XPos, YPos, 0);
        }
    }

}

void CApplicationData::DrawInnerBevel(
    std::shared_ptr<CGraphicSurface> surface, int &pagewidth, int &pageheight)
{
    DInnerBevel->DrawBevel(surface, DInnerBevel->Width(),
        DInnerBevel->Width(),
        pagewidth - DInnerBevel->Width() * 2,
        pageheight - DInnerBevel->Width() * 2);
}

void CApplicationData::DrawOuterBevel(
    std::shared_ptr<CGraphicSurface> surface, int &pagewidth, int &pageheight)
{
    DOuterBevel->DrawBevel(surface, DOuterBevel->Width(),
        DOuterBevel->Width(),
        pagewidth - DOuterBevel->Width() * 2,
        pageheight - DOuterBevel->Width() * 2);
}


void CApplicationData::RenderMenuTitle(const std::string &title,
                                       int &titlebottomy, int &pagewidth,
                                       int &pageheight)
{
    int TitleWidth;
    int TextColor, ShadowColor;

    pagewidth = DWorkingBufferSurface->Width();
    pageheight = DWorkingBufferSurface->Height();

    DrawBackground(DWorkingBufferSurface, pagewidth, pageheight);
    DrawOuterBevel(DWorkingBufferSurface, pagewidth, pageheight);

    DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Giant)]
        ->MeasureText(title, TitleWidth, titlebottomy);
    TextColor =
        DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Giant)]
            ->FindColor("white");
    ShadowColor =
        DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Giant)]
            ->FindColor("black");
    DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Giant)]
        ->DrawTextWithShadow(
            DWorkingBufferSurface, pagewidth / 2 - TitleWidth / 2,
            DOuterBevel->Width(), TextColor, ShadowColor, 1, title);
    PrintDebug(DEBUG_HIGH, "RenderMenuTitle Done\n");
}

void CApplicationData::RenderSplashStep()
{
    double RenderAlpha =
        (double) DCurrentLoadingStep / (double) DTotalLoadingSteps;
    DSplashTileset->DrawTile(DDoubleBufferSurface, 0, 0, 1);
    if (RenderAlpha > 0.0)
    {
        DSplashTileset->DrawTile(DWorkingBufferSurface, 0, 0, 0);
        auto ResourceContext = DDoubleBufferSurface->CreateResourceContext();
        ResourceContext->SetSourceSurface(DWorkingBufferSurface, 0, 0);

        ResourceContext->PaintWithAlpha(RenderAlpha);
    }

    DLoadingResourceContext->DrawSurface(DDoubleBufferSurface, 0, 0, -1, -1, 0,
                                         0);
    DCurrentLoadingStep++;
    DApplication->ProcessEvents(true);
}

void CApplicationData::SoundLoadingCallback(TSoundLibraryLoadingCalldata data)
{
    DApplicationDataPointer->RenderSplashStep();
}

void CApplicationData::ChangeApplicationMode(
    std::shared_ptr<CApplicationMode> mode)
{
    DNextApplicationMode = mode;
    DNextApplicationMode->InitializeChange(shared_from_this());
}

bool CApplicationData::ModeIsChanging()
{
    return DNextApplicationMode.get() != DApplicationMode.get();
}

void CApplicationData::ResetPlayerColors()
{
    for (int Index = 0; Index < to_underlying(EPlayerColor::Max); Index++)
    {
        DLoadingPlayerColors[Index] = static_cast<EPlayerColor>(Index);
    }
}

void CApplicationData::LoadGameMap(int index)
{
    int DetailedMapWidth, DetailedMapHeight;

    // Reset static counter every time a map is loaded
    if (1 != CPlayerAsset::GetIdCounter()) {
        CPlayerAsset::ResetIDCounter();
        CPlayerData::umap.clear();
        for (int Index = 0; Index < to_underlying(EPlayerNumber::Max); Index++)
        {
            DPlayerCommands[Index].DAction = EAssetCapabilityType::None;
            DPlayerCommands[Index].DActors.clear();
        }
    }

    DGameModel = std::make_shared<CGameModel>(index, 0x123456789ABCDEFULL,
                                              DLoadingPlayerColors);

    for (int Index = 1; Index < to_underlying(EPlayerNumber::Max); Index++)
    {
        DGameModel->Player(static_cast<EPlayerNumber>(Index))
            ->IsAI((ptAIEasy <= DLoadingPlayerTypes[Index]) &&
                   (ptAIHard >= DLoadingPlayerTypes[Index]));
    }
    for (int Index = 1; Index < to_underlying(EPlayerNumber::Max); Index++)
    {
        if (DGameModel->Player(static_cast<EPlayerNumber>(Index))->IsAI())
        {
            int Downsample = 1;
            switch (DLoadingPlayerTypes[Index])
            {
                case ptAIEasy:
                    Downsample = CPlayerAsset::UpdateFrequency();
                    break;
                case ptAIMedium:
                    Downsample = CPlayerAsset::UpdateFrequency() / 2;
                    break;
                default:
                    Downsample = CPlayerAsset::UpdateFrequency() / 4;
                    break;
            }
            DAIPlayers[Index] = std::make_shared<CAIPlayer>(
                DGameModel->Player(static_cast<EPlayerNumber>(Index)),
                Downsample);
        }
    }
    DCurrentAssetCapability = EAssetCapabilityType::None;

    DetailedMapWidth =
        DGameModel->Map()->Width() * DTerrainTileset->TileWidth();
    DetailedMapHeight =
        DGameModel->Map()->Height() * DTerrainTileset->TileHeight();

    DMapRenderer = std::make_shared<CMapRenderer>(
        std::make_shared<CMemoryDataSource>(DMapRendererConfigurationData),
        DTerrainTileset, DGameModel->Player(DPlayerNumber)->PlayerMap());
    PrintDebug(DEBUG_LOW, "Creating AssetRenderer\n");
    DAssetRenderer = std::make_shared<CAssetRenderer>(
        DAssetRecolorMap, DAssetTilesets, DMarkerTileset, DCorpseTileset,
        DFireTilesets, DBuildingDeathTileset, DArrowTileset,
        DGameModel->Player(DPlayerNumber),
        DGameModel->Player(DPlayerNumber)->PlayerMap());
    PrintDebug(DEBUG_LOW, "Creating FogRenderer\n");
    DFogRenderer = std::make_shared<CFogRenderer>(
        DFogTileset, DGameModel->Player(DPlayerNumber)->VisibilityMap());
    PrintDebug(DEBUG_LOW, "Creating ViewportRenderer\n");
    DViewportRenderer = std::make_shared<CViewportRenderer>(
        DMapRenderer, DAssetRenderer, DFogRenderer);
    PrintDebug(DEBUG_LOW, "Creating MiniMapRenderer\n");
    DMiniMapRenderer = std::make_shared<CMiniMapRenderer>(
        DMapRenderer, DAssetRenderer, DFogRenderer, DViewportRenderer,
        DDoubleBufferSurface->Format());
    PrintDebug(DEBUG_LOW, "Creating NotificationRenderer\n");
    DNotificationRenderer = std::make_shared<CNotificationRenderer>(DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]);
    PrintDebug(DEBUG_LOW, "Creating UnitDescriptionRenderer\n");
    DUnitDescriptionRenderer = std::make_shared<CUnitDescriptionRenderer>(
        DMiniBevel, DIconTileset, DFonts,
        DGameModel->Player(DPlayerNumber)->Color());
    PrintDebug(DEBUG_LOW, "Creating CUnitActionRenderer\n");
    DUnitActionRenderer = std::make_shared<CUnitActionRenderer>(
        DMiniBevel, DIconTileset, DGameModel->Player(DPlayerNumber));
    PrintDebug(DEBUG_LOW, "Creating CResourceRenderer\n");
    DResourceRenderer = std::make_shared<CResourceRenderer>(
        DMiniIconTileset,
        DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Medium)],
        DGameModel->Player(DPlayerNumber));
    PrintDebug(DEBUG_LOW, "Creating CButtonDescriptionRenderer\n");
    DButtonDescriptionRenderer = std::make_shared<CButtonDescriptionRenderer>(
        DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Large)]);
    PrintDebug(DEBUG_LOW, "Creating CSoundEventRenderer\n");
    DSoundEventRenderer = std::make_shared<CSoundEventRenderer>(
        DSoundLibraryMixer, DGameModel->Player(DPlayerNumber));
    PrintDebug(DEBUG_LOW,
               "Setting CSoundEventRenderer Volume to DSoundVolume\n");
    DSoundEventRenderer->Volume(DSoundVolume);
    PrintDebug(DEBUG_LOW, "Creating CButtonRenderer\n");
    DMenuButtonRenderer = std::make_shared<CButtonRenderer>(
        DButtonRecolorMap, DInnerBevel, DOuterBevel,
        DFonts[to_underlying(CUnitDescriptionRenderer::EFontSize::Medium)]);

    DMenuButtonRenderer->Text("Menu");
    DMenuButtonRenderer->ButtonColor(
        DGameModel->Player(DPlayerNumber)->Color());

    int LeftPanelWidth = std::max(DUnitDescriptionRenderer->MinimumWidth(),
                                  DUnitActionRenderer->MinimumWidth()) +
                         DOuterBevel->Width() * 4;
    LeftPanelWidth =
        std::max(LeftPanelWidth, MINI_MAP_MIN_WIDTH + DInnerBevel->Width() * 4);
    int MinUnitDescrHeight;

    DMiniMapXOffset = DInnerBevel->Width() * 2;
    DUnitDescriptionXOffset = DOuterBevel->Width() * 2;
    DUnitActionXOffset = DUnitDescriptionXOffset;
    DViewportXOffset = LeftPanelWidth + DInnerBevel->Width();

    DMiniMapYOffset = DBorderWidth;
    DUnitDescriptionYOffset = DMiniMapYOffset +
                              (LeftPanelWidth - DInnerBevel->Width() * 3) +
                              DOuterBevel->Width() * 2;
    MinUnitDescrHeight = DUnitDescriptionRenderer->MinimumHeight(
        LeftPanelWidth - DOuterBevel->Width() * 4, 9);
    DUnitActionYOffset =
        DUnitDescriptionYOffset + MinUnitDescrHeight + DOuterBevel->Width() * 3;
    DViewportYOffset = DBorderWidth;

    int MainWindowMinHeight = DUnitDescriptionYOffset + MinUnitDescrHeight +
                              DUnitActionRenderer->MinimumHeight() +
                              DOuterBevel->Width() * 5;
    DMainWindow->SetMinSize(INITIAL_MAP_WIDTH, MainWindowMinHeight);
    DMainWindow->SetMaxSize(
        DViewportXOffset + DetailedMapWidth + DBorderWidth,
        std::max(MainWindowMinHeight, DetailedMapHeight + DBorderWidth * 2));

    ResizeCanvases();
    PrintDebug(DEBUG_LOW, "Canvases Resized\n");
    DMenuButtonRenderer->Width(DViewportXOffset / 2);
    DMenuButtonXOffset =
        DViewportXOffset / 2 - DMenuButtonRenderer->Width() / 2;
    DMenuButtonYOffset = (DViewportYOffset - DOuterBevel->Width()) / 2 -
                         DMenuButtonRenderer->Height() / 2;

    int CurWidth, CurHeight;

    CurWidth = DViewportSurface->Width();
    CurHeight = DViewportSurface->Height();
    DViewportRenderer->InitViewportDimensions(CurWidth, CurHeight);

    // Overlay X,Y offsets


    for (auto WeakAsset : DGameModel->Player(DPlayerNumber)->Assets())
    {
        if (auto Asset = WeakAsset.lock())
        {
            DViewportRenderer->CenterViewport(Asset->Position());
            break;
        }
    }
}

void CApplicationData::ResizeCanvases()
{
    int DrawingAreaAllocationWidth, DrawingAreaAllocationHeight;
    int ViewportWidth, ViewportHeight;
    int UserDescrWidth, UserDescrHeight;
    // Resize the canvas
    if (!DDrawingArea)
    {
        return;
    }
    DrawingAreaAllocationWidth = DDrawingArea->AllocatedWidth();
    DrawingAreaAllocationHeight = DDrawingArea->AllocatedHeight();
    PrintDebug(DEBUG_LOW, "Resizing %d x %d\n", DrawingAreaAllocationWidth,
               DrawingAreaAllocationHeight);
    ViewportWidth =
        DrawingAreaAllocationWidth - DViewportXOffset - DBorderWidth;
    ViewportHeight =
        DrawingAreaAllocationHeight - DViewportYOffset - DBorderWidth;
    if (nullptr != DDoubleBufferSurface)
    {
        int CurWidth, CurHeight;

        CurWidth = DDoubleBufferSurface->Width();
        CurHeight = DDoubleBufferSurface->Height();
        if ((DrawingAreaAllocationWidth != CurWidth) ||
            (DrawingAreaAllocationHeight != CurHeight))
        {
            DDoubleBufferSurface = nullptr;
        }
    }
    if (nullptr == DDoubleBufferSurface)
    {
        DDoubleBufferSurface = CGraphicFactory::CreateSurface(
            DrawingAreaAllocationWidth, DrawingAreaAllocationHeight,
            CGraphicSurface::ESurfaceFormat::
                ARGB32);  // DDrawingArea->CreateSimilarSurface(DrawingAreaAllocationWidth,
                          // DrawingAreaAllocationHeight);
    }
    if (nullptr != DWorkingBufferSurface)
    {
        int CurWidth, CurHeight;

        CurWidth = DWorkingBufferSurface->Width();
        CurHeight = DWorkingBufferSurface->Height();
        if ((DrawingAreaAllocationWidth != CurWidth) ||
            (DrawingAreaAllocationHeight != CurHeight))
        {
            DWorkingBufferSurface = nullptr;
        }
    }
    if (nullptr == DWorkingBufferSurface)
    {
        DWorkingBufferSurface = CGraphicFactory::CreateSurface(
            DrawingAreaAllocationWidth, DrawingAreaAllocationHeight,
            CGraphicSurface::ESurfaceFormat::
                ARGB32);  // DDrawingArea->CreateSimilarSurface(DrawingAreaAllocationWidth,
                          // DrawingAreaAllocationHeight);
    }
    if (!DInnerBevel)
    {
        return;
    }
    if (nullptr == DMiniMapSurface)
    {
        int Dimension = DViewportXOffset - DInnerBevel->Width() * 5;
        DMiniMapSurface = CGraphicFactory::CreateSurface(
            Dimension, Dimension,
            CGraphicSurface::ESurfaceFormat::
                ARGB32);  // DDrawingArea->CreateSimilarSurface(Dimension,
                          // Dimension);
    }
    if (nullptr != DMiniMapSurface)
    {
        uint32_t ColorBlack = 0x000000;
        int CurWidth, CurHeight;

        CurWidth = DMiniMapSurface->Width();
        CurHeight = DMiniMapSurface->Height();
        auto ResourceContext = DMiniMapSurface->CreateResourceContext();
        ResourceContext->SetSourceRGB(ColorBlack);
        ResourceContext->Rectangle(0, 0, CurWidth, CurHeight);
        ResourceContext->Fill();
    }
//    if (!DUnitDescriptionRenderer)
//    {
//        return;
//    }
    if (!DOuterBevel)
    {
        return;
    }
    UserDescrWidth =
        DViewportXOffset - DInnerBevel->Width() - DOuterBevel->Width() * 4;
    UserDescrHeight =
        DUnitDescriptionRenderer->MinimumHeight(UserDescrWidth, 9);
    if (nullptr != DUnitDescriptionSurface)
    {
        int CurWidth, CurHeight;

        CurWidth = DUnitDescriptionSurface->Width();
        CurHeight = DUnitDescriptionSurface->Height();
        if ((CurWidth != UserDescrWidth) || (CurHeight != UserDescrHeight))
        {
            DUnitDescriptionSurface = nullptr;
        }
    }
    if (nullptr == DUnitDescriptionSurface)
    {
        DUnitDescriptionSurface = CGraphicFactory::CreateSurface(
            UserDescrWidth, UserDescrHeight,
            CGraphicSurface::ESurfaceFormat::
                ARGB32);  // DDrawingArea->CreateSimilarSurface(UserDescrWidth,
                          // UserDescrHeight);
    }
    DUnitActionYOffset =
        DUnitDescriptionYOffset + UserDescrHeight + DOuterBevel->Width() * 3;
    if (!DUnitActionRenderer)
    {
        return;
    }
    if (nullptr != DUnitActionSurface)
    {
        int CurWidth, CurHeight;

        CurWidth = DUnitActionSurface->Width();
        CurHeight = DUnitActionSurface->Height();
        if ((CurWidth != DUnitActionRenderer->MinimumWidth()) ||
            (CurHeight != DUnitActionRenderer->MinimumHeight()))
        {
            DUnitActionSurface = nullptr;
        }
    }
    if (nullptr == DUnitActionSurface)
    {
        DUnitActionSurface = CGraphicFactory::CreateSurface(
            DUnitActionRenderer->MinimumWidth(),
            DUnitActionRenderer->MinimumHeight(),
            CGraphicSurface::ESurfaceFormat::
                ARGB32);  // DDrawingArea->CreateSimilarSurface(DUnitActionRenderer->MinimumWidth(),
        // DUnitActionRenderer->MinimumHeight());
    }

    //Notification Renderer surface
    if(nullptr != DNotificationRendererSurface){
        int CurWidth, CurHeight;

        CurWidth = DNotificationRendererSurface->Width();
        CurHeight = DNotificationRendererSurface->Height();

        if((ViewportWidth != CurWidth || (DBorderWidth != CurHeight)))
        {
            DNotificationRendererSurface = nullptr;
        }
    }
    if(nullptr == DNotificationRendererSurface)
    {
        DNotificationRendererSurface = CGraphicFactory::CreateSurface(ViewportWidth,ViewportHeight/4,CGraphicSurface::ESurfaceFormat::ARGB32);
    }


    if (nullptr != DResourceSurface)
    {
        int CurWidth, CurHeight;

        CurWidth = DResourceSurface->Width();
        CurHeight = DResourceSurface->Height();
        if ((ViewportWidth != CurWidth) || (DBorderWidth != CurHeight))
        {
            DResourceSurface = nullptr;
        }
    }
    if (nullptr == DResourceSurface)
    {
        DResourceSurface = CGraphicFactory::CreateSurface(
            ViewportWidth, DBorderWidth,
            CGraphicSurface::ESurfaceFormat::
                ARGB32);  // DDrawingArea->CreateSimilarSurface(ViewportWidth,
                          // DBorderWidth);
    }

    // Button Description Surface
    if (nullptr != DButtonDescriptionSurface)
    {
        int CurWidth, CurHeight;

        CurWidth = DButtonDescriptionSurface->Width();
        CurHeight = DButtonDescriptionSurface->Height();

        if ((ViewportWidth != CurWidth) || (DBorderWidth != CurHeight))
        {
            DButtonDescriptionSurface = nullptr;
        }
    }
    if (nullptr == DButtonDescriptionSurface)
    {
        DButtonDescriptionSurface = CGraphicFactory::CreateSurface(
            ViewportWidth, DBorderWidth,
            CGraphicSurface::ESurfaceFormat::ARGB32);
    }

    if (nullptr != DViewportSurface)
    {
        int CurWidth, CurHeight;

        CurWidth = DViewportSurface->Width();
        CurHeight = DViewportSurface->Height();
        if ((ViewportWidth != CurWidth) || (ViewportHeight != CurHeight))
        {
            DViewportSurface = nullptr;
            DViewportTypeSurface = nullptr;
        }
    }
    if (nullptr == DViewportSurface)
    {
        uint32_t ColorBlack = 0x000000;

        if (0 > ViewportWidth)
        {
            ViewportWidth = 1;
        }
        if (0 > ViewportHeight)
        {
            ViewportHeight = 1;
        }
        DViewportSurface = CGraphicFactory::CreateSurface(
            ViewportWidth, ViewportHeight,
            CGraphicSurface::ESurfaceFormat::
                ARGB32);  // DDrawingArea->CreateSimilarSurface(ViewportWidth,
                          // ViewportHeight);
        auto ResourceContext = DViewportSurface->CreateResourceContext();
        ResourceContext->SetSourceRGB(ColorBlack);
        ResourceContext->Rectangle(0, 0, ViewportWidth, ViewportHeight);
        ResourceContext->Fill();
        DViewportTypeSurface = CGraphicFactory::CreateSurface(
            ViewportWidth, ViewportHeight,
            CGraphicSurface::ESurfaceFormat::
                ARGB32);  // DDrawingArea->CreateSimilarSurface(ViewportWidth,
                          // ViewportHeight);
    }
    PrintDebug(DEBUG_LOW, "Resizing Complete\n");
}

bool CApplicationData::MultiPlayer()
{
    if (EGameSessionType::gstSinglePlayer != DGameSessionType)
    {
        return true;
    }
    return false;
}

void CApplicationData::DeactivateOverlay()
{
    DOverlayActive = false;
}

void CApplicationData::ActivateOverlay()
{
    DOverlayActive = true;
}

void CApplicationData::ToggleOverlay()
{
    if (OverlayActive())
    {
        DeactivateOverlay();
    }
    else
    {
        ActivateOverlay();
    }
}

bool CApplicationData::OverlayActive()
{
    return DOverlayActive;
}

bool CApplicationData::ChatOverlayActive()
{
    return DChatOverlayActive;
}

void CApplicationData::DeactivateChatOverlay()
{
    DChatOverlayActive = false;
}

void CApplicationData::ActivateChatOverlay()
{
    DChatOverlayActive = true;
}

void CApplicationData::ToggleChatOverlay()
{
    if (ChatOverlayActive())
    {
        DeactivateChatOverlay();
    }
    else
    {
        ActivateChatOverlay();
    }
}

int CApplicationData::FindClipID(const std::string &name)
{
    return DSoundLibraryMixer->FindClip(name);
}

void CApplicationData::StartPlayingClip(const std::string &name)
{
    int ClipID = FindClipID(name);
    DSoundLibraryMixer->PlayClip(ClipID, DSoundVolume, 0.0);
}

int CApplicationData::FindSongID(const std::string &name)
{
    return DSoundLibraryMixer->FindSong(name);
}

void CApplicationData::StopPlayingMusic()
{
    DSoundLibraryMixer->StopSong();
}

void CApplicationData::StartPlayingMusic(const std::string &song)
{
    int SongID = FindSongID(song);
    DSoundLibraryMixer->PlaySong(SongID, DMusicVolume);
}

void CApplicationData::SetFXVolume(float fx_vol)
{
    DSoundVolume = fx_vol;
    DSoundEventRenderer->Volume(fx_vol);
}

void CApplicationData::SetMusicVolume(float music_vol)
{
    DMusicVolume = music_vol;
    DSoundLibraryMixer->SongVolume(music_vol);
}

void CApplicationData::SaveVolumeSettings(float fx_vol, float music_vol)
{
    std::string FullPath = DDataPath + "/snd/" + DSoundSettingsFilename;
    nlohmann::json settings;
    settings["soundOption"]["soundVolume"] = fx_vol;
    settings["soundOption"]["musicVolume"] = music_vol;
    if (std::ofstream(FullPath))
    {
        std::ofstream o(FullPath);
        o << std::setw(4) << settings << std::endl;

    }
}

// Load FX/Music volume settings from a config file
// If no settings are found, set to some defaults
void CApplicationData::LoadVolumeSettings(std::string path)
{
    float FX_Vol = -1.0, Music_Vol = -1.0;

    // If sound and music options were previously altered, set to those values
    if (std::ifstream(path))
    {
        std::ifstream i(path);
        nlohmann::json settings;
        i >> settings;
        DSoundVolume = settings["soundOption"]["soundVolume"];
        DMusicVolume = settings["soundOption"]["musicVolume"];
    }
    // Defaults
    else
    {
        DSoundVolume = 1;
        DMusicVolume = 0.5;
    }
}

// Function handle changing state from game play to a menu mode
void CApplicationData::LeaveGame()
{
    StopPlayingMusic();
    StartPlayingMusic("menu");

    // Leave game flag used inside battle mode to signal mode change
    DLeaveGame = false;
    // Active game flag to indicate game in progress
    DActiveGame = false;

    if (MultiPlayer())
    {
        ClientPointer->SendMessage("Leave");
        roomInfo.Clear();
        ChangeApplicationMode(CMultiPlayerOptionsMenuMode::Instance());
    }
    else
    {
        ChangeApplicationMode(CMainMenuMode::Instance());
    }
}

void CApplicationData::ShutdownGame()
{
    // Cleanly terminate any state before closing the application

    StopPlayingMusic();
    DMainWindow->Close();
}

void CApplicationData::ClearMouseButtonState()
{
    DLeftClick = 0;
    DLeftDown = false;
    DRightClick = 0;
    DRightDown = false;
}

void CApplicationData::SetLeaveGameFlag(bool flag)
{
    DLeaveGame = flag;
}

bool CApplicationData::CheckLeaveGameFlag()
{
    return DLeaveGame;
}

int CApplicationData::Run(int argc, char *argv[])
{
    if (argc == 3)
    {
        std::string arg = argv[1];
        if (arg == "-d")
        {
            DDataPath = argv[2];
            // Workaround because GTK code doesn't recognize the commandline options
            // Very ugly! Will make this better. -MLH
            argc = 1;
        }
    }
    return DApplication->Run(argc, argv);
}
