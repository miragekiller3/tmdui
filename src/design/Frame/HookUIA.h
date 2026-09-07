/*
 *  @file
 *  @brief
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2021.4.19
 *
 *  Copyright (C) 2021 miragekiller
 */

#pragma once

#ifndef HOOKUIA_H
#define HOOKUIA_H

#include "tmdef.h"


TM_BGN_NAMESPACE


class CHookUIA : public IUIAutomationEventHandler
{
public:
    CComPtr<IUIAutomation> pAutomation;
    CComPtr<IUIAutomationCacheRequest> pCacheRequest;

public:
    void AddHook()
    {
        pAutomation.CoCreateInstance(__uuidof(CUIAutomation), 0, CLSCTX_INPROC_SERVER);
        pEHTemp = new EventHandler();
        if (pEHTemp == NULL)
        {
            ret = 1; 
            goto cleanup;
        }
        HRESULT hr = pAutomation->CreateCacheRequest(&pCacheRequest);
        hr = pCacheRequest->AddProperty(UIA_NamePropertyId);
        
        wprintf(L"-Adding Event Handlers\n");
        
        IUIAutomationElement * proot=NULL;
        hr = pAutomation->GetRootElement(&proot);
        hr = pAutomation->AddAutomationEventHandler(UIA_Text_TextSelectionChangedEventId,proot,TreeScope_Subtree ,NULL,(IUIAutomationEventHandler *) pEHTemp);
        if (FAILED(hr)) 
        { 
            ret = 1;
            goto cleanup; 
        } 
        wprintf(L"-Press any key to remove event handler and exit\n");
        getchar(); 
        wprintf(L"-Removing Event Handlers.\n");
        hr = pAutomation->RemoveAutomationEventHandler(UIA_Text_TextSelectionChangedEventId,proot,(IUIAutomationEventHandler *)pEHTemp);
        if (FAILED(hr))
        {
            ret = 1;
            goto cleanup;
            
        }
        // Release resources and terminate.
cleanup:if (pEHTemp != NULL) 
            pEHTemp->Release();
        if (pAutomation != NULL) 
            pAutomation->Release();
        CoUninitialize();
        return ret;
} 
    }

};


TM_END_NAMESPACE



#endif //HOOKUIA_H
