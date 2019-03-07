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
#ifndef GUIEVENT_H
#define GUIEVENT_H
#include <cstdint>
#include <memory>

class CGUIWidget;
class CGraphicResourceContext;

using TGUICalldata = void *;

struct SGUIButtonEventType
{
    uint32_t DType;
    static const uint32_t DButtonPress;
    static const uint32_t DDoubleButtonPress;
    static const uint32_t DTripleButtonPress;
    static const uint32_t DButtonRelease;
    bool IsButtonPress() const;
    bool IsDoubleButtonPress() const;
    bool IsTripleButtonPress() const;
    bool IsButtonRelease() const;
};

struct SGUIModifierType
{
    uint32_t DState;
    static const uint32_t Shift;
    static const uint32_t Lock;
    static const uint32_t Control;
    static const uint32_t Alt;
    static const uint32_t Button1;
    static const uint32_t Button2;
    static const uint32_t Button3;
    bool ModifierIsSet(uint32_t val) const;
    void SetModifier(uint32_t val);
    void ClearModifier(uint32_t val);
};

struct SGUIKeyType
{
    uint32_t DValue;
    static const uint32_t Return;
    static const uint32_t LeftShift;
    static const uint32_t RightShift;
    static const uint32_t LeftControl;
    static const uint32_t RightControl;
    static const uint32_t LeftAlt;
    static const uint32_t RightAlt;
    static const uint32_t Escape;
    // Space -> Slash are 0x020 -> 0x02f
    static const uint32_t Space;
    static const uint32_t Exclam;
    static const uint32_t QuoteDbl;
    static const uint32_t NumberSign;
    static const uint32_t Dollar;
    static const uint32_t Percent;
    static const uint32_t Ampersand;
    static const uint32_t Apostrophe;
    static const uint32_t QuoteRight;
    static const uint32_t ParenLeft;
    static const uint32_t ParenRight;
    static const uint32_t Asterisk;
    static const uint32_t Plus;
    static const uint32_t Comma;
    static const uint32_t Minus;
    static const uint32_t Period;
    static const uint32_t Slash;
    // Colon -> At (@) are 0x03a -> 0x040
    static const uint32_t Colon;
    static const uint32_t SemiColon;
    static const uint32_t Less;
    static const uint32_t Equal;
    static const uint32_t Greater;
    static const uint32_t Question;
    static const uint32_t At;
    // BracketLeft -> QuoteLeft are 0x05b -> 0x060
    static const uint32_t BracketLeft;
    static const uint32_t BackSlash;
    static const uint32_t BracketRight;
    static const uint32_t ASCIICircum;
    static const uint32_t UnderScore;
    static const uint32_t Grave;
    static const uint32_t QuoteLeft;
    // BraceLeft -> ASCIITilde are 0x07b -> 0x07e
    static const uint32_t BraceLeft;
    static const uint32_t Bar;
    static const uint32_t BraceRight;
    static const uint32_t ASCIITilde;
    static const uint32_t Delete;
    static const uint32_t BackSpace;
    static const uint32_t UpArrow;
    static const uint32_t DownArrow;
    static const uint32_t LeftArrow;
    static const uint32_t RightArrow;
    static const uint32_t Key0;
    static const uint32_t Key1;
    static const uint32_t Key2;
    static const uint32_t Key3;
    static const uint32_t Key4;
    static const uint32_t Key5;
    static const uint32_t Key6;
    static const uint32_t Key7;
    static const uint32_t Key8;
    static const uint32_t Key9;
    static const uint32_t KeyA;
    static const uint32_t KeyB;
    static const uint32_t KeyC;
    static const uint32_t KeyD;
    static const uint32_t KeyE;
    static const uint32_t KeyF;
    static const uint32_t KeyG;
    static const uint32_t KeyH;
    static const uint32_t KeyI;
    static const uint32_t KeyJ;
    static const uint32_t KeyK;
    static const uint32_t KeyL;
    static const uint32_t KeyM;
    static const uint32_t KeyN;
    static const uint32_t KeyO;
    static const uint32_t KeyP;
    static const uint32_t KeyQ;
    static const uint32_t KeyR;
    static const uint32_t KeyS;
    static const uint32_t KeyT;
    static const uint32_t KeyU;
    static const uint32_t KeyV;
    static const uint32_t KeyW;
    static const uint32_t KeyX;
    static const uint32_t KeyY;
    static const uint32_t KeyZ;
    static const uint32_t Keya;
    static const uint32_t Keyb;
    static const uint32_t Keyc;
    static const uint32_t Keyd;
    static const uint32_t Keye;
    static const uint32_t Keyf;
    static const uint32_t Keyg;
    static const uint32_t Keyh;
    static const uint32_t Keyi;
    static const uint32_t Keyj;
    static const uint32_t Keyk;
    static const uint32_t Keyl;
    static const uint32_t Keym;
    static const uint32_t Keyn;
    static const uint32_t Keyo;
    static const uint32_t Keyp;
    static const uint32_t Keyq;
    static const uint32_t Keyr;
    static const uint32_t Keys;
    static const uint32_t Keyt;
    static const uint32_t Keyu;
    static const uint32_t Keyv;
    static const uint32_t Keyw;
    static const uint32_t Keyx;
    static const uint32_t Keyy;
    static const uint32_t Keyz;
    bool IsKey(uint32_t val) const;
    void SetKey(uint32_t val);
    bool IsAlpha() const;
    bool IsAlphaNumeric() const;
    bool IsDigit() const;
    bool IsUpper() const;
    bool IsLower() const;
    bool IsASCII() const;
    bool IsPunctuation() const;
    bool IsViewableInTextField() const;
};

struct SGUIButtonEvent
{
    SGUIButtonEventType DType;
    double DWindowX;
    double DWindowY;
    SGUIModifierType DModifier;
    int DButton;
    double DScreenX;
    double DScreenY;
};

struct SGUIMotionEvent
{
    double DWindowX;
    double DWindowY;
    SGUIModifierType DModifier;
    double DScreenX;
    double DScreenY;
};

struct SGUIConfigureEvent
{
    int DX;
    int DY;
    int DWidth;
    int DHeight;
};

struct SGUIKeyEvent
{
    SGUIKeyType DValue;
    SGUIModifierType DModifier;
};

enum class EGUIEvent
{
    Expose,
    Motion,
    ButtonPress,
    ButtonRelease,
    KeyPress,
    KeyRelease
};

using TGUIApplicationCallback = void (*)(TGUICalldata data);
using TGUITimeoutCallback = bool (*)(TGUICalldata data);
using TGUIActivateEventCallback = void (*)(std::shared_ptr<CGUIWidget> widget,
                                           TGUICalldata data);
using TGUIDeleteEventCallback = bool (*)(std::shared_ptr<CGUIWidget> widget,
                                         TGUICalldata data);
using TGUIDestroyEventCallback = void (*)(std::shared_ptr<CGUIWidget> widget,
                                          TGUICalldata data);
using TGUIButtonEventCallback = bool (*)(std::shared_ptr<CGUIWidget> widget,
                                         SGUIButtonEvent &event,
                                         TGUICalldata data);
using TGUIMotionEventCallback = bool (*)(std::shared_ptr<CGUIWidget> widget,
                                         SGUIMotionEvent &event,
                                         TGUICalldata data);
using TGUIConfigureEventCallback = bool (*)(std::shared_ptr<CGUIWidget> widget,
                                            SGUIConfigureEvent &event,
                                            TGUICalldata data);
using TGUIDrawEventCallback =
    bool (*)(std::shared_ptr<CGUIWidget> widget,
             std::shared_ptr<CGraphicResourceContext> rc, TGUICalldata data);
using TGUIKeyEventCallback = bool (*)(std::shared_ptr<CGUIWidget> widget,
                                      SGUIKeyEvent &event, TGUICalldata data);

#endif
