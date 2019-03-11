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

    static void LeaveRoomButtonCallback(
        std::shared_ptr<CApplicationData> context);
    static void ExitGameButtonCallback(
        std::shared_ptr<CApplicationData> context);
    static void ReturnRoomButtonCallback(
        std::shared_ptr<CApplicationData> context);

    CGameOverMenuMode(const CGameOverMenuMode &) = delete;
    const CGameOverMenuMode &operator=(const CGameOverMenuMode &) = delete;

  public:
    explicit CGameOverMenuMode(const SPrivateConstructorType &key);
    void InitializeChange(std::shared_ptr<CApplicationData> context) override;

    static std::shared_ptr<CApplicationMode> Instance();
};

#endif
