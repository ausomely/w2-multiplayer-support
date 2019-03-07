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

#include "ChatOverlay.h"


CChatOverlay::CChatOverlay(std::shared_ptr<CApplicationData> context,
    EChatLocation screen)
{
    DContext = context;
    DScreen = screen;
    DSurfaceColor = 0x00112a;

    SetupFontsAndRenderers();
    SetupSurface();

    // Set the width of the renderer and the coordinates of the text entry field
    DEditRenderer->Width(Width());
    DEditLocation.reset(new SRectangle({0, DEditYoffset, Width(),
        DTextMaxHeight}));

    // Maximum number of characters allowed entry in text field
    DMaxCharacters = 60;

    // Set defaults for edit text related data members
    InitializeChat();
    int Height = DHeight - DTextMaxHeight;
    DContext->DrawInnerBevel(DSurface, DWidth, Height);

}

void CChatOverlay::InitializeChat()
{
    DEditSelected = false;
    DEditSelectedCharacter = -1;
    DEditText.clear();
}

void CChatOverlay::ProcessTextEntryFields(int x, int y, bool clicked)
{
    PixelPositionOnChat(x, y);
    if (DContext->DLeftClick && !DContext->DLeftDown)
    {
        if (DEditLocation->PointInside(x, y))
        {
            DEditSelected = true;
            DEditSelectedCharacter = DEditText.length();
        }
        else
        {
            DEditSelected = false;
            DEditSelectedCharacter = -1;
        }
    }

}

void CChatOverlay::ProcessKeyStrokes()
{
    for (auto Key : DContext->DReleasedKeys)
    {
        if (SGUIKeyType::Escape == Key)
        {
            DEditSelected = -1;
        }
        else if ((SGUIKeyType::LeftAlt == Key) || (SGUIKeyType::RightAlt == Key))
        {
            DEditSelected = -1;
            if (EChatLocation::GameScreen == DScreen)
            {
                HideChat();
            }
        }
        else if (DEditSelected)
        {
            SGUIKeyType TempKey;
            TempKey.DValue = Key;
            if ((SGUIKeyType::Delete == Key) || (SGUIKeyType::BackSpace == Key))
            {
                if (DEditSelectedCharacter)
                {
                    DEditText = DEditText.substr(0, DEditText.size() - 1) +
                        DEditText.substr(DEditSelectedCharacter, DEditText.length() -
                        DEditSelectedCharacter);
                    DEditSelectedCharacter--;
                }
                else if (DEditText.length())
                {
                    DEditText = DEditText.substr(1);
                }
            }
            else if (SGUIKeyType::Return == Key)
            {
                if(!DContext->DActiveGame) {
                    DContext->roomInfo.set_messages(2, DContext->roomInfo.messages()[1]);
                    DContext->roomInfo.set_messages(1, DContext->roomInfo.messages()[0]);
                    DContext->roomInfo.set_messages(0, DContext->DUsername + ": " + DEditText);

                    DContext->ClientPointer->SendRoomInfo(DContext);
                }
                else {
                    if(DContext->Text.size() == 3)
                    {
                        DContext->Text.pop_back();
                    }

                    DContext->Text.insert(DContext->Text.begin(), DEditText);
                }
                DEditSelectedCharacter = 0;
                DEditText.clear();

            }
            else if (SGUIKeyType::LeftArrow == Key)
            {
                if (DEditSelectedCharacter)
                {
                    DEditSelectedCharacter--;
                }
            }
            else if ((SGUIKeyType::RightArrow == Key) &&
                (DMaxCharacters > DEditSelectedCharacter))
            {
                if (DEditSelectedCharacter < DEditText.length())
                {
                    DEditSelectedCharacter++;
                }
            }
            else if ((TempKey.IsViewableInTextField()) &&
                (DMaxCharacters > DEditSelectedCharacter))
            {
                DEditText = DEditText.substr(0, DEditSelectedCharacter) +
                    std::string(1, (char) Key) + DEditText.substr(
                        DEditSelectedCharacter, DEditText.length() -
                        DEditSelectedCharacter);
                DEditSelectedCharacter++;
            }
        }
    }
    DContext->DReleasedKeys.clear();

}


void CChatOverlay::DrawChatText()
{
    ClearChatTextArea();
    for( int Index = 1; Index < DContext->Text.size()+1; ++Index)
    {
        if(DContext->Text.size() == 0)
        {
            break;
        }
        DrawText(DContext->Text[Index-1], DContext->DInnerBevel->Width(),
            DHeight - DTextMaxHeight - Index*DTextMaxHeight, DGoldColor,
            DShadowColor);
    }
}

void CChatOverlay::DrawText(std::string text, int xoffset, int yoffset, int fgcolor,
    int bgcolor)
{
    DContext->DFonts[DFontID]->DrawTextWithShadow(Surface(), xoffset, yoffset,
        fgcolor, bgcolor, 1, text);
}

void CChatOverlay::DrawTextEntryField()
{
    DEditRenderer->Text(DEditText, DMaxCharacters > DEditText.length());
    DEditRenderer->DrawEdit(Surface(), 0, DEditYoffset, DEditSelectedCharacter);
}


void CChatOverlay::SetupFontsAndRenderers()
{
    // Choose the font and size the renderers accordingly
    DFontID = to_underlying(CUnitDescriptionRenderer::EFontSize::Large);
    DEditRenderer = std::make_shared<CEditRenderer>(DContext->DButtonRecolorMap,
        DContext->DInnerBevel, DContext->DFonts[DFontID]);
    SetTextColors();
}

void CChatOverlay::SetupSurface()
{
    DTextMaxHeight = DEditRenderer->Height();
    DWidth = DContext->DViewportSurface->Width();
    DHeight = 4 * DTextMaxHeight;
    DEditYoffset = DHeight - DTextMaxHeight;

    // Figure out X,Y offset by which screen the chat is located
    switch (DScreen)
    {
        case EChatLocation::Lobby:
        {
            DXoffset = DContext->DBorderWidth;
            DYoffset = DContext->DWorkingBufferSurface->Height() -
                DContext->DBorderWidth - DHeight;
            DWidth = static_cast<int>(0.94*DWidth);
            break;
        }
        case EChatLocation::GameScreen:
        {
            DXoffset = DContext->DViewportXOffset;
            DYoffset = DContext->DWorkingBufferSurface->Height() -
                DContext->DViewportYOffset - DHeight;
            break;
        }
    }

    // Configure the surface builder
    auto SurfaceBuilder = new CSurfaceBuilder(DWidth, DHeight, DSurfaceColor);

    // Obtain a surface to draw on
    DSurface = SurfaceBuilder->GetOverlaySurface();
}

// Set exit manager flags. Only called by overlay mode. Never called from manager.
void CChatOverlay::HideChat()
{
    DContext->DeactivateChatOverlay();
}

void CChatOverlay::ClearMouseButtonState()
{
    DContext->ClearMouseButtonState();
}

// Paint a background on the surface
void CChatOverlay::DrawBackground()
{
    int PageWidth = DSurface->Width();
    int PageHeight = DSurface->Height();
    DContext->DrawBackground(DSurface, PageWidth, PageHeight);
    DContext->DrawInnerBevel(DSurface, PageWidth, PageHeight);
}

void CChatOverlay::PixelPositionOnChat(int &CurrentX, int &CurrentY)
{
    // Pixel position is within the overlay window
    if ((DXoffset <= CurrentX) && (DXoffset + DWidth > CurrentX) &&
        (DYoffset <= CurrentY) && (DYoffset + DHeight > CurrentY))
    {
        // Pass in coordinates centered on overlay
        CurrentX = CurrentX - DXoffset;
        CurrentY = CurrentY - DYoffset;
    }
    else
    {
        // Otherwise pass in coordinates outside overlay
        CurrentX = 0;
        CurrentY = 0;
    }

}

void CChatOverlay::SetTextColors()
{
    DGoldColor = DContext->DFonts[DFontID]->FindColor("gold");
    DWhiteColor = DContext->DFonts[DFontID]->FindColor("white");
    DShadowColor = DContext->DFonts[DFontID]->FindColor("black");
}

void CChatOverlay::ClearChatTextArea()
{
    int BevelWidth = DContext->DInnerBevel->Width();
    auto ResourceContext = DSurface->CreateResourceContext();
    ResourceContext->SetSourceRGB(DSurfaceColor);
    ResourceContext->Rectangle(0, 0, DWidth, DEditYoffset);
    ResourceContext->Fill();
    int Height = DHeight - DTextMaxHeight;
    DContext->DrawInnerBevel(DSurface, DWidth, Height);

}
