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
#ifndef ASSETDECORATDMAP_H
#define ASSETDECORATDMAP_H
#include <array>
#include <list>
#include <map>

#include "PlayerAsset.h"
#include "TerrainMap.h"
#include "VisibilityMap.h"

class CAssetDecoratedMap : public CTerrainMap
{
  public:
    typedef struct
    {
        std::string DType;
        EPlayerNumber DNumber;
        CTilePosition DTilePosition;
    } SAssetInitialization, *SAssetInitializationRef;

    typedef struct
    {
        EPlayerNumber DNumber;
        int DStone;
        int DGold;
        int DLumber;
    } SResourceInitialization, *SResourceInitializationRef;

  protected:
    friend class CLoad;
    std::list<std::shared_ptr<CPlayerAsset> > DAssets; 
    std::list<SAssetInitialization> DAssetInitializationList;
    std::list<SResourceInitialization> DResourceInitializationList;
    std::vector<std::vector<int> > DSearchMap;
    std::vector<std::vector<int> > DLumberAvailable;
    std::vector<std::vector<int> > DStoneAvailable;

    static std::map<std::string, int> DMapNameTranslation;
    static std::vector<std::shared_ptr<CAssetDecoratedMap> > DAllMaps;

    // Scenario scripting related
    bool DHasScenario;
    std::string DScenarioFilename;

  public:
    template<class Archive>
    void serialize(Archive &ar)
    {
        ar(cereal::base_class<CTerrainMap>(this),
           CEREAL_NVP(DAssets),
           CEREAL_NVP(DSearchMap),
           CEREAL_NVP(DLumberAvailable),
           CEREAL_NVP(DStoneAvailable)
        );
    }
    CAssetDecoratedMap();
    CAssetDecoratedMap(const CAssetDecoratedMap &map);
    virtual ~CAssetDecoratedMap();

    CAssetDecoratedMap &operator=(const CAssetDecoratedMap &map);

    static bool LoadMaps(std::shared_ptr<CDataContainer> container);
    static int FindMapIndex(const std::string &name);
    static std::shared_ptr<const CAssetDecoratedMap> GetMap(int index);
    static std::shared_ptr<CAssetDecoratedMap> DuplicateMap(int index);
    std::map<std::shared_ptr<CPlayerAsset>, int> AssetsDirectionTime;
    std::map<std::shared_ptr<CPlayerAsset>, int> AssetsDirection;

    const bool MapHasScenario() const
    {
        return DHasScenario;
    }
    void SetupScenario(bool truth, std::string name)
    {
        DHasScenario = truth;
        DScenarioFilename = name;
    }

    const std::string GetScenarioFilename() const
    {
        return DScenarioFilename;
    }

    int PlayerCount() const
    {
        return DResourceInitializationList.size() - 1;
    };

    bool AddAsset(std::shared_ptr<CPlayerAsset> asset);
    bool RemoveAsset(std::shared_ptr<CPlayerAsset> asset);
    std::weak_ptr<CPlayerAsset> FindNearestAsset(const CPixelPosition &pos,
                                                 EPlayerNumber number,
                                                 EAssetType type);
    bool CanPlaceAsset(const CTilePosition &pos, int size,
                       std::shared_ptr<CPlayerAsset> ignoreasset);
    CTilePosition FindAssetPlacement(std::shared_ptr<CPlayerAsset> placeasset,
                                     std::shared_ptr<CPlayerAsset> fromasset,
                                     const CTilePosition &nexttiletarget);

    void RemoveLumber(const CTilePosition &pos, const CTilePosition &from,
                      int amount);
    void RemoveStone(const CTilePosition &pos, const CTilePosition &from,
                      int amount);

    bool LoadMap(std::shared_ptr<CDataSource> source);

    const std::list<std::shared_ptr<CPlayerAsset> > &Assets() const;
    const std::list<SAssetInitialization> &AssetInitializationList() const;
    const std::list<SResourceInitialization> &ResourceInitializationList() const;

    std::shared_ptr<CAssetDecoratedMap> CreateInitializeMap() const;
    std::shared_ptr<CVisibilityMap> CreateVisibilityMap() const;
    bool UpdateMap(const CVisibilityMap &vismap,
                   const CAssetDecoratedMap &resmap);
    CTilePosition FindNearestReachableTileType(const CTilePosition &pos,
                                               ETileType type);
};

#endif
