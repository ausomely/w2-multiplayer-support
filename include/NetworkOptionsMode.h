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
#ifndef NETWORKOPTIONSMODE_H
#define NETWORKOPTIONSMODE_H

#include "EditOptionsMode.h"

class CNetworkOptionsMode : public CEditOptionsMode{
    protected:
        struct SPrivateConstructorType{};
        static std::shared_ptr< CNetworkOptionsMode > DNetworkOptionsModePointer;
        
        static bool ValidHostnameCallback(const std::string &str);
        static bool ValidPortNumberCallback(const std::string &str);
        static void NetworkOptionsUpdateButtonCallback(std::shared_ptr< CApplicationData > context);
        static void OptionsButtonCallback(std::shared_ptr< CApplicationData > context);
                                               
        CNetworkOptionsMode(const CNetworkOptionsMode &) = delete;
        const CNetworkOptionsMode &operator =(const CNetworkOptionsMode &) = delete;
        
    public:
        explicit CNetworkOptionsMode(const SPrivateConstructorType &key);
        
        static std::shared_ptr< CApplicationMode > Instance();
        virtual void InitializeChange(std::shared_ptr< CApplicationData > context) override;
};

#endif
