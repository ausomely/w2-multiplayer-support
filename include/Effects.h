#ifndef EFFECTS_H
#define EFFECTS_H

#include "ContextRetrieval.h"

class CContextRetrieval;

class CEffects
{
protected:
public:
    virtual void DoEffect(EPlayerNumber Player) = 0;
    virtual void DoEffectArea(EPlayerNumber Player, int x_begin, int y_begin, int x_end, int
    y_end) = 0;
    bool IsHealthEffect;
};

//Assets are going to be added 
class CEffectsAddAsset : public CEffects
{
    private:
        std::string DAssetName;
        int DAmount;
        std::shared_ptr<CContextRetrieval> DContextRetrieval;


    public:
        explicit CEffectsAddAsset(std::string assetName, int amount, 
            std::shared_ptr<CContextRetrieval> contextRetrieval);
        void DoEffect(EPlayerNumber Player) override;
        void DoEffectArea(EPlayerNumber Player, int x_begin, int y_begin, int x_end, int
        y_end) override {};

};

class CEffectsRemoveAsset : public CEffects
{
    private:
        std::string DAssetName;
        int DAmount;
        std::shared_ptr<CContextRetrieval> DContextRetrieval;

    public:
        explicit CEffectsRemoveAsset(std::string assetName, int amount, 
            std::shared_ptr<CContextRetrieval> contextRetrieval);
        void DoEffect(EPlayerNumber Player) override;
    void DoEffectArea(EPlayerNumber Player, int x_begin, int y_begin, int x_end, int
    y_end) override {};

};

class CEffectsModifyHealth : public CEffects {
private:
    int DAmount;
    std::shared_ptr<CContextRetrieval> DContextRetrieval;

public:
    explicit CEffectsModifyHealth(int amount,
                                  std::shared_ptr<CContextRetrieval> contextRetrieval);
    void DoEffect(EPlayerNumber Player) override;
    void DoEffectArea(EPlayerNumber Player, int x_begin, int y_begin, int x_end, int y_end);
};

class CEffectsModifyPlayerResources : public CEffects
{
    private:
        std::string DResourceName;
        int DAmount;
        std::shared_ptr<CContextRetrieval> DContextRetrieval;

    public:
        explicit CEffectsModifyPlayerResources(std::string resourceName, int amount, 
            std::shared_ptr<CContextRetrieval> contextRetrieval);
        void DoEffect(EPlayerNumber Player) override;
    void DoEffectArea(EPlayerNumber Player, int x_begin, int y_begin, int x_end, int
    y_end) override {};


};

class CEffectsWin : public CEffects
{
    private:
        std::shared_ptr<CContextRetrieval> DContextRetrieval;
    public:
    explicit CEffectsWin(std::shared_ptr<CContextRetrieval> contextRetrieval);
    void DoEffect(EPlayerNumber Player) override;
    void DoEffectArea(EPlayerNumber Player, int x_begin, int y_begin, int x_end, int
    y_end) override {};

};

class CEffectsLoss : public CEffects
{
    private:
    std::shared_ptr<CContextRetrieval> DContextRetrieval;

    public:
    explicit CEffectsLoss(std::shared_ptr<CContextRetrieval> contextRetrieval);
    void DoEffect(EPlayerNumber Player) override;
    void DoEffectArea(EPlayerNumber Player, int x_begin, int y_begin, int x_end, int
    y_end) override {};
};

#endif
