/*
 *  @file
 *  @brief  drag drop
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2009.02.18
 *
 *  Copyright (C) 2009 miragekiller
 */

#include "tmdrag.h"
#include "tmdui.h"

#include <gtk/gtk.h>

#include <cstdio>
#include <string>
#include <vector>


namespace
{

static GdkDragAction _TMToGdkDragAction(EDragEff effectMask)
{
    return (GdkDragAction)effectMask;
//     GdkDragAction action = (GdkDragAction)0;
//     if (effectMask & TM::CDragBase::e_efCopy)
//         action = (GdkDragAction)(action | GDK_ACTION_COPY);
//     if (effectMask & TM::CDragBase::e_efMove)
//         action = (GdkDragAction)(action | GDK_ACTION_MOVE);
//     if (effectMask & TM::CDragBase::e_efLink)
//         action = (GdkDragAction)(action | GDK_ACTION_LINK);
//     return action;
}

static EDragEff _TMFromGdkDragAction(GdkDragAction action)
{
    return (EDragEff)action;

//     DWORD effectMask = TM::CDragBase::e_efNone;
//     if (action & GDK_ACTION_COPY)
//         effectMask |= TM::CDragBase::e_efCopy;
//     if (action & GDK_ACTION_MOVE)
//         effectMask |= TM::CDragBase::e_efMove;
//     if (action & GDK_ACTION_LINK)
//         effectMask |= TM::CDragBase::e_efLink;
//     return effectMask;
}

static std::string _TMWideToUtf8(const wchar_t* text)
{
    if (!text)
        return std::string();

    gchar* utf8 = NULL;
    if (sizeof(wchar_t) == sizeof(gunichar))
        utf8 = g_ucs4_to_utf8(reinterpret_cast<const gunichar*>(text), -1, NULL, NULL, NULL);
    else
        utf8 = g_utf16_to_utf8(reinterpret_cast<const gunichar2*>(text), -1, NULL, NULL, NULL);

    std::string result = utf8 ? utf8 : "";
    g_free(utf8);
    return result;
}

static std::string _TMPathToUri(TMCStr path)
{
    if (!path || !path[0])
        return std::string();

    gchar* uri = g_filename_to_uri(path, NULL, NULL);
    std::string result = uri ? uri : "";
    g_free(uri);
    return result;
}

static std::string _TMMimeForFormat(WORD format)
{
    switch (format)
    {
    case TM::CDragBase::e_cfText:
    case TM::CDragBase::e_cfUnicodeText:
        return "text/plain;charset=utf-8";
    case TM::CDragBase::e_cfHDROP:
        return "text/uri-list";
    case TM::CDragBase::e_cfHtml:
        return "text/html";
    case TM::CDragBase::e_cfRtf:
        return "text/rtf";
    case TM::CDragBase::e_cfPng:
        return "image/png";
    case TM::CDragBase::e_cfJpeg:
        return "image/jpeg";
    case TM::CDragBase::e_cfDib:
    case TM::CDragBase::e_cfBitmap:
        return "image/bmp";
    case TM::CDragBase::e_cfTiff:
        return "image/tiff";
    default:
        break;
    }

    char name[64] = {0};
    std::snprintf(name, sizeof(name), "application/x-tm-format-%u", (unsigned)format);
    return std::string(name);
}

static size_t _TMTextLength(const char* text, size_t bytes)
{
    size_t count = 0;
    while (count < bytes && text[count] != '\0')
        ++count;
    return count;
}

struct TM_DragPayload
{
    WORD format;
    DWORD aspect;
    LONG index;
    std::string mime;
    std::string bytes;
};

struct TM_GtkDropObject
{
    guint32 magic;
    WORD format;
    DWORD aspect;
    LONG index;
    std::string mime;
    std::string bytes;
};

struct TM_GtkDropBundle
{
    guint32 magic;
    std::vector<TM_GtkDropObject*> items;
    std::vector<std::string> mimeTypes;
    std::vector<WORD> formatIds;

    TM_GtkDropBundle() : magic(0x544D4442) {}

    ~TM_GtkDropBundle()
    {
        Clear();
    }

    void Clear()
    {
        for (size_t i = 0; i < items.size(); ++i)
            if (items[i])
                delete items[i];
        items.clear();
        mimeTypes.clear();
        formatIds.clear();
    }

    void AddObject(TM_GtkDropObject* obj)
    {
        if (!obj)
            return;
        items.push_back(obj);
        mimeTypes.push_back(obj->mime);
        formatIds.push_back(obj->format);
    }

    TM_GtkDropObject* FindByMime(const std::string& mime) const
    {
        for (size_t i = 0; i < mimeTypes.size(); ++i)
            if (mimeTypes[i] == mime)
                return items[i];
        return NULL;
    }

    TM_GtkDropObject* FindByFormat(WORD fmt, bool(*matchFn)(const std::string&, WORD)) const
    {
        for (size_t i = 0; i < formatIds.size(); ++i)
        {
            if (formatIds[i] == fmt)
                return items[i];
            if (matchFn && matchFn(mimeTypes[i], fmt))
                return items[i];
        }
        return NULL;
    }

    size_t GetItemCount() const { return items.size(); }
    const std::string& GetMimeAt(size_t idx) const { return mimeTypes[idx < mimeTypes.size() ? idx : 0]; }
    WORD GetFormatAt(size_t idx) const { return formatIds[idx < formatIds.size() ? idx : 0]; }
};

static const guint32 s_tmDropMagic = 0x544D4450; // "TMDP"
static const guint32 s_tmBundleMagic = 0x544D4442; // "TMDB"



struct TM_GtkDropRegister
{
    GtkWidget* widget;
    TM::IDropTarget* dropTarget;
    TM_GtkDropBundle* dragData;
    GdkDragContext* activeContext;
    GArray* targets;
    gsize requestIndex;
    DWORD keyState;
    DWORD effect;
    POINTL point;
    guint time;
    bool entered;
    bool dropPending;
    gulong motionHandler;
    gulong leaveHandler;
    gulong dropHandler;
    gulong dataReceivedHandler;

    TM_GtkDropRegister()
        : widget(NULL)
        , dropTarget(NULL)
        , dragData(NULL)
        , activeContext(NULL)
        , targets(NULL)
        , requestIndex(0)
        , keyState(TM::CDragBase::e_efNone)
        , effect(TM::CDragBase::e_efNone)
        , point()
        , time(0)
        , entered(false)
        , dropPending(false)
        , motionHandler(0)
        , leaveHandler(0)
        , dropHandler(0)
        , dataReceivedHandler(0)
    {
    }

    ~TM_GtkDropRegister()
    {
        if (dropTarget)
            dropTarget->Release();
        if (dragData)
            delete dragData;
        if (targets)
            g_array_unref(targets);
    }
};

static const char* s_tmGtkDropRegisterKey = "tm.drag.register";

static void _TMGtkDropRegisterDestroy(gpointer data)
{
    TM_GtkDropRegister* reg = static_cast<TM_GtkDropRegister*>(data);
    if (!reg)
        return;
    if (reg->widget)
    {
        if (reg->motionHandler)
            g_signal_handler_disconnect(reg->widget, reg->motionHandler);
        if (reg->leaveHandler)
            g_signal_handler_disconnect(reg->widget, reg->leaveHandler);
        if (reg->dropHandler)
            g_signal_handler_disconnect(reg->widget, reg->dropHandler);
        if (reg->dataReceivedHandler)
            g_signal_handler_disconnect(reg->widget, reg->dataReceivedHandler);
    }
    delete reg;
}

static TM_GtkDropObject* _TMToDropObject(IDataObject* data)
{
    TM_GtkDropObject* obj = reinterpret_cast<TM_GtkDropObject*>(data);
    if (!obj)
        return NULL;
    if (obj->magic != s_tmDropMagic)
        return NULL;
    return obj;
}

static TM_GtkDropBundle* _TMToDropBundle(IDataObject* data)
{
    TM_GtkDropBundle* bundle = reinterpret_cast<TM_GtkDropBundle*>(data);
    if (!bundle)
        return NULL;
    if (bundle->magic != s_tmBundleMagic)
        return NULL;
    return bundle;
}

static DWORD _TMKeyStateFromGtk()
{
    GdkModifierType state = (GdkModifierType)0;
    DWORD keyState = 0;
    if (gtk_get_current_event_state(&state))
    {
        if (state & GDK_SHIFT_MASK)
            keyState |= MK_SHIFT;
        if (state & GDK_CONTROL_MASK)
            keyState |= MK_CONTROL;
    }
    return keyState;
}

static POINTL _TMPointLFromGtk(gint x, gint y)
{
    POINTL pt = { x, y };
    return pt;
}

static TM_GtkDropBundle* _TMBuildDropBundleFromContext(GdkDragContext* context)
{
    if (!context)
        return NULL;

    TM_GtkDropBundle* bundle = new TM_GtkDropBundle();
    GList* targets = gdk_drag_context_list_targets(context);
    for (GList* it = targets; it != NULL; it = it->next)
    {
        GdkAtom target = (GdkAtom)it->data;
        const gchar* targetName = gdk_atom_name(target);
        if (!targetName)
            continue;

        TM_GtkDropObject* obj = new TM_GtkDropObject();
        obj->magic = s_tmDropMagic;
        obj->aspect = DVASPECT_CONTENT;
        obj->index = -1;
        obj->mime = targetName;
        obj->format = _TMFormatFromMime(obj->mime);
        bundle->AddObject(obj);
        g_free((gpointer)targetName);
    }
    return bundle;
}

static GArray* _TMTargetAtomsFromContext(GdkDragContext* context)
{
    GArray* targets = g_array_new(FALSE, FALSE, sizeof(GdkAtom));
    if (!context)
        return targets;

    for (GList* it = gdk_drag_context_list_targets(context); it != NULL; it = it->next)
    {
        GdkAtom target = (GdkAtom)it->data;
        g_array_append_val(targets, target);
    }
    return targets;
}

static bool _TMIsTextLikeFormat(WORD fmt)
{
    return fmt == TM::CDragBase::e_cfText
        || fmt == TM::CDragBase::e_cfUnicodeText
        || fmt == TM::CDragBase::e_cfHtml
        || fmt == TM::CDragBase::e_cfRtf;
}

static bool _TMMimeMatchesFormat(const std::string& mime, WORD fmt)
{
    if (mime.empty())
        return false;

    if (fmt == TM::CDragBase::e_cfHDROP)
        return g_str_has_prefix(mime.c_str(), "text/uri-list");

    if (_TMIsTextLikeFormat(fmt))
        return g_str_has_prefix(mime.c_str(), "text/") || g_str_has_prefix(mime.c_str(), "UTF8_STRING");

    if (fmt == TM::CDragBase::e_cfPng)
        return g_str_has_prefix(mime.c_str(), "image/png");
    if (fmt == TM::CDragBase::e_cfJpeg)
        return g_str_has_prefix(mime.c_str(), "image/jpeg") || g_str_has_prefix(mime.c_str(), "image/jpg");
    if (fmt == TM::CDragBase::e_cfTiff)
        return g_str_has_prefix(mime.c_str(), "image/tiff");
    if (fmt == TM::CDragBase::e_cfDib || fmt == TM::CDragBase::e_cfBitmap)
        return g_str_has_prefix(mime.c_str(), "image/");

    return mime == _TMMimeForFormat(fmt);
}

static WORD _TMFormatFromMime(const std::string& mime)
{
    if (g_str_has_prefix(mime.c_str(), "text/uri-list"))
        return TM::CDragBase::e_cfHDROP;
    if (g_str_has_prefix(mime.c_str(), "text/html"))
        return TM::CDragBase::e_cfHtml;
    if (g_str_has_prefix(mime.c_str(), "text/rtf"))
        return TM::CDragBase::e_cfRtf;
    if (g_str_has_prefix(mime.c_str(), "image/png"))
        return TM::CDragBase::e_cfPng;
    if (g_str_has_prefix(mime.c_str(), "image/jpeg") || g_str_has_prefix(mime.c_str(), "image/jpg"))
        return TM::CDragBase::e_cfJpeg;
    if (g_str_has_prefix(mime.c_str(), "image/tiff"))
        return TM::CDragBase::e_cfTiff;
    if (g_str_has_prefix(mime.c_str(), "image/"))
        return TM::CDragBase::e_cfDib;
    if (g_str_has_prefix(mime.c_str(), "text/") || g_str_has_prefix(mime.c_str(), "UTF8_STRING"))
        return TM::CDragBase::e_cfUnicodeText;
    return TM::CDragBase::e_cfNull;
}

static TM_GtkDropObject* _TMBuildDropObject(const GtkSelectionData* sel)
{
    if (!sel)
        return NULL;

    TM_GtkDropObject* obj = new TM_GtkDropObject();
    obj->magic = s_tmDropMagic;
    obj->aspect = DVASPECT_CONTENT;
    obj->index = -1;

    GdkAtom target = gtk_selection_data_get_target(const_cast<GtkSelectionData*>(sel));
    const gchar* targetName = gdk_atom_name(target);
    if (targetName)
        obj->mime = targetName;
    g_free((gpointer)targetName);

    obj->format = _TMFormatFromMime(obj->mime);

    gint length = gtk_selection_data_get_length(const_cast<GtkSelectionData*>(sel));
    const guchar* bytes = gtk_selection_data_get_data(const_cast<GtkSelectionData*>(sel));
    if (length > 0 && bytes)
        obj->bytes.assign(reinterpret_cast<const char*>(bytes), reinterpret_cast<const char*>(bytes) + length);

    return obj;
}

static std::string _TMDropTextUtf8(const TM_GtkDropObject* obj)
{
    if (!obj)
        return std::string();

    if (obj->mime == "text/uri-list")
    {
        gchar** uris = g_uri_list_extract_uris(obj->bytes.c_str());
        if (!uris || !uris[0])
        {
            g_strfreev(uris);
            return std::string();
        }
        std::string first = uris[0];
        g_strfreev(uris);
        return first;
    }

    if (!obj->bytes.empty())
        return obj->bytes;

    return std::string();
}

static bool _TMAssignUtf8ToWide(const std::string& utf8, TM::CStringW* str)
{
    if (!str)
        return false;

    if (utf8.empty())
    {
        str->Empty();
        return false;
    }

    if (sizeof(wchar_t) == sizeof(gunichar))
    {
        glong itemsWritten = 0;
        gunichar* w = g_utf8_to_ucs4(utf8.c_str(), -1, NULL, &itemsWritten, NULL);
        if (!w)
        {
            str->Empty();
            return false;
        }
        TM::CStringW tmp;
        tmp.assign(reinterpret_cast<const wchar_t*>(w), (size_t)itemsWritten);
        str->swap(tmp);
        g_free(w);
        return true;
    }

    glong itemsWritten = 0;
    gunichar2* w16 = g_utf8_to_utf16(utf8.c_str(), -1, NULL, &itemsWritten, NULL);
    if (!w16)
    {
        str->Empty();
        return false;
    }
    TM::CStringW tmp;
    tmp.assign(reinterpret_cast<const wchar_t*>(w16), (size_t)itemsWritten);
    str->swap(tmp);
    g_free(w16);
    return true;
}

class TM_DragImpl : public TM::IDragApp
{
private:
    enum
    {
        e_infoText = 1,
        e_infoUris = 2,
        e_infoCustomBase = 1000,
    };

    std::string m_textUtf8;
    std::vector<std::string> m_fileUris;
    std::vector<TM_DragPayload> m_custom;
    TM::IDragSource* m_source;
    const void* m_key;
    GtkWidget* m_widget;
    GdkDragContext* m_context;
    GMainLoop* m_loop;
    DWORD m_resultEffect;
    TMPointI m_ptOffset;
    gulong m_dragDataGetHandler;
    gulong m_dragBeginHandler;
    gulong m_dragEndHandler;
    gulong m_dragFailedHandler;

    EDragEff m_effTarget;
    TM_GtkDropBundle* m_dragData;

    void ResetRuntime()
    {
        m_source = NULL;
        m_key = NULL;
        m_widget = NULL;
        m_context = NULL;
        m_resultEffect = TM::CDragBase::e_efNone;
        m_dragDataGetHandler = 0;
        m_dragBeginHandler = 0;
        m_dragEndHandler = 0;
        m_dragFailedHandler = 0;
    }

    void DisconnectHandlers()
    {
        if (!m_widget)
            return;
        if (m_dragDataGetHandler)
            g_signal_handler_disconnect(m_widget, m_dragDataGetHandler);
        if (m_dragBeginHandler)
            g_signal_handler_disconnect(m_widget, m_dragBeginHandler);
        if (m_dragEndHandler)
            g_signal_handler_disconnect(m_widget, m_dragEndHandler);
        if (m_dragFailedHandler)
            g_signal_handler_disconnect(m_widget, m_dragFailedHandler);
        m_dragDataGetHandler = 0;
        m_dragBeginHandler = 0;
        m_dragEndHandler = 0;
        m_dragFailedHandler = 0;
    }

    void QuitLoop()
    {
        if (m_loop && g_main_loop_is_running(m_loop))
            g_main_loop_quit(m_loop);
    }

    GtkTargetList* CreateTargetList() const
    {
        GtkTargetList* list = gtk_target_list_new(NULL, 0);
        if (!m_textUtf8.empty())
            gtk_target_list_add_text_targets(list, e_infoText);
        if (!m_fileUris.empty())
            gtk_target_list_add_uri_targets(list, e_infoUris);
        for (size_t i = 0; i < m_custom.size(); ++i)
        {
            GdkAtom atom = gdk_atom_intern(m_custom[i].mime.c_str(), FALSE);
            gtk_target_list_add(list, atom, 0, e_infoCustomBase + (guint)i);
        }
        return list;
    }

    const TM_DragPayload* FindCustomPayload(guint info) const
    {
        if (info < e_infoCustomBase)
            return NULL;
        size_t index = (size_t)(info - e_infoCustomBase);
        if (index >= m_custom.size())
            return NULL;
        return &m_custom[index];
    }

    static void OnDragDataGet(GtkWidget* widget, GdkDragContext* context, GtkSelectionData* selectionData, guint info, guint time, gpointer userData)
    {
        tm_unused(widget);
        tm_unused(context);
        tm_unused(time);

        TM_DragImpl* self = static_cast<TM_DragImpl*>(userData);
        if (!self)
            return;

        if (info == e_infoText && !self->m_textUtf8.empty())
        {
            gtk_selection_data_set_text(selectionData, self->m_textUtf8.c_str(), (gint)self->m_textUtf8.size());
            return;
        }

        if (info == e_infoUris && !self->m_fileUris.empty())
        {
            gchar** uris = g_new0(gchar*, self->m_fileUris.size() + 1);
            for (gsize i = 0; i < self->m_fileUris.size(); ++i)
                uris[i] = g_strdup(self->m_fileUris[i].c_str());
            gtk_selection_data_set_uris(selectionData, uris);
            g_strfreev(uris);
            return;
        }

        const TM_DragPayload* payload = self->FindCustomPayload(info);
        if (!payload)
            return;

        GdkAtom target = gtk_selection_data_get_target(selectionData);
        gtk_selection_data_set(selectionData, target, 8,
            reinterpret_cast<const guchar*>(payload->bytes.data()), (gint)payload->bytes.size());
    }

    static void OnDragBegin(GtkWidget* widget, GdkDragContext* context, gpointer userData)
    {
        tm_unused(widget);
        TM_DragImpl* self = static_cast<TM_DragImpl*>(userData);
        if (!self)
            return;
        self->m_context = context;
    }

    static void OnDragEnd(GtkWidget* widget, GdkDragContext* context, gpointer userData)
    {
        tm_unused(widget);
        TM_DragImpl* self = static_cast<TM_DragImpl*>(userData);
        if (!self)
            return;
        self->m_resultEffect = _TMFromGdkDragAction(gdk_drag_context_get_selected_action(context));
        self->m_context = NULL;
        self->QuitLoop();
    }

    static gboolean OnDragFailed(GtkWidget* widget, GdkDragContext* context, GtkDragResult result, gpointer userData)
    {
        tm_unused(widget);
        tm_unused(context);
        tm_unused(result);

        TM_DragImpl* self = static_cast<TM_DragImpl*>(userData);
        if (!self)
            return FALSE;
        self->m_resultEffect = TM::CDragBase::e_efNone;
        self->QuitLoop();
        return FALSE;
    }

public:
    TM_DragImpl()
        : m_source(NULL)
        , m_key(NULL)
        , m_widget(NULL)
        , m_context(NULL)
        , m_loop(NULL)
        , m_resultEffect(TM::CDragBase::e_efNone)
        , m_ptOffset()
        , m_dragDataGetHandler(0)
        , m_dragBeginHandler(0)
        , m_dragEndHandler(0)
        , m_dragFailedHandler(0)
    {
    }

    virtual void Init() tm_override
    {
        m_textUtf8.clear();
        m_fileUris.clear();
        m_custom.clear();
        ResetRuntime();
    }

    virtual void Close() tm_override
    {
        DisconnectHandlers();
        if (m_widget)
            gtk_drag_source_unset(m_widget);
        if (m_loop)
        {
            g_main_loop_unref(m_loop);
            m_loop = NULL;
        }
        m_textUtf8.clear();
        m_fileUris.clear();
        m_custom.clear();
        ResetRuntime();
    }

    virtual void AddMem(const void* pMem, size_t s, WORD fmt, DWORD dwAspect, LONG lindex) tm_override
    {
        if (!pMem || s == 0)
            return;

        if (fmt == TM::CDragBase::e_cfText)
        {
            const char* text = static_cast<const char*>(pMem);
            m_textUtf8.assign(text, _TMTextLength(text, s));
            return;
        }

        TM_DragPayload payload;
        payload.format = fmt;
        payload.aspect = dwAspect;
        payload.index = lindex;
        payload.mime = _TMMimeForFormat(fmt);
        payload.bytes.assign(static_cast<const char*>(pMem), static_cast<const char*>(pMem) + s);
        m_custom.push_back(payload);
    }

    virtual void AddGlobal(HGLOBAL hGlb, WORD fmt, DWORD dwAspect, LONG lindex) tm_override
    {
        tm_unused(hGlb);
        tm_unused(fmt);
        tm_unused(dwAspect);
        tm_unused(lindex);
    }

    virtual void AddTextA(const char* pStr, WORD fmt, DWORD dwAspect, LONG lindex) tm_override
    {
        tm_unused(fmt);
        tm_unused(dwAspect);
        tm_unused(lindex);
        m_textUtf8 = pStr ? pStr : "";
    }

    virtual void AddTextW(const wchar_t* pStr, WORD fmt, DWORD dwAspect, LONG lindex) tm_override
    {
        tm_unused(fmt);
        tm_unused(dwAspect);
        tm_unused(lindex);
        m_textUtf8 = _TMWideToUtf8(pStr);
    }

    virtual void AddTextAW(TMCStr pStr, DWORD dwAspect, LONG lindex) tm_override
    {
        tm_unused(dwAspect);
        tm_unused(lindex);
#ifdef TM_UNICODE
        AddTextW(pStr, TM::CDragBase::e_cfUnicodeText, DVASPECT_CONTENT, 1);
#else
        AddTextA(pStr, TM::CDragBase::e_cfText, DVASPECT_CONTENT, 1);
#endif
    }

    virtual void AddMeta(HENHMETAFILE hMeta, WORD fmt, DWORD dwAspect, LONG lindex) tm_override
    {
        tm_unused(hMeta);
        tm_unused(fmt);
        tm_unused(dwAspect);
        tm_unused(lindex);
    }

    virtual void AddStream(IStream* pStream, WORD fmt, DWORD dwAspect, LONG lindex) tm_override
    {
        tm_unused(pStream);
        tm_unused(fmt);
        tm_unused(dwAspect);
        tm_unused(lindex);
    }

    virtual void AddFiles(TMCStr pFiles, DWORD dwAspect, LONG lindex) tm_override
    {
        tm_unused(dwAspect);
        tm_unused(lindex);
        m_fileUris.clear();
        if (!pFiles)
            return;

        const TCHAR* path = pFiles;
        while (*path)
        {
            std::string uri = _TMPathToUri(path);
            if (!uri.empty())
                m_fileUris.push_back(uri);
            path += StrLen(path) + 1;
        }
    }

    virtual void AddFileArray(TMCStr* ppFiles, size_t dwCount, DWORD dwAspect, LONG lindex) tm_override
    {
        tm_unused(dwAspect);
        tm_unused(lindex);
        m_fileUris.clear();
        if (!ppFiles)
            return;

        for (size_t i = 0; i < dwCount && ppFiles[i]; ++i)
        {
            std::string uri = _TMPathToUri(ppFiles[i]);
            if (!uri.empty())
                m_fileUris.push_back(uri);
        }
    }

    virtual void SetImage(TMPointI ptOffset, TMHDC hDC, const TMRectI& rc, BYTE alpha, TMBool bSrcAlpha, TMColor clrKey) tm_override
    {
        tm_unused(hDC);
        tm_unused(rc);
        tm_unused(alpha);
        tm_unused(bSrcAlpha);
        tm_unused(clrKey);
        m_ptOffset = ptOffset;
    }

    virtual TMBool ShowImage(TMBool bShow) tm_override
    {
        tm_unused(bShow);
        return false;
    }

    virtual DWORD DoDrag(const void* key, TM::IDragSource* source, EDragEff dwEffMask) tm_override
    {
        if (!key)
            return TM::CDragBase::e_efNone;
        if (m_textUtf8.empty() && m_fileUris.empty() && m_custom.empty())
            return TM::CDragBase::e_efNone;

        TMHWnd hNative = TM::tuiGetHWND(reinterpret_cast<HTUI>(const_cast<void*>(key)));
        GtkWidget* widget = hNative ? GTK_WIDGET(hNative) : NULL;
        if (!widget)
            return TM::CDragBase::e_efNone;

        DisconnectHandlers();
        if (m_widget)
            gtk_drag_source_unset(m_widget);
        if (m_loop)
        {
            g_main_loop_unref(m_loop);
            m_loop = NULL;
        }

        GtkTargetList* targetList = CreateTargetList();
        GdkDragAction actions = _TMToGdkDragAction(dwEffMask);
        gtk_drag_source_set(widget, GDK_BUTTON1_MASK, NULL, 0, actions);
        gtk_drag_source_set_target_list(widget, targetList);

        m_widget = widget;
        m_source = source;
        m_key = key;
        m_resultEffect = TM::CDragBase::e_efNone;

        m_dragDataGetHandler = g_signal_connect(widget, "drag-data-get", G_CALLBACK(OnDragDataGet), this);
        m_dragBeginHandler = g_signal_connect(widget, "drag-begin", G_CALLBACK(OnDragBegin), this);
        m_dragEndHandler = g_signal_connect(widget, "drag-end", G_CALLBACK(OnDragEnd), this);
        m_dragFailedHandler = g_signal_connect(widget, "drag-failed", G_CALLBACK(OnDragFailed), this);

        GdkDragContext* context = gtk_drag_begin_with_coordinates(widget, targetList, actions, 1, NULL, m_ptOffset.x, m_ptOffset.y);
        gtk_target_list_unref(targetList);
        if (!context)
        {
            DisconnectHandlers();
            gtk_drag_source_unset(widget);
            ResetRuntime();
            return TM::CDragBase::e_efNone;
        }

        m_context = context;
        m_loop = g_main_loop_new(NULL, FALSE);
        g_main_loop_run(m_loop);

        DWORD effect = m_resultEffect;
        DisconnectHandlers();
        gtk_drag_source_unset(widget);
        g_main_loop_unref(m_loop);
        m_loop = NULL;
        ResetRuntime();
        return effect;
    }

    virtual TMBool Stop(const void* key) tm_override
    {
        if (key != m_key)
            return false;
        if (m_context)
            gtk_drag_cancel(m_context);
        QuitLoop();
        return true;
    }
};

static TM_DragImpl s_ddImpl;

} // namespace

TM::IDragApp* DragGetApp()
{
    return &s_ddImpl;
}

static void _TMGtkDropRegisterRequestNext(TM_GtkDropRegister* reg)
{
    if (!reg || !reg->widget || !reg->activeContext || !reg->targets)
        return;

    if (reg->requestIndex >= reg->targets->len)
    {
        DWORD effect = reg->effect;
        reg->dropTarget->Drop(reinterpret_cast<IDataObject*>(reg->dragData), reg->keyState, reg->point, &effect);
        gtk_drag_finish(reg->activeContext, effect != DROPEFFECT_NONE, FALSE, reg->time);
        reg->dropPending = false;
        return;
    }

    GdkAtom target = g_array_index(reg->targets, GdkAtom, reg->requestIndex++);
    gtk_drag_get_data(reg->widget, reg->activeContext, target, reg->time);
}

struct IDropTargetImpl
{
    struct STATE
    {
        COMPointL pt;
        COMDword  ks;

        STATE(gint x, gint y)
        {
            pt.x = x;
            pt.y = y;
            ks = _TMKeyStateFromGtk();
        }
    };

    static gboolean _TMGtkOnDragMotion(GtkWidget* widget, GdkDragContext* context, gint x, gint y, guint time, gpointer userData)
    {
        DbgAssert(userData);

        TM_GtkDropRegister* reg = static_cast<TM_GtkDropRegister*>(userData);
        if (!reg || !reg->dropTarget)
            return FALSE;

        STATE st(x, y);
        EDragEff eff = _TMFromGdkDragAction(gdk_drag_context_get_suggested_action(context));
        if (eff == DROPEFFECT_NONE)
            eff = _TMFromGdkDragAction(gdk_drag_context_get_actions(context));

        reg->widget = widget;
        reg->activeContext = context;
        reg->point = _TMPointLFromGtk(x, y);
        reg->keyState = st.ks;

        if (!reg->dropTarget->m_bEntry)
        {
            if (reg->dragData)
                delete reg->dragData;
            reg->dragData = _TMBuildDropBundleFromContext(context);
            reg->dropTarget->m_bEntry = true;
            reg->entered = true;
            reg->dropTarget->DragEnter(reinterpret_cast<IDataObject*>(reg->dragData), st.ks, st.pt, &eff);
        }
        else
            reg->dropTarget->DragOver(st.ks, st.pt, &eff);

        reg->effect = eff;
        gdk_drag_status(context, _TMToGdkDragAction(eff), time);
        return eff != DROPEFFECT_NONE;
    }

    static void _TMGtkOnDragLeave(GtkWidget* widget, GdkDragContext* context, guint time, gpointer userData)
    {
        tm_unused(widget);
        tm_unused(context);
        tm_unused(time);
        DbgAssert(userData);

        TM_GtkDropRegister* reg = static_cast<TM_GtkDropRegister*>(userData);
        if (!reg || !reg->dropTarget)
            return;

        reg->dropTarget->m_bEntry = false;
        reg->entered = false;
        reg->dropTarget->DragLeave();
    }

static gboolean _TMGtkOnDragDrop(GtkWidget* widget, GdkDragContext* context, gint x, gint y, guint time, gpointer userData)
{
    DbgAssert(userData);

    TM_GtkDropRegister* reg = static_cast<TM_GtkDropRegister*>(userData);
    if (!reg || !reg->dropTarget)
        return FALSE;

    reg->dropTarget->m_bEntry = false;
    reg->entered = false;

    EDragEff eff = _TMFromGdkDragAction(gdk_drag_context_get_suggested_action(context));
    if (eff == DROPEFFECT_NONE)
        eff = _TMFromGdkDragAction(gdk_drag_context_get_actions(context));
    reg->effect = eff;

    if (reg->dragData)
    {
        delete reg->dragData;
        reg->dragData = new TM_GtkDropBundle();
    }
    else
    {
        reg->dragData = new TM_GtkDropBundle();
    }
    if (reg->targets)
        g_array_unref(reg->targets);

    reg->widget = widget;
    reg->activeContext = context;
    reg->point = _TMPointLFromGtk(x, y);
    reg->keyState = _TMKeyStateFromGtk();
    reg->time = time;
    reg->requestIndex = 0;
    reg->dropPending = true;
    reg->targets = _TMTargetAtomsFromContext(context);

    if (!reg->targets || reg->targets->len == 0)
    {
        gtk_drag_finish(context, FALSE, FALSE, time);
        reg->dropPending = false;
        return FALSE;
    }

    _TMGtkDropRegisterRequestNext(reg);
    return TRUE;
}

static void _TMGtkOnDragDataReceived(GtkWidget* widget, GdkDragContext* context, gint x, gint y, GtkSelectionData* data, guint info, guint time, gpointer userData)
{
    tm_unused(widget);
    tm_unused(context);
    tm_unused(x);
    tm_unused(y);
    tm_unused(info);
    tm_unused(time);

    TM_GtkDropRegister* reg = static_cast<TM_GtkDropRegister*>(userData);
    if (!reg || !reg->dropPending || !reg->dragData)
        return;

    TM_GtkDropObject* obj = _TMBuildDropObject(data);
    if (obj)
        reg->dragData->AddObject(obj);

    _TMGtkDropRegisterRequestNext(reg);
}
};

TM_CAPI TMBool DragRegisterGtk(TMHWnd hWnd, TM::IDropTarget* dt)
{
    DbgAssert(hWnd && dt);
    GtkWidget* widget = hWnd ? GTK_WIDGET(hWnd) : NULL;

    TM_GtkDropRegister* reg = new TM_GtkDropRegister();
    reg->widget = widget;
    reg->dropTarget = dt;
    reg->dropTarget->AddRef();

    GdkDragAction actions = (GdkDragAction)(GDK_ACTION_COPY | GDK_ACTION_MOVE | GDK_ACTION_LINK);
    gtk_drag_dest_set(widget,
        (GtkDestDefaults)(GTK_DEST_DEFAULT_MOTION | GTK_DEST_DEFAULT_HIGHLIGHT | GTK_DEST_DEFAULT_DROP),
        NULL, 0, actions);

    GtkTargetList* list = gtk_target_list_new(NULL, 0);
    gtk_target_list_add_text_targets(list, 1);
    gtk_target_list_add_uri_targets(list, 2);
    gtk_drag_dest_set_target_list(widget, list);
    gtk_target_list_unref(list);

    reg->motionHandler = g_signal_connect(widget, "drag-motion", G_CALLBACK(IDropTargetImpl::_TMGtkOnDragMotion), reg);
    reg->leaveHandler = g_signal_connect(widget, "drag-leave", G_CALLBACK(IDropTargetImpl::_TMGtkOnDragLeave), reg);
    reg->dropHandler = g_signal_connect(widget, "drag-drop", G_CALLBACK(IDropTargetImpl::_TMGtkOnDragDrop), reg);
    reg->dataReceivedHandler = g_signal_connect(widget, "drag-data-received", G_CALLBACK(IDropTargetImpl::_TMGtkOnDragDataReceived), reg);
    g_object_set_data_full(G_OBJECT(widget), s_tmGtkDropRegisterKey, reg, _TMGtkDropRegisterDestroy);
    return true;
}

TM_CAPI TMBool DragRevokeGtk(TMHWnd hWnd)
{
    GtkWidget* widget = hWnd ? GTK_WIDGET(hWnd) : NULL;
    if (!widget)
        return false;

    TM_GtkDropRegister* reg = static_cast<TM_GtkDropRegister*>(g_object_get_data(G_OBJECT(widget), s_tmGtkDropRegisterKey));
    if (reg && reg->dropTarget && reg->entered)
        reg->dropTarget->DragLeave();

    gtk_drag_dest_unset(widget);
    g_object_set_data(G_OBJECT(widget), s_tmGtkDropRegisterKey, NULL);
    return true;
}

TM_CAPI IDataObject* GtkDropDataCreate(const void* gtkSelectionData)
{
    return reinterpret_cast<IDataObject*>(
        _TMBuildDropObject(reinterpret_cast<const GtkSelectionData*>(gtkSelectionData)));
}

TM_CAPI void GtkDropDataDestroy(IDataObject* data)
{
    TM_GtkDropObject* obj = _TMToDropObject(data);
    if (!obj)
        return;
    delete obj;
}

static TM_GtkDropObject* _TMFindDropObject(IDataObject* data, WORD fmt)
{
    TM_GtkDropObject* single = _TMToDropObject(data);
    if (single)
    {
        if (_TMMimeMatchesFormat(single->mime, fmt))
            return single;
        return NULL;
    }

    TM_GtkDropBundle* bundle = _TMToDropBundle(data);
    if (!bundle)
        return NULL;
    return bundle->FindByFormat(fmt, &_TMMimeMatchesFormat);
}

TMBool DropGetData(IDataObject* data, void** ppv, WORD fmt, DWORD dwAspect, LONG lindex, DWORD tymed)
{
    tm_unused(dwAspect);
    tm_unused(lindex);

    if (!ppv)
        return false;
    *ppv = NULL;

    TM_GtkDropObject* obj = _TMFindDropObject(data, fmt);
    if (!obj)
        return false;
    if (!(tymed == TM::CDragBase::e_tpNull || tymed == TM::CDragBase::e_tpHGlobal))
        return false;

    // During DragEnter/DragOver we may only know available target types, not payload bytes yet.
    *ppv = obj->bytes.empty() ? NULL : (void*)obj->bytes.data();
    return true;
}

TMBool DropGetStringA(IDataObject* data, TM::CStringA* str, WORD fmt, DWORD dwAspect, LONG lindex)
{
    tm_unused(dwAspect);
    tm_unused(lindex);

    if (!str)
        return false;

    TM_GtkDropObject* obj = _TMFindDropObject(data, fmt);
    if (!obj || !_TMIsTextLikeFormat(fmt))
    {
        str->Empty();
        return false;
    }

    std::string utf8 = _TMDropTextUtf8(obj);
    if (utf8.empty())
    {
        str->Empty();
        return false;
    }

    TM::CStringA tmp;
    tmp.assign(utf8.c_str(), utf8.size());
    str->swap(tmp);
    return true;
}

TMBool DropGetStringW(IDataObject* data, TM::CStringW* str, WORD fmt, DWORD dwAspect, LONG lindex)
{
    tm_unused(dwAspect);
    tm_unused(lindex);

    TM_GtkDropObject* obj = _TMFindDropObject(data, fmt);
    if (!obj || !_TMIsTextLikeFormat(fmt))
    {
        if (str)
            str->Empty();
        return false;
    }

    return _TMAssignUtf8ToWide(_TMDropTextUtf8(obj), str);
}

static std::vector<std::string> _TMUriListToLocalPaths(const std::string& uriList)
{
    std::vector<std::string> paths;

    if (uriList.empty())
        return paths;

    gchar** uris = g_uri_list_extract_uris(uriList.c_str());
    if (!uris)
        return paths;

    for (gint i = 0; uris[i] != NULL; ++i)
    {
        gchar* path = g_filename_from_uri(uris[i], NULL, NULL);
        if (path)
        {
            paths.push_back(std::string(path));
            g_free(path);
        }
    }

    g_strfreev(uris);
    return paths;
}

TMBool DropGetUriList(IDataObject* data, TM::CStringA* pUriList)
{
    if (!pUriList)
        return false;

    TM_GtkDropObject* obj = _TMFindDropObject(data, TM::CDragBase::e_cfHDROP);
    if (!obj || obj->mime != "text/uri-list" || obj->bytes.empty())
    {
        pUriList->Empty();
        return false;
    }

    TM::CStringA tmp;
    tmp.assign(obj->bytes.c_str(), obj->bytes.size());
    pUriList->swap(tmp);
    return true;
}

TMBool DropGetLocalFileListW(IDataObject* data, TM::CStringW* pFileList)
{
    if (!pFileList)
        return false;

    TM_GtkDropObject* obj = _TMFindDropObject(data, TM::CDragBase::e_cfHDROP);
    if (!obj || obj->mime != "text/uri-list" || obj->bytes.empty())
    {
        pFileList->Empty();
        return false;
    }

    std::vector<std::string> filePaths = _TMUriListToLocalPaths(obj->bytes);
    if (filePaths.empty())
    {
        pFileList->Empty();
        return false;
    }

    std::string fileListUtf8;
    for (size_t i = 0; i < filePaths.size(); ++i)
    {
        fileListUtf8 += filePaths[i];
        fileListUtf8 += '\0';
    }
    fileListUtf8 += '\0';

    return _TMAssignUtf8ToWide(fileListUtf8, pFileList);
}

TMBool DropGetLocalFileListA(IDataObject* data, TM::CStringA* pFileList)
{
    if (!pFileList)
        return false;

    TM_GtkDropObject* obj = _TMFindDropObject(data, TM::CDragBase::e_cfHDROP);
    if (!obj || obj->mime != "text/uri-list" || obj->bytes.empty())
    {
        pFileList->Empty();
        return false;
    }

    std::vector<std::string> filePaths = _TMUriListToLocalPaths(obj->bytes);
    if (filePaths.empty())
    {
        pFileList->Empty();
        return false;
    }

    std::string fileList;
    for (size_t i = 0; i < filePaths.size(); ++i)
    {
        fileList += filePaths[i];
        fileList += '\0';
    }
    fileList += '\0';

    TM::CStringA tmp;
    tmp.assign(fileList.c_str(), fileList.size());
    pFileList->swap(tmp);
    return true;
}

// ---------- GTK Drop Bundle API (Multi-type enumeration) ----------

TM_CAPI IDataObject* GtkDropBundleCreate()
{
    return reinterpret_cast<IDataObject*>(new TM_GtkDropBundle());
}

TM_CAPI void GtkDropBundleAddObject(IDataObject* bundle, const void* gtkSelectionData)
{
    TM_GtkDropBundle* b = _TMToDropBundle(bundle);
    if (!b)
        return;

    TM_GtkDropObject* obj = _TMBuildDropObject(reinterpret_cast<const GtkSelectionData*>(gtkSelectionData));
    if (obj)
        b->AddObject(obj);
}

TM_CAPI void GtkDropBundleDestroy(IDataObject* bundle)
{
    TM_GtkDropBundle* b = _TMToDropBundle(bundle);
    if (!b)
        return;
    delete b;
}

TM_CAPI size_t GtkDropBundleGetItemCount(IDataObject* bundle)
{
    TM_GtkDropBundle* b = _TMToDropBundle(bundle);
    return b ? b->GetItemCount() : 0;
}

TM_CAPI const char* GtkDropBundleGetMimeAt(IDataObject* bundle, size_t index)
{
    TM_GtkDropBundle* b = _TMToDropBundle(bundle);
    if (!b || index >= b->GetItemCount())
        return "";
    return b->GetMimeAt(index).c_str();
}

TM_CAPI WORD GtkDropBundleGetFormatAt(IDataObject* bundle, size_t index)
{
    TM_GtkDropBundle* b = _TMToDropBundle(bundle);
    if (!b || index >= b->GetItemCount())
        return TM::CDragBase::e_cfNull;
    return b->GetFormatAt(index);
}

TM_CAPI TMBool GtkDropBundleHasFormat(IDataObject* bundle, WORD fmt)
{
    TM_GtkDropBundle* b = _TMToDropBundle(bundle);
    if (!b)
        return false;
    return b->FindByFormat(fmt, &_TMMimeMatchesFormat) != NULL;
}

TM_CAPI TMBool GtkDropBundleGetString(IDataObject* bundle, WORD fmt, TM::CStringW* str)
{
    TM_GtkDropBundle* b = _TMToDropBundle(bundle);
    if (!b || !str)
        return false;

    if (!str)
        return false;

    TM_GtkDropObject* obj = b->FindByFormat(fmt, &_TMMimeMatchesFormat);
    if (!obj || !_TMIsTextLikeFormat(fmt))
    {
        str->Empty();
        return false;
    }

    std::string utf8 = _TMDropTextUtf8(obj);
    if (utf8.empty())
    {
        str->Empty();
        return false;
    }

    return _TMAssignUtf8ToWide(utf8, str);
}

