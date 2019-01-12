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
#include "SoundOptionsMode.h"
#include "ApplicationData.h"
#include "OptionsMenuMode.h"

std::shared_ptr< CSoundOptionsMode > CSoundOptionsMode::DSoundOptionsModePointer;

CSoundOptionsMode::CSoundOptionsMode(const SPrivateConstructorType &key){
    DTitle = "Sound Options";
    DButtonTexts.push_back("OK");
    DButtonFunctions.push_back(SoundOptionsUpdateButtonCallback);
    DButtonTexts.push_back("Cancel");
    DButtonFunctions.push_back(OptionsButtonCallback);
    
    DEditTitles.push_back("FX Volume:");
    DEditValidationFunctions.push_back(ValidSoundLevelCallback);
    DEditTitles.push_back("Music Volume:");
    DEditValidationFunctions.push_back(ValidSoundLevelCallback);
}

bool CSoundOptionsMode::ValidSoundLevelCallback(const std::string &str){
    try{
        int Level = std::stoi(str);
        if((0 <= Level)&&(100 >= Level)){
            return str == std::to_string(Level);
        }
    }
    catch(std::exception &E){
        return false;
    }
    return false;
}

void CSoundOptionsMode::SoundOptionsUpdateButtonCallback(std::shared_ptr< CApplicationData > context){    
    for(int Index = 0; Index < DSoundOptionsModePointer->DEditText.size(); Index++){
        if(!DSoundOptionsModePointer->DEditValidationFunctions[Index](DSoundOptionsModePointer->DEditText[Index])){
            return;   
        }
    }
    context->DSoundVolume = std::stod(DSoundOptionsModePointer->DEditText[0]) / 100.0;
    context->DMusicVolume = std::stod(DSoundOptionsModePointer->DEditText[1]) / 100.0;
    context->DSoundLibraryMixer->PlaySong(context->DSoundLibraryMixer->FindSong("menu"), context->DMusicVolume);   
    context->ChangeApplicationMode(COptionsMenuMode::Instance());
}

void CSoundOptionsMode::OptionsButtonCallback(std::shared_ptr< CApplicationData > context){
    context->ChangeApplicationMode(COptionsMenuMode::Instance());
}
       
std::shared_ptr< CApplicationMode > CSoundOptionsMode::Instance(){
    if(DSoundOptionsModePointer == nullptr){
        DSoundOptionsModePointer = std::make_shared< CSoundOptionsMode >(SPrivateConstructorType());
    }
    return DSoundOptionsModePointer;
}

void CSoundOptionsMode::InitializeChange(std::shared_ptr< CApplicationData > context){
    DEditSelected = -1;
    DEditSelectedCharacter = -1;
    DButtonLocations.clear();
    DEditLocations.clear();
    DEditText.clear();
    DEditText.push_back(std::to_string((int)(context->DSoundVolume * 100)));
    DEditText.push_back(std::to_string((int)(context->DMusicVolume * 100)));
}

