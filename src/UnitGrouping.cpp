#include "UnitGrouping.h"
     std::list<std::weak_ptr<CPlayerAsset>> CUnitGrouping::DGroup1;
     std::list<std::weak_ptr<CPlayerAsset>> CUnitGrouping::DGroup2;
     std::list<std::weak_ptr<CPlayerAsset>> CUnitGrouping::DGroup3;
     std::list<std::weak_ptr<CPlayerAsset>> CUnitGrouping::DGroup4;
     std::list<std::weak_ptr<CPlayerAsset>> CUnitGrouping::DGroup5;


bool CUnitGrouping::AddUnits(std::list<std::weak_ptr<CPlayerAsset>> SelectedAssets,int GroupNum)
{
     //Add units to group GROUPNUM if they are not in that group
     
     switch (GroupNum)
     {
          case 1:
               if(DGroup1.size() + SelectedAssets.size() <= 9 ){
                    for(auto i : SelectedAssets)
                    {
                         bool inGroup = false;
                         for(auto Group : DGroup1 )
                         { 
                              if(Group.lock() == i.lock()){ //probably bad practice
                                   inGroup = true;
                              }
                         }
                         if(!inGroup){
                              DGroup1.push_back(i);
                         }
                    }

                    return true;
               }
               break;
          case 2:
               if(DGroup2.size() + SelectedAssets.size() <= 9 ){
                    for(auto i : SelectedAssets)
                    {
                         bool inGroup = false;
                         for(auto Group : DGroup2 )
                         {
                              if(Group.lock() == i.lock()){
                                   inGroup = true;
                              }
                         }
                         if(!inGroup){
                              DGroup2.push_back(i);
                         }
                    }

                    return true;
               }
               break;
          case 3:
               if(DGroup3.size() + SelectedAssets.size() <= 9 ){
                    for(auto i : SelectedAssets)
                    {
                         bool inGroup = false;
                         for(auto Group : DGroup3 )
                         {
                              if(Group.lock() == i.lock()){
                                   inGroup = true;
                              }
                         }
                         if(!inGroup){
                              DGroup3.push_back(i);
                         }
                    }

                    return true;
               }
               break;
          case 4:
               if(DGroup4.size() + SelectedAssets.size() <= 9 ){
                    for(auto i : SelectedAssets)
                    {
                         bool inGroup = false;
                         for(auto Group : DGroup4 )
                         {
                              if(Group.lock() == i.lock()){
                                   inGroup = true;
                              }
                         }
                         if(!inGroup){
                              DGroup4.push_back(i);
                         }
                    }

                    return true;
               }
               break;
          case 5:
               if(DGroup5.size() + SelectedAssets.size() <= 9 ){
                    for(auto i : SelectedAssets)
                    {
                         bool inGroup = false;
                         for(auto Group : DGroup5 )
                         {
                              if(Group.lock() == i.lock()){
                                   inGroup = true;
                              }
                         }
                         if(!inGroup){
                              DGroup5.push_back(i);
                         }
                    }

                    return true;
               }
               break;
     
          default:
               return false;
               break;
     }

     return false;
}

bool CUnitGrouping::RemoveUnits(std::list<std::weak_ptr<CPlayerAsset>> SelectedAssets)
{
     for(auto asset : SelectedAssets){
          for(auto i = DGroup1.begin(); i != DGroup1.end(); i++){
               auto group = *i;
               if(asset.lock() == group.lock()){
                    DGroup1.erase(i);
                    if(DGroup1.size() == 0){ //This has to be here or eles we get an iterator error 
                         break; 
                    }
                    i = DGroup1.begin();
               }
              
          }

          for(auto i = DGroup2.begin(); i != DGroup2.end(); i++){
               auto group = *i;
               if(asset.lock() == group.lock()){
                    DGroup2.erase(i);
                     if(DGroup2.size() == 0){ //This has to be here or eles we get an iterator error 
                         break; 
                    }
                    i = DGroup2.begin();
               }
              
          }

          for(auto i = DGroup3.begin(); i != DGroup3.end(); i++){
               auto group = *i;
               if(asset.lock() == group.lock()){
                    DGroup3.erase(i);
                     if(DGroup3.size() == 0){ //This has to be here or eles we get an iterator error 
                         break; 
                    }
                    i = DGroup3.begin();
               }
              
          }
    
          for(auto i = DGroup4.begin(); i != DGroup4.end(); i++){
               auto group = *i;
               if(asset.lock() == group.lock()){
                    DGroup4.erase(i);
                     if(DGroup4.size() == 0){ //This has to be here or eles we get an iterator error 
                         break; 
                    }
                    i = DGroup4.begin();
               }
              
          }

          for(auto i = DGroup5.begin(); i != DGroup5.end(); i++){
               auto group = *i;
               if(asset.lock() == group.lock()){
                    DGroup5.erase(i);
                     if(DGroup5.size() == 0){ //This has to be here or eles we get an iterator error 
                         break; 
                    }
                    i = DGroup5.begin();
               }
              
          }        

     }
}


std::list<std::weak_ptr<CPlayerAsset>> CUnitGrouping::GetGroup(int GroupNum)
{

     switch (GroupNum)
     {
          case 1:
               return DGroup1;
               break;
          case 2:
              return DGroup2;
               break;
          case 3:
               return DGroup3;
               break;
          case 4:
               return DGroup4;
               break;
          case 5:
               return DGroup5;
               break;
     
          default:
               break;
     }
     std::list<std::weak_ptr<CPlayerAsset>> empty;
     return empty;
}


CUnitGrouping::CUnitGrouping(/* args */)
{
     DMaxSize = 9;
}
