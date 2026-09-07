/*
 *  @file
 *  @brief  CMessageBox
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2020.8.13
 *
 *  Copyright (C) 2020 miragekiller
 */

#pragma once

#ifndef TMMESSAGEBOX_H
#define TMMESSAGEBOX_H

#include "tmdef.h"

#ifdef TM_WIN
#   include <windows.h>
#endif

// #ifdef TM_X11
// #   include <X11/Xlib.h>
// #   include <X11/keysym.h>
// #endif
// 

#ifdef TM_APPLE
// add .pro in QT: LIBS += -framework CoreFoundation
#   include <CoreFoundation/CoreFoundation.h>
#endif

#ifdef TM_QT
#   include <QMessageBox>
#endif

#ifdef TM_GTK
#   include <gtk/gtk.h>
#   include <string>
#   include <cstdlib>
#endif


TM_BGN_NAMESPACE

#ifdef TM_WIN

struct CMessageBoxWin
{
public:
    enum EResult
    {
        eResultNone = -1,
        eResultOk = IDOK,
        eResultCancel = IDCANCEL,
        eResultAbort = IDABORT,
        eResultRetry = IDRETRY,
        eResultIgnore = IDIGNORE,
        eResultYes = IDYES,
        eResultNo = IDNO,
        eResultClose = IDCLOSE,
        eResultHelp = IDHELP,
    };

    enum EType
    {
        eTypeOk = MB_OK,
        eTypeOkCancel = MB_OKCANCEL,
        eTypeAbortRetryIgnore = MB_ABORTRETRYIGNORE,
        eTypeYesNoCancel = MB_YESNOCANCEL,
        eTypeYesNo = MB_YESNO,
        eTypeRetryCancel = MB_RETRYCANCEL,
        
        
        eTypeIconHand = MB_ICONHAND,
        eTypeIconQuestion = MB_ICONQUESTION,
        eTypeIconExclamation = MB_ICONEXCLAMATION,
        eTypeOkIconAsterisk = MB_ICONASTERISK,
        
        eTypeUserIcon = MB_USERICON,
        eTypeIconWarning = MB_ICONWARNING,
        eTypeIconError = MB_ICONERROR,
        eTypeIconInformation = MB_ICONINFORMATION,
        eTypeIconStop = MB_ICONSTOP,
        
        eTypeDefButton1 = MB_DEFBUTTON1,
        eTypeDefButton2 = MB_DEFBUTTON2,
        eTypeDefButton3 = MB_DEFBUTTON3,
        eTypeDefButton4 = MB_DEFBUTTON4,
        
        eTypeApplModal = MB_APPLMODAL,
        eTypeSystemModal = MB_SYSTEMMODAL,
        eTypeTaskModal = MB_TASKMODAL,
        eTypeHelp = MB_HELP,
        
        eTypeNoFocus = MB_NOFOCUS,
        eTypeSetForeground = MB_SETFOREGROUND,
        eTypeDefaultDesktopOnly = MB_DEFAULT_DESKTOP_ONLY,
        
        eTypeTopmost = MB_TOPMOST,
        eTypeRight = MB_RIGHT,
        eTypeRTLReading = MB_RTLREADING,
        
        eTypeServiceNotification = MB_SERVICE_NOTIFICATION,
        
        eTypeTypeMask = MB_TYPEMASK,
        eTypeIconMask = MB_ICONMASK,
        eTypeDefMask = MB_DEFMASK,
        eTypeModeMask = MB_MODEMASK,
        eTypeMiscMask = MB_MISCMASK,
    };

    static EResult done(HWND hOwner, TMCStr lpszText, TMCStr lpszCaption = TM_T(""), TMUInt nType = eTypeOk)
    {
        return (EResult)::MessageBox(hOwner, lpszText, lpszCaption, nType);
    }
};

#endif // TM_WIN

#ifdef TM_APPLE

struct CMessageBoxMac
{
public:
    enum EResult
    {
        eResultNone = -1,
        eResultOk = 1,
        eResultCancel,
        eResultAbort,
        eResultRetry,
        eResultIgnore,
        eResultYes,
        eResultNo,
        eResultClose,
        eResultHelp,
    };
    
    enum EType
    {
        eTypeOk,
        eTypeOkCancel,
        eTypeAbortRetryIgnore,
        eTypeYesNoCancel,
        eTypeYesNo,
        eTypeRetryCancel,
    };
    
    static EResult done(TMHWndNative hOwner, TMCStr lpszText, TMCStr lpszCaption = TM_T(""), TMUInt nType = eTypeOk)
    {
        CFStringRef but[3] = {0};
        CFOptionFlags flag[4] =
        {
            kCFUserNotificationCancelResponse,
            kCFUserNotificationCancelResponse,
            kCFUserNotificationCancelResponse,
            kCFUserNotificationCancelResponse
        };

        switch (nType)
        {
        case eTypeOk:
            but[0] = CFSTR("Ok"); flag[0] = eResultOk;
            break;

        case eTypeOkCancel:
            but[0] = CFSTR("Ok"); but[1] = CFSTR("Cancel");
            flag[0] = eResultOk; flag[1] = eResultCancel;
            break;

        case eTypeAbortRetryIgnore:
            but[0] = CFSTR("Abort"); but[1] = CFSTR("Retry"); but[2] = CFSTR("Ignore");
            flag[0] = eResultAbort; flag[1] = eResultRetry; flag[2] = eResultIgnore;
            break;

        case eTypeYesNoCancel:
            but[0] = CFSTR("Yes"); but[1] = CFSTR("No"); but[2] = CFSTR("Cancel");
            flag[0] = eResultYes; flag[1] = eResultNo; flag[2] = eResultCancel;
            break;

        case eTypeYesNo:
            but[0] = CFSTR("Yes"); but[1] = CFSTR("No");
            flag[0] = eResultYes; flag[1] = eResultNo;
            break;

        case eTypeRetryCancel:
            but[0] = CFSTR("Retry"); but[1] = CFSTR("Cancel");
            flag[0] = eResultRetry; flag[1] = eResultCancel;
            break;
        }

        CFStringRef header_ref = CFStringCreateWithCString(NULL,lpszCaption,strlen(lpszCaption));
        CFStringRef message_ref = CFStringCreateWithCString(NULL,lpszText,strlen(lpszText));
        
        CFOptionFlags result;
        
        CFUserNotificationDisplayAlert(
            0,
            kCFUserNotificationNoteAlertLevel,
            NULL,
            NULL,
            NULL,
            header_ref,
            message_ref,
            but[0],
            but[1],
            but[2],
            &result
           );
        
        CFRelease(header_ref);
        CFRelease(message_ref);
        return (EResult)flag[result];
    }
};

#endif // TM_APPLE


#ifdef TM_GTK

struct CMessageBoxGtk
{
public:
    enum EResult
    {
        eResultNone = -1,
        eResultOk = 1,
        eResultCancel,
        eResultAbort,
        eResultRetry,
        eResultIgnore,
        eResultYes,
        eResultNo,
        eResultClose,
        eResultHelp,
    };

    enum EType
    {
        eTypeOk = 0,
        eTypeOkCancel,
        eTypeAbortRetryIgnore,
        eTypeYesNoCancel,
        eTypeYesNo,
        eTypeRetryCancel,

        eTypeIconHand = 0x0010,
        eTypeIconQuestion = 0x0020,
        eTypeIconExclamation = 0x0030,
        eTypeOkIconAsterisk = 0x0040,

        eTypeIconWarning = eTypeIconExclamation,
        eTypeIconError = eTypeIconHand,
        eTypeIconInformation = eTypeOkIconAsterisk,
        eTypeIconStop = eTypeIconHand,

        eTypeTypeMask = 0x000f,
        eTypeIconMask = 0x00f0,
    };

private:
    enum
    {
        eGtkResponseIgnore = 1,
    };

    static GtkMessageType _toMsgType(TMUInt nType)
    {
        switch (nType & eTypeIconMask)
        {
        case eTypeIconHand:
            return GTK_MESSAGE_ERROR;
        case eTypeIconQuestion:
            return GTK_MESSAGE_QUESTION;
        case eTypeIconExclamation:
            return GTK_MESSAGE_WARNING;
        case eTypeOkIconAsterisk:
        default:
            return GTK_MESSAGE_INFO;
        }
    }

    static EResult _toResult(int response)
    {
        switch (response)
        {
        case GTK_RESPONSE_OK:
        case GTK_RESPONSE_ACCEPT:
            return eResultOk;
        case GTK_RESPONSE_CANCEL:
            return eResultCancel;
        case GTK_RESPONSE_YES:
            return eResultYes;
        case GTK_RESPONSE_NO:
            return eResultNo;
        case GTK_RESPONSE_HELP:
            return eResultHelp;
        case GTK_RESPONSE_CLOSE:
            return eResultClose;
        case GTK_RESPONSE_REJECT:
            return eResultAbort;
        case GTK_RESPONSE_APPLY:
            return eResultRetry;
        case eGtkResponseIgnore:
            return eResultIgnore;
        default:
            return eResultNone;
        }
    }

#ifdef TM_GTK4
    static void _onResponse(GtkDialog* dialog, int response, void* userData)
    {
        int* outResponse = (int*)userData;
        *outResponse = response;
        gtk_window_destroy(GTK_WINDOW(dialog));
    }
#endif

public:
    static EResult done(GtkWindow* hOwner, TMCStr text, TMCStr caption = TM_T(""), TMUInt nType = eTypeOk)
    {
        GtkWidget* dialog = gtk_message_dialog_new(
            hOwner,
            GTK_DIALOG_MODAL,
            _toMsgType(nType),
            GTK_BUTTONS_NONE,
            "%s",
            text ? text : "");

        if (caption && caption[0] != '\0')
        {
            gtk_window_set_title(GTK_WINDOW(dialog), caption);
        }

        switch (nType & eTypeTypeMask)
        {
        case eTypeOk:
            gtk_dialog_add_button(GTK_DIALOG(dialog), "OK", GTK_RESPONSE_OK);
            break;
        case eTypeOkCancel:
            gtk_dialog_add_button(GTK_DIALOG(dialog), "OK", GTK_RESPONSE_OK);
            gtk_dialog_add_button(GTK_DIALOG(dialog), "Cancel", GTK_RESPONSE_CANCEL);
            break;
        case eTypeAbortRetryIgnore:
            gtk_dialog_add_button(GTK_DIALOG(dialog), "Abort", GTK_RESPONSE_REJECT);
            gtk_dialog_add_button(GTK_DIALOG(dialog), "Retry", GTK_RESPONSE_APPLY);
            gtk_dialog_add_button(GTK_DIALOG(dialog), "Ignore", eGtkResponseIgnore);
            break;
        case eTypeYesNoCancel:
            gtk_dialog_add_button(GTK_DIALOG(dialog), "Yes", GTK_RESPONSE_YES);
            gtk_dialog_add_button(GTK_DIALOG(dialog), "No", GTK_RESPONSE_NO);
            gtk_dialog_add_button(GTK_DIALOG(dialog), "Cancel", GTK_RESPONSE_CANCEL);
            break;
        case eTypeYesNo:
            gtk_dialog_add_button(GTK_DIALOG(dialog), "Yes", GTK_RESPONSE_YES);
            gtk_dialog_add_button(GTK_DIALOG(dialog), "No", GTK_RESPONSE_NO);
            break;
        case eTypeRetryCancel:
            gtk_dialog_add_button(GTK_DIALOG(dialog), "Retry", GTK_RESPONSE_APPLY);
            gtk_dialog_add_button(GTK_DIALOG(dialog), "Cancel", GTK_RESPONSE_CANCEL);
            break;
        default:
            gtk_dialog_add_button(GTK_DIALOG(dialog), "OK", GTK_RESPONSE_OK);
            break;
        }

        int response = GTK_RESPONSE_NONE;
#ifdef TM_GTK4
        g_signal_connect(dialog, "response", G_CALLBACK(_onResponse), &response);
        gtk_window_present(GTK_WINDOW(dialog));
        while (response == GTK_RESPONSE_NONE)
        {
            g_main_context_iteration(0, TRUE);
        }
#else
        response = gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
#endif
        return _toResult(response);
    }
};

#endif // TM_GTK


#ifdef TM_QT

class CMessageBoxQt
{
public:
    enum EResult
    {
        eResultNone = QMessageBox::NoButton,
        eResultOk = QMessageBox::Ok,
        eResultCancel = QMessageBox::Cancel,
        eResultAbort = QMessageBox::Abort,
        eResultRetry = QMessageBox::Retry,
        eResultIgnore = QMessageBox::Ignore,
        eResultYes = QMessageBox::Yes,
        eResultNo = QMessageBox::No,
        eResultClose = QMessageBox::Close,
        eResultHelp = QMessageBox::Help,

        qResultSave = QMessageBox::Save,
        qResultSaveAll = QMessageBox::SaveAll,
        qResultOpen = QMessageBox::Open,
        qResultYesToAll = QMessageBox::YesToAll,
        qResultNoToAll = QMessageBox::NoToAll,
        qResultDiscard = QMessageBox::Discard,
        qResultApply = QMessageBox::Apply,
        qResultReset = QMessageBox::Reset,
        qResultRestoreDefaults = QMessageBox::RestoreDefaults,
    };

    enum EType
    {
        eTypeOk = QMessageBox::Ok,
        eTypeOkCancel = QMessageBox::Ok | QMessageBox::Cancel,
        eTypeAbortRetryIgnore = QMessageBox::Abort | QMessageBox::Retry | QMessageBox::Ignore,
        eTypeYesNoCancel = QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
        eTypeYesNo = QMessageBox::Yes | QMessageBox::No,
        eTypeRetryCancel = QMessageBox::Retry | QMessageBox::Cancel,

        qTypeSave = QMessageBox::Save,
        qTypeSaveAll = QMessageBox::SaveAll,
        qTypeOpen = QMessageBox::Open,
        qTypeYes = QMessageBox::Yes,
        qTypeYesToAll = QMessageBox::YesToAll,
        qTypeNo = QMessageBox::No,
        qTypeNoToAll = QMessageBox::NoToAll,
        qTypeAbort = QMessageBox::Abort,
        qTypeRetry = QMessageBox::Retry,
        qTypeIgnore = QMessageBox::Ignore,
        qTypeClose = QMessageBox::Close,
        qTypeCancel = QMessageBox::Cancel,
        qTypeDiscard = QMessageBox::Discard,
        qTypeHelp = QMessageBox::Help,
        qTypeApply = QMessageBox::Apply,
        qTypeReset = QMessageBox::Reset,
        qTypeRestoreDefaults = QMessageBox::RestoreDefaults,
        
        qTypeFirstButton = QMessageBox::FirstButton,
        qTypeLastButton = QMessageBox::LastButton,
        
        qTypeYesAll = QMessageBox::YesAll,
        qTypeNoAll = QMessageBox::NoAll,
        
        qTypeDefault = QMessageBox::Default,
        qTypeEscape = QMessageBox::Escape,
        qTypeFlagMask = QMessageBox::FlagMask,
        qTypeButtonMask = QMessageBox::ButtonMask,
    };

    static EResult done(QWidget* hOwner, QString lpszText, QString lpszCaption = QString(""), TMUInt nType = eTypeOk)
    {
        int rb = QMessageBox::information(hOwner, lpszCaption, lpszText, nType);
        return (EResult)rb;
    }

    static EResult done(QWidget* hOwner, TMCStr lpszText, TMCStr lpszCaption = TM_T(""), TMUInt nType = eTypeOk)
    {
        return done(hOwner, TM_T2Q(lpszCaption), TM_T2Q(lpszText), nType);
    }
};

#endif // TM_QT


#ifdef TM_QT
    typedef CMessageBoxQt       CMessageBox;
#elif defined (TM_APPLE)
    typedef CMessageBoxMac      CMessageBox;
#elif defined (TM_GTK)
    typedef CMessageBoxGtk      CMessageBox;
#elif defined (TM_WIN)
    typedef CMessageBoxWin      CMessageBox;
#else
#   error not support
#endif // #ifdef TM_QT #else


TM_END_NAMESPACE



#endif // ATINPUT_H
