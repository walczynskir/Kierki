#include "pch.h"
#include "CppUnitTest.h"
#include "../../Kierki/BaseJson.h"
#include "util.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace KierkiTests
{
    TEST_CLASS(CBaseJsonTests)
    {
    public:

        TEST_METHOD(GetA_ReturnsExpectedValue)
        {
            CBaseJson json;
            std::string data = R"({"name":"Robert","age":42})";
            Assert::IsTrue(json.parseDirect(data));

            auto name = json.getA("name");
            Assert::IsTrue(name.has_value());
            Assert::AreEqual(std::string("Robert"), name.value());
        }

        TEST_METHOD(GetArrayA_ReturnsExpectedValues)
        {
            CBaseJson json;
            std::string data = R"({"colors":["red","green","blue"]})";
            Assert::IsTrue(json.parseDirect(data));

            auto colors = json.getArrayA("colors");
            Assert::AreEqual(size_t(3), colors.size());
            Assert::AreEqual(std::string("red"), colors[0]);
            Assert::AreEqual(std::string("green"), colors[1]);
            Assert::AreEqual(std::string("blue"), colors[2]);
        }

        TEST_METHOD(GetSectionNamesA_ReturnsTopLevelKeys)
        {
            CBaseJson json;
            std::string data = R"({"user":{"name":"Robert"},"settings":{"theme":"dark"}})";
            Assert::IsTrue(json.parseDirect(data));

            auto sections = json.getSectionNamesA();
            Assert::AreEqual(size_t(2), sections.size());
            Assert::IsTrue(std::find(sections.begin(), sections.end(), "user") != sections.end());
            Assert::IsTrue(std::find(sections.begin(), sections.end(), "settings") != sections.end());
        }

        TEST_METHOD(GetAllSectionPathsA_ReturnsRecursiveKeys)
        {
            CBaseJson json;
            std::string data = R"({"user":{"name":"Robert","roles":["admin","editor"]}})";
            Assert::IsTrue(json.parseDirect(data));

            auto paths = json.getAllSectionPathsA();
            Assert::IsTrue(std::find(paths.begin(), paths.end(), "user") != paths.end());
            Assert::IsTrue(std::find(paths.begin(), paths.end(), "user.name") != paths.end());
            Assert::IsTrue(std::find(paths.begin(), paths.end(), "user.roles") != paths.end());
            Assert::IsTrue(std::find(paths.begin(), paths.end(), "user.roles[0]") != paths.end());
            Assert::IsTrue(std::find(paths.begin(), paths.end(), "user.roles[1]") != paths.end());
        }

        TEST_METHOD(GetW_HandlesUTF8Conversion)
        {
            CBaseJson json;

            std::wstring original = L"Zażółć gęślą jaźń";
            std::string escaped = escapeUnicode(original);

            std::string data = R"({"greeting":")" + escaped + R"("})";
            Assert::IsTrue(json.parseDirect(data));

            auto wide = json.getW("greeting");
            Assert::IsTrue(wide.has_value());
            Assert::IsTrue(wide.value().find(L"Zażółć") != std::wstring::npos);
        }

        TEST_METHOD(RoundTripUnicodeConversion)
        {
            CBaseJson json;

            // Original wide string
            std::wstring original = L"Zażółć gęślą jaźń";

            // Escape it for JSON
            std::string escaped = escapeUnicode(original);
            std::string jsonText = R"({"greeting":")" + escaped + R"("})";

            // Parse JSON
            Assert::IsTrue(json.parseDirect(jsonText));

            // Extract wide string
            auto wide = json.getW("greeting");
            Assert::IsTrue(wide.has_value());

            // Compare original and parsed
            Assert::AreEqual(original, wide.value());
        }

    };
}
