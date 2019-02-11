#ifndef NOTIFICATIONRENDERER_H
#define NOTIFICATIONRENDERER_H
#include "FontTileset.h"
#include "GameModel.h"
#include <vector>
class CNotificationRenderer{

    static std::vector<std::string> notifications; // Stores all the notifications as strings
    static int TimeCount; //Keeps track of when to delete notificatios

    std::shared_ptr <CGraphicTileset> DIcontileset;
    std::shared_ptr<CFontTileset> DFont;
    int DTextHeight;
    int DForegroundColor;
    int DBackgroundColor;


    public:
    CNotificationRenderer(std::shared_ptr<CFontTileset> font);

    void DrawNotifications(std::shared_ptr<CGraphicSurface> surface);
    void AddNotification(std::string NewNotification);
    int IncTime(); //Also returns time count for testing


};








#endif