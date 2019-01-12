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
#include "CommentSkipLineDataSource.h"

CCommentSkipLineDataSource::CCommentSkipLineDataSource(std::shared_ptr< CDataSource > source, char commentchar) : CLineDataSource(source){
    DCommentCharacter = commentchar;
}

bool CCommentSkipLineDataSource::Read(std::string &line){
    std::string TempLine;
    
    while(true){
        if(!CLineDataSource::Read(TempLine)){
            return false;
        }
        if(!TempLine.length() || TempLine[0] != DCommentCharacter){
            line = TempLine;
            break;
        }
        if((2 <= TempLine.length())&&(TempLine[1] == DCommentCharacter)){
            line = TempLine.substr(1);
            break;
        }
    }
    return true;
}

