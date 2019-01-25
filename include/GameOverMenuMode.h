#ifndef GAMEOVERMENUMODE_H
#define GAMEOVERMENUMODE_H

#include "ButtonMenuMode.h"

class CGameOverMenuMode : public CButtonMenuMode
{
  protected:
    struct SPrivateConstructorType
    {
    };
    static std::shared_ptr<CApplicationMode> DGameOverMenuPointer;

    static void MainMenuButtonCallback(
        std::shared_ptr<CApplicationData> context);
    static void ExitGameButtonCallback(
        std::shared_ptr<CApplicationData> context);

    CGameOverMenuMode(const CGameOverMenuMode &) = delete;
    const CGameOverMenuMode &operator=(const CGameOverMenuMode &) = delete;

  public:
    explicit CGameOverMenuMode(const SPrivateConstructorType &key);

    static std::shared_ptr<CApplicationMode> Instance();
};

#endif