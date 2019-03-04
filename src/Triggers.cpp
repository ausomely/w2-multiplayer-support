#include "Triggers.h"

static std::chrono::time_point<std::chrono::steady_clock> DStartTime; 

CTriggerLocation::CTriggerLocation(std::shared_ptr<CContextRetrieval> contextRetrieval,
            int x_begin, int y_begin, int x_end, int y_end, std::vector<std::shared_ptr<CEffects> > effects)
{
    DContextRetrieval = contextRetrieval;
    DX_begin = x_begin;
    DY_begin = y_begin;
    DX_end = x_end;
    DY_end = y_end;
    DEffects = effects;

}

// Returns true if atleast one unit is in the area
bool CTriggerLocation::Check(EPlayerNumber Player)
{
    auto AssetList = DContextRetrieval->GetAssets(Player);

    for(auto asset : AssetList){
         auto ass = asset.lock();
        auto posX = ass->TilePositionX();
        auto posY = ass->TilePositionY();
        if(posX >= DX_begin && posX <= DX_end &&
            posY >= DY_begin && posY <= DY_end)
            {
                return true;
            }
    }

    return false;
}

void CTriggerLocation::InitiateEffects(EPlayerNumber Player)
{
    for(auto &effect : DEffects){
        if(!effect->IsHealthEffect){
            effect->DoEffect(Player);
        }else{
            effect->DoEffectArea(Player,DX_begin,DY_begin,DX_end,DY_end);
        }
    }
}


/* WOE YE WHO TREAD HERE!!!
CTriggerTime::CTriggerTime(std::shared_ptr<CContextRetrieval> contextRetrieval,
        int goalTime, std::vector<std::shared_ptr<CEffects> > effects)
{
    DStartTime->
}
*/














CTriggerResource::CTriggerResource(std::shared_ptr<CContextRetrieval> contextRetrieval,
            int goalGold, int goalLumber, int goalStone,
            std::vector<std::shared_ptr<CEffects> > effects)
{

    DContextRetrieval = contextRetrieval;
    DEffects = effects;

    DGoalGold = goalGold;
    DGoalLumber = goalLumber;
    DGoalStone = goalStone;

}

bool CTriggerResource::Check(EPlayerNumber Player)
{
    auto curPlayer = DContextRetrieval->GetPlayer(Player);

    if(curPlayer->Gold() >= DGoalGold && curPlayer->Lumber() >= DGoalLumber
       /*&& curPlayer->Stone() >= DGoalStone*/){
        return true;
    }

    return false;
}

void CTriggerResource::InitiateEffects(EPlayerNumber Player)
{
    for(auto &effect : DEffects){
        effect->DoEffect(Player);
    }
}

CTriggerAssetCreation::CTriggerAssetCreation(std::shared_ptr<CContextRetrieval> contextRetrieval,
     std::vector<std::shared_ptr<CEffects> > effects)
{
    DEffects = effects;
    DContextRetrieval = contextRetrieval;

    for(int i = 0; i < to_underlying(EPlayerNumber::Max); i++)
    {
        PrevAssetAmount.at(i) = 0;
    }
    

}

bool CTriggerAssetCreation::Check(EPlayerNumber Player)
{
    auto assetList = DContextRetrieval->GetAssets(Player);

    int &tempSize = PrevAssetAmount.at(to_underlying(Player));

    if(assetList.size() > tempSize){
        tempSize = assetList.size();
        return true;
    }
    tempSize = assetList.size();
    return false;

}

void CTriggerAssetCreation::InitiateEffects(EPlayerNumber Player)
{
    for(auto &effect : DEffects){
        effect->DoEffect(Player);
    }
}


CTriggerAssetDeletion::CTriggerAssetDeletion(std::shared_ptr<CContextRetrieval> contextRetrieval,
     std::vector<std::shared_ptr<CEffects> > effects)
{
    DEffects = effects;
    DContextRetrieval = contextRetrieval;

    for(int i = 0; i < to_underlying(EPlayerNumber::Max); i++)
    {
        PrevAssetAmount.at(i) = 0;
    }
    

}

bool CTriggerAssetDeletion::Check(EPlayerNumber Player)
{
    auto assetList = DContextRetrieval->GetAssets(Player);

    int &tempSize = PrevAssetAmount.at(to_underlying(Player));

    if(assetList.size() < tempSize){
        tempSize = assetList.size();
        return true;
    }
    tempSize = assetList.size();
    return false;

}

void CTriggerAssetDeletion::InitiateEffects(EPlayerNumber Player)
{
    for(auto &effect : DEffects){
        effect->DoEffect(Player);
    }
}