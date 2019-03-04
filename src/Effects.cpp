#include "Effects.h"
#include "ContextRetrieval.h"


CEffectsAddAsset::CEffectsAddAsset(std::string assetName, int amount, 
            std::shared_ptr<CContextRetrieval> contextRetrieval)
{
    DAssetName = assetName;
    DAmount = amount;
    DContextRetrieval = contextRetrieval;
    IsHealthEffect = 0;
}

//Might want to give this coordinates and set coordinates of assets when they are created
void CEffectsAddAsset::DoEffect(EPlayerNumber Player)
{
    auto CurPlayer = DContextRetrieval->GetPlayer(Player);

    for ( auto Asset : DContextRetrieval->GetAssets(Player))
    {
        if (Asset.lock()->Name() == "TownHall")
        {
            for(int i = 0; i < DAmount; i++)
            {
                auto PlaceAsset = CurPlayer->CreateAsset(DAssetName);
                auto TilePos = CurPlayer->PlayerMap()->FindAssetPlacement(
                PlaceAsset, Asset.lock(),
                  CTilePosition(CurPlayer->PlayerMap()->Width() - 1,
                                CurPlayer->PlayerMap()->Height() - 1));
                PlaceAsset->TilePosition(TilePos);
            }
            break;
        }
    }


}

CEffectsRemoveAsset::CEffectsRemoveAsset(std::string assetName, int amount, 
            std::shared_ptr<CContextRetrieval> contextRetrieval)
{
    DAssetName = assetName;
    DAmount = amount;
    DContextRetrieval = contextRetrieval;
    IsHealthEffect = 0;
}

void CEffectsRemoveAsset::DoEffect(EPlayerNumber Player)
{
    auto CurPlayer = DContextRetrieval->GetPlayer(Player);
    auto CurPlayerAsset = DContextRetrieval->GetAssets(Player);
    
    int delCount = 0;
    //Probably going to break when you delete stuff. But who knows
   for(auto asset : CurPlayerAsset)
   {
       if(delCount < DAmount && asset.lock()->Name() == DAssetName )
       {
        CurPlayer->DeleteAsset(asset.lock());

        delCount++;
       }
   }

}

CEffectsModifyHealth::CEffectsModifyHealth(int amount,
    std::shared_ptr<CContextRetrieval> contextRetrieval)
{
    DAmount = amount;
    DContextRetrieval = contextRetrieval;
    IsHealthEffect = 1;
}

//Do this for everything except the area trigger
void CEffectsModifyHealth::DoEffect(EPlayerNumber Player)
{
    auto AssetList = DContextRetrieval->GetAssets(Player);
    auto curPlayer = DContextRetrieval->GetPlayer(Player);

    for(auto asset : AssetList){
        asset.lock()->IncrementHitPoints(DAmount);
        if(asset.lock()->HitPoints() <= 0){
            curPlayer->DeleteAsset(asset.lock());

        }
    }

}
//Might need to also delete the asset if it has negative health
void CEffectsModifyHealth::DoEffectArea(EPlayerNumber Player, int x_begin,
    int y_begin, int x_end, int y_end)
{
    auto AssetList = DContextRetrieval->GetAssets(Player);
    auto curPlayer = DContextRetrieval->GetPlayer(Player);

    for(auto asset : AssetList)
    {
        auto ass = asset.lock();
        auto posX = ass->TilePositionX();
        auto posY = ass->TilePositionY();
        if (posX >= x_begin && posX <= x_end &&
            posY >= y_begin && posY <= y_end)
        {
            asset.lock()->IncrementHitPoints(DAmount);
            if (ass->HitPoints() <= 0)
            {
                curPlayer->DeleteAsset(asset.lock());
            }
        }
    }
}

CEffectsModifyPlayerResources::CEffectsModifyPlayerResources(std::string resourceName,
    int amount, std::shared_ptr<CContextRetrieval> contextRetrieval)
{
    DResourceName = resourceName;
    DAmount = amount;
    DContextRetrieval = contextRetrieval;
    IsHealthEffect = 0;
}

void CEffectsModifyPlayerResources::DoEffect(EPlayerNumber Player)
{
    auto curPlayer = DContextRetrieval->GetPlayer(Player);

    if("Gold" == DResourceName)
    {
        curPlayer->IncrementGold(DAmount);
    }else if("Lumber" == DResourceName)
    {
        curPlayer->IncrementLumber(DAmount);
    }else if("Stone" == DResourceName){

    //    curPlayer->IncrementStone(DAmount); need to merge with master
    }
}

//TODO: Win and loss
CEffectsWin::CEffectsWin(std::shared_ptr<CContextRetrieval> contextRetrieval)
{
    DContextRetrieval = contextRetrieval;
}

void CEffectsWin::DoEffect(EPlayerNumber Player)
{
    auto CurPlayer = DContextRetrieval->GetPlayer(Player);
    auto Map = DContextRetrieval->GetMap();

    for (int Index = 1; Index <= Map->PlayerCount(); Index++)
    {
        auto OtherPlayer = DContextRetrieval->GetPlayer(static_cast<EPlayerNumber>(Index));
        if(CurPlayer->Number() != OtherPlayer->Number())
        {
            for (auto Asset : OtherPlayer->Assets())
            {
                OtherPlayer->DeleteAsset(Asset.lock());
            }

        }
    }
}

CEffectsLoss::CEffectsLoss(std::shared_ptr<CContextRetrieval> contextRetrieval)
{
    DContextRetrieval = contextRetrieval;
}

void CEffectsLoss::DoEffect(EPlayerNumber Player)
{
    auto CurPlayer = DContextRetrieval->GetPlayer(Player);

    for(auto asset : CurPlayer->Assets())
    {
        CurPlayer->DeleteAsset(asset.lock());
    }
}