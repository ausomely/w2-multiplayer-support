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
#ifndef PLAYERASSET_H
#define PLAYERASSET_H
#include <unordered_map>
#include <vector>
#include "DataSource.h"
#include "GameDataTypes.h"
#include "Position.h"


class CPlayerAsset;
class CPlayerAssetType;
class CPlayerData;

class CActivatedPlayerCapability
{
  protected:
    std::shared_ptr<CPlayerAsset> DActor;
    std::shared_ptr<CPlayerData> DPlayerData;
    std::shared_ptr<CPlayerAsset> DTarget;

  public:
    CActivatedPlayerCapability(std::shared_ptr<CPlayerAsset> actor,
                               std::shared_ptr<CPlayerData> playerdata,
                               std::shared_ptr<CPlayerAsset> target);
    virtual ~CActivatedPlayerCapability(){};

    virtual int PercentComplete(int max) = 0;
    virtual bool IncrementStep() = 0;
    virtual void Cancel() = 0;
    bool IsRock;
};

class CPlayerCapability
{
  public:
    enum class ETargetType
    {
        None = 0,
        Asset,
        Terrain,
        TerrainOrAsset,
        Player
    };

  protected:
    std::string DName;
    EAssetCapabilityType DAssetCapabilityType;
    ETargetType DTargetType;

    CPlayerCapability(const std::string &name, ETargetType targettype);

    static std::unordered_map<std::string, std::shared_ptr<CPlayerCapability> >
        &NameRegistry();
    static std::unordered_map<int, std::shared_ptr<CPlayerCapability> >
        &TypeRegistry();
    static bool Register(std::shared_ptr<CPlayerCapability> capability);

  public:
    virtual ~CPlayerCapability(){};

    std::string Name() const
    {
        return DName;
    };

    EAssetCapabilityType AssetCapabilityType() const
    {
        return DAssetCapabilityType;
    };

    ETargetType TargetType() const
    {
        return DTargetType;
    };

    static std::shared_ptr<CPlayerCapability> FindCapability(
        EAssetCapabilityType type);
    static std::shared_ptr<CPlayerCapability> FindCapability(
        const std::string &name);

    static EAssetCapabilityType NameToType(const std::string &name);
    static std::string TypeToName(EAssetCapabilityType type);

    virtual bool CanInitiate(std::shared_ptr<CPlayerAsset> actor,
                             std::shared_ptr<CPlayerData> playerdata) = 0;
    virtual bool CanApply(std::shared_ptr<CPlayerAsset> actor,
                          std::shared_ptr<CPlayerData> playerdata,
                          std::shared_ptr<CPlayerAsset> target) = 0;
    virtual bool ApplyCapability(std::shared_ptr<CPlayerAsset> actor,
                                 std::shared_ptr<CPlayerData> playerdata,
                                 std::shared_ptr<CPlayerAsset> target) = 0;
};

class CPlayerUpgrade
{
  protected:
    std::string DName;
    int DArmor;
    int DSight;
    int DSpeed;
    int DBasicDamage;
    int DPiercingDamage;
    int DRange;
    int DGoldCost;
    int DLumberCost;
    int DResearchTime;

    std::vector<EAssetType> DAffectedAssets;
    std::vector<EAssetType> DAssetRequirements;
    std::vector<EAssetCapabilityType> DAssetCapabilityRequirements;

    static std::unordered_map<std::string, std::shared_ptr<CPlayerUpgrade> >
        DRegistryByName;
    static std::unordered_map<int, std::shared_ptr<CPlayerUpgrade> >
        DRegistryByType;

  public:
    bool DUpgradeActive;

    CPlayerUpgrade();

    std::string Name() const
    {
        return DName;
    };

    int Armor() const
    {
        return DArmor;
    };

    int Sight() const
    {
        return DSight;
    };

    int Speed() const
    {
        return DSpeed;
    };

    int BasicDamage() const
    {
        return DBasicDamage;
    };

    int PiercingDamage() const
    {
        return DPiercingDamage;
    };

    int Range() const
    {
        return DRange;
    };

    int GoldCost() const
    {
        return DGoldCost;
    };

    int LumberCost() const
    {
        return DLumberCost;
    };

    int ResearchTime() const
    {
        return DResearchTime;
    };

    const std::vector<EAssetType> &AffectedAssets() const
    {
        return DAffectedAssets;
    };

    std::vector<EAssetType> AssetRequirements()
    {
        return DAssetRequirements;
    };

    std::vector<EAssetCapabilityType> AssetCapabilityRequirements()
    {
        return DAssetCapabilityRequirements;
    };

    static std::vector<std::string> LookupUpgradeRequirements(std::string name);

    static bool LoadUpgrades(std::shared_ptr<CDataContainer> container);
    static bool Load(std::shared_ptr<CDataSource> source);
    static std::shared_ptr<CPlayerUpgrade> FindUpgradeFromType(
        EAssetCapabilityType type);
    static std::shared_ptr<CPlayerUpgrade> FindUpgradeFromName(
        const std::string &name);
};

class CPlayerAssetType
{
  protected:
    std::weak_ptr<CPlayerAssetType> DThis;
    std::string DName;
    EAssetType DType;
    EPlayerColor DColor;
    EPlayerNumber DNumber;
    std::vector<bool> DCapabilities;
    std::vector<EAssetType> DAssetRequirements;
    std::vector<std::shared_ptr<CPlayerUpgrade>> DAssetUpgrades;
    int DHitPoints;
    int DArmor;
    int DSight;
    int DConstructionSight;
    int DSize;
    int DSpeed;
    int DGoldCost;
    int DLumberCost;
    int DFoodConsumption;
    int DBuildTime;
    int DAttackSteps;
    int DReloadSteps;
    int DBasicDamage;
    int DPiercingDamage;
    int DRange;
    static std::unordered_map<std::string, std::shared_ptr<CPlayerAssetType> >
        DRegistry;
    static std::vector<std::string> DTypeStrings;
    static std::unordered_map<std::string, EAssetType> DNameTypeTranslation;

  public:
    CPlayerAssetType();
    CPlayerAssetType(std::shared_ptr<CPlayerAssetType> res);
    ~CPlayerAssetType();

    int ArmorUpgrade() const;
    int SightUpgrade() const;
    int SpeedUpgrade() const;
    int BasicDamageUpgrade() const;
    int PiercingDamageUpgrade() const;
    int RangeUpgrade() const;

    std::string Name() const
    {
        return DName;
    };

    EAssetType Type() const
    {
        return DType;
    };

    EPlayerColor Color() const
    {
        return DColor;
    };


    EPlayerNumber Number() const
    {
        return DNumber;
    };

    void ChangeOwner(EPlayerNumber num,EPlayerColor color){

        DNumber = num;
        DColor = color;
    }

    std::vector<std::shared_ptr<CPlayerUpgrade>> GetUpgrades(){
        return DAssetUpgrades;
    }

    int HitPoints() const
    {
        return DHitPoints;
    };

    int Armor() const
    {
        return DArmor;
    };

    int Sight() const
    {
        return DSight;
    };

    int ConstructionSight() const
    {
        return DConstructionSight;
    };

    int Size() const
    {
        return DSize;
    };

    int Speed() const
    {
        return DSpeed;
    };

    int GoldCost() const
    {
        return DGoldCost;
    };

    int LumberCost() const
    {
        return DLumberCost;
    };

    int FoodConsumption() const
    {
        return DFoodConsumption;
    };

    int BuildTime() const
    {
        return DBuildTime;
    };

    int AttackSteps() const
    {
        return DAttackSteps;
    };

    int ReloadSteps() const
    {
        return DReloadSteps;
    };

    int BasicDamage() const
    {
        return DBasicDamage;
    };

    int PiercingDamage() const
    {
        return DPiercingDamage;
    };

    int Range() const
    {
        return DRange;
    };



    bool HasCapability(EAssetCapabilityType capability) const
    {
        if ((0 > to_underlying(capability)) ||
            (DCapabilities.size() <= to_underlying(capability)))
        {
            return false;
        }
        return DCapabilities[to_underlying(capability)];
    };

    std::vector<EAssetCapabilityType> Capabilities() const;

    void AddCapability(EAssetCapabilityType capability)
    {
        if ((0 > to_underlying(capability)) ||
            (DCapabilities.size() <= to_underlying(capability)))
        {
            return;
        }
        DCapabilities[to_underlying(capability)] = true;
    };

    void RemoveCapability(EAssetCapabilityType capability)
    {
        if ((0 > to_underlying(capability)) ||
            (DCapabilities.size() <= to_underlying(capability)))
        {
            return;
        }
        DCapabilities[to_underlying(capability)] = false;
    };

    void AddUpgrade(std::shared_ptr<CPlayerUpgrade> upgrade)
    {
        DAssetUpgrades.push_back(upgrade);
    };

    std::vector<EAssetType> AssetRequirements() const
    {
        return DAssetRequirements;
    };

    static EAssetType NameToType(const std::string &name);
    static std::string TypeToName(EAssetType type);
    static int MaxSight();
    static bool LoadTypes(std::shared_ptr<CDataContainer> container);
    static bool Load(std::shared_ptr<CDataSource> source);
    static std::shared_ptr<CPlayerAssetType> FindDefaultFromName(
        const std::string &name);
    static std::shared_ptr<CPlayerAssetType> FindDefaultFromType(
        EAssetType type);
    static std::shared_ptr<std::unordered_map<std::string, std::shared_ptr<CPlayerAssetType> > > DuplicateRegistry(
        EPlayerNumber number, EPlayerColor color);

    std::shared_ptr<CPlayerAsset> Construct();
};

typedef struct
{
    EAssetAction DAction;
    EAssetCapabilityType DCapability;
    std::shared_ptr<CPlayerAsset> DAssetTarget;
    std::shared_ptr<CActivatedPlayerCapability> DActivatedCapability;
} SAssetCommand, *SAssetCommandRef;

class CPlayerAsset
{
  protected:
    int DId;
    int DCreationCycle;
    int DHitPoints;
    int DGold;
    int DLumber;
    int DStone;
    uint32_t DRandomId;
    int DStep;
    int DMoveRemainderX;
    int DMoveRemainderY;
    CPixelPosition DPosition;
    EDirection DDirection;
    std::vector<SAssetCommand> DCommands;
    std::shared_ptr<CPlayerAssetType> DType;
    static int DUpdateFrequency;
    static int DUpdateDivisor;
    static int DIdCounter;

  public:
    CPlayerAsset(std::shared_ptr<CPlayerAssetType> type);
    CPlayerAsset();
    ~CPlayerAsset();

    static int UpdateFrequency()
    {
        return DUpdateFrequency;
    };

    static int UpdateFrequency(int freq);

    //0 for check if it exists, 1 for activate, 2 for deactivate. Returns false if we dont have it. Otherwise true
    bool RangerTrackingManager(int command){
        std::vector<std::shared_ptr<CPlayerUpgrade>> Upgrades = DType->GetUpgrades();

        for(auto upgrade : Upgrades)
        {
            if(upgrade->Name() == "RangerTracking"){
                switch(command){
                    case 0:
                        return true;
                        break;
                    case 1:
                        upgrade->DUpgradeActive = true;
                        return true;
                        break;
                    case 2:
                        upgrade->DUpgradeActive = false;
                        return true;
                        break;
                }
            }
        }
        return false;

    }



    int Id() const
    {
        return DId;
    };

    void SetId(int NewId)
    {
        DId = NewId;
    };

    static void IncIdCounter()
    {
        DIdCounter++;
    };

    static int GetIdCounter()
    {
        return DIdCounter;
    };

    static void ResetIdCounter()
    {
        DIdCounter = 1;
    };

    uint32_t RandomId() const
    {
        return DRandomId;
    };

    void SetRandomId(uint32_t id)
    {
        DRandomId = id;
    };

    std::string Name() const
    {
        return DType->Name();
    };

    bool Alive() const
    {
        return 0 < DHitPoints;
    };

    int CreationCycle() const
    {
        return DCreationCycle;
    };

    int CreationCycle(int cycle)
    {
        return DCreationCycle = cycle;
    };

    int HitPoints() const
    {
        return DHitPoints;
    };

    int HitPoints(int hitpts)
    {
        return DHitPoints = hitpts;
    };

    int IncrementHitPoints(int hitpts)
    {
        DHitPoints += hitpts;
        if (MaxHitPoints() < DHitPoints)
        {
            DHitPoints = MaxHitPoints();
        }
        return DHitPoints;
    };

    int DecrementHitPoints(int hitpts)
    {
        DHitPoints -= hitpts;
        if (0 > DHitPoints)
        {
            DHitPoints = 0;
        }
        return DHitPoints;
    };

    int Gold() const
    {
        return DGold;
    };

    int Gold(int gold)
    {
        return DGold = gold;
    };

    int IncrementGold(int gold)
    {
        DGold += gold;
        return DGold;
    };

    int DecrementGold(int gold)
    {
        DGold -= gold;
        return DGold;
    };

    int Lumber() const
    {
        return DLumber;
    };

    int Lumber(int lumber)
    {
        return DLumber = lumber;
    };

    int IncrementLumber(int lumber)
    {
        DLumber += lumber;
        return DLumber;
    };

    int DecrementLumber(int lumber)
    {
        DLumber -= lumber;
        return DLumber;
    };

    int Stone() const
    {
        return DStone;
    };

    int Stone(int Stone)
    {
        return DStone = Stone;
    };

    int IncrementStone(int Stone)
    {
        DStone += Stone;
        return DStone;
    };

    int DecrementStone(int Stone)
    {
        DStone -= Stone;
        return DStone;
    };

    int Step() const
    {
        return DStep;
    };

    int Step(int step)
    {
        return DStep = step;
    };

    void ResetStep()
    {
        DStep = 0;
    };

    void IncrementStep()
    {
        DStep++;
    };

    CTilePosition TilePosition() const;

    CTilePosition TilePosition(const CTilePosition &pos);

    int TilePositionX() const;

    int TilePositionX(int x);

    int TilePositionY() const;

    int TilePositionY(int y);

    CPixelPosition Position() const
    {
        return DPosition;
    };

    CPixelPosition Position(const CPixelPosition &pos);

    bool TileAligned() const
    {
        return DPosition.TileAligned();
    };

    int PositionX() const
    {
        return DPosition.X();
    };

    int PositionX(int x);

    int PositionY() const
    {
        return DPosition.Y();
    };

    int PositionY(int y);

    CPixelPosition ClosestPosition(const CPixelPosition &pos) const;

    int CommandCount() const
    {
        return DCommands.size();
    };

    void ClearCommand()
    {
        DCommands.clear();
    };

    void PushCommand(const SAssetCommand &command)
    {
        DCommands.push_back(command);
    };

    void EnqueueCommand(const SAssetCommand &command)
    {
        DCommands.insert(DCommands.begin(), command);
    };

    void PopCommand()
    {
        if (!DCommands.empty())
        {
            DCommands.pop_back();
        }
    };

    SAssetCommand CurrentCommand() const
    {
        if (!DCommands.empty())
        {
           return DCommands.back();
        }
        SAssetCommand RetVal;
        RetVal.DAction = EAssetAction::None;
        return RetVal;
    };

    SAssetCommand NextCommand() const
    {
        if (1 < DCommands.size())
        {
            return DCommands[DCommands.size() - 2];
        }
        SAssetCommand RetVal;
        RetVal.DAction = EAssetAction::None;
        return RetVal;
    };

    EAssetAction Action() const
    {
        if (!DCommands.empty())
        {
            return DCommands.back().DAction;
        }
        return EAssetAction::None;
    };

    bool HasAction(EAssetAction action) const
    {
        for (auto Command : DCommands)
        {
            if (action == Command.DAction)
            {
                return true;
            }
        }
        return false;
    };

    bool HasActiveCapability(EAssetCapabilityType capability) const
    {
        for (auto Command : DCommands)
        {
            if (EAssetAction::Capability == Command.DAction)
            {
                if (capability == Command.DCapability)
                {
                    return true;
                }
            }
        }
        return false;
    };

    std::shared_ptr<CPlayerAsset> GetAttackedTargets() const
    {
        for (auto Command : DCommands)
        {
            if (Command.DCapability == EAssetCapabilityType::Attack)
            {
                return Command.DAssetTarget;
            }

        }
        return nullptr;


    };

    bool Interruptible() const;

    EDirection Direction() const
    {
        return DDirection;
    };

    EDirection Direction(EDirection direction)
    {
        return DDirection = direction;
    };

    int MaxHitPoints() const
    {
        return DType->HitPoints();
    };

    EAssetType Type() const
    {
        return DType->Type();
    };

    std::shared_ptr<CPlayerAssetType> AssetType() const
    {
        return DType;
    };

    void ChangeType(std::shared_ptr<CPlayerAssetType> type)
    {
        DType = type;
    };

    EPlayerColor Color() const
    {
        return DType->Color();
    };

    EPlayerNumber Number() const
    {
        return DType->Number();
    };

    void ChangeOwner(EPlayerNumber num, EPlayerColor color)
    {
        DType->ChangeOwner(num,color);
    }

    int Armor() const
    {
        return DType->Armor();
    };

    int Sight() const
    {
        return EAssetAction::Construct == Action() ? DType->ConstructionSight()
                                                   : DType->Sight();
    };

    int Size() const
    {
        return DType->Size();
    };

    int Speed() const
    {
        return DType->Speed();
    };

    int GoldCost() const
    {
        return DType->GoldCost();
    };

    int LumberCost() const
    {
        return DType->LumberCost();
    };

    int FoodConsumption() const
    {
        return DType->FoodConsumption();
    };

    int BuildTime() const
    {
        return DType->BuildTime();
    };

    int AttackSteps() const
    {
        return DType->AttackSteps();
    };

    int ReloadSteps() const
    {
        return DType->ReloadSteps();
    };

    int BasicDamage() const
    {
        return DType->BasicDamage();
    };

    int PiercingDamage() const
    {
        return DType->PiercingDamage();
    };

    int Range() const
    {
        return DType->Range();
    };

    int ArmorUpgrade() const
    {
        return DType->ArmorUpgrade();
    };

    int SightUpgrade() const
    {
        return DType->SightUpgrade();
    };

    int SpeedUpgrade() const
    {
        return DType->SpeedUpgrade();
    };

    int BasicDamageUpgrade() const
    {
        return DType->BasicDamageUpgrade();
    };

    int PiercingDamageUpgrade() const
    {
        return DType->PiercingDamageUpgrade();
    };

    int RangeUpgrade() const
    {
        return DType->RangeUpgrade();
    };

    int EffectiveArmor() const
    {
        return Armor() + ArmorUpgrade();
    };

    int EffectiveSight() const
    {
        return Sight() + SightUpgrade();
    };

    int EffectiveSpeed() const
    {
        return Speed() + SpeedUpgrade();
    };

    int EffectiveBasicDamage() const
    {
        return BasicDamage() + BasicDamageUpgrade();
    };

    int EffectivePiercingDamage() const
    {
        return PiercingDamage() + PiercingDamageUpgrade();
    };

    int EffectiveRange() const
    {
        return Range() + RangeUpgrade();
    };

    bool HasCapability(EAssetCapabilityType capability) const
    {
        return DType->HasCapability(capability);
    };

    std::vector<EAssetCapabilityType> Capabilities() const
    {
        return DType->Capabilities();
    };

    bool MoveStep(
        std::vector<std::vector<std::shared_ptr<CPlayerAsset> > > &occupancymap,
        std::vector<std::vector<bool> > &diagonals);
};

#endif
