/*
    Copyright (c) 2015, Christopher Nitta
    All rights reserved.

    All source material (source code, images, sounds, etc.) have been provided to
    University of California, Davis students of course ECS 160 for educational
    purposes. It may not be distributed beyond those enrolled in the course without
    prior permission from the copyright holder.

    All sound files, sound fonts, midi files, and images that have been included 
    that were extracted from original Warcraft II by Blizzard Entertainment 
    were found freely available via internet sources and have been labeld as 
    abandonware. They have been included in this distribution for educational 
    purposes only and this copyright notice does not attempt to claim any 
    ownership of this material.
*/
#include "EditRenderer.h"

CEditRenderer::CEditRenderer(std::shared_ptr< CGraphicRecolorMap > colors, std::shared_ptr< CBevel > innerbevel, std::shared_ptr< CFontTileset > font){
    DColorMap = colors;
    DInnerBevel = innerbevel;
    DFont = font;
    DBackgroundColor = EPlayerColor::None;

    DLightIndices.resize(to_underlying(EPlayerColor::Max));
    DDarkIndices.resize(to_underlying(EPlayerColor::Max));             
    DDarkIndices[to_underlying(EPlayerColor::None)] = DDarkIndices[to_underlying(EPlayerColor::Blue)] = DColorMap->FindColor("blue-dark");
    DDarkIndices[to_underlying(EPlayerColor::Red)] = DColorMap->FindColor("red-dark");
    DDarkIndices[to_underlying(EPlayerColor::Green)] = DColorMap->FindColor("green-dark");
    DDarkIndices[to_underlying(EPlayerColor::Purple)] = DColorMap->FindColor("purple-dark");
    DDarkIndices[to_underlying(EPlayerColor::Orange)] = DColorMap->FindColor("orange-dark");
    DDarkIndices[to_underlying(EPlayerColor::Yellow)] = DColorMap->FindColor("yellow-dark");
    DDarkIndices[to_underlying(EPlayerColor::Black)] = DColorMap->FindColor("black-dark");
    DDarkIndices[to_underlying(EPlayerColor::White)] = DColorMap->FindColor("white-dark");

    DLightIndices[to_underlying(EPlayerColor::None)] = DLightIndices[to_underlying(EPlayerColor::Blue)] = DColorMap->FindColor("blue-light");
    DLightIndices[to_underlying(EPlayerColor::Red)] = DColorMap->FindColor("red-light");
    DLightIndices[to_underlying(EPlayerColor::Green)] = DColorMap->FindColor("green-light");
    DLightIndices[to_underlying(EPlayerColor::Purple)] = DColorMap->FindColor("purple-light");
    DLightIndices[to_underlying(EPlayerColor::Orange)] = DColorMap->FindColor("orange-light");
    DLightIndices[to_underlying(EPlayerColor::Yellow)] = DColorMap->FindColor("yellow-light");
    DLightIndices[to_underlying(EPlayerColor::Black)] = DColorMap->FindColor("black-light");
    DLightIndices[to_underlying(EPlayerColor::White)] = DColorMap->FindColor("white-light");
    
    DWhiteIndex = DFont->FindColor("white");
    DGoldIndex = DFont->FindColor("gold");
    DRedIndex = DFont->FindColor("red");
    DBlackIndex = DFont->FindColor("black");
    
    DWidth = 0;
    DHeight = 0;
    MinimumCharacters(16);
}

std::string CEditRenderer::Text(const std::string &text, bool valid){
    DText = text;
    DTextValid = valid;
    return DText;
}

int CEditRenderer::MinimumCharacters(int minchar){
    if(0 < minchar){
        int TextWidth, TextHeight;
        std::string TempText;
        
        for(int Index = 0; Index < minchar; Index++){
            TempText += "X";
        }
        TempText += "|";
        DFont->MeasureText(TempText, TextWidth, TextHeight);    
        DMinimumCharacters = minchar;
    
        DMinimumWidth = TextWidth + DInnerBevel->Width() * 2;
        DMinimumHeight = TextHeight + DInnerBevel->Width() * 2;
        if(DWidth < DMinimumWidth){
            DWidth = DMinimumWidth;   
        }
        if(DHeight < DMinimumHeight){
            DHeight = DMinimumHeight;   
        }
    }
    return DMinimumCharacters;
}

int CEditRenderer::Width(int width){
    if(DMinimumWidth <= width){
        DWidth = width;   
    }
    return DWidth;
}

int CEditRenderer::Height(int height){
    if(DMinimumHeight <= height){
        DHeight = height;   
    }
    return DHeight;
}

void CEditRenderer::DrawEdit(std::shared_ptr< CGraphicSurface > surface, int x, int y, int cursorpos){
    auto ResourceContext = surface->CreateResourceContext();
    int BevelWidth = DInnerBevel->Width();
    int TextColorIndex;
    int TextWidth, TextHeight;
    std::string RenderText;
    
    ResourceContext->SetSourceRGBA( DColorMap->ColorValue(0 <= cursorpos ? DLightIndices[to_underlying(DBackgroundColor)] : DDarkIndices[to_underlying(DBackgroundColor)], 0) );
    ResourceContext->Rectangle(x, y, DWidth, DHeight);
    ResourceContext->Fill();
    
    //DColorMap->DrawTileRectangle(drawable, x, y, DWidth, DHeight, 0 <= cursorpos ? DLightIndices[DBackgroundColor] : DDarkIndices[DBackgroundColor]);
    if(0 <= cursorpos){
        if(DText.length()){
            bool RemoveCharacters = false;
            RenderText = DText.substr(0,cursorpos);
            RenderText += "|";
            do{
                DFont->MeasureText(RenderText, TextWidth, TextHeight); 
                TextWidth += BevelWidth * 2;
                if(TextWidth > DWidth){
                    RenderText = RenderText.substr(1, RenderText.length()-1);
                    RemoveCharacters = true;
                }
            }while(DWidth < TextWidth);
            if(!RemoveCharacters){
                if(cursorpos < DText.length()){
                    RenderText += DText.substr(cursorpos);    
                }
            }
        }
        else{
            RenderText = "|";   
        }
    }
    else{
        RenderText = DText;
    }
    do{
        DFont->MeasureText(RenderText, TextWidth, TextHeight); 
        TextWidth += BevelWidth * 2;
        if(TextWidth > DWidth){
            RenderText = RenderText.substr(0, RenderText.length()-1);
        }
    }while(DWidth < TextWidth);
    if(DTextValid){
        TextColorIndex = 0 <= cursorpos ? DWhiteIndex : DGoldIndex;
    }
    else{
        TextColorIndex = DRedIndex;   
    }
    DFont->DrawTextWithShadow(surface, x + BevelWidth, y + BevelWidth, TextColorIndex, DBlackIndex, 1, RenderText);
    DInnerBevel->DrawBevel(surface, x + BevelWidth, y + BevelWidth, DWidth - BevelWidth * 2, DHeight - BevelWidth * 2);
}

