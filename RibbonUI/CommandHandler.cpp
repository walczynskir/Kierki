// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.


#include "stdafx.h"

#include "CommandHandler.h"
#include "RibbonFramework.h"
#include "resource.h"
#include "HeartsData.h"
#include "TypeDefs.h"
#include "Kierki.h"

// Static method to create an instance of the object.
__checkReturn HRESULT CCommandHandler::CreateInstance(__deref_out IUICommandHandler **ppCommandHandler, HWND a_hAppWnd)
{
    if (!ppCommandHandler)
    {
        return E_POINTER;
    }

    if (a_hAppWnd == NULL)
    {
        return E_INVALIDARG;
    }

    *ppCommandHandler = NULL;

    HRESULT hr = S_OK;
   
    CCommandHandler* pCommandHandler = new CCommandHandler();
    pCommandHandler->SetWndApp(a_hAppWnd);

    if (pCommandHandler != NULL)
    {
        *ppCommandHandler = static_cast<IUICommandHandler *>(pCommandHandler);
        pCommandHandler->SetWndApp(a_hAppWnd);
    }
    else
    {
        hr = E_OUTOFMEMORY;
    }

    return hr;
}

// IUnknown method implementations.
STDMETHODIMP_(ULONG) CCommandHandler::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CCommandHandler::Release()
{
    LONG cRef = InterlockedDecrement(&m_cRef);
    if (cRef == 0)
    {
        delete this;
    }

    return cRef;
}

STDMETHODIMP CCommandHandler::QueryInterface(REFIID iid, void** ppv)
{
    if (iid == __uuidof(IUnknown))
    {
        *ppv = static_cast<IUnknown*>(this);
    }
    else if (iid == __uuidof(IUICommandHandler))
    {
        *ppv = static_cast<IUICommandHandler*>(this);
    }
    else 
    {
        *ppv = NULL;
        return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
}

//
//  FUNCTION: UpdateProperty()
//
//  PURPOSE: Called by the Ribbon framework when a command property (PKEY) needs to be updated.
//
//  COMMENTS:
//
//    This function is used to provide new command property values, such as labels, icons, or
//    tooltip information, when requested by the Ribbon framework.  
//    
//    In this SimpleRibbon sample, the method is not implemented.  
//
STDMETHODIMP CCommandHandler::UpdateProperty(
    UINT nCmdID,
    __in REFPROPERTYKEY key,
    __in_opt const PROPVARIANT* ppropvarCurrentValue,
    __out PROPVARIANT* ppropvarNewValue)
{
    UNREFERENCED_PARAMETER(nCmdID);
    UNREFERENCED_PARAMETER(key);
    UNREFERENCED_PARAMETER(ppropvarCurrentValue);
    UNREFERENCED_PARAMETER(ppropvarNewValue);

    return E_NOTIMPL;
}

//
//  FUNCTION: Execute()
//
//  PURPOSE: Called by the Ribbon framework when a command is executed by the user.  For example, when
//           a button is pressed.
//
STDMETHODIMP CCommandHandler::Execute(
    UINT nCmdID,
    UI_EXECUTIONVERB verb,
    __in_opt const PROPERTYKEY* key,
    __in_opt const PROPVARIANT* ppropvarValue,
    __in_opt IUISimplePropertySet* pCommandExecutionProperties)
{
    UNREFERENCED_PARAMETER(pCommandExecutionProperties);
    UNREFERENCED_PARAMETER(ppropvarValue);
    UNREFERENCED_PARAMETER(key);
    UNREFERENCED_PARAMETER(verb);
    UNREFERENCED_PARAMETER(nCmdID);


    switch (nCmdID)
    {
 
    case IDC_CMD_EXIT:
        PostQuitMessage(0);
        break;

    case IDC_GAME_NEW:
        OnGameNew();
        break;

    case IDC_GAME_OPEN:
        OnGameOpen();
        break;

    default:
        break;
    }

    return S_OK;
}


void CCommandHandler::OnGameNew()
{
    CHeartsData* l_pData = CHeartsData::GetData(m_hWndApp);
    if (l_pData->m_gameData.GetGame() != E_GM_NOTHING)
    {
        if (!DecisionBox(m_hWndApp, IDS_NEWGAME))
        {
            return;
        }
    }
    StartGame(m_hWndApp, false);
}

void CCommandHandler::OnGameOpen()
{
    CHeartsData* l_pData = CHeartsData::GetData(m_hWndApp);
    if (l_pData->m_gameData.GetGame() != E_GM_NOTHING)
    {
        if (DecisionBox(m_hWndApp, IDS_SAVEGAMEBEFOREOPENQUESTION))
        {
            SaveGame(m_hWndApp);
        }
    }
    TCHAR l_sFile[1024];
    if (!GetOpenSaveFile(m_hWndApp, true, l_sFile, ArraySize(l_sFile)))
    {
        return;
    }
    l_pData->m_gameData.Restore(l_sFile);
    StartGame(m_hWndApp, true);
}

