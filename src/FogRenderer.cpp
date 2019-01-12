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
#include "FogRenderer.h"
#include "Debug.h"
#include "FileDataSink.h"
#include "GraphicFactory.h"
#include <sstream>
#include <iomanip>

CFogRenderer::CFogRenderer(std::shared_ptr< CGraphicTileset > tileset, std::shared_ptr< CVisibilityMap > map){
    DTileset = tileset;
    DMap = map;
    DPartialIndex = DTileset->FindTile("partial");
    for(int Index = 0; Index < 0x100; Index++){
        std::ostringstream TempStringStream;
        TempStringStream<<std::setfill('0')<<std::hex<<std::uppercase<<std::setw(2)<<Index;
        
        DFogIndices.push_back(DTileset->FindTile(std::string("pf-") + TempStringStream.str()));
        DBlackIndices.push_back(DTileset->FindTile(std::string("pb-") + TempStringStream.str()));
    }
    DSeenIndex = DFogIndices[0x00];
    DNoneIndex = DBlackIndices[0x00];
}


void CFogRenderer::DrawMap(std::shared_ptr<CGraphicSurface> surface, const SRectangle &rect){
    int TileWidth, TileHeight;
    
    TileWidth = DTileset->TileWidth();
    TileHeight = DTileset->TileHeight();

    for(int YIndex = rect.DYPosition / TileHeight, YPos = -(rect.DYPosition % TileHeight); YPos < rect.DHeight; YIndex++, YPos += TileHeight){
        for(int XIndex = rect.DXPosition / TileWidth, XPos = -(rect.DXPosition % TileWidth); XPos < rect.DWidth; XIndex++, XPos += TileWidth){
            CVisibilityMap::ETileVisibility TileType = DMap->TileType(XIndex, YIndex);
            
            if(CVisibilityMap::ETileVisibility::None == TileType){
                DTileset->DrawTile(surface, XPos, YPos, DNoneIndex);
                continue;
            }
            else if(CVisibilityMap::ETileVisibility::Visible == TileType){
                continue;
            }
            if((CVisibilityMap::ETileVisibility::Seen == TileType)||(CVisibilityMap::ETileVisibility::SeenPartial == TileType)){
                DTileset->DrawTile(surface, XPos, YPos, DSeenIndex);
            }
            if((CVisibilityMap::ETileVisibility::PartialPartial == TileType)||(CVisibilityMap::ETileVisibility::Partial == TileType)){
                int VisibilityIndex = 0, VisibilityMask = 0x1;
                
                for(int YOff = -1; YOff < 2; YOff++){
                    for(int XOff = -1; XOff < 2; XOff++){
                        if(YOff || XOff){
                            CVisibilityMap::ETileVisibility VisTile = DMap->TileType(XIndex + XOff, YIndex + YOff);
                            
                            if(CVisibilityMap::ETileVisibility::Visible == VisTile){
                                VisibilityIndex |= VisibilityMask;   
                            }
                            VisibilityMask <<= 1;
                        }
                    }
                }
                DTileset->DrawTile(surface, XPos, YPos, DFogIndices[VisibilityIndex]);
            }
            
            if((CVisibilityMap::ETileVisibility::PartialPartial == TileType)||(CVisibilityMap::ETileVisibility::SeenPartial == TileType)){
                int VisibilityIndex = 0, VisibilityMask = 0x1;
                
                for(int YOff = -1; YOff < 2; YOff++){
                    for(int XOff = -1; XOff < 2; XOff++){
                        if(YOff || XOff){
                            CVisibilityMap::ETileVisibility VisTile = DMap->TileType(XIndex + XOff, YIndex + YOff);
                            
                            if((CVisibilityMap::ETileVisibility::Visible == VisTile)||(CVisibilityMap::ETileVisibility::Partial == VisTile)||(CVisibilityMap::ETileVisibility::Seen == VisTile)){
                                VisibilityIndex |= VisibilityMask;   
                            }
                            VisibilityMask <<= 1;
                        }
                    }
                }
                DTileset->DrawTile(surface, XPos, YPos, DBlackIndices[VisibilityIndex]);
            }
        }
    }
}

void CFogRenderer::DrawMiniMap(std::shared_ptr<CGraphicSurface> surface){
    auto ResourceContext = surface->CreateResourceContext();
    
    ResourceContext->SetLineWidth(1);
    ResourceContext->SetLineCap(CGraphicResourceContext::ELineCap::Square);
    for(int YPos = 0; YPos < DMap->Height(); YPos++){
        int XPos = 0;
        
        while(XPos < DMap->Width()){
            auto TileType = DMap->TileType(XPos, YPos);
            int XAnchor = XPos;
            while((XPos < DMap->Width())&&(DMap->TileType(XPos, YPos) == TileType)){
                XPos++;   
            }
            if(CVisibilityMap::ETileVisibility::Visible != TileType){
                uint32_t ColorRGBA = 0x00000000;
                switch(TileType){
                    case CVisibilityMap::ETileVisibility::None:           ColorRGBA = 0xFF000000;
                                                                            break;
                    case CVisibilityMap::ETileVisibility::Seen:
                    case CVisibilityMap::ETileVisibility::SeenPartial:    ColorRGBA = 0xA8000000;
                                                                            break;
                    default:                                                ColorRGBA = 0x54000000;
                                                                            break;                                   
                }
                ResourceContext->SetSourceRGBA(ColorRGBA);
                ResourceContext->MoveTo(XAnchor,YPos);
                ResourceContext->LineTo(XPos - 1,YPos);
                ResourceContext->Stroke();   
            }
        }
    }
}

