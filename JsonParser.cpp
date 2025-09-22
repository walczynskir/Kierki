
#include "stdafx.h"
#include "JsonParser.h"
#include <fstream>
#include <codecvt>
#include <locale>
#include <regex>



// --- Constructors ---
JsonParser::JsonParser() = default;
JsonParser::JsonParser(Json json) : m_json(std::move(json)) {}

// --- Parsing ---
bool JsonParser::parse(const std::string& source, bool isFile)
{
    try
    {
        if (isFile)
        {
            std::ifstream ifs(source);
            if (!ifs.is_open())
                return false;
            ifs >> m_json;
        }
        else
        {
            m_json = Json::parse(source);
        }
        return true;
    }
    catch (...)
    {
        m_json = Json{};
        return false;
    }
}

bool JsonParser::parse(const std::wstring& source, bool isFile)
{
    return parse(wstringToUtf8(source), isFile);
}

// --- Accessors ---
const JsonParser::Json& JsonParser::data() const { return m_json; }
JsonParser::Json& JsonParser::data() { return m_json; }

bool JsonParser::contains(const std::string& key) const
{
    return m_json.contains(key);
}

bool JsonParser::is_object() const { return m_json.is_object(); }
bool JsonParser::is_array() const { return m_json.is_array(); }

// --- Sections ---
JsonParser JsonParser::section(const std::string& key) const
{
    if (m_json.contains(key))
        return JsonParser(m_json.at(key));
    return JsonParser(Json{});
}

JsonParser JsonParser::section(size_t index) const
{
    if (m_json.is_array() && index < m_json.size())
        return JsonParser(m_json.at(index));
    return JsonParser(Json{});
}

// --- Arrays ---
size_t JsonParser::size() const
{
    return m_json.is_array() ? m_json.size() : 0;
}

std::vector<JsonParser> JsonParser::elements() const
{
    std::vector<JsonParser> result;
    if (m_json.is_array())
    {
        for (const auto& el : m_json)
            result.emplace_back(el);
    }
    return result;
}

// --- Path navigation ---
JsonParser JsonParser::section_path(const std::string& path) const
{
    const Json* current = &m_json;
    static const std::regex tokenRegex(R"(([^.\[\]]+)|\[(\d+)\])");

    auto begin = std::sregex_iterator(path.begin(), path.end(), tokenRegex);
    auto end = std::sregex_iterator();

    for (auto it = begin; it != end; ++it)
    {
        if ((*it)[1].matched) // object key
        {
            std::string key = (*it)[1].str();
            if (!current->is_object() || !current->contains(key))
                return JsonParser(Json{});
            current = &((*current)[key]);
        }
        else if ((*it)[2].matched) // array index
        {
            size_t index = std::stoul((*it)[2].str());
            if (!current->is_array() || index >= current->size())
                return JsonParser(Json{});
            current = &((*current)[index]);
        }
    }
    return JsonParser(*current);
}

// --- Save ---
bool JsonParser::save(const std::string& filename) const
{
    try
    {
        std::ofstream ofs(filename);
        if (!ofs.is_open())
            return false;
        ofs << m_json.dump(4); // pretty print
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool JsonParser::save(const std::wstring& filename) const
{
    return save(wstringToUtf8(filename));
}

std::optional<std::wstring> JsonParser::get_optional_w(const std::string& key) const
{
    if (!m_json.contains(key))
        return std::nullopt;

    try {
        std::string utf8 = m_json.at(key).get<std::string>();
        return utf8ToWstring(utf8);
    }
    catch (...) {
        return std::nullopt;
    }
}

std::wstring JsonParser::get_w(const std::string& key) const
{
    return utf8ToWstring(m_json.at(key).get<std::string>());
}

std::optional<std::wstring> JsonParser::get_path_w(const std::string& path) const
{
    auto sub = section_path(path);
    if (sub.data().is_null())
        return std::nullopt;

    try {
        std::string utf8 = sub.data().get<std::string>();
        return utf8ToWstring(utf8);
    }
    catch (...) {
        return std::nullopt;
    }
}

// --- Utils ---
std::string JsonParser::wstringToUtf8(const std::wstring& wstr)
{
    if (wstr.empty()) return {};

    int sizeNeeded = WideCharToMultiByte(
        CP_UTF8, 0, wstr.data(), (int)wstr.size(),
        nullptr, 0, nullptr, nullptr);

    std::string result(sizeNeeded, 0);
    WideCharToMultiByte(
        CP_UTF8, 0, wstr.data(), (int)wstr.size(),
        result.data(), sizeNeeded, nullptr, nullptr);

    return result;
}

std::wstring JsonParser::utf8ToWstring(const std::string& str)
{
    if (str.empty()) return {};

    int sizeNeeded = MultiByteToWideChar(
        CP_UTF8, 0, str.data(), (int)str.size(),
        nullptr, 0);

    std::wstring result(sizeNeeded, 0);
    MultiByteToWideChar(
        CP_UTF8, 0, str.data(), (int)str.size(),
        result.data(), sizeNeeded);

    return result;
}