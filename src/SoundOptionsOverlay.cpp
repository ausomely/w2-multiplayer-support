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

#include "SoundOptionsOverlay.h"
#include "OverlayManagement.h"
#include "HorizontalButtonAlignment.h"
#include <fstream>
#include <iomanip>
#include <SoundOptionsOverlay.h>
#include "json.hpp"
#include "Rectangle.h"


CSoundOptionsOverlay::CSoundOptionsOverlay(std::shared_ptr<COverlayManagement>
    manager)
{
    DOverlayManager = manager;
    DContext = DOverlayManager->Context();

    int CanvasWidth = DOverlayManager->CanvasWidth();
    int CanvasHeight = DOverlayManager->CanvasHeight();

    DEditSelected = -1;
    DEditSelectedCharacter = -1;
    DEditLocations.clear();
    DEditText.resize(0);
    DEditText.push_back(std::to_string(static_cast<int>(DContext->DSoundVolume * 100)));
    DEditText.push_back(std::to_string(static_cast<int>(DContext->DMusicVolume *
    100)));

    DEditLocations.resize(0);

    DButtonTexts.push_back("OK");
    DButtonTexts.push_back("Cancel");

    DEditTitles.push_back("FX Volume:");
    DEditTitles.push_back("Music Volume");

    DFontID = to_underlying(CUnitDescriptionRenderer::EFontSize::Large);

//    MeasureText();

    DButtonStack =
        std::make_shared<CHorizontalButtonAlignment>(DContext, DButtonTexts,
            EPosition::South, CanvasWidth, CanvasHeight);
    /*
    DButtonFunctions.push_back(SoundOptionsUpdateButtonCallback);
    DButtonFunctions.push_back(OptionsButtonCallback);

    DEditValidationFunctions.push_back(ValidSoundLevelCallback);
    DEditValidationFunctions.push_back(ValidSoundLevelCallback);
    */

}

//! Handle inputs
void CSoundOptionsOverlay::Input(int x, int y, bool clicked)
{
    // Process text entry fields
    ProcessTextEntryFields(x, y, clicked);
    ProcessKeyStrokes();

    // Check button state (last)
    ProcessButtonStack();
}

void CSoundOptionsOverlay::ProcessTextEntryFields(int X, int Y, bool Clicked)
{
    if (DContext->DLeftClick && !DContext->DLeftDown)
    {
        bool ClickedEdit = false;
        for (int Index = 0; Index < DEditLocations.size(); Index++)
        {
            if (DEditLocations[Index].PointInside(X, Y))
            {
                if (Index != DEditSelected)
                {
                    DEditSelected = Index;
                    DEditSelectedCharacter = DEditText.at(Index).size();
                    ClickedEdit = true;
                }
            }
        }
        if (!ClickedEdit)
        {
            DEditSelected = -1;
        }
    }
}

void CSoundOptionsOverlay::ProcessKeyStrokes()
{
    for (auto Key : DContext->DReleasedKeys)
    {
        if (SGUIKeyType::Escape == Key)
        {
            DEditSelected = -1;
        }
        else if (0 <= DEditSelected)
        {
            SGUIKeyType TempKey;
            TempKey.DValue = Key;
            if ((SGUIKeyType::Delete == Key) || (SGUIKeyType::BackSpace == Key))
            {
                if (DEditSelectedCharacter)
                {
                    DEditText[DEditSelected] =
                        DEditText[DEditSelected].substr(
                            0, DEditText[DEditSelected].size() - 1) +
                            DEditText[DEditSelected].substr(
                                DEditSelectedCharacter,
                                DEditText[DEditSelected].length() -
                                    DEditSelectedCharacter);
                    DEditSelectedCharacter--;
                }
                else if (DEditText[DEditSelected].length())
                {
                    DEditText[DEditSelected] =
                        DEditText[DEditSelected].substr(1);
                }
            }
            else if (SGUIKeyType::LeftArrow == Key)
            {
                if (DEditSelectedCharacter)
                {
                    DEditSelectedCharacter--;
                }
            }
            else if (SGUIKeyType::RightArrow == Key)
            {
                if (DEditSelectedCharacter < DEditText[DEditSelected].length())
                {
                    DEditSelectedCharacter++;
                }
            }
            else if (TempKey.IsAlphaNumeric() || (SGUIKeyType::Period == Key))
            {
                DEditText[DEditSelected] =
                    DEditText[DEditSelected].substr(0, DEditSelectedCharacter) +
                        std::string(1, (char) Key) +
                        DEditText[DEditSelected].substr(
                            DEditSelectedCharacter,
                            DEditText[DEditSelected].length() -
                                DEditSelectedCharacter);
                DEditSelectedCharacter++;
            }
        }
    }
    DContext->DReleasedKeys.clear();

}

void CSoundOptionsOverlay::ProcessButtonStack()
{
    if (DButtonStack->ButtonPressedInStack())
    {
        if (0 == DButtonStack->ButtonPressedIndex())
        {
            SaveSettings();
        }
        DOverlayManager->SetMode(EOverlay::InGameMenu);
        DOverlayManager->ClearMouseButtonState();
    }
}

//! Handle drawing
void CSoundOptionsOverlay::Draw(int x, int y, bool clicked)
{
    DrawTextFields();
    DButtonStack->DrawStack(DOverlayManager->Surface(), x, y, clicked);
}

void CSoundOptionsOverlay::DrawTextFields()
{
    bool ValidSoundLevel = true;
    int Width = DOverlayManager->Width();
    int Height = DOverlayManager->Height();
    int Center = Width / 2.5;
    int TextWidth, TextHeight;
    int OptionSkip = DContext->DOptionsEditRenderer->Height() * 3/2;
    int OptionTop = static_cast<int>(Height / 2 -
        (OptionSkip * DEditTitles.size())/2);

    SetTextColors();

    // Clear out text locations vector
    DEditLocations.clear();

    // Center - DTextMaxWidth
    for (int Index = 0; Index < DEditTitles.size(); ++Index)
    {
        // ValidSoundLevel used below to set Inactive button when an invalid
        // entry is in the field.
        if (!ValidateSoundLevel(DEditText[Index]))
        {
            ValidSoundLevel = false;
        }

        DContext->DFonts[DFontID]->MeasureText(DEditTitles[Index], TextWidth,
            TextHeight);

        DContext->DFonts[DFontID]->DrawTextWithShadow(DOverlayManager->Surface(),
            Center - TextWidth, OptionTop + DTextYoffset, DWhiteColor,
            DShadowColor, 1, DEditTitles[Index]);

        DContext->DOptionsEditRenderer->Text(DEditText[Index],
            ValidateSoundLevel(DEditText[Index]));

        DContext->DOptionsEditRenderer->DrawEdit(DOverlayManager->Surface(),
            Center, OptionTop, Index == DEditSelected ? DEditSelectedCharacter : -1);

        DEditLocations.push_back(SRectangle(
            {Center, OptionTop, DContext->DOptionsEditRenderer->Width(),
                DContext->DOptionsEditRenderer->Height()}));

        OptionTop += OptionSkip;
    }

    if (!ValidSoundLevel)
    {
        DButtonStack->MarkInactiveButton(0);
    }
    else
    {
        DButtonStack->ClearInactiveButton(0);
    }
}

bool CSoundOptionsOverlay::ValidateSoundLevel(const std::string &str)
{
    try
    {
        int Level = std::stoi(str);
        if ((0 <= Level) && (100 >= Level))
        {
            return str == std::to_string(Level);
        }
    }
    catch (std::exception &E)
    {
        return false;
    }
    return false;
}

void CSoundOptionsOverlay::SaveSettings()
{
    float FX_Vol = std::stof(DEditText[0]) / 100.0;
    float Music_Vol = std::stof(DEditText[1]) / 100.0;


    // Set in game context
    DContext->SetFXVolume(FX_Vol);
    DContext->SetMusicVolume(Music_Vol);

    // Save to sound settings file
    DContext->SaveVolumeSettings(FX_Vol, Music_Vol);
}

void CSoundOptionsOverlay::SetTextColors()
{
    DGoldColor = DContext->DFonts[DFontID]->FindColor("gold");
    DWhiteColor = DContext->DFonts[DFontID]->FindColor("white");
    DShadowColor = DContext->DFonts[DFontID]->FindColor("black");
}
