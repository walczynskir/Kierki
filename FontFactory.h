#pragma once
#include <windows.h>
#include <map>
#include <rcommon/rstring.h> // Assuming tstring is defined here


enum class FontStyle {
    Normal,
    Fancy
};

class CFontFactory {
public:
    static CFontFactory& Instance();

    // Delete copy/move constructors and assignment
    CFontFactory(const CFontFactory&) = delete;
    CFontFactory(CFontFactory&&) = delete;
    CFontFactory& operator=(const CFontFactory&) = delete;
    CFontFactory& operator=(CFontFactory&&) = delete;

    void SetFontStyle(FontStyle a_style);
    FontStyle GetFontStyle() const;

    static void SetFontNames(const tstring& a_sNormalFont, const tstring& a_sFancyFont);

    HFONT GetFont(HWND a_hWnd, int a_iSize = 9, int a_iWeight = FW_NORMAL);
    void ClearCache();
    bool Initialized() const;

private:
    CFontFactory() = default;
    ~CFontFactory();

    static CFontFactory& InstanceInternal();


    FontStyle m_styleCurrent = FontStyle::Normal;
    std::map<tstring, HFONT> m_fontCache;

    tstring m_sNormalFont = _T("");
    tstring m_sFancyFont = _T("");

    int GetDpiForWindowSafe(HWND a_hWnd) const;
    tstring GetFaceName() const;
    tstring MakeKey(int a_dpi, int a_iSize, int a_iWeight) const;
};
