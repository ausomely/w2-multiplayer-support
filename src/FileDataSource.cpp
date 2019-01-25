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
#include "FileDataSource.h"
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>
#include "FileDataContainer.h"
#include "IOFactory.h"
#include "Path.h"

CFileDataSource::CFileDataSource(const std::string &filename, int fd)
        : CDataSource()
{
    DFullPath = CPath::CurrentPath().Simplify(filename).ToString();

    DFileHandle = fd;
    if (DFullPath.length() && (0 > fd))
    {
        DFileHandle = open(DFullPath.c_str(), O_RDONLY);
        DCloseFile = true;
    }
}

CFileDataSource::~CFileDataSource()
{
    if (DCloseFile && (0 <= DFileHandle))
    {
        close(DFileHandle);
    }
}

int CFileDataSource::Read(void *data, int length)
{
    if (0 <= DFileHandle)
    {
        int BytesRead = read(DFileHandle, data, length);

        if (0 < BytesRead)
        {
            return BytesRead;
        }
    }
    return -1;
}

std::shared_ptr<CDataContainer> CFileDataSource::Container()
{
    std::string ContainerName = CPath(DFullPath).Containing().ToString();

    if (ContainerName.length())
    {
        return std::make_shared<CDirectoryDataContainer>(ContainerName);
    }
    return nullptr;
}

std::shared_ptr<CIOChannel> CFileDataSource::IOChannel()
{
    return CIOFactory::NewFileDescriptorIOChannel(DFileHandle,
                                                  shared_from_this());
};
