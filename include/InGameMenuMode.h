#ifndef INGAMEMENUMODE_H
#define INGAMEMENUMODE_H

#include "ButtonMenuMode.h"

class CInGameMenuMode : public CButtonMenuMode
{
  protected:
    struct SPrivateConstructorType
    {
    };
    static std::shared_ptr<CApplicationMode> DInGameMenuPointer;

    static void MainMenuButtonCallback(
        std::shared_ptr<CApplicationData> conetext);
    static void SoundOptionsButtonCallback(
        std::shared_ptr<CApplicationData> context);
    static void ReturnToGameButtonCallback(
        std::shared_ptr<CApplicationData> context);
    static void ExitGameButtonCallback(
        std::shared_ptr<CApplicationData> context);

    CInGameMenuMode(const CInGameMenuMode &) = delete;
    const CInGameMenuMode &operator=(const CInGameMenuMode &) = delete;

  public:
    explicit CInGameMenuMode(const SPrivateConstructorType &key);

    void InitializeChange(std::shared_ptr<CApplicationData> context) override;
    static std::shared_ptr<CApplicationMode> Instance();
};

#endif
