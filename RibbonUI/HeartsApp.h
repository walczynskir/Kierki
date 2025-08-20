#pragma once

//
//  CLASS: CHeartsApp : IUIApplication
//
//  PURPOSE: Implements interface IUIApplication that defines methods
//           required to manage Framework events.
//
//  COMMENTS:
//
//    CHeartsApp implements the IUIApplication interface which is required for any ribbon application.
//    IUIApplication contains callbacks made by the ribbon framework to the application
//    during various updates like command creation/destruction and view state changes.
//    Taken from MS provided examples 
//
class CHeartsRibbonApp
    : public IUIApplication // Applications must implement IUIApplication.
{
public:

    // Static method to create an instance of the object.
    static HRESULT CreateInstance(__deref_out IUIApplication** ppApplication);

    // IUnknown methods.
    STDMETHOD_(ULONG, AddRef());
    STDMETHOD_(ULONG, Release());
    STDMETHOD(QueryInterface(REFIID iid, void** ppv));

    // IUIApplication methods
    STDMETHOD(OnCreateUICommand)(UINT nCmdID,
        __in UI_COMMANDTYPE typeID,
        __deref_out IUICommandHandler** ppCommandHandler);

    STDMETHOD(OnViewChanged)(UINT viewId,
        __in UI_VIEWTYPE typeId,
        __in IUnknown* pView,
        UI_VIEWVERB verb,
        INT uReasonCode);

    STDMETHOD(OnDestroyUICommand)(UINT32 commandId,
        __in UI_COMMANDTYPE typeID,
        __in_opt IUICommandHandler* commandHandler);

    void SetAppWnd(HWND a_hWndApp) { m_hWndApp = a_hWndApp; };

private:
    CHeartsRibbonApp()
        : m_cRef(1)
        , m_pCommandHandler(NULL)
    {
    }

    ~CHeartsRibbonApp()
    {
        if (m_pCommandHandler)
        {
            m_pCommandHandler->Release();
            m_pCommandHandler = NULL;
        }
    }

	HWND m_hWndApp;                       // Handle to the main application window.


    LONG m_cRef;                            // Reference count.
    IUICommandHandler* m_pCommandHandler;  // Generic Command Handler
};
