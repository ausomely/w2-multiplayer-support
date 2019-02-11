#include "NotificationRenderer.h"
#include "TextFormatter.h"


std::vector<std::string> CNotificationRenderer::notifications;
int CNotificationRenderer::TimeCount = 0;
CNotificationRenderer::CNotificationRenderer(std::shared_ptr<CFontTileset> font)
{
    int Width;
    DTextHeight;
    DFont = font;
    DForegroundColor = DFont -> FindColor("black");
    DBackgroundColor = DFont -> FindColor("gold");

    DFont-> MeasureText("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz",Width, DTextHeight);

}

void CNotificationRenderer::DrawNotifications(std::shared_ptr<CGraphicSurface> surface)
{
    int Width, Height;
    int TextYOffset;
    int XOffset;
    int WidthSeperation;

    Width = surface->Width();
    Height = surface->Height();
    TextYOffset = Height / 1.5 - DTextHeight / 2;
    WidthSeperation = Width / 4;
    XOffset = 0;

    //Erase notifications after a set time
    if(0 == TimeCount % 60  && TimeCount){
        notifications.erase(notifications.begin());
    }

    int Count = 0;
    //Only print out 3 notifications at a time
    for(auto i = notifications.begin(); i != notifications.end(); i++)
    {
        if(Count < 3){
            DFont->DrawTextWithShadow(surface,XOffset, TextYOffset, DForegroundColor,DBackgroundColor,1,*i);   
            TextYOffset -= DTextHeight;
        }else{
            break;
        }
        Count++;
    }
    

    //DFont->DrawTextWithShadow(surface,XOffset, TextYOffset, DForegroundColor,DBackgroundColor,1,notification);
}

void CNotificationRenderer::AddNotification(std::string NewNotification){
    notifications.push_back(NewNotification);
}

int CNotificationRenderer::IncTime(){
    return TimeCount++;
}