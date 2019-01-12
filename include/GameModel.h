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
#ifndef GAMEMODEL_H
#define GAMEMODEL_H

#include "RandomNumberGenerator.h"
#include "AssetDecoratedMap.h"
#include "RouterMap.h"
#include "FileDataSource.h"
#include "Rectangle.h"

enum class EEventType{
    None = 0,
    WorkComplete,
    Selection,
    Acknowledge,
    Ready,
    Death,
    Attacked,
    MissleFire,
    MissleHit,
    Harvest,
    MeleeHit,
    PlaceAction,
    ButtonTick,
    Max
};

using SGameEvent = struct GAMEEVENT_TAG{
    EEventType DType;
    std::shared_ptr< CPlayerAsset > DAsset;
};

class CPlayerData{
    protected:
        bool DIsAI;
        EPlayerNumber DNumber;
        EPlayerColor DColor;
        std::shared_ptr< CVisibilityMap > DVisibilityMap;
        std::shared_ptr< CAssetDecoratedMap > DActualMap;
        std::shared_ptr< CAssetDecoratedMap > DPlayerMap;
        std::shared_ptr< std::unordered_map< std::string, std::shared_ptr< CPlayerAssetType > > > DAssetTypes;
        std::list< std::weak_ptr< CPlayerAsset > > DAssets;
        std::vector< bool > DUpgrades;
        std::vector< SGameEvent > DGameEvents;
        int DGold;
        int DLumber;
        int DGameCycle;
        
    public:
        CPlayerData(std::shared_ptr< CAssetDecoratedMap > map, EPlayerNumber number, EPlayerColor color);
        
        int GameCycle() const{
            return DGameCycle;   
        };
        
        void IncrementCycle(){
            DGameCycle++;
        };
        
        EPlayerNumber Number() const{
            return DNumber;
        };
        
        EPlayerColor Color() const{
            return DColor;
        };
        
        bool IsAI() const{
            return DIsAI;
        };
        
        bool IsAI(bool isai){
            return DIsAI = isai;
        };
        
        bool IsAlive() const{
            return DAssets.size();
        };
        int Gold() const{
            return DGold;   
        };
        int Lumber() const{
            return DLumber;  
        };
        int IncrementGold(int gold){
            DGold += gold;
            return DGold;   
        };
        int DecrementGold(int gold){
            DGold -= gold;
            return DGold;   
        };
        int IncrementLumber(int lumber){
            DLumber += lumber;
            return DLumber;  
        };
        int DecrementLumber(int lumber){
            DLumber -= lumber;
            return DLumber;  
        };
        int FoodConsumption() const;
        int FoodProduction() const;
        
        std::shared_ptr< CVisibilityMap > VisibilityMap() const{
            return DVisibilityMap;  
        };
        std::shared_ptr< CAssetDecoratedMap > PlayerMap() const{
            return DPlayerMap;   
        };
        std::list< std::weak_ptr< CPlayerAsset > > Assets() const{
            return DAssets;  
        };
        std::shared_ptr< std::unordered_map< std::string, std::shared_ptr< CPlayerAssetType > > > &AssetTypes(){
            return DAssetTypes;
        };
        std::shared_ptr< CPlayerAsset > CreateMarker(const CPixelPosition &pos, bool addtomap);
        std::shared_ptr< CPlayerAsset > CreateAsset(const std::string &assettypename);
        void DeleteAsset(std::shared_ptr< CPlayerAsset > asset);
        bool AssetRequirementsMet(const std::string &assettypename);
        void UpdateVisibility();
        std::list< std::weak_ptr< CPlayerAsset > > SelectAssets(const SRectangle &selectarea, EAssetType assettype, bool selectidentical = false);
        std::weak_ptr< CPlayerAsset > SelectAsset(const CPixelPosition &pos, EAssetType assettype);
        std::weak_ptr< CPlayerAsset > FindNearestOwnedAsset(const CPixelPosition &pos, const std::vector< EAssetType > assettypes);
        std::shared_ptr< CPlayerAsset > FindNearestAsset(const CPixelPosition &pos, EAssetType assettype);
        std::weak_ptr< CPlayerAsset > FindNearestEnemy(const CPixelPosition &pos, int range);
        CTilePosition FindBestAssetPlacement(const CTilePosition &pos, std::shared_ptr< CPlayerAsset > builder, EAssetType assettype, int buffer);
        std::list< std::weak_ptr< CPlayerAsset > > IdleAssets() const;
        int PlayerAssetCount(EAssetType type);
        int FoundAssetCount(EAssetType type);
        void AddUpgrade(const std::string &upgradename);
        bool HasUpgrade(EAssetCapabilityType upgrade) const{
            if((0 > to_underlying(upgrade))||(DUpgrades.size() <= static_cast<decltype(DUpgrades.size())>(upgrade))){
                return false;   
            }
            return DUpgrades[static_cast<decltype(DUpgrades.size())>(upgrade)];
        };
        
        const std::vector< SGameEvent > &GameEvents() const{
            return DGameEvents;
        };
        void ClearGameEvents(){
            DGameEvents.clear();  
        };
        void AddGameEvent(const SGameEvent &event){
            DGameEvents.push_back(event);  
        };
        void AppendGameEvents(const std::vector< SGameEvent > &events){
            DGameEvents.insert(DGameEvents.end(), events.begin(), events.end());
        };
        
};

class CGameModel{
    protected:
        CRandomNumberGenerator DRandomNumberGenerator;
        std::shared_ptr< CAssetDecoratedMap > DActualMap;
        std::vector< std::vector< std::shared_ptr< CPlayerAsset > > > DAssetOccupancyMap;
        std::vector< std::vector< bool > > DDiagonalOccupancyMap;
        CRouterMap DRouterMap;
        std::array< std::shared_ptr< CPlayerData >, to_underlying(EPlayerNumber::Max)> DPlayers;
        int DGameCycle;
        int DHarvestTime;
        int DHarvestSteps;
        int DMineTime;
        int DMineSteps;
        int DConveyTime;
        int DConveySteps;
        int DDeathTime;
        int DDeathSteps;
        int DDecayTime;
        int DDecaySteps;
        int DLumberPerHarvest;
        int DGoldPerMining;

    public:
        CGameModel(int mapindex, uint64_t seed, const std::array< EPlayerColor, to_underlying(EPlayerNumber::Max)> &newcolors); 
        
        int GameCycle() const{
            return DGameCycle;
        };
        
        bool ValidAsset(std::shared_ptr< CPlayerAsset > asset);
        std::shared_ptr< CAssetDecoratedMap > Map() const{
            return DActualMap;  
        };
        std::shared_ptr< CPlayerData > Player(EPlayerNumber number) const;
        void Timestep();
        void ClearGameEvents();
};

#endif

