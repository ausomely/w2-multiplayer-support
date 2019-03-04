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
#ifndef TRIGGERCONTROLLER_H
#define TRIGGERCONTROLLER_H

#include "ApplicationData.h"
#include "GameModel.h"
#include "Effects.h"
#include "Triggers.h"
#include <vector>
#include <string>

class CTrigger;

class CTriggerController
{
  protected:
    struct SPrivateConstructorType{};
    static std::shared_ptr<CTriggerController> DTriggerControllerPtr;

    std::vector<std::shared_ptr<CTrigger> > DTriggers;
    std::shared_ptr<CContextRetrieval> DContextRetrieval;
    std::shared_ptr<CApplicationData> DContext;

    CTriggerController(const CTriggerController &) = delete;
    const CTriggerController &operator=(const CTriggerController &) = delete;

  public:
    CTriggerController(const SPrivateConstructorType &key,
        std::shared_ptr<CApplicationData> context);
    void ReadFile(std::string path);
    void CheckTriggers();
    int PlayerCount();
    static std::shared_ptr<CTriggerController> Instance(
        std::shared_ptr<CApplicationData> context);
};

#endif
