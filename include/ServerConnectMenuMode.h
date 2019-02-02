/*
    Copyright (c) 2015, Christopher Nitta
    All rights reserved.
    All source material (source code, images, sounds, etc.) have been provided
    to University of California, Davis students of course ECS 160 for educational
    purposes. It may not be distributed beyond those enrolled in the course
    without prior permission from the copyright holder.
    All sound files, sound fonts, midi files, and images that have been included
    that were extracted from original Warcraft II by Blizzard Entertainment
    were found freely available via iznternet sources and have been labeld as
    abandonware. They have been included in this distribution for educational
    purposes only and this copyright notice does not attempt to claim any
    ownership of this material.
*/
#ifndef SERVERCONNECTMENUMODE_H
#define SERVERCONNECTMENUMODE_H

#include <string>
#include <vector>
#include "ApplicationMode.h"
#include "Rectangle.h"
#include "RoomInfo.pb.h"

using TServerConnectCallbackFunction =
    void (*)(std::shared_ptr<CApplicationData>);

class CServerConnectMenuMode : public CApplicationMode
{
  protected:
    struct SPrivateConstructorType
    {
    };

    //RoomList info
    RoomInfo::RoomInfoPackage roomList;

    static std::shared_ptr<CServerConnectMenuMode> DServerConnectMenuModePointer;
    std::string DTitle;
    std::vector<std::string> DButtonTexts;
    std::vector<TServerConnectCallbackFunction> DButtonFunctions;
    std::vector<SRectangle> DButtonLocations;
    std::vector<TServerConnectCallbackFunction> DJoinButtonFunctions;
    std::vector<SRectangle> DJoinButtonLocations;
    std::vector<SRectangle> DPlayerTypeButtonLocations;

    bool DButtonHovered;
    int DMapOffset;

    int CurrentX, CurrentY;
    int BufferWidth, BufferHeight;
    int TitleHeight = 0;
    int TextWidth, TextHeight, MaxTextWidth;
    int ColumnWidth, RowHeight;
    int TextTop, BackButtonLeft, BackButtonTop, AIButtonLeft, ColorButtonHeight;
    int GoldColor, WhiteColor, ShadowColor;
    bool ButtonXAlign = false, ButtonHovered = false;
    int LargeFontSize;


    void DrawText(std::shared_ptr<CApplicationData> context,
        std::string text, int xpos, int ypos, int color);
    static void BackButtonCallback(std::shared_ptr<CApplicationData> context);
    // static void SoundOptionsButtonCallback(
    //     std::shared_ptr<CApplicationData> context);
    // static void NetworkOptionsButtonCallback(
    //     std::shared_ptr<CApplicationData> context);
    // static void MainMenuButtonCallback(
    //     std::shared_ptr<CApplicationData> context);

    CServerConnectMenuMode(const CServerConnectMenuMode &) = delete;
    const CServerConnectMenuMode &operator=(const CServerConnectMenuMode &) = delete;

  public:
    explicit CServerConnectMenuMode(const SPrivateConstructorType &key);
    static std::shared_ptr<CApplicationMode> Instance();

    virtual void InitializeChange(std::shared_ptr<CApplicationData> context) override;
    virtual void Input(std::shared_ptr<CApplicationData> context) override;
    virtual void Calculate(std::shared_ptr<CApplicationData> context) override;
    virtual void Render(std::shared_ptr<CApplicationData> context) override;

};

#endif
