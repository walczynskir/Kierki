#include "stdafx.h"
#include "FontFactory.h"
#include <rcommon/ROwnExc.h>


CFontFactory::~CFontFactory() {
    ClearCache();
}


void CFontFactory::SetFontNames(const tstring& a_sNormalFont, const tstring& a_sFancyFont)
{
    CFontFactory& l_instance = InstanceInternal();
    l_instance.m_sNormalFont = a_sNormalFont;
    l_instance.m_sFancyFont = a_sFancyFont;
}



CFontFactory& CFontFactory::Instance()
{

    CFontFactory& l_instance = InstanceInternal();

    if (!l_instance.Initialized())
        throw ROwnExc(_T("FontFactory: SetFontNames method havn't been called yet"));
        
    return l_instance;
}


bool CFontFactory::Initialized() const
{
    return (!m_sFancyFont.empty() && !m_sNormalFont.empty());
}

CFontFactory& CFontFactory::InstanceInternal() 
{

    static CFontFactory l_instance;
    return l_instance;
}

HFONT CFontFactory::GetFont(HWND a_hWnd, int a_iSize, int a_iWeight) {
    int l_dpi = GetDpiForWindowSafe(a_hWnd);
    int l_dy = -MulDiv(a_iSize, l_dpi, 72);
    tstring l_sKey = MakeKey(l_dpi, l_dy, a_iWeight);

    auto l_it = m_fontCache.find(l_sKey);
    if (l_it != m_fontCache.end())
        return l_it->second;

    tstring l_sFace = GetFaceName();

    HFONT l_hFont = ::CreateFont(
        l_dy, 0, 0, 0, a_iWeight, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
        l_sFace.c_str());

    m_fontCache[l_sKey] = l_hFont;
    return l_hFont;
}

int CFontFactory::GetDpiForWindowSafe(HWND a_hWnd) const {
    HDC l_hdc = ::GetDC(a_hWnd);
    int l_dpi = ::GetDeviceCaps(l_hdc, LOGPIXELSY);
    ::ReleaseDC(a_hWnd, l_hdc);
    return l_dpi;
}

tstring CFontFactory::GetFaceName() const {
    return  m_styleCurrent == FontStyle::Fancy ? m_sFancyFont : m_sNormalFont;
}

tstring CFontFactory::MakeKey(int a_dpi, int a_iSize, int a_iWeight) const {
    return FormatTextT("{}_{}pt_{}dpi_{}", GetFaceName(), a_iSize, a_dpi, a_iWeight);
}

void CFontFactory::ClearCache() {
    for (auto& l_pair : m_fontCache)
        ::DeleteObject(l_pair.second);
    m_fontCache.clear();
}

void CFontFactory::SetFontStyle(FontStyle a_style) {
    m_styleCurrent = a_style;   // cache cleared only at exit, just to keep all FONTs available
}

FontStyle CFontFactory::GetFontStyle() const {
    return m_styleCurrent;
}
