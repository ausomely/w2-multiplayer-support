#ifndef UNITGROUPING_h
#define UNITGROUPING_h

#include "PlayerAsset.h"
#include "list"
/*
5 groups bound to the keys 1-5 for selecting and 6-0 for adding. Backspace removes units from groups.
 The adding and erasing occurs in battlemode input function.
*/
class CUnitGrouping
{
private:
    static std::list<std::weak_ptr<CPlayerAsset>> DGroup1;
    static std::list<std::weak_ptr<CPlayerAsset>> DGroup2;
    static std::list<std::weak_ptr<CPlayerAsset>> DGroup3;
    static std::list<std::weak_ptr<CPlayerAsset>> DGroup4;
    static std::list<std::weak_ptr<CPlayerAsset>> DGroup5;

    int DMaxSize;


public:

    CUnitGrouping();
    /*Returns true if it can add the units to the selected group. Else returns false*/
    bool AddUnits(std::list<std::weak_ptr<CPlayerAsset>> SelectedAssets,int GroupNum);
    /*When backspace is hit removes all selected units from all groups*/
    bool RemoveUnits(std::list<std::weak_ptr<CPlayerAsset>> SelectedAssets);
    std::list<std::weak_ptr<CPlayerAsset>> GetGroup(int GroupNum);
};











#endif