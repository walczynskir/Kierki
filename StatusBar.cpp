#include "stdafx.h"
#include "StatusBar.h"
#include "resource.h"
#include "FontFactory.h"
#include <rcommon/RSystemExc.h>

#include <gdiplus.h>
using namespace Gdiplus;


CStatusBar::~CStatusBar()
{
    if (m_hIconLeft == nullptr)
    {
        ::DeleteObject(m_hIconLeft);
        m_hIconLeft = nullptr;
    }
    if (m_hIconRight == nullptr)
    {
        ::DeleteObject(m_hIconRight);
        m_hIconRight = nullptr;
    }

}



BOOL CStatusBar::Create(HWND hParent, HINSTANCE hInst, BYTE a_btStart, BYTE a_btEnd, int a_dxSlider, UINT a_idIconLeft, UINT a_idIconRight, COLORREF a_clrLabel)
{
    m_hWndParent = hParent;
    m_hInstance = hInst;
    m_dxSlider = a_dxSlider;
    m_clrLabel = a_clrLabel;

    m_hWndStatusbar = ::CreateWindowEx(0, STATUSCLASSNAME, nullptr,
        WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP,
        0, 0, 0, 0,
        m_hWndParent, nullptr, m_hInstance, nullptr);
    if (m_hWndStatusbar == nullptr)
        throw RSystemExc(_T("STATUSBAR_CREATE:MAIN"));

    m_hWndLabel = ::CreateWindowEx(0, _T("STATIC"), _T(""),
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        0, 0, 80, 20, m_hWndParent, nullptr, m_hInstance, nullptr);
    if (m_hWndLabel == nullptr)
        throw RSystemExc(_T("STATUSBAR_CREATE:LABEL"));

    SetFont();

    m_hWndIconLeft = ::CreateWindowEx(0, _T("STATIC"), nullptr,
        WS_CHILD | WS_VISIBLE | SS_ICON,
        0, 0, CW_USEDEFAULT, CW_USEDEFAULT, m_hWndParent, nullptr, m_hInstance, nullptr);

    if (m_hWndIconLeft == nullptr)
        throw RSystemExc(_T("STATUSBAR_CREATE:ICONLEFT"));

    if (a_idIconLeft > 0)
    {
        m_hIconLeft = ::LoadIcon(m_hInstance, MAKEINTRESOURCE(a_idIconLeft));
        if (m_hIconLeft != nullptr)
            ::SendMessage(m_hWndIconLeft, STM_SETICON, reinterpret_cast<WPARAM>(m_hIconLeft), 0);
    }

    m_hWndSlider = ::CreateWindowEx(0, TRACKBAR_CLASS, nullptr,
        WS_CHILD | WS_VISIBLE | TBS_NOTICKS | TBS_TOOLTIPS | TBS_FIXEDLENGTH,
        0, 0, m_dxSlider, CW_USEDEFAULT, m_hWndParent, nullptr, m_hInstance, nullptr);
    if (m_hWndSlider == nullptr)
        throw RSystemExc(_T("STATUSBAR_CREATE:SLIDER"));

    m_hWndIconRight = ::CreateWindowEx(0, _T("STATIC"), nullptr,
        WS_CHILD | WS_VISIBLE | SS_ICON,
        0, 0, CW_USEDEFAULT, CW_USEDEFAULT, m_hWndParent, nullptr, m_hInstance, nullptr);
    if (m_hWndIconRight == nullptr)
        throw RSystemExc(_T("STATUSBAR_CREATE:ICONRIGHT"));
    if (a_idIconRight > 0)
    {
        m_hIconRight = ::LoadIcon(m_hInstance, MAKEINTRESOURCE(a_idIconRight));
        if (m_hIconRight != nullptr)
            ::SendMessage(m_hWndIconRight, STM_SETICON, reinterpret_cast<WPARAM>(m_hIconRight), 0);
    }

    ::SendMessage(m_hWndSlider, TBM_SETRANGE, TRUE, MAKELONG(a_btStart, a_btEnd));
    ::SendMessage(m_hWndSlider, TBM_SETTHUMBLENGTH, 16, 0); 

    Resize();

    return TRUE;
}


void CStatusBar::SetLabelText(const tstring& a_sText)
{
    ::SetWindowText(m_hWndLabel, a_sText.c_str());
}

void CStatusBar::SetSliderRange(int min, int max)
{
    SendMessage(m_hWndSlider, TBM_SETRANGE, TRUE, MAKELONG(min, max));
}

void CStatusBar::SetSliderValue(int value)
{
    SendMessage(m_hWndSlider, TBM_SETPOS, TRUE, value);
}

int CStatusBar::GetSliderValue() const
{
    return SendMessage(m_hWndSlider, TBM_GETPOS, 0, 0);
}

void CStatusBar::Resize()
{
    // ask for  alignment of statusbar
    ::SendMessage(m_hWndStatusbar, WM_SIZE, 0, 0);

    PositionControls();

    // to make the slider accessible 
    ::SetWindowPos(m_hWndSlider, HWND_TOP, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

}


void CStatusBar::GetWindowRect(LPRECT a_pRect) const
{
    ::GetWindowRect(m_hWndStatusbar, a_pRect);
}

HWND CStatusBar::GetSliderTooltipWnd() const
{
    return reinterpret_cast<HWND>(::SendMessage(m_hWndSlider, TBM_GETTOOLTIPS, 0, 0));
}



LRESULT CStatusBar::OnTooltipNeedText(LPNMHDR a_pNmHdr)
{
    NMTTDISPINFO* l_info = reinterpret_cast<NMTTDISPINFO*>(a_pNmHdr);
    int l_iValue = GetSliderValue();

    TCHAR l_sTooltip[256];
    ::LoadString(::GetModuleHandle(NULL), IDS_SLIDER_TT, l_sTooltip, ArraySize(l_sTooltip));

    _sntprintf_s(l_info->szText, ArraySize(l_info->szText), _T("%s: %d %%"), l_sTooltip, static_cast<int>(l_iValue * 100 / GetSliderRangeMax() - GetSliderRangeMin())); // Custom text
    return TRUE;
}


void CStatusBar::Refresh()
{
    SetFont();
}



void CStatusBar::PositionControls()
{
    RECT l_rectStatusBar;
    ::SendMessage(m_hWndStatusbar, SB_GETRECT, 0, reinterpret_cast<LPARAM>(&l_rectStatusBar)); // second part, parts are 0-indexed

    POINT pt = { l_rectStatusBar.left, l_rectStatusBar.top };

    ::ClientToScreen(m_hWndStatusbar, &pt);
    ::ScreenToClient(m_hWndParent, &pt);

    int l_dxWidth = RectWidth(l_rectStatusBar) - 4;   // (-2 pix from both sides)
    int l_dyHeight = RectHeight(l_rectStatusBar) - 4; // (-2 pix from both sides)
    int l_dxIcon = l_dyHeight;

    int l_dxLabelWidth = l_dxWidth - l_dxIcon - m_dxSlider - l_dxIcon - 12; // (-4 px + -4px + -4px)

    int l_x = pt.x + 2;
    int l_y = pt.y + 2;
    // TODO defer window pos instead of Move
    ::MoveWindow(m_hWndLabel, l_x, l_y, l_dxLabelWidth, l_dyHeight, TRUE);

    l_x = l_x + l_dxLabelWidth + 4;
    ::MoveWindow(m_hWndIconLeft, l_x, l_y, l_dxIcon, l_dyHeight, TRUE);

    l_x = l_x + l_dxIcon + 4;
    ::MoveWindow(m_hWndSlider, l_x, l_y, m_dxSlider, l_dyHeight, TRUE);

    l_x = l_x + m_dxSlider + 4;
    ::MoveWindow(m_hWndIconRight, l_x, l_y, l_dxIcon, l_dyHeight, TRUE);

}

int CStatusBar::GetSliderRangeMin() const
{
    return ::SendMessage(m_hWndSlider, TBM_GETRANGEMIN, 0, 0);
}


int CStatusBar::GetSliderRangeMax() const
{
    return ::SendMessage(m_hWndSlider, TBM_GETRANGEMAX, 0, 0);

}


void CStatusBar::SetFont() 
{
    HFONT l_hFont = CFontFactory::Instance().GetFont(m_hWndLabel);
    ::SendMessage(m_hWndLabel, WM_SETFONT, reinterpret_cast<WPARAM>(l_hFont), 0);
}


LRESULT CStatusBar::OnLabelColor(HDC a_hDC)
{
    ::SetTextColor(a_hDC, m_clrLabel); 
    ::SetBkMode(a_hDC, OPAQUE);

    SetBkColor(a_hDC, GetSysColor(COLOR_3DFACE));
    return reinterpret_cast<LRESULT>(::GetSysColorBrush(COLOR_3DFACE));
}
