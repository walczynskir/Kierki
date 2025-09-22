#pragma once

#include <string>
#include <optional>
#include <vector>
#include <external/json.hpp>


/* Example usage
 
 json files are by deafaukt UTF-8 decoded. So all strings in json file are std::string (UTF-8).


JSON structure:
{
  "users": [
    { "name": "Alice", "age": 30 },
    { "name": "Bob", "age": 25 }
  ],
  "settings": {
    "theme": "dark",
    "notifications": true
  }
}

#include "JsonParser.hpp"
#include <iostream>

int main()
{
    JsonParser parser;
    parser.parse("config.json");

    // Direct path to object
    auto theme = parser.get_path<std::string>("settings.theme");
    if (theme) std::cout << "Theme: " << *theme << "\n";

    // Direct path to array element
    auto bob = parser.get_path<std::string>("users[1].name");
    if (bob) std::cout << "Second user: " << *bob << "\n";

    // Direct path to nested number
    auto age = parser.get_path<int>("users[0].age");
    if (age) std::cout << "First user age: " << *age << "\n";
}

wstringToUtf8 → useful when loading JSON from a std::wstring filename.
utf8ToWstring → useful if you ever need to display JSON content (stored as UTF-8 strings) in a std::wstring context (e.g. Win32 MessageBoxW, UI text, etc).
*/



class JsonParser
{
public:
    using Json = nlohmann::json;

    JsonParser();
    explicit JsonParser(Json json);

    // Parse from std::string (filename or raw JSON string)
    bool parse(const std::string& source, bool isFile = true);

    // Parse from std::wstring (filename or raw JSON string)
    bool parse(const std::wstring& source, bool isFile = true);

    // Access underlying json
    const Json& data() const;
    Json& data();

    // Safe getter (throws if key missing)
    template<typename T>
    T get(const std::string& key) const
    {
        return m_json.at(key).get<T>();
    }

    // Safe optional getter
    template<typename T>
    std::optional<T> get_optional(const std::string& key) const
    {
        if (!m_json.contains(key))
            return std::nullopt;

        try {
            return m_json.at(key).get<T>();
        }
        catch (...) {
            return std::nullopt;
        }
    }

    bool contains(const std::string& key) const;
    bool is_object() const;
    bool is_array() const;

    // Subsection by key or index
    JsonParser section(const std::string& key) const;
    JsonParser section(size_t index) const;

    // Array helpers
    size_t size() const;
    std::vector<JsonParser> elements() const;

    // Path navigation
    JsonParser section_path(const std::string& path) const;

    template<typename T>
    std::optional<T> get_path(const std::string& path) const
    {
        auto sub = section_path(path);
        if (sub.data().is_null())
            return std::nullopt;

        try {
            return sub.data().get<T>();
        }
        catch (...) {
            return std::nullopt;
        }
    }

    std::optional<std::wstring> get_optional_w(const std::string& key) const;

    std::wstring get_w(const std::string& key) const;

    std::optional<std::wstring> get_path_w(const std::string& path) const;

    // Save to file
    bool save(const std::string& filename) const;
    bool save(const std::wstring& filename) const;

    static std::string wstringToUtf8(const std::wstring& wstr);
    static std::wstring utf8ToWstring(const std::string& str);

private:
    Json m_json{};

};
