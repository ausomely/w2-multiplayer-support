#ifndef THEGAME_CM_JOINMULTIPLAYEROPTIONS_H
#define THEGAME_CM_JOINMULTIPLAYEROPTIONS_H

#include "ButtonMenuMode.h"

class CJoinMultiPlayerOptions : public CButtonMenuMode
{
protected:
    struct SPrivateConstructorType
    {
    };
    static std::shared_ptr<CApplicationMode> DJoinMultiPlayerOptionsPointer;

    static void DirectConnectionButtonCallback(
            std::shared_ptr<CApplicationData> context);
    static void ServerConnectionButtonCallback(
            std::shared_ptr<CApplicationData> context);
    static void MultiPlayerGameButtonCallback(
            std::shared_ptr<CApplicationData> context);

    CJoinMultiPlayerOptions(const CJoinMultiPlayerOptions &) = delete;
    const CJoinMultiPlayerOptions &operator=(
            const CJoinMultiPlayerOptions &) = delete;

public:
    explicit CJoinMultiPlayerOptions(const SPrivateConstructorType &key);

    static std::shared_ptr<CApplicationMode> Instance();
};

#endif //THEGAME_CM_JOINMULTIPLAYEROPTIONS_H
