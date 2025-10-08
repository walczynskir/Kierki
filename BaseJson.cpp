#include "StdAfx.h"
#include "BaseJson.h"
#include <windows.h>
#include <vector>
#include <wininet.h>


// TODO Consider using WinHTTP instead of WinINet for better performance and security.
// 
// Load JSON from GitHub or resource
bool CBaseJson::load(const tstring& a_sUrl, int a_idResource, HINSTANCE a_hInstance, bool a_bOnlyResource)
{

    // Try GitHub
 	std::string l_sJsonData;    //utf-8 data!

    if (a_bOnlyResource)
    {
        if (!LoadFromResource(a_idResource, a_hInstance, l_sJsonData))
            return false;
    }
    else
    {
        if (!LoadFromUrl(a_sUrl, l_sJsonData) &&
            !LoadFromResource(a_idResource, a_hInstance, l_sJsonData))
            return false;
    }

    return (m_parser.parse(l_sJsonData, false));
}


// Access JSON (UTF-8 string)
std::optional<std::string> CBaseJson::getA(const std::string& a_sPath) const
{
    return m_parser.get_path<std::string>(a_sPath);
}

// Access JSON (wide string)
std::optional<std::wstring> CBaseJson::getW(const std::string& a_sPath) const
{
    return m_parser.get_path_w(a_sPath);
}

// Access JSON (wide string)
std::optional<tstring> CBaseJson::get(const std::string& a_sPath) const
{
#ifdef _UNICODE
    return getW(a_sPath);
#else
	return getA(a_sPath);
#endif  
}



std::vector<std::string> CBaseJson::getArrayA(const std::string& a_sPath) const
{
    std::vector<std::string> l_vectResults;
    auto section = m_parser.section_path(a_sPath);
    if (!section.is_array()) 
        return l_vectResults;

    for (auto& l_element : section.elements()) 
    {
        if (l_element.data().is_string())
            l_vectResults.push_back(l_element.data().get<std::string>());
    }
    return l_vectResults;
}

std::vector<std::wstring> CBaseJson::getArrayW(const std::string& a_sPath) const
{
    std::vector<std::wstring> l_vectResults;
    auto l_section = m_parser.section_path(a_sPath);
    if (!l_section.is_array()) 
        return l_vectResults;

    for (auto& l_element : l_section.elements()) 
    {
        if (l_element.data().is_string())
        {
            std::string l_sValue = l_element.data().get<std::string>();
            l_vectResults.push_back(JsonParser::utf8ToWstring(l_sValue));
        }
    }
    return l_vectResults;
}

std::vector<tstring> CBaseJson::getArray(const std::string& a_sPath) const
{
 #ifdef _UNICODE
	return getArrayW(a_sPath);
#else
	return getArrayA(a_sPath);
#endif
}

std::vector<std::string> CBaseJson::getSectionNamesA() const
{
    std::vector<std::string> l_vectNames;
    for (auto l_it = m_parser.data().begin(); l_it != m_parser.data().end(); ++l_it) 
        l_vectNames.push_back(l_it.key());
  
    return l_vectNames;
}

std::vector<std::wstring> CBaseJson::getSectionNamesW() const
{
    std::vector<std::wstring> l_vectNames;
    for (auto l_it = m_parser.data().begin(); l_it != m_parser.data().end(); ++l_it)
        l_vectNames.push_back(JsonParser::utf8ToWstring(l_it.key()));
    return l_vectNames;
}


std::vector<tstring> CBaseJson::getSectionNames() const
{
#ifdef _UNICODE
	return getSectionNamesW();
#else
	return getSectionNamesA();
#endif
}



// --- Recursive helper ---
static void collectPaths(const nlohmann::json& a_json, const std::string& a_sPrefix, std::vector<std::string>& a_vectPaths)
{
    if (a_json.is_object()) 
    {
        for (auto l_it = a_json.begin(); l_it != a_json.end(); ++l_it) 
        {
            std::string l_sFullKey = a_sPrefix.empty() ? l_it.key() : a_sPrefix + "." + l_it.key();
            a_vectPaths.push_back(l_sFullKey);
            collectPaths(l_it.value(), l_sFullKey, a_vectPaths); // recurse
        }
    }
    else if (a_json.is_array()) 
    {
        for (size_t l_iAt = 0; l_iAt < a_json.size(); ++l_iAt)
        {
            std::string l_sFullKey = a_sPrefix + "[" + std::to_string(l_iAt) + "]";
            a_vectPaths.push_back(l_sFullKey);
            collectPaths(a_json[l_iAt], l_sFullKey, a_vectPaths); // recurse
        }
    }
}

std::vector<std::string> CBaseJson::getAllSectionPathsA() const
{
    std::vector<std::string> l_vectResults;
    collectPaths(m_parser.data(), "", l_vectResults);
    return l_vectResults;
}

std::vector<std::wstring> CBaseJson::getAllSectionPathsW() const
{
    auto l_vectSectionsUtf8 = getAllSectionPathsA();
    std::vector<std::wstring> l_vectResults;
    for (const auto& l_sSection : l_vectSectionsUtf8) 
    {
        l_vectResults.push_back(JsonParser::utf8ToWstring(l_sSection));
    }
    return l_vectResults;
}


std::vector<tstring> CBaseJson::getAllSectionPaths() const
{
#ifdef _UNICODE
	return getAllSectionPathsW();
#else
	return getAllSectionPathsA();
#endif

}


bool CBaseJson::LoadFromUrl(const tstring& a_sUrl, std::string& a_sContent) 
{
    HINTERNET l_hInternet = ::InternetOpen(_T("Hearts"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (!l_hInternet)
        return false;

#ifdef _DEBUG
	DWORD l_dwFlags = INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_PRAGMA_NOCACHE; // force reloading in debug mode
#else
    DWORD l_dwFlags = INTERNET_FLAG_RELOAD;
#endif

    HINTERNET l_hFile = ::InternetOpenUrl(l_hInternet, a_sUrl.c_str(), NULL, 0, l_dwFlags, 0);
    if (!l_hFile)
    {
        ::InternetCloseHandle(l_hInternet);
        return false;
    }

    // check for errors
    DWORD l_dwStatusCode = 0;
    DWORD l_dwStatusCodeSize = sizeof(l_dwStatusCode);
    if (::HttpQueryInfo(l_hFile, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &l_dwStatusCode, &l_dwStatusCodeSize, NULL)) 
    {
        if (l_dwStatusCode != 200)
        {
            ::InternetCloseHandle(l_hFile);
            ::InternetCloseHandle(l_hInternet);
            return false;
        }
    }

	// UTF-8 buffer
    char l_sBuffer[4096];
    DWORD l_dwBytesRead;
    std::ostringstream l_oss;

    while (::InternetReadFile(l_hFile, l_sBuffer, sizeof(l_sBuffer), &l_dwBytesRead) && l_dwBytesRead != 0) 
        l_oss.write(l_sBuffer, l_dwBytesRead);

    a_sContent = l_oss.str();

    ::InternetCloseHandle(l_hFile);
    ::InternetCloseHandle(l_hInternet);
    return true;
}


bool CBaseJson::LoadFromResource(int a_idResource, HINSTANCE a_hInstance, std::string& a_sContent)
{
    HRSRC l_hRes = ::FindResource(a_hInstance, MAKEINTRESOURCE(a_idResource), _T("JSON"));
    if (!l_hRes)
        return false;

    HGLOBAL l_hData = ::LoadResource(a_hInstance, l_hRes);
    if (!l_hData)
        return false;

    DWORD l_dwSize = SizeofResource(a_hInstance, l_hRes);
    const char* l_pData = static_cast<const char*>(LockResource(l_hData));
    if (!l_pData) 
        return false;

    a_sContent.assign(l_pData, l_dwSize);
    return true;
}


#ifdef TESTING_BUILD
bool CBaseJson::parseDirect(const std::string& a_jsonUtf8)
{
    return m_parser.parse(a_jsonUtf8, false);
}
#endif
