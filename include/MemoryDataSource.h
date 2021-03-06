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
#ifndef MEMORYDATASOURCE_H
#define MEMORYDATASOURCE_H

#include <vector>
#include "DataSource.h"

class CMemoryDataSource : public CDataSource
{
  protected:
    std::vector<char> DData;
    int DOffset;

  public:
    CMemoryDataSource(const std::vector<char> &src);
    int Read(void *data, int length);
};

#endif
