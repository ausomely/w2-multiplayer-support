#ifndef CONTEXTRETRIEVAL_H
#define CONTEXTRETRIEVAL_H

#include "ApplicationData.h"


class CContextRetrieval
{
    private:
        static std::shared_ptr<CContextRetrieval> DContextRetrievalPtr;
        struct SPrivateConstructorType{};


        std::shared_ptr<CApplicationData> DContext;

        //Remove the copy constructors 
        CContextRetrieval(const CContextRetrieval &) = delete;
        const CContextRetrieval &operator =(const CContextRetrieval &) = delete;
        
    public:

        std::shared_ptr<CPlayerData> GetPlayer(EPlayerNumber Player);

        std::list<std::weak_ptr<CPlayerAsset>> GetAssets(EPlayerNumber Player);

        std::list<std::shared_ptr<CPlayerAsset>> GetAllAssets();
        std::shared_ptr<CApplicationData> GetContext();
        std::shared_ptr<CAssetDecoratedMap> GetMap();


        static std::shared_ptr<CContextRetrieval> Instance(
                std::shared_ptr<CApplicationData> context);

        CContextRetrieval(const SPrivateConstructorType &key,
                std::shared_ptr<CApplicationData> context);
        ~CContextRetrieval() {};
};

#endif