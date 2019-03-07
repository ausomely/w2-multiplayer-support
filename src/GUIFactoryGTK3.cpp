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
#include "GUIFactoryGTK3.h"
#include <glib.h>
#include <glib/gi18n.h>
#include "Debug.h"
#include "GraphicFactoryCairo.h"

const uint32_t SGUIButtonEventType::DButtonPress = GDK_BUTTON_PRESS;
const uint32_t SGUIButtonEventType::DDoubleButtonPress = GDK_2BUTTON_PRESS;
const uint32_t SGUIButtonEventType::DTripleButtonPress = GDK_3BUTTON_PRESS;
const uint32_t SGUIButtonEventType::DButtonRelease = GDK_BUTTON_RELEASE;

bool SGUIButtonEventType::IsButtonPress() const
{
    return DType == DButtonPress;
}

bool SGUIButtonEventType::IsDoubleButtonPress() const
{
    return DType == DDoubleButtonPress;
}

bool SGUIButtonEventType::IsTripleButtonPress() const
{
    return DType == DTripleButtonPress;
}

bool SGUIButtonEventType::IsButtonRelease() const
{
    return DType == DButtonRelease;
}

const uint32_t SGUIModifierType::Shift = GDK_SHIFT_MASK;
const uint32_t SGUIModifierType::Lock = GDK_LOCK_MASK;
const uint32_t SGUIModifierType::Control = GDK_CONTROL_MASK;
const uint32_t SGUIModifierType::Alt = GDK_MOD1_MASK;
const uint32_t SGUIModifierType::Button1 = GDK_BUTTON1_MASK;
const uint32_t SGUIModifierType::Button2 = GDK_BUTTON2_MASK;
const uint32_t SGUIModifierType::Button3 = GDK_BUTTON3_MASK;
bool SGUIModifierType::ModifierIsSet(uint32_t val) const
{
    return DState & val;
}

void SGUIModifierType::SetModifier(uint32_t val)
{
    DState |= val;
}

void SGUIModifierType::ClearModifier(uint32_t val)
{
    DState &= ~val;
}

const uint32_t SGUIKeyType::Return = GDK_KEY_Return;
const uint32_t SGUIKeyType::LeftShift = GDK_KEY_Shift_L;
const uint32_t SGUIKeyType::RightShift = GDK_KEY_Shift_R;
const uint32_t SGUIKeyType::LeftControl = GDK_KEY_Control_L;
const uint32_t SGUIKeyType::RightControl = GDK_KEY_Control_R;
const uint32_t SGUIKeyType::LeftAlt = GDK_KEY_Alt_L;
const uint32_t SGUIKeyType::RightAlt = GDK_KEY_Alt_R;
const uint32_t SGUIKeyType::Escape = GDK_KEY_Escape;
// Space -> Slash are 0x020 -> 0x02f
const uint32_t SGUIKeyType::Space = GDK_KEY_space;
const uint32_t SGUIKeyType::Exclam = GDK_KEY_exclam;
const uint32_t SGUIKeyType::QuoteDbl = GDK_KEY_quotedbl;
const uint32_t SGUIKeyType::NumberSign = GDK_KEY_numbersign;
const uint32_t SGUIKeyType::Dollar = GDK_KEY_dollar;
const uint32_t SGUIKeyType::Percent = GDK_KEY_percent;
const uint32_t SGUIKeyType::Ampersand = GDK_KEY_ampersand;
const uint32_t SGUIKeyType::Apostrophe = GDK_KEY_apostrophe;
const uint32_t SGUIKeyType::QuoteRight = GDK_KEY_quoteright;
const uint32_t SGUIKeyType::ParenLeft = GDK_KEY_parenleft;
const uint32_t SGUIKeyType::ParenRight = GDK_KEY_parenright;
const uint32_t SGUIKeyType::Asterisk = GDK_KEY_asterisk;
const uint32_t SGUIKeyType::Plus = GDK_KEY_plus;
const uint32_t SGUIKeyType::Comma = GDK_KEY_comma;
const uint32_t SGUIKeyType::Minus = GDK_KEY_minus;
const uint32_t SGUIKeyType::Period = GDK_KEY_period;
const uint32_t SGUIKeyType::Slash = GDK_KEY_slash;
// Colon -> At (@) are 0x03a -> 0x040
const uint32_t SGUIKeyType::Colon = GDK_KEY_colon;
const uint32_t SGUIKeyType::SemiColon = GDK_KEY_semicolon;
const uint32_t SGUIKeyType::Less = GDK_KEY_less;
const uint32_t SGUIKeyType::Equal = GDK_KEY_equal;
const uint32_t SGUIKeyType::Greater = GDK_KEY_greater;
const uint32_t SGUIKeyType::Question = GDK_KEY_question;
const uint32_t SGUIKeyType::At = GDK_KEY_at;
// BracketLeft -> QuoteLeft are 0x05b -> 0x060
const uint32_t SGUIKeyType::BracketLeft = GDK_KEY_bracketleft;
const uint32_t SGUIKeyType::BackSlash = GDK_KEY_backslash;
const uint32_t SGUIKeyType::BracketRight = GDK_KEY_bracketright;
const uint32_t SGUIKeyType::ASCIICircum = GDK_KEY_asciicircum;
const uint32_t SGUIKeyType::UnderScore = GDK_KEY_underscore;
const uint32_t SGUIKeyType::Grave = GDK_KEY_grave;
const uint32_t SGUIKeyType::QuoteLeft = GDK_KEY_quoteleft;
// BraceLeft -> ASCIITilde are 0x07b -> 0x07e
const uint32_t SGUIKeyType::BraceLeft = GDK_KEY_braceleft;
const uint32_t SGUIKeyType::Bar = GDK_KEY_bar;
const uint32_t SGUIKeyType::BraceRight = GDK_KEY_braceright;
const uint32_t SGUIKeyType::ASCIITilde = GDK_KEY_asciitilde;
const uint32_t SGUIKeyType::Delete = GDK_KEY_Delete;
const uint32_t SGUIKeyType::BackSpace = GDK_KEY_BackSpace;
const uint32_t SGUIKeyType::UpArrow = GDK_KEY_Up;
const uint32_t SGUIKeyType::DownArrow = GDK_KEY_Down;
const uint32_t SGUIKeyType::LeftArrow = GDK_KEY_Left;
const uint32_t SGUIKeyType::RightArrow = GDK_KEY_Right;
const uint32_t SGUIKeyType::Key0 = GDK_KEY_0;
const uint32_t SGUIKeyType::Key1 = GDK_KEY_1;
const uint32_t SGUIKeyType::Key2 = GDK_KEY_2;
const uint32_t SGUIKeyType::Key3 = GDK_KEY_3;
const uint32_t SGUIKeyType::Key4 = GDK_KEY_4;
const uint32_t SGUIKeyType::Key5 = GDK_KEY_5;
const uint32_t SGUIKeyType::Key6 = GDK_KEY_6;
const uint32_t SGUIKeyType::Key7 = GDK_KEY_7;
const uint32_t SGUIKeyType::Key8 = GDK_KEY_8;
const uint32_t SGUIKeyType::Key9 = GDK_KEY_9;
const uint32_t SGUIKeyType::KeyA = GDK_KEY_A;
const uint32_t SGUIKeyType::KeyB = GDK_KEY_B;
const uint32_t SGUIKeyType::KeyC = GDK_KEY_C;
const uint32_t SGUIKeyType::KeyD = GDK_KEY_D;
const uint32_t SGUIKeyType::KeyE = GDK_KEY_E;
const uint32_t SGUIKeyType::KeyF = GDK_KEY_F;
const uint32_t SGUIKeyType::KeyG = GDK_KEY_G;
const uint32_t SGUIKeyType::KeyH = GDK_KEY_H;
const uint32_t SGUIKeyType::KeyI = GDK_KEY_I;
const uint32_t SGUIKeyType::KeyJ = GDK_KEY_J;
const uint32_t SGUIKeyType::KeyK = GDK_KEY_K;
const uint32_t SGUIKeyType::KeyL = GDK_KEY_L;
const uint32_t SGUIKeyType::KeyM = GDK_KEY_M;
const uint32_t SGUIKeyType::KeyN = GDK_KEY_N;
const uint32_t SGUIKeyType::KeyO = GDK_KEY_O;
const uint32_t SGUIKeyType::KeyP = GDK_KEY_P;
const uint32_t SGUIKeyType::KeyQ = GDK_KEY_Q;
const uint32_t SGUIKeyType::KeyR = GDK_KEY_R;
const uint32_t SGUIKeyType::KeyS = GDK_KEY_S;
const uint32_t SGUIKeyType::KeyT = GDK_KEY_T;
const uint32_t SGUIKeyType::KeyU = GDK_KEY_U;
const uint32_t SGUIKeyType::KeyV = GDK_KEY_V;
const uint32_t SGUIKeyType::KeyW = GDK_KEY_W;
const uint32_t SGUIKeyType::KeyX = GDK_KEY_X;
const uint32_t SGUIKeyType::KeyY = GDK_KEY_Y;
const uint32_t SGUIKeyType::KeyZ = GDK_KEY_Z;
const uint32_t SGUIKeyType::Keya = GDK_KEY_a;
const uint32_t SGUIKeyType::Keyb = GDK_KEY_b;
const uint32_t SGUIKeyType::Keyc = GDK_KEY_c;
const uint32_t SGUIKeyType::Keyd = GDK_KEY_d;
const uint32_t SGUIKeyType::Keye = GDK_KEY_e;
const uint32_t SGUIKeyType::Keyf = GDK_KEY_f;
const uint32_t SGUIKeyType::Keyg = GDK_KEY_g;
const uint32_t SGUIKeyType::Keyh = GDK_KEY_h;
const uint32_t SGUIKeyType::Keyi = GDK_KEY_i;
const uint32_t SGUIKeyType::Keyj = GDK_KEY_j;
const uint32_t SGUIKeyType::Keyk = GDK_KEY_k;
const uint32_t SGUIKeyType::Keyl = GDK_KEY_l;
const uint32_t SGUIKeyType::Keym = GDK_KEY_m;
const uint32_t SGUIKeyType::Keyn = GDK_KEY_n;
const uint32_t SGUIKeyType::Keyo = GDK_KEY_o;
const uint32_t SGUIKeyType::Keyp = GDK_KEY_p;
const uint32_t SGUIKeyType::Keyq = GDK_KEY_q;
const uint32_t SGUIKeyType::Keyr = GDK_KEY_r;
const uint32_t SGUIKeyType::Keys = GDK_KEY_s;
const uint32_t SGUIKeyType::Keyt = GDK_KEY_t;
const uint32_t SGUIKeyType::Keyu = GDK_KEY_u;
const uint32_t SGUIKeyType::Keyv = GDK_KEY_v;
const uint32_t SGUIKeyType::Keyw = GDK_KEY_w;
const uint32_t SGUIKeyType::Keyx = GDK_KEY_x;
const uint32_t SGUIKeyType::Keyy = GDK_KEY_y;
const uint32_t SGUIKeyType::Keyz = GDK_KEY_z;

bool SGUIKeyType::IsKey(uint32_t val) const
{
    return DValue == val;
}

void SGUIKeyType::SetKey(uint32_t val)
{
    DValue = val;
}

bool SGUIKeyType::IsAlpha() const
{
    return IsUpper() || IsLower();
}

bool SGUIKeyType::IsAlphaNumeric() const
{
    return IsAlpha() || IsDigit();
}

bool SGUIKeyType::IsDigit() const
{
    return ((Key0 <= DValue) && (Key9 >= DValue));
}

bool SGUIKeyType::IsUpper() const
{
    return ((KeyA <= DValue) && (KeyZ >= DValue));
}

bool SGUIKeyType::IsLower() const
{
    return ((Keya <= DValue) && (Keyz >= DValue));
}

bool SGUIKeyType::IsASCII() const
{
    return 127 >= DValue;
}

bool SGUIKeyType::IsPunctuation() const
{
    if ((Exclam <= DValue) && (Slash >= DValue))
    {
        return true;
    }
    else if ((Colon <= DValue) && (At >= DValue))
    {
        return true;
    }
    else if ((BracketLeft <= DValue) && (QuoteLeft >= DValue))
    {
        return true;
    }
    else if ((BraceLeft <= DValue) && (ASCIITilde >= DValue))
    {
        return true;
    }

    return false;
}

bool SGUIKeyType::IsViewableInTextField() const
{
    return (IsAlphaNumeric() || IsPunctuation() || (Space == DValue));
}

std::shared_ptr<CGUIApplication> CGUIFactory::DApplicationPointer = nullptr;

std::shared_ptr<CGUIApplication> CGUIFactory::ApplicationInstance(
    const std::string &appname)
{
    if (!DApplicationPointer && appname.length())
    {
        DApplicationPointer = std::make_shared<CGUIApplicationGTK3>(appname);
    }
    return DApplicationPointer;
}

std::shared_ptr<CGUIDisplay> CGUIFactory::DefaultDisplay()
{
    return std::make_shared<CGUIDisplayGTK3>(gdk_display_get_default());
}

std::shared_ptr<CGUIDrawingArea> CGUIFactory::NewDrawingArea()
{
    return std::make_shared<CGUIDrawingAreaGTK3>(gtk_drawing_area_new());
}

std::shared_ptr<CGUIBox> CGUIFactory::NewBox(CGUIBox::EOrientation orientation,
                                             int spacing)
{
    return std::make_shared<CGUIBoxGTK3>(
        gtk_box_new(orientation == CGUIBox::EOrientation::Horizontal
                        ? GTK_ORIENTATION_HORIZONTAL
                        : GTK_ORIENTATION_VERTICAL,
                    spacing));
}

std::shared_ptr<CGUIMenu> CGUIFactory::NewMenu()
{
    return std::make_shared<CGUIMenuGTK3>(gtk_menu_new());
}

std::shared_ptr<CGUIMenuBar> CGUIFactory::NewMenuBar()
{
    return std::make_shared<CGUIMenuBarGTK3>(gtk_menu_bar_new());
}

std::shared_ptr<CGUIMenuItem> CGUIFactory::NewMenuItem(const std::string &label)
{
    return std::make_shared<CGUIMenuItemGTK3>(
        gtk_menu_item_new_with_label(label.c_str()));
}

std::shared_ptr<CGUIFileFilter> CGUIFactory::NewFileFilter()
{
    return std::make_shared<CGUIFileFilterGTK3>(gtk_file_filter_new());
}

std::shared_ptr<CGUIFileChooserDialog> CGUIFactory::NewFileChooserDialog(
    const std::string &title, bool openfile, std::shared_ptr<CGUIWindow> parent)
{
    GtkWindow *ParentWidget = NULL;
    if (parent)
    {
        std::shared_ptr<CGUIWindowGTK3> ParentWindow =
            std::dynamic_pointer_cast<CGUIWindowGTK3>(parent);

        ParentWidget = GTK_WINDOW(ParentWindow->Widget());
    }
    if (openfile)
    {
        return std::make_shared<CGUIFileChooserDialogGTK3>(
            gtk_file_chooser_dialog_new(title.c_str(), ParentWidget,
                                        GTK_FILE_CHOOSER_ACTION_OPEN,
                                        _("_Cancel"), GTK_RESPONSE_CANCEL,
                                        _("_Open"), GTK_RESPONSE_ACCEPT, NULL));
    }
    else
    {
        return std::make_shared<CGUIFileChooserDialogGTK3>(
            gtk_file_chooser_dialog_new(title.c_str(), ParentWidget,
                                        GTK_FILE_CHOOSER_ACTION_SAVE,
                                        _("_Cancel"), GTK_RESPONSE_CANCEL,
                                        _("_Save"), GTK_RESPONSE_ACCEPT, NULL));
    }
}

CGUIApplicationGTK3::CGUIApplicationGTK3(const std::string &appname)
{
    DApplication =
        gtk_application_new(appname.c_str(), G_APPLICATION_FLAGS_NONE);
    // g_signal_connect(DApplication, "startup", G_CALLBACK (StartupCallback),
    // NULL);
    g_signal_connect(DApplication, "activate", G_CALLBACK(ActivateCallback),
                     this);

    DActivateCalldata = nullptr;
    DActivateCallback = nullptr;

    DTimerHandle = -1;
    DTimerCalldata = nullptr;
    DTimerCallback = nullptr;
    // DPeriodicTimeout = std::make_shared< CPeriodicTimeout > (50);
}

CGUIApplicationGTK3::~CGUIApplicationGTK3() {}

void CGUIApplicationGTK3::ActivateCallback(GtkApplication *app, gpointer data)
{
    CGUIApplicationGTK3 *App = static_cast<CGUIApplicationGTK3 *>(data);
    if (App->DActivateCallback)
    {
        App->DActivateCallback(App->DActivateCalldata);
    }
}

gboolean CGUIApplicationGTK3::TimeoutCallback(gpointer data)
{
    CGUIApplicationGTK3 *App = static_cast<CGUIApplicationGTK3 *>(data);
    if (App->DTimerCallback)
    {
        if (App->DTimerCallback(App->DTimerCalldata))
        {
            App->DTimerHandle =
                g_timeout_add(App->DPeriodicTimeout->MiliSecondsUntilDeadline(),
                              TimeoutCallback, data);
        }
    }
    return FALSE;
}

void CGUIApplicationGTK3::SetActivateCallback(TGUICalldata calldata,
                                              TGUIApplicationCallback callback)
{
    DActivateCalldata = calldata;
    DActivateCallback = callback;
}

void CGUIApplicationGTK3::SetTimer(int timems, TGUICalldata calldata,
                                   TGUITimeoutCallback callback)
{
    if (0 >= timems)
    {
        if (0 <= DTimerHandle)
        {
            g_source_remove(DTimerHandle);
            DTimerHandle = -1;
        }
        DTimerCalldata = nullptr;
        DTimerCallback = nullptr;
        return;
    }
    if (callback)
    {
        if (0 <= DTimerHandle)
        {
            g_source_remove(DTimerHandle);
            DTimerHandle = -1;
        }
        DTimerCalldata = calldata;
        DTimerCallback = callback;
        DPeriodicTimeout = std::make_shared<CPeriodicTimeout>(timems);
        DTimerHandle =
            g_timeout_add(DPeriodicTimeout->MiliSecondsUntilDeadline(),
                          TimeoutCallback, this);
    }
}

void CGUIApplicationGTK3::ProcessEvents(bool block)
{
    gtk_main_iteration_do(block ? TRUE : FALSE);
}

int CGUIApplicationGTK3::Run(int argc, char *argv[])
{
    return g_application_run(G_APPLICATION(DApplication), argc, argv);
}

void CGUIApplicationGTK3::Quit()
{
    g_application_quit(G_APPLICATION(DApplication));
}

std::shared_ptr<CGUIWindow> CGUIApplicationGTK3::NewWindow()
{
    return std::make_shared<CGUIWindowGTK3>(
        gtk_application_window_new(DApplication));
}

CGUIDisplayGTK3::CGUIDisplayGTK3(GdkDisplay *display) : CGUIDisplay()
{
    DDisplay = display;
}

CGUIDisplayGTK3::~CGUIDisplayGTK3() {}

void CGUIDisplayGTK3::Sync()
{
    gdk_display_sync(DDisplay);
}

void CGUIDisplayGTK3::Flush()
{
    gdk_display_flush(DDisplay);
}

std::shared_ptr<CGUICursor> CGUIDisplayGTK3::NewCursor(
    CGUICursor::ECursorType type)
{
    GdkCursorType CursorType = GDK_BLANK_CURSOR;
    switch (type)
    {
        case CGUICursor::ECursorType::RightPointer:
            CursorType = GDK_RIGHT_PTR;
            break;
        case CGUICursor::ECursorType::LeftPointer:
            CursorType = GDK_LEFT_PTR;
            break;
        case CGUICursor::ECursorType::CenterPointer:
            CursorType = GDK_CENTER_PTR;
            break;
        case CGUICursor::ECursorType::Crosshair:
            CursorType = GDK_CROSSHAIR;
            break;
        default:
            break;
    }

    return std::make_shared<CGUICursorGTK3>(
        gdk_cursor_new_for_display(DDisplay, CursorType));
}

CGUICursorGTK3::CGUICursorGTK3(GdkCursor *cursor) : CGUICursor()
{
    DCursor = cursor;
}

CGUICursorGTK3::~CGUICursorGTK3() {}

CGUICursor::ECursorType CGUICursorGTK3::Type()
{
    switch (gdk_cursor_get_cursor_type(DCursor))
    {
        case GDK_CENTER_PTR:
            return ECursorType::CenterPointer;
        case GDK_LEFT_PTR:
            return ECursorType::LeftPointer;
        case GDK_RIGHT_PTR:
            return ECursorType::RightPointer;
        case GDK_CROSSHAIR:
            return ECursorType::Crosshair;
        default:
            return ECursorType::Blank;
    }
}

CGUIWidgetGTK3::CGUIWidgetGTK3(GtkWidget *widget, bool reference)
{
    DWidget = widget;
    // if(reference){ // Seems to fail on exit if not always referenced.
    g_object_ref(DWidget);
    //}

    DActivateCalldata = nullptr;
    DActivateCallback = nullptr;

    DButtonPressCalldata = nullptr;
    DButtonPressCallback = nullptr;

    DButtonReleaseCalldata = nullptr;
    DButtonReleaseCallback = nullptr;

    DMotionCalldata = nullptr;
    DMotionCallback = nullptr;

    DKeyPressCalldata = nullptr;
    DKeyPressCallback = nullptr;

    DKeyReleaseCalldata = nullptr;
    DKeyReleaseCallback = nullptr;

    DExposeCalldata = nullptr;
    DExposeCallback = nullptr;

    DConfigureCalldata = nullptr;
    DConfigureCallback = nullptr;

    DDrawCalldata = nullptr;
    DDrawCallback = nullptr;
}

CGUIWidgetGTK3::~CGUIWidgetGTK3()
{
    if (DWidget)
    {
        gtk_widget_destroy(DWidget);
    }
}

void CGUIWidgetGTK3::ActivateEventCallback(GtkWidget *widget, gpointer data)
{
    CGUIWidgetGTK3 *Widget = static_cast<CGUIWidgetGTK3 *>(data);

    if (Widget->DActivateCallback)
    {
        Widget->DActivateCallback(Widget->shared_from_this(),
                                  Widget->DActivateCalldata);
    }
}

gboolean CGUIWidgetGTK3::ButtonPressEventCallback(GtkWidget *widget,
                                                  GdkEventButton *event,
                                                  gpointer data)
{
    CGUIWidgetGTK3 *Widget = static_cast<CGUIWidgetGTK3 *>(data);

    if (Widget->DButtonPressCallback)
    {
        SGUIButtonEvent TempEvent;
        TempEvent.DType.DType = event->type;
        TempEvent.DWindowX = event->x;
        TempEvent.DWindowY = event->y;
        TempEvent.DModifier.DState = event->state;
        TempEvent.DButton = event->button;
        TempEvent.DScreenX = event->x_root;
        TempEvent.DScreenY = event->y_root;

        return Widget->DButtonPressCallback(Widget->shared_from_this(),
                                            TempEvent,
                                            Widget->DButtonPressCalldata);
    }
    return FALSE;
}

gboolean CGUIWidgetGTK3::ButtonReleaseEventCallback(GtkWidget *widget,
                                                    GdkEventButton *event,
                                                    gpointer data)
{
    CGUIWidgetGTK3 *Widget = static_cast<CGUIWidgetGTK3 *>(data);

    if (Widget->DButtonPressCallback)
    {
        SGUIButtonEvent TempEvent;
        TempEvent.DType.DType = event->type;
        TempEvent.DWindowX = event->x;
        TempEvent.DWindowY = event->y;
        TempEvent.DModifier.DState = event->state;
        TempEvent.DButton = event->button;
        TempEvent.DScreenX = event->x_root;
        TempEvent.DScreenY = event->y_root;

        return Widget->DButtonReleaseCallback(Widget->shared_from_this(),
                                              TempEvent,
                                              Widget->DButtonReleaseCalldata);
    }
    return FALSE;
}

gboolean CGUIWidgetGTK3::DeleteEventCallback(GtkWidget *widget, GdkEvent *event,
                                             gpointer data)
{
    CGUIWidgetGTK3 *Widget = static_cast<CGUIWidgetGTK3 *>(data);

    PrintDebug(DEBUG_HIGH, "CGUIWidgetGTK3::DeleteEventCallback\n");
    if (Widget->DDeleteCallback)
    {
        Widget->DDeleteCallback(Widget->shared_from_this(),
                                Widget->DDeleteCalldata);
    }
    return FALSE;
}

void CGUIWidgetGTK3::DestroyEventCallback(GtkWidget *widget, gpointer data)
{
    CGUIWidgetGTK3 *Widget = static_cast<CGUIWidgetGTK3 *>(data);

    PrintDebug(DEBUG_HIGH, "CGUIWidgetGTK3::DestroyEventCallback\n");
    if (Widget->DDestroyCallback)
    {
        Widget->DDestroyCallback(Widget->shared_from_this(),
                                 Widget->DDestroyCalldata);
    }
}

gboolean CGUIWidgetGTK3::MotionNotifyEventCallback(GtkWidget *widget,
                                                   GdkEventMotion *event,
                                                   gpointer data)
{
    CGUIWidgetGTK3 *Widget = static_cast<CGUIWidgetGTK3 *>(data);

    if (Widget->DMotionCallback)
    {
        SGUIMotionEvent TempEvent;

        TempEvent.DWindowX = event->x;
        TempEvent.DWindowY = event->y;
        TempEvent.DModifier.DState = event->state;
        TempEvent.DScreenX = event->x_root;
        TempEvent.DScreenY = event->y_root;

        return Widget->DMotionCallback(Widget->shared_from_this(), TempEvent,
                                       Widget->DMotionCalldata);
    }
    return FALSE;
}

gboolean CGUIWidgetGTK3::KeyPressEventCallback(GtkWidget *widget,
                                               GdkEventKey *event,
                                               gpointer data)
{
    CGUIWidgetGTK3 *Widget = static_cast<CGUIWidgetGTK3 *>(data);

    if (Widget->DKeyPressCallback)
    {
        SGUIKeyEvent TempEvent;

        TempEvent.DValue.DValue = event->keyval;
        TempEvent.DModifier.DState = event->state;

        return Widget->DKeyPressCallback(Widget->shared_from_this(), TempEvent,
                                         Widget->DKeyPressCalldata);
    }
    return FALSE;
}

gboolean CGUIWidgetGTK3::KeyReleaseEventCallback(GtkWidget *widget,
                                                 GdkEventKey *event,
                                                 gpointer data)
{
    CGUIWidgetGTK3 *Widget = static_cast<CGUIWidgetGTK3 *>(data);

    if (Widget->DKeyReleaseCallback)
    {
        SGUIKeyEvent TempEvent;

        TempEvent.DValue.DValue = event->keyval;
        TempEvent.DModifier.DState = event->state;

        return Widget->DKeyReleaseCallback(
            Widget->shared_from_this(), TempEvent, Widget->DKeyReleaseCalldata);
    }
    return FALSE;
}

gboolean CGUIWidgetGTK3::ExposeEventCallback(GtkWidget *widget,
                                             GdkEventExpose *event,
                                             gpointer data)
{
    CGUIWidgetGTK3 *Widget = static_cast<CGUIWidgetGTK3 *>(data);

    if (Widget->DExposeCallback)
    {
        SGUIConfigureEvent TempEvent;

        TempEvent.DX = event->area.x;
        TempEvent.DY = event->area.y;
        TempEvent.DWidth = event->area.width;
        TempEvent.DHeight = event->area.height;

        return Widget->DExposeCallback(Widget->shared_from_this(), TempEvent,
                                       Widget->DExposeCalldata);
    }
    return FALSE;
}

gboolean CGUIWidgetGTK3::ConfigureEventCallback(GtkWidget *widget,
                                                GdkEventConfigure *event,
                                                gpointer data)
{
    CGUIWidgetGTK3 *Widget = static_cast<CGUIWidgetGTK3 *>(data);

    if (Widget->DConfigureCallback)
    {
        SGUIConfigureEvent TempEvent;

        TempEvent.DX = event->x;
        TempEvent.DY = event->y;
        TempEvent.DWidth = event->width;
        TempEvent.DHeight = event->height;

        return Widget->DConfigureCallback(Widget->shared_from_this(), TempEvent,
                                          Widget->DConfigureCalldata);
    }
    return FALSE;
}

gboolean CGUIWidgetGTK3::DrawEventCallback(GtkWidget *widget, cairo_t *cr,
                                           gpointer data)
{
    CGUIWidgetGTK3 *Widget = static_cast<CGUIWidgetGTK3 *>(data);

    if (Widget->DDrawCallback)
    {
        return Widget->DDrawCallback(
            Widget->shared_from_this(),
            std::make_shared<CGraphicResourceContextCairo>(cr, true),
            Widget->DDrawCalldata);
    }
    return FALSE;
}

void CGUIWidgetGTK3::Show()
{
    gtk_widget_show(DWidget);
}

void CGUIWidgetGTK3::ShowAll()
{
    gtk_widget_show_all(DWidget);
}

void CGUIWidgetGTK3::Hide()
{
    gtk_widget_hide(DWidget);
}

int CGUIWidgetGTK3::AllocatedWidth()
{
    GtkAllocation AllocatedArea;

    gtk_widget_get_allocation(DWidget, &AllocatedArea);
    return AllocatedArea.width;
}

int CGUIWidgetGTK3::AllocatedHeight()
{
    GtkAllocation AllocatedArea;

    gtk_widget_get_allocation(DWidget, &AllocatedArea);
    return AllocatedArea.height;
}

void CGUIWidgetGTK3::Invalidate()
{
    GtkAllocation AllocatedArea;

    gtk_widget_get_allocation(DWidget, &AllocatedArea);

    gtk_widget_queue_draw_area(DWidget, 0, 0, AllocatedArea.width,
                               AllocatedArea.height);
}

void CGUIWidgetGTK3::SetCursor(std::shared_ptr<CGUICursor> cursor)
{
    std::shared_ptr<CGUICursorGTK3> Cursor =
        std::dynamic_pointer_cast<CGUICursorGTK3>(cursor);

    gdk_window_set_cursor(gtk_widget_get_window(DWidget), Cursor->DCursor);
}

std::shared_ptr<CGraphicSurface> CGUIWidgetGTK3::CreateSimilarSurface(
    int width, int height)
{
    return std::make_shared<CGraphicSurfaceCairo>(
        gdk_window_create_similar_image_surface(gtk_widget_get_window(DWidget),
                                                CAIRO_FORMAT_ARGB32, width,
                                                height, 0));
}

std::shared_ptr<CGraphicResourceContext> CGUIWidgetGTK3::CreateResourceContext()
{
#if GTK_CHECK_VERSION(3, 22, 0)  //(GDK_VERSION_MAX_ALLOWED >= GDK_VERSION_3_22)
    GtkAllocation AllocatedArea;

    gtk_widget_get_allocation(DWidget, &AllocatedArea);
    cairo_rectangle_int_t WidgetRectangle{AllocatedArea.x, AllocatedArea.y,
                                          AllocatedArea.width,
                                          AllocatedArea.height};
    cairo_region_t *Region = cairo_region_create_rectangle(&WidgetRectangle);
    GdkWindow *Window = gtk_widget_get_window(DWidget);
    GdkDrawingContext *DrawingContext =
        gdk_window_begin_draw_frame(Window, Region);
    cairo_region_destroy(Region);

    return std::make_shared<CGraphicResourceContextCairo>(Window,
                                                          DrawingContext);
#else
    GdkWindow *Window = gtk_widget_get_window(DWidget);
    return std::make_shared<CGraphicResourceContextCairo>(
        gdk_cairo_create(Window));
#endif
}

void CGUIWidgetGTK3::EnableEvent(EGUIEvent event)
{
    gint CurrentMask = gtk_widget_get_events(DWidget);
    switch (event)
    {
        case EGUIEvent::Expose:
            CurrentMask |= GDK_EXPOSURE_MASK;
            break;
        case EGUIEvent::Motion:
            CurrentMask |= GDK_POINTER_MOTION_MASK;
            break;
        case EGUIEvent::ButtonPress:
            CurrentMask |= GDK_BUTTON_PRESS_MASK;
            break;
        case EGUIEvent::ButtonRelease:
            CurrentMask |= GDK_BUTTON_RELEASE_MASK;
            break;
        case EGUIEvent::KeyPress:
            CurrentMask |= GDK_KEY_PRESS_MASK;
            break;
        case EGUIEvent::KeyRelease:
            CurrentMask |= GDK_KEY_RELEASE_MASK;
            break;
        default:
            break;
    }
    gtk_widget_set_events(DWidget, CurrentMask);
}

void CGUIWidgetGTK3::DisableEvent(EGUIEvent event)
{
    gint CurrentMask = gtk_widget_get_events(DWidget);
    switch (event)
    {
        case EGUIEvent::Expose:
            CurrentMask &= ~GDK_EXPOSURE_MASK;
            break;
        case EGUIEvent::Motion:
            CurrentMask &= ~GDK_POINTER_MOTION_MASK;
            break;
        case EGUIEvent::ButtonPress:
            CurrentMask &= ~GDK_BUTTON_PRESS_MASK;
            break;
        case EGUIEvent::ButtonRelease:
            CurrentMask &= ~GDK_BUTTON_RELEASE_MASK;
            break;
        case EGUIEvent::KeyPress:
            CurrentMask &= ~GDK_KEY_PRESS_MASK;
            break;
        case EGUIEvent::KeyRelease:
            CurrentMask &= ~GDK_KEY_RELEASE_MASK;
            break;
        default:
            break;
    }
    gtk_widget_set_events(DWidget, CurrentMask);
}

void CGUIWidgetGTK3::SetActivateEventCallback(
    TGUICalldata calldata, TGUIActivateEventCallback callback)
{
    DActivateCalldata = calldata;
    DActivateCallback = callback;
    if (callback)
    {
        g_signal_connect(DWidget, "activate", G_CALLBACK(ActivateEventCallback),
                         this);
    }
}

void CGUIWidgetGTK3::SetButtonPressEventCallback(
    TGUICalldata calldata, TGUIButtonEventCallback callback)
{
    DButtonPressCalldata = calldata;
    DButtonPressCallback = callback;
    if (callback)
    {
        g_signal_connect(DWidget, "button-press-event",
                         G_CALLBACK(ButtonPressEventCallback), this);
    }
}

void CGUIWidgetGTK3::SetButtonReleaseEventCallback(
    TGUICalldata calldata, TGUIButtonEventCallback callback)
{
    DButtonReleaseCalldata = calldata;
    DButtonReleaseCallback = callback;
    if (callback)
    {
        g_signal_connect(DWidget, "button-release-event",
                         G_CALLBACK(ButtonReleaseEventCallback), this);
    }
}

void CGUIWidgetGTK3::SetDeleteEventCallback(TGUICalldata calldata,
                                            TGUIDeleteEventCallback callback)
{
    DDeleteCalldata = calldata;
    DDeleteCallback = callback;
    if (callback)
    {
        g_signal_connect(DWidget, "delete-event",
                         G_CALLBACK(DeleteEventCallback), this);
    }
}

void CGUIWidgetGTK3::SetDestroyEventCallback(TGUICalldata calldata,
                                             TGUIDestroyEventCallback callback)
{
    DDestroyCalldata = calldata;
    DDestroyCallback = callback;
    if (callback)
    {
        g_signal_connect(DWidget, "destroy", G_CALLBACK(DestroyEventCallback),
                         this);
    }
}

void CGUIWidgetGTK3::SetMotionEventCallback(TGUICalldata calldata,
                                            TGUIMotionEventCallback callback)
{
    DMotionCalldata = calldata;
    DMotionCallback = callback;
    if (callback)
    {
        g_signal_connect(DWidget, "motion-notify-event",
                         G_CALLBACK(MotionNotifyEventCallback), this);
    }
}

void CGUIWidgetGTK3::SetKeyPressEventCallback(TGUICalldata calldata,
                                              TGUIKeyEventCallback callback)
{
    DKeyPressCalldata = calldata;
    DKeyPressCallback = callback;
    if (callback)
    {
        g_signal_connect(DWidget, "key-press-event",
                         G_CALLBACK(KeyPressEventCallback), this);
    }
}

void CGUIWidgetGTK3::SetKeyReleaseEventCallback(TGUICalldata calldata,
                                                TGUIKeyEventCallback callback)
{
    DKeyReleaseCalldata = calldata;
    DKeyReleaseCallback = callback;
    if (callback)
    {
        g_signal_connect(DWidget, "key-release-event",
                         G_CALLBACK(KeyReleaseEventCallback), this);
    }
}

void CGUIWidgetGTK3::SetExposeEventCallback(TGUICalldata calldata,
                                            TGUIConfigureEventCallback callback)
{
    DExposeCalldata = calldata;
    DExposeCallback = callback;
    if (callback)
    {
        g_signal_connect(DWidget, "expose-event",
                         G_CALLBACK(ExposeEventCallback), this);
    }
}

void CGUIWidgetGTK3::SetConfigureEventCallback(
    TGUICalldata calldata, TGUIConfigureEventCallback callback)
{
    DConfigureCalldata = calldata;
    DConfigureCallback = callback;
    if (callback)
    {
        g_signal_connect(DWidget, "configure-event",
                         G_CALLBACK(ConfigureEventCallback), this);
    }
}

void CGUIWidgetGTK3::SetDrawEventCallback(TGUICalldata calldata,
                                          TGUIDrawEventCallback callback)
{
    DDrawCalldata = calldata;
    DDrawCallback = callback;
    if (callback)
    {
        g_signal_connect(DWidget, "draw", G_CALLBACK(DrawEventCallback), this);
    }
}

CGUIContainerGTK3::CGUIContainerGTK3(GtkWidget *widget, bool reference)
        : CGUIWidgetGTK3(widget, reference)
{
}

CGUIContainerGTK3::~CGUIContainerGTK3() {}

void CGUIContainerGTK3::SetBorderWidth(int width)
{
    gtk_container_set_border_width(GTK_CONTAINER(DWidget), width);
}

void CGUIContainerGTK3::Add(std::shared_ptr<CGUIWidget> widget)
{
    std::shared_ptr<CGUIWidgetGTK3> WidgetToAdd =
        std::dynamic_pointer_cast<CGUIWidgetGTK3>(widget);

    gtk_container_add(GTK_CONTAINER(DWidget), WidgetToAdd->Widget());
}

void CGUIContainerGTK3::Remove(std::shared_ptr<CGUIWidget> widget)
{
    std::shared_ptr<CGUIWidgetGTK3> WidgetToAdd =
        std::dynamic_pointer_cast<CGUIWidgetGTK3>(widget);

    gtk_container_remove(GTK_CONTAINER(DWidget), WidgetToAdd->Widget());
}

CGUILabelGTK3::CGUILabelGTK3(GtkWidget *widget, bool reference)
        : CGUIWidgetGTK3(widget, reference)
{
}

CGUILabelGTK3::~CGUILabelGTK3() {}

std::string CGUILabelGTK3::GetText()
{
    return gtk_label_get_text(GTK_LABEL(DWidget));
}

void CGUILabelGTK3::SetText(const std::string &str)
{
    gtk_label_set_text(GTK_LABEL(DWidget), str.c_str());
}

void CGUILabelGTK3::SetMarkup(const std::string &str)
{
    gtk_label_set_markup(GTK_LABEL(DWidget), str.c_str());
}

CGUIDrawingAreaGTK3::CGUIDrawingAreaGTK3(GtkWidget *widget, bool reference)
        : CGUIWidgetGTK3(widget, reference)
{
}

CGUIDrawingAreaGTK3::~CGUIDrawingAreaGTK3() {}

CGUIBoxGTK3::CGUIBoxGTK3(GtkWidget *widget, bool reference)
        : CGUIContainerGTK3(widget, reference)
{
}

CGUIBoxGTK3::~CGUIBoxGTK3() {}

void CGUIBoxGTK3::PackStart(std::shared_ptr<CGUIWidget> widget, bool expand,
                            bool fill, int padding)
{
    std::shared_ptr<CGUIWidgetGTK3> WidgetToAdd =
        std::dynamic_pointer_cast<CGUIWidgetGTK3>(widget);

    gtk_box_pack_start(GTK_BOX(DWidget), WidgetToAdd->Widget(), expand, fill,
                       padding);
}

void CGUIBoxGTK3::PackEnd(std::shared_ptr<CGUIWidget> widget, bool expand,
                          bool fill, int padding)
{
    std::shared_ptr<CGUIWidgetGTK3> WidgetToAdd =
        std::dynamic_pointer_cast<CGUIWidgetGTK3>(widget);

    gtk_box_pack_end(GTK_BOX(DWidget), WidgetToAdd->Widget(), expand, fill,
                     padding);
}

CGUIMenuShellGTK3::CGUIMenuShellGTK3(GtkWidget *widget, bool reference)
        : CGUIContainerGTK3(widget, reference)
{
}

CGUIMenuShellGTK3::~CGUIMenuShellGTK3() {}

void CGUIMenuShellGTK3::Append(std::shared_ptr<CGUIWidget> widget)
{
    std::shared_ptr<CGUIWidgetGTK3> WidgetToAdd =
        std::dynamic_pointer_cast<CGUIWidgetGTK3>(widget);

    gtk_menu_shell_append(GTK_MENU_SHELL(DWidget), WidgetToAdd->Widget());
}

CGUIMenuGTK3::CGUIMenuGTK3(GtkWidget *widget, bool reference)
        : CGUIMenuShellGTK3(widget, reference)
{
}

CGUIMenuGTK3::~CGUIMenuGTK3() {}

CGUIMenuBarGTK3::CGUIMenuBarGTK3(GtkWidget *widget, bool reference)
        : CGUIMenuShellGTK3(widget, reference)
{
}

CGUIMenuBarGTK3::~CGUIMenuBarGTK3() {}

CGUIMenuItemGTK3::CGUIMenuItemGTK3(GtkWidget *widget, bool reference)
        : CGUIContainerGTK3(widget, reference)
{
}

CGUIMenuItemGTK3::~CGUIMenuItemGTK3() {}

std::shared_ptr<CGUILabel> CGUIMenuItemGTK3::GetLabel()
{
    GList *List = gtk_container_get_children(GTK_CONTAINER(DWidget));

    return std::make_shared<CGUILabelGTK3>(GTK_WIDGET(List->data), true);
}

void CGUIMenuItemGTK3::SetSubMenu(std::shared_ptr<CGUIWidget> widget)
{
    std::shared_ptr<CGUIWidgetGTK3> WidgetToAdd =
        std::dynamic_pointer_cast<CGUIWidgetGTK3>(widget);

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(DWidget), WidgetToAdd->Widget());
}

CGUIWindowGTK3::CGUIWindowGTK3(GtkWidget *widget, bool reference)
        : CGUIContainerGTK3(widget, reference)
{
}

CGUIWindowGTK3::~CGUIWindowGTK3() {}

void CGUIWindowGTK3::SetMinSize(int width, int height)
{
    GdkGeometry Geometry;
    Geometry.min_width = width;
    Geometry.min_height = height;
    gtk_window_set_geometry_hints(GTK_WINDOW(DWidget), NULL, &Geometry,
                                  GDK_HINT_MIN_SIZE);
}

void CGUIWindowGTK3::SetMaxSize(int width, int height)
{
    GdkGeometry Geometry;
    Geometry.max_width = width;
    Geometry.max_height = height;
    gtk_window_set_geometry_hints(GTK_WINDOW(DWidget), NULL, &Geometry,
                                  GDK_HINT_MAX_SIZE);
}

void CGUIWindowGTK3::SetMinSize(std::shared_ptr<CGUIWidget> widget, int width,
                                int height)
{
    std::shared_ptr<CGUIWidgetGTK3> WidgetToSize =
        std::dynamic_pointer_cast<CGUIWidgetGTK3>(widget);
    GdkGeometry Geometry;
    Geometry.min_width = width;
    Geometry.min_height = height;
    gtk_window_set_geometry_hints(GTK_WINDOW(DWidget), WidgetToSize->Widget(),
                                  &Geometry, GDK_HINT_MIN_SIZE);
}

void CGUIWindowGTK3::SetMaxSize(std::shared_ptr<CGUIWidget> widget, int width,
                                int height)
{
    std::shared_ptr<CGUIWidgetGTK3> WidgetToSize =
        std::dynamic_pointer_cast<CGUIWidgetGTK3>(widget);
    GdkGeometry Geometry;
    Geometry.max_width = width;
    Geometry.max_height = height;
    gtk_window_set_geometry_hints(GTK_WINDOW(DWidget), WidgetToSize->Widget(),
                                  &Geometry, GDK_HINT_MAX_SIZE);
}

void CGUIWindowGTK3::SetTitle(const std::string &title)
{
    gtk_window_set_title(GTK_WINDOW(DWidget), title.c_str());
}

void CGUIWindowGTK3::Close()
{
    gtk_window_close(GTK_WINDOW(DWidget));
}

CGUIFileFilterGTK3::CGUIFileFilterGTK3(GtkFileFilter *filter, bool reference)
{
    DFilter = filter;
    // if(reference){
    g_object_ref(DFilter);
    //}
}

CGUIFileFilterGTK3::~CGUIFileFilterGTK3()
{
    g_object_unref(DFilter);
}

void CGUIFileFilterGTK3::AddPattern(const std::string &pattern)
{
    gtk_file_filter_add_pattern(DFilter, pattern.c_str());
}

CGUIFileChooserDialogGTK3::CGUIFileChooserDialogGTK3(GtkWidget *widget,
                                                     bool reference)
        : CGUIWindowGTK3(widget, reference)
{
}

CGUIFileChooserDialogGTK3::~CGUIFileChooserDialogGTK3() {}

void CGUIFileChooserDialogGTK3::SetFilter(std::shared_ptr<CGUIFileFilter> filter)
{
    std::shared_ptr<CGUIFileFilterGTK3> FilterToAdd =
        std::dynamic_pointer_cast<CGUIFileFilterGTK3>(filter);

    gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(DWidget),
                                FilterToAdd->DFilter);
}

void CGUIFileChooserDialogGTK3::SetCurrentFolder(const std::string &folder)
{
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(DWidget),
                                        folder.c_str());
}

std::string CGUIFileChooserDialogGTK3::GetFilename()
{
    gchar *Filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(DWidget));
    std::string ReturnName((char *) Filename);
    g_free(Filename);
    return ReturnName;
}

int CGUIFileChooserDialogGTK3::Run()
{
    return GTK_RESPONSE_CANCEL == gtk_dialog_run(GTK_DIALOG(DWidget)) ? 0 : 1;
}
