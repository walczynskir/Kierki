#pragma once
#include <windows.h>
#include <commctrl.h>
#include <rcommon/rstring.h>

class CStatusBar {
public:
    ~CStatusBar();
    BOOL Create(HWND hParent, HINSTANCE hInst, BYTE a_btStart, BYTE a_btEnd, int a_dxSlider, UINT a_idIconLeft, UINT a_idIconRight, COLORREF a_clrLabel);
    void Resize(); // Call on WM_SIZE
    void SetLabelText(const tstring& a_sText);
    HWND GetLabelWnd() const { return m_hWndLabel; };

    void SetSliderRange(int min, int max);
    void SetSliderValue(int value);
    int  GetSliderValue() const;
    void SetSliderWidth(int a_iWidth) { m_dxSlider = a_iWidth; };
    HWND GetSliderWnd() const { return m_hWndSlider; };
    HWND GetSliderTooltipWnd() const;

    void GetWindowRect(LPRECT a_pRect) const;

    LRESULT OnTooltipNeedText(LPNMHDR a_pNmHdr);
    void Refresh();

    LRESULT OnLabelColor(HDC a_hDC);

private:
    HWND m_hWndParent = nullptr;
    HINSTANCE m_hInstance = nullptr;

    HWND m_hWndStatusbar = nullptr;
    HWND m_hWndLabel = nullptr;
    HWND m_hWndIconLeft = nullptr;
    HWND m_hWndSlider = nullptr;
    HWND m_hWndIconRight = nullptr;

    HICON m_hIconLeft = nullptr;
    HICON m_hIconRight = nullptr;

    int m_dxSlider = 0;

    COLORREF m_clrLabel;

    void PositionControls();
    int GetSliderRangeMin() const;
    int GetSliderRangeMax() const;
    void SetFont();

};
