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
#include "PlayerAsset.h"
#include <algorithm>
#include "CommentSkipLineDataSource.h"
#include "Debug.h"

int CPlayerAsset::DIdCounter = 1;

std::unordered_map<std::string, std::shared_ptr<CPlayerAssetType> >
    CPlayerAssetType::DRegistry;
std::vector<std::string> CPlayerAssetType::DTypeStrings(
    {"None", "Peasant", "Footman","Knight", "Archer", "Ranger", "GoldMine", "TownHall",
     "Keep", "Castle", "Farm", "Barracks", "LumberMill", "Blacksmith",
     "ScoutTower", "GuardTower", "CannonTower"});
std::unordered_map<std::string, EAssetType> CPlayerAssetType::
    DNameTypeTranslation({{"None", EAssetType::None},
                          {"Peasant", EAssetType::Peasant},
                          {"Footman", EAssetType::Footman},
                          {"Knight", EAssetType::Knight},
                          {"Archer", EAssetType::Archer},
                          {"Ranger", EAssetType::Ranger},
                          {"GoldMine", EAssetType::GoldMine},
                          {"TownHall", EAssetType::TownHall},
                          {"Keep", EAssetType::Keep},
                          {"Castle", EAssetType::Castle},
                          {"Farm", EAssetType::Farm},
                          {"Barracks", EAssetType::Barracks},
                          {"LumberMill", EAssetType::LumberMill},
                          {"Blacksmith", EAssetType::Blacksmith},
                          {"ScoutTower", EAssetType::ScoutTower},
                          {"GuardTower", EAssetType::GuardTower},
                          {"CannonTower", EAssetType::CannonTower}});

CActivatedPlayerCapability::CActivatedPlayerCapability(
    std::shared_ptr<CPlayerAsset> actor,
    std::shared_ptr<CPlayerData> playerdata,
    std::shared_ptr<CPlayerAsset> target)
{
    DActor = actor;
    DPlayerData = playerdata;
    DTarget = target;
}

CPlayerCapability::CPlayerCapability(const std::string &name,
                                     ETargetType targettype)
{
    DName = name;
    DAssetCapabilityType = NameToType(name);
    DTargetType = targettype;
}

std::unordered_map<std::string, std::shared_ptr<CPlayerCapability> >
    &CPlayerCapability::NameRegistry()
{
    static std::unordered_map<std::string, std::shared_ptr<CPlayerCapability> >
        TheRegistry;

    return TheRegistry;
}

std::unordered_map<int, std::shared_ptr<CPlayerCapability> >
    &CPlayerCapability::TypeRegistry()
{
    static std::unordered_map<int, std::shared_ptr<CPlayerCapability> >
        TheRegistry;

    return TheRegistry;
}

bool CPlayerCapability::Register(std::shared_ptr<CPlayerCapability> capability)
{
    if (nullptr != FindCapability(capability->Name()))
    {
        return false;
    }
    NameRegistry()[capability->Name()] = capability;
    TypeRegistry()[to_underlying(NameToType(capability->Name()))] = capability;
    return true;
}

std::shared_ptr<CPlayerCapability> CPlayerCapability::FindCapability(
    EAssetCapabilityType type)
{
    auto Iterator = TypeRegistry().find(to_underlying(type));

    if (Iterator != TypeRegistry().end())
    {
        return Iterator->second;
    }
    return std::shared_ptr<CPlayerCapability>();
}

std::shared_ptr<CPlayerCapability> CPlayerCapability::FindCapability(
    const std::string &name)
{
    auto Iterator = NameRegistry().find(name);

    if (Iterator != NameRegistry().end())
    {
        return Iterator->second;
    }
    return std::shared_ptr<CPlayerCapability>();
}

EAssetCapabilityType CPlayerCapability::NameToType(const std::string &name)
{
    static std::unordered_map<std::string, EAssetCapabilityType>
        NameTypeTranslation(
            {{"None", EAssetCapabilityType::None},
             {"BuildPeasant", EAssetCapabilityType::BuildPeasant},
             {"BuildFootman", EAssetCapabilityType::BuildFootman},
             {"BuildKnight", EAssetCapabilityType::BuildKnight},
             {"BuildArcher", EAssetCapabilityType::BuildArcher},
             {"BuildRanger", EAssetCapabilityType::BuildRanger},
             {"BuildGoldMine", EAssetCapabilityType::BuildGoldMine},
             {"BuildFarm", EAssetCapabilityType::BuildFarm},
             {"BuildTownHall", EAssetCapabilityType::BuildTownHall},
             {"BuildBarracks", EAssetCapabilityType::BuildBarracks},
             {"BuildLumberMill", EAssetCapabilityType::BuildLumberMill},
             {"BuildBlacksmith", EAssetCapabilityType::BuildBlacksmith},
             {"BuildKeep", EAssetCapabilityType::BuildKeep},
             {"BuildCastle", EAssetCapabilityType::BuildCastle},
             {"BuildScoutTower", EAssetCapabilityType::BuildScoutTower},
             {"BuildGuardTower", EAssetCapabilityType::BuildGuardTower},
             {"BuildCannonTower", EAssetCapabilityType::BuildCannonTower},
             {"Move", EAssetCapabilityType::Move},
             {"Repair", EAssetCapabilityType::Repair},
             {"Mine", EAssetCapabilityType::Mine},
             {"BuildSimple", EAssetCapabilityType::BuildSimple},
             {"BuildAdvanced", EAssetCapabilityType::BuildAdvanced},
             {"Convey", EAssetCapabilityType::Convey},
             {"Cancel", EAssetCapabilityType::Cancel},
             {"BuildWall", EAssetCapabilityType::BuildWall},
             {"Attack", EAssetCapabilityType::Attack},
             {"StandGround", EAssetCapabilityType::StandGround},
             {"Patrol", EAssetCapabilityType::Patrol},
             {"WeaponUpgrade1", EAssetCapabilityType::WeaponUpgrade1},
             {"WeaponUpgrade2", EAssetCapabilityType::WeaponUpgrade2},
             {"WeaponUpgrade3", EAssetCapabilityType::WeaponUpgrade3},
             {"ArrowUpgrade1", EAssetCapabilityType::ArrowUpgrade1},
             {"ArrowUpgrade2", EAssetCapabilityType::ArrowUpgrade2},
             {"ArrowUpgrade3", EAssetCapabilityType::ArrowUpgrade3},
             {"ArmorUpgrade1", EAssetCapabilityType::ArmorUpgrade1},
             {"ArmorUpgrade2", EAssetCapabilityType::ArmorUpgrade2},
             {"ArmorUpgrade3", EAssetCapabilityType::ArmorUpgrade3},
             {"Longbow", EAssetCapabilityType::Longbow},
             {"RangerTracking", EAssetCapabilityType::RangerTracking},
             {"RangerScouting", EAssetCapabilityType::RangerScouting},
             {"Marksmanship", EAssetCapabilityType::Marksmanship}});
    auto Iterator = NameTypeTranslation.find(name);

    if (Iterator != NameTypeTranslation.end())
    {
        return Iterator->second;
    }
    PrintError("Unknown capability name \"%s\"\n", name.c_str());
    return EAssetCapabilityType::None;
}

std::string CPlayerCapability::TypeToName(EAssetCapabilityType type)
{
    static std::vector<std::string> TypeStrings({
        "None",
        "BuildPeasant",
        "BuildFootman",
        "BuildKnight",
        "BuildArcher",
        "BuildRanger",
        "BuildGoldMine",
        "BuildFarm",
        "BuildTownHall",
        "BuildBarracks",
        "BuildLumberMill",
        "BuildBlacksmith",
        "BuildKeep",
        "BuildCastle",
        "BuildScoutTower",
        "BuildGuardTower",
        "BuildCannonTower",
        "Move",
        "Repair",
        "Mine",
        "BuildSimple",
        "BuildAdvanced",
        "Convey",
        "Cancel",
        "BuildWall",
        "Attack",
        "StandGround",
        "Patrol",
        "WeaponUpgrade1",
        "WeaponUpgrade2",
        "WeaponUpgrade3",
        "ArrowUpgrade1",
        "ArrowUpgrade2",
        "ArrowUpgrade3",
        "ArmorUpgrade1",
        "ArmorUpgrade2",
        "ArmorUpgrade3",
        "Longbow",
        "RangerTracking",
        "RangerScouting",
        "Marksmanship",
    });
    if ((0 > to_underlying(type)) ||
        (to_underlying(type) >= TypeStrings.size()))
    {
        return "";
    }
    return TypeStrings[to_underlying(type)];
}

std::unordered_map<std::string, std::shared_ptr<CPlayerUpgrade> >
    CPlayerUpgrade::DRegistryByName;
std::unordered_map<int, std::shared_ptr<CPlayerUpgrade> >
    CPlayerUpgrade::DRegistryByType;

CPlayerUpgrade::CPlayerUpgrade() {}

bool CPlayerUpgrade::LoadUpgrades(std::shared_ptr<CDataContainer> container)
{
    auto FileIterator = container->First();
    if (FileIterator == nullptr)
    {
        PrintError("FileIterator == nullptr\n");
        return false;
    }

    while ((FileIterator != nullptr) && (FileIterator->IsValid()))
    {
        std::string Filename = FileIterator->Name();
        FileIterator->Next();
        if (Filename.rfind(".dat") == (Filename.length() - 4))
        {
            if (!CPlayerUpgrade::Load(container->DataSource(Filename)))
            {
                PrintError("Failed to load upgrade \"%s\".\n",
                           Filename.c_str());
                continue;
            }
            else
            {
                PrintDebug(DEBUG_LOW, "Loaded upgrade \"%s\".\n",
                           Filename.c_str());
            }
        }
    }

    return true;
}

bool CPlayerUpgrade::Load(std::shared_ptr<CDataSource> source)
{
    CCommentSkipLineDataSource LineSource(source, '#');
    std::string Name, TempString, RequirementName;
    std::shared_ptr<CPlayerUpgrade> PlayerUpgrade;
    EAssetCapabilityType UpgradeType, UpgradeRequirement;
    int AffectedAssetCount, AssetRequirementCount, UpgradeRequirementCount;
    bool ReturnStatus = false;

    if (nullptr == source)
    {
        return false;
    }
    if (!LineSource.Read(Name))
    {
        PrintError("Failed to get player upgrade name.\n");
        return false;
    }
    UpgradeType = CPlayerCapability::NameToType(Name);
    if ((EAssetCapabilityType::None == UpgradeType) &&
        (Name != CPlayerCapability::TypeToName(EAssetCapabilityType::None)))
    {
        PrintError("Unknown upgrade type %s.\n", Name.c_str());
        return false;
    }
    auto Iterator = DRegistryByName.find(Name);
    if (DRegistryByName.end() != Iterator)
    {
        PlayerUpgrade = Iterator->second;
    }
    else
    {
        PlayerUpgrade = std::make_shared<CPlayerUpgrade>();
        PlayerUpgrade->DName = Name;
        DRegistryByName[Name] = PlayerUpgrade;
        DRegistryByType[to_underlying(UpgradeType)] = PlayerUpgrade;

        if(Name == "RangerTracking"){
            PlayerUpgrade->DUpgradeActive = false;

        }else{
            PlayerUpgrade->DUpgradeActive = true;
        }
    }
    try
    {
        if (!LineSource.Read(TempString))
        {
            PrintError("Failed to get upgrade armor.\n");
            goto LoadExit;
        }
        PlayerUpgrade->DArmor = std::stoi(TempString);
        if (!LineSource.Read(TempString))
        {
            PrintError("Failed to get upgrade sight.\n");
            goto LoadExit;
        }
        PlayerUpgrade->DSight = std::stoi(TempString);
        if (!LineSource.Read(TempString))
        {
            PrintError("Failed to get upgrade speed.\n");
            goto LoadExit;
        }
        PlayerUpgrade->DSpeed = std::stoi(TempString);
        if (!LineSource.Read(TempString))
        {
            PrintError("Failed to get upgrade basic damage.\n");
            goto LoadExit;
        }
        PlayerUpgrade->DBasicDamage = std::stoi(TempString);
        if (!LineSource.Read(TempString))
        {
            PrintError("Failed to get upgrade piercing damage.\n");
            goto LoadExit;
        }
        PlayerUpgrade->DPiercingDamage = std::stoi(TempString);
        if (!LineSource.Read(TempString))
        {
            PrintError("Failed to get upgrade range.\n");
            goto LoadExit;
        }
        PlayerUpgrade->DRange = std::stoi(TempString);
        if (!LineSource.Read(TempString))
        {
            PrintError("Failed to get upgrade gold cost.\n");
            goto LoadExit;
        }
        PlayerUpgrade->DGoldCost = std::stoi(TempString);
        if (!LineSource.Read(TempString))
        {
            PrintError("Failed to get upgrade lumber cost.\n");
            goto LoadExit;
        }
        PlayerUpgrade->DLumberCost = std::stoi(TempString);
        if (!LineSource.Read(TempString))
        {
            PrintError("Failed to get upgrade research time.\n");
            goto LoadExit;
        }
        PlayerUpgrade->DResearchTime = std::stoi(TempString);
        if (!LineSource.Read(TempString))
        {
            PrintError("Failed to get upgrade affected asset count.\n");
            goto LoadExit;
        }
        AffectedAssetCount = std::stoi(TempString);
        for (int Index = 0; Index < AffectedAssetCount; Index++)
        {
            if (!LineSource.Read(TempString))
            {
                PrintError("Failed to read upgrade affected asset %d.\n",
                           Index);
                goto LoadExit;
            }
            PlayerUpgrade->DAffectedAssets.push_back(
                CPlayerAssetType::NameToType(TempString));
        }
        if (!LineSource.Read(TempString))
        {
            PrintError("Failed to get upgrade asset requirement count.\n");
            goto LoadExit;
        }
        AssetRequirementCount = std::stoi(TempString);
        for (int Index = 0; Index < AssetRequirementCount; Index++)
        {
            if (!LineSource.Read(TempString))
            {
                PrintError("Failed to read upgrade asset requirement %d.\n",
                    Index);
                goto LoadExit;
            }
            PlayerUpgrade->DAssetRequirements.push_back(
                CPlayerAssetType::NameToType(TempString));
        }
        if (!LineSource.Read(TempString))
        {
            PrintError("Failed to get upgrade capability requirement count.\n");
            goto LoadExit;
        }
        UpgradeRequirementCount = std::stoi(TempString);
        for (int Index = 0; Index < UpgradeRequirementCount; Index++)
        {
            if (!LineSource.Read(TempString))
            {
                PrintError("Failed to read upgrade capability requirement %d.\n",
                    Index);
                goto LoadExit;
            }
            PlayerUpgrade->DAssetCapabilityRequirements.push_back(
                CPlayerCapability::NameToType(TempString));
        }
        ReturnStatus = true;
    }
    catch (std::exception &E)
    {
        PrintError("%s\n", E.what());
    }
LoadExit:
    return ReturnStatus;
}

//! Lookup requirements of a given upgrade capability type
// Given an asset type string, lookup both the asset and upgrade capability
// requirements.
std::vector<std::string> CPlayerUpgrade::LookupUpgradeRequirements(std::string name)
{
    std::vector<std::string> Results;

    auto UpgradePtr = CPlayerUpgrade::FindUpgradeFromName(name);
    auto AssetReqs = UpgradePtr->AssetRequirements();

    // Given upgrade has asset requirements. Lookup name and push onto Results
    if (!AssetReqs.empty())
    {
        for (auto EType : AssetReqs)
        {
            Results.push_back(CPlayerAssetType::TypeToName(EType));
        }
    }

    auto CapabilityReqs = UpgradePtr->AssetCapabilityRequirements();

    // Given upgrade has capability requirements. Lookup name and push onto Results.
    if (!CapabilityReqs.empty())
    {
        for (auto EType : CapabilityReqs)
        {
            Results.push_back(CPlayerCapability::TypeToName(EType));
        }
    }

    return Results;
}


std::shared_ptr<CPlayerUpgrade> CPlayerUpgrade::FindUpgradeFromType(
    EAssetCapabilityType type)
{
    auto Iterator = DRegistryByType.find(to_underlying(type));

    if (Iterator != DRegistryByType.end())
    {
        return Iterator->second;
    }
    return std::shared_ptr<CPlayerUpgrade>();
}

std::shared_ptr<CPlayerUpgrade> CPlayerUpgrade::FindUpgradeFromName(
    const std::string &name)
{
    auto Iterator = DRegistryByName.find(name);

    if (Iterator != DRegistryByName.end())
    {
        return Iterator->second;
    }
    return std::shared_ptr<CPlayerUpgrade>();
}

CPlayerAssetType::CPlayerAssetType()
{
    DCapabilities.resize(to_underlying(EAssetCapabilityType::Max));
    for (int Index = 0; Index < DCapabilities.size(); Index++)
    {
        DCapabilities[Index] = false;
    }
    DHitPoints = 1;
    DArmor = 0;
    DSight = 0;
    DConstructionSight = 0;
    DSize = 1;
    DSpeed = 0;
    DGoldCost = 0;
    DLumberCost = 0;
    DFoodConsumption = 0;
    DBuildTime = 0;
    DAttackSteps = 0;
    DReloadSteps = 0;
    DBasicDamage = 0;
    DPiercingDamage = 0;
    DRange = 0;
}

CPlayerAssetType::CPlayerAssetType(std::shared_ptr<CPlayerAssetType> asset)
{
    if (nullptr != asset)
    {
        DName = asset->DName;
        DType = asset->DType;
        DColor = asset->DColor;
        DCapabilities = asset->DCapabilities;
        DAssetRequirements = asset->DAssetRequirements;
        DHitPoints = asset->DHitPoints;
        DArmor = asset->DArmor;
        DSight = asset->DSight;
        DConstructionSight = asset->DConstructionSight;
        DSize = asset->DSize;
        DSpeed = asset->DSpeed;
        DGoldCost = asset->DGoldCost;
        DLumberCost = asset->DLumberCost;
        //DStoneCost = asset->DStoneCost;
        DFoodConsumption = asset->DFoodConsumption;
        DBuildTime = asset->DBuildTime;
        DAttackSteps = asset->DAttackSteps;
        DReloadSteps = asset->DReloadSteps;
        DBasicDamage = asset->DBasicDamage;
        DPiercingDamage = asset->DPiercingDamage;
        DRange = asset->DRange;
    }
}

CPlayerAssetType::~CPlayerAssetType() {}

int CPlayerAssetType::ArmorUpgrade() const
{
    int ReturnValue = 0;
    for (auto Upgrade : DAssetUpgrades)
    {
        if(Upgrade->DUpgradeActive){
            ReturnValue += Upgrade->Armor();
        }
    }
    return ReturnValue;
}

int CPlayerAssetType::SightUpgrade() const
{
    int ReturnValue = 0;
    for (auto Upgrade : DAssetUpgrades)
    {
        if(Upgrade->DUpgradeActive){
            ReturnValue += Upgrade->Sight();
        }
    }
    return ReturnValue;
}

int CPlayerAssetType::SpeedUpgrade() const
{
    int ReturnValue = 0;
    for (auto Upgrade : DAssetUpgrades)
    {
        if(Upgrade->DUpgradeActive){
            ReturnValue += Upgrade->Speed();
        }
    }
    return ReturnValue;
}

int CPlayerAssetType::BasicDamageUpgrade() const
{
    int ReturnValue = 0;
    for (auto Upgrade : DAssetUpgrades)
    {
        if(Upgrade->DUpgradeActive){
            ReturnValue += Upgrade->BasicDamage();
        }
    }
    return ReturnValue;
}

int CPlayerAssetType::PiercingDamageUpgrade() const
{
    int ReturnValue = 0;
    for (auto Upgrade : DAssetUpgrades)
    {
        if(Upgrade->DUpgradeActive){
            ReturnValue += Upgrade->PiercingDamage();
        }
    }
    return ReturnValue;
}

int CPlayerAssetType::RangeUpgrade() const
{
    int ReturnValue = 0;
    for (auto Upgrade : DAssetUpgrades)
    {
        if(Upgrade->DUpgradeActive){
            ReturnValue += Upgrade->Range();
        }
    }
    return ReturnValue;
}

std::vector<EAssetCapabilityType> CPlayerAssetType::Capabilities() const
{
    std::vector<EAssetCapabilityType> ReturnVector;

    for (int Index = to_underlying(EAssetCapabilityType::None);
         Index < to_underlying(EAssetCapabilityType::Max); Index++)
    {
        if (DCapabilities[Index])
        {
            ReturnVector.push_back((EAssetCapabilityType) Index);
        }
    }
    return ReturnVector;
}

EAssetType CPlayerAssetType::NameToType(const std::string &name)
{
    auto Iterator = DNameTypeTranslation.find(name);
    if (Iterator != DNameTypeTranslation.end())
    {
        return Iterator->second;
    }
    return EAssetType::None;
}

std::string CPlayerAssetType::TypeToName(EAssetType type)
{
    if ((0 > to_underlying(type)) ||
        (to_underlying(type) >= DTypeStrings.size()))
    {
        return "";
    }
    return DTypeStrings[to_underlying(type)];
}

int CPlayerAssetType::MaxSight()
{
    int MaxSightFound = 0;

    for (auto &ResType : DRegistry)
    {
        MaxSightFound =
            MaxSightFound > ResType.second->DSight + ResType.second->DSize
                ? MaxSightFound
                : ResType.second->DSight + ResType.second->DSize;
    }
    return MaxSightFound;
}

bool CPlayerAssetType::LoadTypes(std::shared_ptr<CDataContainer> container)
{
    auto FileIterator = container->First();
    if (FileIterator == nullptr)
    {
        PrintError("FileIterator == nullptr\n");
        return false;
    }

    while ((FileIterator != nullptr) && (FileIterator->IsValid()))
    {
        std::string Filename = FileIterator->Name();
        FileIterator->Next();
        if (Filename.rfind(".dat") == (Filename.length() - 4))
        {
            if (!CPlayerAssetType::Load(container->DataSource(Filename)))
            {
                if (Filename.c_str() == "MiniIcons.dat" || "Terrain.dat")
                  continue;

                PrintError("Failed to load resource \"%s\".\n",
                           Filename.c_str());
                continue;
            }
            else
            {
                PrintDebug(DEBUG_LOW, "Loaded resource \"%s\".\n",
                           Filename.c_str());
            }
        }
    }
    std::shared_ptr<CPlayerAssetType> PlayerAssetType =
        std::make_shared<CPlayerAssetType>();
    PlayerAssetType->DThis = PlayerAssetType;
    PlayerAssetType->DName = "None";
    PlayerAssetType->DType = EAssetType::None;
    PlayerAssetType->DColor = EPlayerColor::None;
    PlayerAssetType->DHitPoints = 256;
    DRegistry["None"] = PlayerAssetType;

    return true;
}

bool CPlayerAssetType::Load(std::shared_ptr<CDataSource> source)
{
    CCommentSkipLineDataSource LineSource(source, '#');
    std::string Name, TempString;
    std::shared_ptr<CPlayerAssetType> PlayerAssetType;
    EAssetType AssetType;
    int CapabilityCount, AssetRequirementCount;
    bool ReturnStatus = false;

    if (nullptr == source)
    {
        return false;
    }
    if (!LineSource.Read(Name))
    {
        PrintError("Failed to get resource type name.\n");
        return false;
    }
    AssetType = NameToType(Name);
    if ((EAssetType::None == AssetType) &&
        (Name != DTypeStrings[to_underlying(EAssetType::None)]))
    {
        if (Name.c_str() == "./MiniIcons.png" || "./Terrain.png")
          return false;
        PrintError("Unknown resource type %s.\n", Name.c_str());
        return false;
    }
    auto Iterator = DRegistry.find(Name);
    if (DRegistry.end() != Iterator)
    {
        PlayerAssetType = Iterator->second;
    }
    else
    {
        PlayerAssetType = std::make_shared<CPlayerAssetType>();
        PlayerAssetType->DThis = PlayerAssetType;
        PlayerAssetType->DName = Name;
        DRegistry[Name] = PlayerAssetType;
    }
    PlayerAssetType->DType = AssetType;
    PlayerAssetType->DColor = EPlayerColor::None;
    try
    {
        if (!LineSource.Read(TempString))
        {
            PrintError("Failed to get resource type hit points.\n");
            goto LoadExit;
        }
        PlayerAssetType->DHitPoints = std::stoi(TempString);
        if (!LineSource.Read(TempString))
        {
            PrintError("Failed to get resource type armor.\n");
            goto LoadExit;
        }
        PlayerAssetType->DArmor = std::stoi(TempString);
        if (!LineSource.Read(TempString))
        {
            PrintError("Failed to get resource type sight.\n");
            goto LoadExit;
        }
        PlayerAssetType->DSight = std::stoi(TempString);
        if (!LineSource.Read(TempString))
        {
            PrintError("Failed to get resource type construction sight.\n");
            goto LoadExit;
        }
        PlayerAssetType->DConstructionSight = std::stoi(TempString);
        if (!LineSource.Read(TempString))
        {
            PrintError("Failed to get resource type size.\n");
            goto LoadExit;
        }
        PlayerAssetType->DSize = std::stoi(TempString);
        if (!LineSource.Read(TempString))
        {
            PrintError("Failed to get resource type speed.\n");
            goto LoadExit;
        }
        PlayerAssetType->DSpeed = std::stoi(TempString);
        if (!LineSource.Read(TempString))
        {
            PrintError("Failed to get resource type gold cost.\n");
            goto LoadExit;
        }
        PlayerAssetType->DGoldCost = std::stoi(TempString);
        if (!LineSource.Read(TempString))
        {
            PrintError("Failed to get resource type lumber cost.\n");
            goto LoadExit;
        }
        PlayerAssetType->DLumberCost = std::stoi(TempString);
        if (!LineSource.Read(TempString))
        {
            PrintError("Failed to get resource type food consumption.\n");
            goto LoadExit;
        }
        // PlayerAssetType->DStoneCost = std::stoi(TempString);
        // if (!LineSource.Read(TempString))
        // {
        //     PrintError("Failed to get resource type stone cost.\n");
        //     goto LoadExit;
        // }
        PlayerAssetType->DFoodConsumption = std::stoi(TempString);
        if (!LineSource.Read(TempString))
        {
            PrintError("Failed to get resource type build time.\n");
            goto LoadExit;
        }
        PlayerAssetType->DBuildTime = std::stoi(TempString);
        if (!LineSource.Read(TempString))
        {
            PrintError("Failed to get resource type attack steps.\n");
            goto LoadExit;
        }
        PlayerAssetType->DAttackSteps = std::stoi(TempString);
        if (!LineSource.Read(TempString))
        {
            PrintError("Failed to get resource type reload steps.\n");
            goto LoadExit;
        }
        PlayerAssetType->DReloadSteps = std::stoi(TempString);
        if (!LineSource.Read(TempString))
        {
            PrintError("Failed to get resource type basic damage.\n");
            goto LoadExit;
        }
        PlayerAssetType->DBasicDamage = std::stoi(TempString);
        if (!LineSource.Read(TempString))
        {
            PrintError("Failed to get resource type piercing damage.\n");
            goto LoadExit;
        }
        PlayerAssetType->DPiercingDamage = std::stoi(TempString);
        if (!LineSource.Read(TempString))
        {
            PrintError("Failed to get resource type range.\n");
            goto LoadExit;
        }
        PlayerAssetType->DRange = std::stoi(TempString);
        if (!LineSource.Read(TempString))
        {
            PrintError("Failed to get capability count.\n");
            goto LoadExit;
        }
        CapabilityCount = std::stoi(TempString);
        PlayerAssetType->DCapabilities.resize(
            to_underlying(EAssetCapabilityType::Max));
        for (int Index = 0; Index < PlayerAssetType->DCapabilities.size();
             Index++)
        {
            PlayerAssetType->DCapabilities[Index] = false;
        }
        for (int Index = 0; Index < CapabilityCount; Index++)
        {
            if (!LineSource.Read(TempString))
            {
                PrintError("Failed to read capability %d.\n", Index);
                goto LoadExit;
            }
            PlayerAssetType->AddCapability(
                CPlayerCapability::NameToType(TempString));
        }

        if (!LineSource.Read(TempString))
        {
            PrintError("Failed to get asset requirement count.\n");
            goto LoadExit;
        }
        AssetRequirementCount = std::stoi(TempString);
        for (int Index = 0; Index < AssetRequirementCount; Index++)
        {
            if (!LineSource.Read(TempString))
            {
                PrintError("Failed to read asset requirement %d.\n", Index);
                goto LoadExit;
            }
            PlayerAssetType->DAssetRequirements.push_back(
                NameToType(TempString));
        }

        ReturnStatus = true;
    }
    catch (std::exception &E)
    {
        PrintError("%s\n", E.what());
    }
LoadExit:
    return ReturnStatus;
}

std::shared_ptr<CPlayerAssetType> CPlayerAssetType::FindDefaultFromName(
    const std::string &name)
{
    auto Iterator = DRegistry.find(name);

    if (Iterator != DRegistry.end())
    {
        return Iterator->second;
    }
    return std::shared_ptr<CPlayerAssetType>();
}

std::shared_ptr<CPlayerAssetType> CPlayerAssetType::FindDefaultFromType(
    EAssetType type)
{
    return FindDefaultFromName(TypeToName(type));
}

std::shared_ptr<std::unordered_map<std::string, std::shared_ptr<CPlayerAssetType> > > CPlayerAssetType::
    DuplicateRegistry(EPlayerNumber number, EPlayerColor color)
{
    std::shared_ptr<
        std::unordered_map<std::string, std::shared_ptr<CPlayerAssetType> > >
        ReturnRegistry(
            new std::unordered_map<std::string,
                                   std::shared_ptr<CPlayerAssetType> >);

    for (auto &It : DRegistry)
    {
        std::shared_ptr<CPlayerAssetType> NewAssetType(
            new CPlayerAssetType(It.second));
        NewAssetType->DThis = NewAssetType;
        
        NewAssetType->DNumber = number;
        NewAssetType->DColor = color;
        (*ReturnRegistry)[It.first] = NewAssetType;
    }
    return ReturnRegistry;
}

std::shared_ptr<CPlayerAsset> CPlayerAssetType::Construct()
{
    if (auto ThisShared = DThis.lock())
    {
        return std::shared_ptr<CPlayerAsset>(new CPlayerAsset(ThisShared));
    }
    return nullptr;
}

int CPlayerAsset::DUpdateFrequency = 1;
int CPlayerAsset::DUpdateDivisor = 32;

int CPlayerAsset::UpdateFrequency(int freq)
{
    if (0 < freq)
    {
        DUpdateFrequency = freq;
        DUpdateDivisor = 32 * DUpdateFrequency;
    }
    return DUpdateFrequency;
}

CPlayerAsset::CPlayerAsset(std::shared_ptr<CPlayerAssetType> type)
        : DPosition(0, 0)
{
    DCreationCycle = 0;
    DType = type;
    DHitPoints = type->HitPoints();
    DGold = 0;
    DLumber = 0;
    DStep = 0;
    DStone = 0;
    DMoveRemainderX = 0;
    DMoveRemainderY = 0;
    DDirection = EDirection::South;
    TilePosition(CTilePosition());
}

CPlayerAsset::CPlayerAsset(){DId  = -1;}

CPlayerAsset::~CPlayerAsset() {}

CTilePosition CPlayerAsset::TilePosition() const
{
    CTilePosition ReturnPosition;

    ReturnPosition.SetFromPixel(DPosition);
    return ReturnPosition;
}

CTilePosition CPlayerAsset::TilePosition(const CTilePosition &pos)
{
    DPosition.SetFromTile(pos);
    return pos;
}

int CPlayerAsset::TilePositionX() const
{
    CTilePosition ReturnPosition;

    ReturnPosition.SetFromPixel(DPosition);
    return ReturnPosition.X();
}

int CPlayerAsset::TilePositionX(int x)
{
    DPosition.SetXFromTile(x);
    return x;
}

int CPlayerAsset::TilePositionY() const
{
    CTilePosition ReturnPosition;

    ReturnPosition.SetFromPixel(DPosition);
    return ReturnPosition.Y();
}

int CPlayerAsset::TilePositionY(int y)
{
    DPosition.SetYFromTile(y);
    return y;
}

CPixelPosition CPlayerAsset::Position(const CPixelPosition &pos)
{
    return DPosition = pos;
}

int CPlayerAsset::PositionX(int x)
{
    return DPosition.X(x);
}

int CPlayerAsset::PositionY(int y)
{
    return DPosition.Y(y);
};

CPixelPosition CPlayerAsset::ClosestPosition(const CPixelPosition &pos) const
{
    return pos.ClosestPosition(DPosition, Size());
}

bool CPlayerAsset::Interruptible() const
{
    SAssetCommand Command = CurrentCommand();

    switch (Command.DAction)
    {
        case EAssetAction::Construct:
        case EAssetAction::Build:
        case EAssetAction::MineGold:
        case EAssetAction::ConveyLumber:
        case EAssetAction::ConveyGold:
        case EAssetAction::Death:
        case EAssetAction::Decay:
            return false;
        case EAssetAction::Capability:
            if (Command.DAssetTarget)
            {
                return EAssetAction::Construct !=
                       Command.DAssetTarget->Action();
            }
        default:
            return true;
    }
}

bool CPlayerAsset::MoveStep(
    std::vector<std::vector<std::shared_ptr<CPlayerAsset> > > &occupancymap,
    std::vector<std::vector<bool> > &diagonals)
{
    EDirection CurrentOctant = DPosition.TileOctant();
    const int DeltaX[] = {0, 5, 7, 5, 0, -5, -7, -5};
    const int DeltaY[] = {-7, -5, 0, 5, 7, 5, 0, -5};
    CTilePosition CurrentTile, NewTilePosition;
    CPixelPosition CurrentPosition(DPosition);

    CurrentTile.SetFromPixel(DPosition);
    if ((EDirection::Max == CurrentOctant) || (CurrentOctant == DDirection))
    {  // Aligned just move
        int NewX = Speed() * DeltaX[to_underlying(DDirection)] *
                       CPosition::TileWidth() +
                   DMoveRemainderX;
        int NewY = Speed() * DeltaY[to_underlying(DDirection)] *
                       CPosition::TileHeight() +
                   DMoveRemainderY;
        DMoveRemainderX = NewX % DUpdateDivisor;
        DMoveRemainderY = NewY % DUpdateDivisor;
        DPosition.IncrementX(NewX / DUpdateDivisor);
        DPosition.IncrementY(NewY / DUpdateDivisor);
    }
    else
    {  // Entering
        int NewX = Speed() * DeltaX[to_underlying(DDirection)] *
                       CPosition::TileWidth() +
                   DMoveRemainderX;
        int NewY = Speed() * DeltaY[to_underlying(DDirection)] *
                       CPosition::TileHeight() +
                   DMoveRemainderY;
        int TempMoveRemainderX = NewX % DUpdateDivisor;
        int TempMoveRemainderY = NewY % DUpdateDivisor;
        CPixelPosition NewPosition(DPosition.X() + NewX / DUpdateDivisor,
                                   DPosition.Y() + NewY / DUpdateDivisor);
        if (NewPosition.TileOctant() == DDirection)
        {
            // Center in tile
            NewTilePosition.SetFromPixel(NewPosition);
            NewPosition.SetFromTile(NewTilePosition);
            TempMoveRemainderX = TempMoveRemainderY = 0;
        }
        DPosition = NewPosition;
        DMoveRemainderX = TempMoveRemainderX;
        DMoveRemainderY = TempMoveRemainderY;
    }
    NewTilePosition.SetFromPixel(DPosition);

    if (CurrentTile != NewTilePosition)
    {
        bool Diagonal = (CurrentTile.X() != NewTilePosition.X()) &&
                        (CurrentTile.Y() != NewTilePosition.Y());
        int DiagonalX = std::min(CurrentTile.X(), NewTilePosition.X());
        int DiagonalY = std::min(CurrentTile.Y(), NewTilePosition.Y());
        if (occupancymap[NewTilePosition.Y()][NewTilePosition.X()] ||
            (Diagonal && diagonals[DiagonalY][DiagonalX]))
        {
            bool ReturnValue = false;

            /*if(EAssetAction::Walk ==
            occupancymap[NewTilePosition.Y()][NewTilePosition.X()]->Action()){
                ReturnValue =
            occupancymap[NewTilePosition.Y()][NewTilePosition.X()]->Direction()
            == CurrentPosition.TileOctant();
            }*/
            NewTilePosition = CurrentTile;
            DPosition = CurrentPosition;
            return ReturnValue;
        }
        if (Diagonal)
        {
            diagonals[DiagonalY][DiagonalX] = true;
        }
        occupancymap[NewTilePosition.Y()][NewTilePosition.X()] =
            occupancymap[CurrentTile.Y()][CurrentTile.X()];
        occupancymap[CurrentTile.Y()][CurrentTile.X()] = nullptr;
    }

    IncrementStep();
    return true;
}
