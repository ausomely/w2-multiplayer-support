/*
    Copyright (c) 2015, Christopher Nitta
    All rights reserved.

    All source material (source code, images, sounds, etc.) have been provided to
    University of California, Davis students of course ECS 160 for educational
    purposes. It may not be distributed beyond those enrolled in the course without
    prior permission from the copyright holder.

    All sound files, sound fonts, midi files, and images that have been included 
    that were extracted from original Warcraft II by Blizzard Entertainment 
    were found freely available via internet sources and have been labeld as 
    abandonware. They have been included in this distribution for educational 
    purposes only and this copyright notice does not attempt to claim any 
    ownership of this material.
*/
#include "NetworkOptionsMode.h"
#include "ApplicationData.h"
#include "OptionsMenuMode.h"

std::shared_ptr< CNetworkOptionsMode > CNetworkOptionsMode::DNetworkOptionsModePointer;

CNetworkOptionsMode::CNetworkOptionsMode(const SPrivateConstructorType &key){
    DTitle = "Network Options";
    DButtonTexts.push_back("OK");
    DButtonFunctions.push_back(NetworkOptionsUpdateButtonCallback);
    DButtonTexts.push_back("Cancel");
    DButtonFunctions.push_back(OptionsButtonCallback);
    
    DEditTitles.push_back("User Name:");
    DEditValidationFunctions.push_back(ValidHostnameCallback);
    DEditTitles.push_back("Remote Hostname:");
    DEditValidationFunctions.push_back(ValidHostnameCallback);
    DEditTitles.push_back("Remote Hostname:");
    DEditValidationFunctions.push_back(ValidPortNumberCallback);
}

bool CNetworkOptionsMode::ValidHostnameCallback(const std::string &str){
    int CharSinceDot = 0;

    if(253 < str.length()){
        return false;    
    }
    if(0 == str.length()){
        return false;
    }
    for(auto Char : str){
        if('.' == Char){
            if(0 == CharSinceDot){
                return false;
            }
            CharSinceDot = 0;
        }
        else{
            CharSinceDot++;
            if(63 < CharSinceDot){
                return false;   
            }
            if(('-' != Char)&&(!(('0' <= Char)&&('9' >= Char)))&&(!(('a' <= Char)&&('z' >= Char)))&&(!(('A' <= Char)&&('Z' >= Char)))){
                return false;
            }
        }
    }
    return true;
}

bool CNetworkOptionsMode::ValidPortNumberCallback(const std::string &str){
    try{
        int Port = std::stoi(str);
        if((1024 < Port)&&(65535 >= Port)){
            return str == std::to_string(Port);
        }
    }
    catch(std::exception &E){
        return false;
    }
    return false;
}

void CNetworkOptionsMode::NetworkOptionsUpdateButtonCallback(std::shared_ptr< CApplicationData > context){    
    for(int Index = 0; Index < DNetworkOptionsModePointer->DEditText.size(); Index++){
        if(!DNetworkOptionsModePointer->DEditValidationFunctions[Index](DNetworkOptionsModePointer->DEditText[Index])){
            return;   
        }
    }
    context->DUsername = DNetworkOptionsModePointer->DEditText[0];
    context->DRemoteHostname = DNetworkOptionsModePointer->DEditText[1];
    context->DMultiplayerPort = std::stoi(DNetworkOptionsModePointer->DEditText[2]);
    context->ChangeApplicationMode(COptionsMenuMode::Instance());
}

void CNetworkOptionsMode::OptionsButtonCallback(std::shared_ptr< CApplicationData > context){
    context->ChangeApplicationMode(COptionsMenuMode::Instance());
}
       
std::shared_ptr< CApplicationMode > CNetworkOptionsMode::Instance(){
    if(DNetworkOptionsModePointer == nullptr){
        DNetworkOptionsModePointer = std::make_shared< CNetworkOptionsMode >(SPrivateConstructorType());
    }
    return DNetworkOptionsModePointer;
}

void CNetworkOptionsMode::InitializeChange(std::shared_ptr< CApplicationData > context){
    DEditSelected = -1;
    DEditSelectedCharacter = -1;
    DButtonLocations.clear();
    DEditLocations.clear();
    DEditText.clear();
    DEditText.push_back(context->DUsername);
    DEditText.push_back(context->DRemoteHostname);
    DEditText.push_back(std::to_string(context->DMultiplayerPort));
}

