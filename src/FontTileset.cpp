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
#include "FontTileset.h"
#include "LineDataSource.h"
#include "Tokenizer.h"
#include "Debug.h"
#include <stdio.h>
#include <stdlib.h>


CFontTileset::CFontTileset(){
    
}

CFontTileset::~CFontTileset(){

}

uint32_t CFontTileset::TopBottomSearch(void *data, uint32_t pixel){
    CFontTileset *Font = static_cast<CFontTileset *>(data);
    int Row = Font->DSearchCall / Font->DTileWidth;
    
    Font->DSearchCall++;
    if(pixel & 0xFF000000){
        if(Row < Font->DTopOpaque){
            Font->DTopOpaque = Row;
        }
        Font->DBottomOpaque = Row;
    }
    
    return pixel;
}

bool CFontTileset::LoadFont(std::shared_ptr< CGraphicRecolorMap > colormap, std::shared_ptr< CDataSource > source){
    CLineDataSource LineSource(source);
    std::string TempString;
    bool ReturnStatus = false;
    std::vector< int > BottomOccurence;
    int BestLine = 0;
    
    if(!CGraphicMulticolorTileset::LoadTileset(colormap, source)){
        return false;    
    }    
        
    DCharacterWidths.resize(DTileCount);
    DDeltaWidths.resize(DTileCount);
    DCharacterTops.resize(DTileCount);
    DCharacterBottoms.resize(DTileCount);
    DCharacterBaseline = DTileHeight;
    try{
        for(int Index = 0; Index < DTileCount; Index++){
            if(!LineSource.Read(TempString)){
                goto LoadFontExit;
            }
            DCharacterWidths[Index] = std::stoi(TempString);
        }
        for(int FromIndex = 0; FromIndex < DTileCount; FromIndex++){
            std::vector< std::string > Values;
            DDeltaWidths[FromIndex].resize(DTileCount);
            if(!LineSource.Read(TempString)){
                goto LoadFontExit;
            }
            CTokenizer::Tokenize(Values, TempString);
            if(Values.size() != DTileCount){
                goto LoadFontExit;
            }
            for(int ToIndex = 0; ToIndex < DTileCount; ToIndex++){
                DDeltaWidths[FromIndex][ToIndex] = std::stoi(Values[ToIndex]);
            }
        }
        ReturnStatus = true;
    }
    catch(std::exception &E){
        PrintError("%s\n",E.what());
    }
    
    BottomOccurence.resize(DTileHeight + 1);
    for(int Index = 0; Index < BottomOccurence.size(); Index++){
        BottomOccurence[Index] = 0;
    }
    for(int Index = 0; Index < DTileCount; Index++){
        DTopOpaque = DTileHeight;
        DBottomOpaque = 0;
        DSearchCall = 0;
        DSurfaceTileset->Transform(DSurfaceTileset, 0, Index * DTileHeight, DTileWidth, DTileHeight, 0, Index * DTileHeight, this, TopBottomSearch); 
        DCharacterTops[Index] = DTopOpaque;
        DCharacterBottoms[Index] = DBottomOpaque;
        BottomOccurence[DBottomOpaque]++;
    }
    for(int Index = 1; Index < BottomOccurence.size(); Index++){
        if(BottomOccurence[BestLine] < BottomOccurence[Index]){
            BestLine = Index;
        }
    }
    DCharacterBaseline = BestLine;

    
LoadFontExit:
    return ReturnStatus;
}

void CFontTileset::DrawText(std::shared_ptr<CGraphicSurface> surface, int xpos, int ypos, const std::string &str){
    int LastChar, NextChar;
    for(int Index = 0; Index < str.length(); Index++){
        NextChar = str[Index] - ' ';
        
        if(Index){
            xpos += DCharacterWidths[LastChar] + DDeltaWidths[LastChar][NextChar]; 
        }
        CGraphicTileset::DrawTile(surface, xpos, ypos, NextChar);
        LastChar = NextChar;
    }
}

void CFontTileset::DrawTextColor(std::shared_ptr<CGraphicSurface> surface, int xpos, int ypos, int colorindex, const std::string &str){
    int LastChar, NextChar;
    
    if((0 > colorindex)||(colorindex >= DColoredTilesets.size())){
        return;    
    }
    for(int Index = 0; Index < str.length(); Index++){
        NextChar = str[Index] - ' ';
        
        if(Index){
            xpos += DCharacterWidths[LastChar] + DDeltaWidths[LastChar][NextChar]; 
        }
        
        CGraphicMulticolorTileset::DrawTile(surface, xpos, ypos, NextChar, colorindex);
        
        LastChar = NextChar;
    }
}

void CFontTileset::DrawTextWithShadow(std::shared_ptr<CGraphicSurface> surface, int xpos, int ypos, int color, int shadowcol, int shadowwidth, const std::string &str){
    if((0 > color)||(color >= DColoredTilesets.size())){
        PrintDebug(DEBUG_HIGH,"  Invalid color %d of %zd\n",color, DColoredTilesets.size());
        return;    
    }
    if((0 > shadowcol)||(shadowcol >= DColoredTilesets.size())){
        PrintDebug(DEBUG_HIGH,"  Invalid shadcolor %d of %zd\n",shadowcol, DColoredTilesets.size());
        return;    
    }
    DrawTextColor(surface, xpos + shadowwidth, ypos + shadowwidth, shadowcol, str);
    DrawTextColor(surface, xpos, ypos, color, str);
}

void CFontTileset::MeasureText(const std::string &str, int &width, int &height){
    int TempTop, TempBottom;
    MeasureTextDetailed(str, width, height, TempTop, TempBottom);
}

void CFontTileset::MeasureTextDetailed(const std::string &str, int &width, int &height, int &top, int &bottom){
    int LastChar, NextChar;
    width = 0;
    top = DTileHeight;
    bottom = 0;
    for(int Index = 0; Index < str.length(); Index++){
        NextChar = str[Index] - ' ';
        
        if(Index){
            width += DDeltaWidths[LastChar][NextChar]; 
        }
        width += DCharacterWidths[NextChar]; 
        if(DCharacterTops[NextChar] < top){
            top = DCharacterTops[NextChar];   
        }
        if(DCharacterBottoms[NextChar] > bottom){
            bottom = DCharacterBottoms[NextChar];   
        }
        LastChar = NextChar;
    }
    height = DTileHeight;
}

