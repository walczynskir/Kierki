#pragma once
#include <string>
#include <optional>
#include <rcommon/rstring.h>
#include "JsonParser.h"

class BaseJson
{
public:
    // Load JSON from GitHub URL, fallback to resource
    bool load(const tstring& a_sUrl, int a_idResource, HINSTANCE a_hInstance);

     // Accessors (UTF-8 and UNICODE)
    std::optional<std::string> getA(const std::string& path) const;
    std::optional<std::wstring> getW(const std::string& path) const;
    std::optional<tstring> get(const std::string& path) const;

    // Return array (UTF-8 and UNICODE)
    std::vector<std::string> getArrayA(const std::string& path) const;
    std::vector<std::wstring> getArrayW(const std::string& path) const;
    std::vector<tstring> getArray(const std::string& path) const;

    // Return seciotn names (UTF-8 and UNICODE)
    std::vector<std::string> getSectionNamesA() const;
    std::vector<std::wstring> getSectionNamesW() const;
    std::vector<tstring> getSectionNames() const;

    // List nested section names recursively (dot notation)
    std::vector<std::string> getAllSectionPathsA() const;
    std::vector<std::wstring> getAllSectionPathsW() const;
    std::vector<tstring> getAllSectionPaths() const;


private:
    JsonParser m_parser;

    // bool loadFromUrl(const tstring& a_sUrl, const tstring& a_sPathLocal); // urlmon.lib version
    bool LoadFromUrl(const tstring& a_sUrl, std::string& a_sContent);       // HINTERNET version
	//  bool loadFromResource(int resourceId, HINSTANCE hInstance, const tstring& a_sPathLocal); // version with temp file
    bool LoadFromResource(int a_idResource, HINSTANCE a_hInstance, std::string& a_sContent);
};


