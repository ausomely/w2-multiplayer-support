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
#include "FileDataContainer.h"
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>
#include "FileDataSink.h"
#include "FileDataSource.h"
#include "Path.h"

CDirectoryDataContainerIterator::CDirectoryDataContainerIterator(
    const std::string &path, const SPrivateConstructorKey &key)
        : CDataContainerIterator()
{
    DDirectory = opendir(path.c_str());
    if (nullptr != DDirectory)
    {
        if (0 != readdir_r(DDirectory, &DEntry, &DEntryResult))
        {
            DEntry.d_name[0] = '\0';
            DEntryResult = nullptr;
        }
    }
}

CDirectoryDataContainerIterator::~CDirectoryDataContainerIterator()
{
    if (nullptr != DDirectory)
    {
        closedir(DDirectory);
    }
}

std::string CDirectoryDataContainerIterator::Name()
{
    return DEntry.d_name;
}

bool CDirectoryDataContainerIterator::IsContainer()
{
    return DT_DIR == DEntry.d_type;
}

bool CDirectoryDataContainerIterator::IsValid()
{
    return (nullptr != DEntryResult) || ('\0' != DEntry.d_name[0]);
}

void CDirectoryDataContainerIterator::Next()
{
    if (nullptr != DDirectory)
    {
        if (nullptr == DEntryResult)
        {
            DEntry.d_name[0] = '\0';
        }
        else
        {
            if (0 != readdir_r(DDirectory, &DEntry, &DEntryResult))
            {
                DEntry.d_name[0] = '\0';
                DEntryResult = nullptr;
            }
            if (nullptr == DEntryResult)
            {
                DEntry.d_name[0] = '\0';
            }
        }
    }
}

CDirectoryDataContainer::CDirectoryDataContainer(const std::string &path)
        : CDataContainer()
{
    DFullPath = CPath::CurrentPath().Simplify(path).ToString();
}

CDirectoryDataContainer::~CDirectoryDataContainer() {}

std::shared_ptr<CDataContainerIterator> CDirectoryDataContainer::First()
{
    if (DFullPath.length())
    {
        return std::make_shared<CDirectoryDataContainerIterator>(
            DFullPath,
            CDirectoryDataContainerIterator::SPrivateConstructorKey());
    }
    return nullptr;
}

std::shared_ptr<CDataSource> CDirectoryDataContainer::DataSource(
    const std::string &name)
{
    std::string FileName = CPath(DFullPath).Simplify(CPath(name)).ToString();

    if (FileName.length())
    {
        return std::make_shared<CFileDataSource>(FileName);
    }
    return nullptr;
}

std::shared_ptr<CDataSink> CDirectoryDataContainer::DataSink(
    const std::string &name)
{
    std::string FileName = CPath(DFullPath).Simplify(CPath(name)).ToString();

    if (FileName.length())
    {
        return std::make_shared<CFileDataSink>(FileName);
    }
    return nullptr;
}

std::shared_ptr<CDataContainer> CDirectoryDataContainer::Container()
{
    std::string ContainerName = CPath(DFullPath).Containing().ToString();

    if (ContainerName.length())
    {
        return std::make_shared<CDirectoryDataContainer>(ContainerName);
    }
    return nullptr;
}

std::shared_ptr<CDataContainer> CDirectoryDataContainer::DataContainer(
    const std::string &name)
{
    std::string ContainerName =
        CPath(DFullPath).Simplify(CPath(name)).ToString();

    if (ContainerName.length())
    {
        return std::make_shared<CDirectoryDataContainer>(ContainerName);
    }
    return nullptr;
}
