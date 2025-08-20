// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.

#include "stdafx.h"
#include "RibbonFramework.h"
#include "HeartsApp.h"
#include "RibbonData.h"

const TCHAR cc_sRibbonBinary[] = L"HEARTS_RIBBON";
//
//  FUNCTION: InitializeFramework(HWND)
//
//  PURPOSE:  Initialize the Ribbon framework and bind a Ribbon to the application.
//
//  COMMENTS:
//
//    To get a Ribbon to display, the Ribbon framework must be initialized. 
//    This involves three important steps:
//      1) Instantiating the Ribbon framework object (CLSID_UIRibbonFramework).
//      2) Passing the host HWND and IUIApplication object to the framework.
//      3) Loading the binary markup compiled by UICC.exe.
//
//
bool InitializeFramework(HWND a_hWnd, RibbonData& a_ribbonData)
{
    // Here we instantiate the Ribbon framework object.
    HRESULT hr = CoCreateInstance(CLSID_UIRibbonFramework, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&(a_ribbonData.m_pFramework)));
    if (FAILED(hr))
    {
        return false;
    }   

    // Next, we create the application object (IUIApplication) and call the framework Initialize method, 
    // passing the application object and the host HWND that the Ribbon will attach itself to.
    hr = CHeartsRibbonApp::CreateInstance(&(a_ribbonData.m_pApplication));
    if (FAILED(hr))
    {
        return false;
    } 

    dynamic_cast<CHeartsRibbonApp*>(a_ribbonData.m_pApplication)->SetAppWnd(a_hWnd);
    hr = a_ribbonData.m_pFramework->Initialize(a_hWnd, a_ribbonData.m_pApplication);
    if (FAILED(hr))
    {
        return false;
    }

    // Finally, we load the binary markup.  This will initiate callbacks to the IUIApplication object 
    // that was provided to the framework earlier, allowing command handlers to be bound to individual
    // commands.
    hr = a_ribbonData.m_pFramework->LoadUI(::GetModuleHandle(NULL), cc_sRibbonBinary);
    if (FAILED(hr))
    {
        return false;
    }

    return true;
}

//
//  FUNCTION: DestroyFramework()
//
//  PURPOSE:  Tears down the Ribbon framework.
//
//
void DestroyFramework(RibbonData& a_ribbonData)
{
    if (a_ribbonData.m_pFramework)
    {
        a_ribbonData.m_pFramework->Destroy();
        a_ribbonData.m_pFramework->Release();
        a_ribbonData.m_pFramework = NULL;
    }

    if (a_ribbonData.m_pApplication)
    {
        a_ribbonData.m_pApplication->Release();
        a_ribbonData.m_pApplication = NULL;
    }
}




//
//  FUNCTION: GetRibbonHeight()
//
//  PURPOSE:  Get the ribbon height.
//
//
UINT GetRibbonHeight(RibbonData& a_ribbonData)
{
    UINT ribbonHeight = 0;
    if (a_ribbonData.m_pFramework)
    {
        IUIRibbon* pRibbon;
        a_ribbonData.m_pFramework->GetView(0, IID_PPV_ARGS(&pRibbon));
        pRibbon->GetHeight(&ribbonHeight);
        pRibbon->Release();
    }
    return ribbonHeight;
}