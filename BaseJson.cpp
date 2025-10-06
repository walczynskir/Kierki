#include "StdAfx.h"
#include "BaseJson.h"
#include <windows.h>
// #include <urlmon.h>
#include <vector>
#include <wininet.h>
// #pragma comment(lib, "urlmon.lib") not needed, as I am using wininet.lib


#pragma todo("Consider using WinHTTP instead of WinINet for better performance and security.")
#pragma todo("Improve formatting .")
// Load JSON from GitHub or resource
bool CBaseJson::load(const tstring& a_sUrl, int a_idResource, HINSTANCE a_hInstance)
{

    // Try GitHub
 	std::string l_sJsonData;    //utf-8 data!

    // TODO add to parameters whether load only from resources
#ifndef _DEBUG
    if (!LoadFromUrl(a_sUrl, l_sJsonData))
#endif // _DEBUG
        if (!LoadFromResource(a_idResource, a_hInstance, l_sJsonData))
            return false;
    return (m_parser.parse(l_sJsonData, false));
}


// Access JSON (UTF-8 string)
std::optional<std::string> CBaseJson::getA(const std::string& path) const
{
    return m_parser.get_path<std::string>(path);
}

// Access JSON (wide string)
std::optional<std::wstring> CBaseJson::getW(const std::string& path) const
{
    return m_parser.get_path_w(path);
}

// Access JSON (wide string)
std::optional<tstring> CBaseJson::get(const std::string& path) const
{
#ifdef _UNICODE
    return getW(path);
#else
	return getA(path);
#endif  
}



std::vector<std::string> CBaseJson::getArrayA(const std::string& path) const
{
    std::vector<std::string> result;
    auto section = m_parser.section_path(path);
    if (!section.is_array()) return result;

    for (auto& el : section.elements()) {
        if (el.data().is_string()) {
            result.push_back(el.data().get<std::string>());
        }
    }
    return result;
}

std::vector<std::wstring> CBaseJson::getArrayW(const std::string& path) const
{
    std::vector<std::wstring> result;
    auto section = m_parser.section_path(path);
    if (!section.is_array()) return result;

    for (auto& el : section.elements()) {
        if (el.data().is_string()) {
            std::string utf8 = el.data().get<std::string>();
            result.push_back(JsonParser::utf8ToWstring(utf8));
        }
    }
    return result;
}

std::vector<tstring> CBaseJson::getArray(const std::string& path) const
{
 #ifdef _UNICODE
	return getArrayW(path);
#else
	return getArrayA(path);
#endif
}

std::vector<std::string> CBaseJson::getSectionNamesA() const
{
    std::vector<std::string> names;
    for (auto it = m_parser.data().begin(); it != m_parser.data().end(); ++it) {
        names.push_back(it.key());
    }
    return names;
}

std::vector<std::wstring> CBaseJson::getSectionNamesW() const
{
    std::vector<std::wstring> names;
    for (auto it = m_parser.data().begin(); it != m_parser.data().end(); ++it) {
        names.push_back(JsonParser::utf8ToWstring(it.key()));
    }
    return names;
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
static void collectPaths(const nlohmann::json& j,
    const std::string& prefix,
    std::vector<std::string>& paths)
{
    if (j.is_object()) {
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string fullKey = prefix.empty() ? it.key() : prefix + "." + it.key();
            paths.push_back(fullKey);
            collectPaths(it.value(), fullKey, paths); // recurse
        }
    }
    else if (j.is_array()) {
        for (size_t i = 0; i < j.size(); ++i) {
            std::string fullKey = prefix + "[" + std::to_string(i) + "]";
            paths.push_back(fullKey);
            collectPaths(j[i], fullKey, paths); // recurse
        }
    }
}

std::vector<std::string> CBaseJson::getAllSectionPathsA() const
{
    std::vector<std::string> result;
    collectPaths(m_parser.data(), "", result);
    return result;
}

std::vector<std::wstring> CBaseJson::getAllSectionPathsW() const
{
    auto utf8paths = getAllSectionPathsA();
    std::vector<std::wstring> result;
    for (const auto& p : utf8paths) {
        result.push_back(JsonParser::utf8ToWstring(p));
    }
    return result;
}


std::vector<tstring> CBaseJson::getAllSectionPaths() const
{
#ifdef _UNICODE
	return getAllSectionPathsW();
#else
	return getAllSectionPathsA();
#endif

}


/*
// * urlmon version
// Download file from URL
bool JsonHelper::loadFromUrl(const tstring& a_sUrl, const tstring& a_sPathLocal)
{
    HRESULT hr = URLDownloadToFileW(nullptr, a_sUrl.c_str(), a_sPathLocal.c_str(), 0, nullptr);
    return SUCCEEDED(hr);
}
*/

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
    if (!l_hFile) {
        InternetCloseHandle(l_hInternet);
        return false;
    }

    // check for errors
    DWORD l_dwStatusCode = 0;
    DWORD l_dwStatusCodeSize = sizeof(l_dwStatusCode);
    if (::HttpQueryInfo(l_hFile, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &l_dwStatusCode, &l_dwStatusCodeSize, NULL)) {
        if (l_dwStatusCode != 200) {
            InternetCloseHandle(l_hFile);
            InternetCloseHandle(l_hInternet);
            return false;
        }
    }

	// UTF-8 buffer
    char buffer[4096];
    DWORD bytesRead;
    std::ostringstream oss;

    while (::InternetReadFile(l_hFile, buffer, sizeof(buffer), &bytesRead) && bytesRead != 0) {
        oss.write(buffer, bytesRead);
    }

    a_sContent = oss.str();

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
