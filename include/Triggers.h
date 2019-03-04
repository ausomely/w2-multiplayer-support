#ifndef TRIGGERS_H
#define TRIGGERS_H

#include "ApplicationData.h"
#include "ContextRetrieval.h"
#include "Effects.h"
#include <chrono>
#include <time.h>

class CEffects;
class CContextRetrieval;

class CTrigger 
{
    private:
    public:
        virtual bool Check(EPlayerNumber Player) = 0;
        virtual void InitiateEffects(EPlayerNumber Player) = 0;
};

class CTriggerLocation : public CTrigger
{
    private:
        int DX_begin;
        int DY_begin;
        int DX_end;
        int DY_end;
        std::vector<std::shared_ptr<CEffects> > DEffects; //might need to be a vector of pointers

        std::shared_ptr<CContextRetrieval> DContextRetrieval;


    public:
        explicit CTriggerLocation(std::shared_ptr<CContextRetrieval> contextRetrieval,
            int x_begin, int y_begin, int x_end, int y_end,
            std::vector<std::shared_ptr<CEffects> > effects);
        bool Check(EPlayerNumber Player) override;
        void InitiateEffects(EPlayerNumber Player) override;
        
};

class CTriggerTime : public CTrigger
{
    private:

        static std::chrono::time_point<std::chrono::steady_clock> DStartTime; 
        std::chrono::duration<double> DGoalTime;
        std::vector<std::shared_ptr<CEffects> > DEffects;

    public:
        explicit CTriggerTime(std::shared_ptr<CContextRetrieval> contextRetrieval,
        int goalTime, std::vector<std::shared_ptr<CEffects> > effects);
        bool Check(EPlayerNumber Player) override;
        void InitiateEffects(EPlayerNumber Player) override;

};

class CTriggerResource : public CTrigger
{
    private:
        int DGoalGold;
        int DGoalLumber;
        int DGoalStone;
        std::vector<std::shared_ptr<CEffects> > DEffects; //might need to be a vector of pointers
        std::shared_ptr<CContextRetrieval> DContextRetrieval;


    public:
   explicit CTriggerResource(std::shared_ptr<CContextRetrieval> contextRetrieval,
            int goalGold, int goalLumber, int goalStone,
            std::vector<std::shared_ptr<CEffects> > effects);
        bool Check(EPlayerNumber Player) override;
        void InitiateEffects(EPlayerNumber Player) override;
};

class CTriggerAssetCreation : public CTrigger
{
    private:
        std::array<int,to_underlying(EPlayerNumber::Max)> PrevAssetAmount;

        std::vector<std::shared_ptr<CEffects> > DEffects; //might need to be a vector of pointers
        std::shared_ptr<CContextRetrieval> DContextRetrieval;

    public:
        explicit CTriggerAssetCreation(std::shared_ptr<CContextRetrieval> contextRetrieval,
             std::vector<std::shared_ptr<CEffects> > effects);
        bool Check(EPlayerNumber Player) override;
        void InitiateEffects(EPlayerNumber Player) override;
};

class CTriggerAssetDeletion : public CTrigger
{
    private:
        std::array<int,to_underlying(EPlayerNumber::Max)> PrevAssetAmount;

        std::vector<std::shared_ptr<CEffects> > DEffects; //might need to be a vector of pointers
        std::shared_ptr<CContextRetrieval> DContextRetrieval;
    public:
        explicit CTriggerAssetDeletion(std::shared_ptr<CContextRetrieval> contextRetrieval,
             std::vector<std::shared_ptr<CEffects> > effects);
        bool Check(EPlayerNumber Player) override;
        void InitiateEffects(EPlayerNumber Player) override;
};
#endif