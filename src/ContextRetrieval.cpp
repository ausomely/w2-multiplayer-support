#include "ContextRetrieval.h"
#include "ApplicationData.h"
#include "PlayerAsset.h"
#include "GameModel.h"

std::shared_ptr<CContextRetrieval> CContextRetrieval::DContextRetrievalPtr;

CContextRetrieval::CContextRetrieval(const SPrivateConstructorType &key,
    std::shared_ptr<CApplicationData> context)
{
    DContext = context;
}


std::shared_ptr<CContextRetrieval> CContextRetrieval::Instance(std::shared_ptr<CApplicationData> context)
{
    if(nullptr == DContextRetrievalPtr){
        DContextRetrievalPtr = std::make_shared<CContextRetrieval>(SPrivateConstructorType{},
            context);
    }

    return DContextRetrievalPtr;
}

std::shared_ptr<CPlayerData> CContextRetrieval::GetPlayer(EPlayerNumber Player)
{
    return DContext->DGameModel->Player(Player);
}

std::list<std::weak_ptr<CPlayerAsset> > CContextRetrieval::GetAssets(EPlayerNumber Player)
{
    return DContext->DGameModel->Player(Player)->Assets();
}

std::list<std::shared_ptr<CPlayerAsset>> CContextRetrieval::GetAllAssets()
{
    return DContext->DGameModel->Map()->Assets();
}

std::shared_ptr<CApplicationData> CContextRetrieval::GetContext()
{
    return DContext;
}

std::shared_ptr<CAssetDecoratedMap> CContextRetrieval::GetMap()
{
    return DContext->DSelectedMap;
}

