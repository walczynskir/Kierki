#include "stdafx.h"
#include "HelpJson.h"
#include "resource.h"


constexpr LPCTSTR c_sInstructionsPath = _T("https://raw.githubusercontent.com/walczynskir/Kierki/master/instructions/instructions_");
constexpr LPCTSTR c_sDefaultInstructions = _T("instructions_en-US.json"); // Fallback in case the lack of language ID
constexpr LPCTSTR c_sInstructionsSuffix = _T(".json");
constexpr LPCSTR c_sJsonSect_Title = ".title";	// UTF-8
constexpr LPCSTR c_sJsonSect_Instructions = "instructions";	// UTF-8
constexpr const char c_sJson_Separator = '.';	// UTF-8






std::vector<std::pair<std::string, tstring>> CHelpJson::getSectionTitles() const
{
    std::vector<std::pair<std::string, tstring>> l_vectDesc;
    auto l_sectionNames = getSectionNamesA(); // ASCII section names

    for (const auto& l_section : l_sectionNames) {
        std::optional<tstring> l_optDesc = get(l_section + c_sJsonSect_Title);
		l_vectDesc.emplace_back(l_section, l_optDesc.value_or(_T("")));
	}

    return l_vectDesc;
}


std::vector<tstring> CHelpJson::getInstructions(const std::string& a_sSection)
{
	return getArray(a_sSection + c_sJson_Separator + c_sJsonSect_Instructions);
}


std::vector<std::string> CHelpJson::getInstructionsA(const std::string& a_sSection)
{
	return getArrayA(a_sSection + c_sJson_Separator + c_sJsonSect_Instructions);
}


std::vector<std::wstring> CHelpJson::getInstructionsW(const std::string& a_sSection)
{
	return getArrayW(a_sSection + c_sJson_Separator + c_sJsonSect_Instructions);
}



bool CHelpJson::load(const LanguageManager& a_lang)
{
	return CBaseJson::load(GetInstructionFileNameFromLangID(a_lang), IDR_JSON_INSTRUCTIONS, GetModuleHandle(NULL));
}



tstring CHelpJson::GetInstructionFileNameFromLangID(const LanguageManager& a_lang)
{

	tstring l_sDefaultUrl = tstring(c_sInstructionsPath) + tstring(c_sDefaultInstructions);
	// Pobierz nazwê lokalizacji na podstawie LANGID, np. "pl-PL"
	if (!a_lang.GetCurrentLangID().has_value())
		return l_sDefaultUrl; // Fallback jeœli brak jêzyka


	TCHAR l_sName[LOCALE_NAME_MAX_LENGTH] = {};
	if (!LCIDToLocaleName(MAKELCID(a_lang.GetCurrentLangID().value(), SORT_DEFAULT), l_sName, LOCALE_NAME_MAX_LENGTH, 0)) {
		return l_sDefaultUrl; // Fallback jeœli coœ pójdzie nie tak
	}

	// Zbuduj nazwê pliku
	return c_sInstructionsPath + tstring(l_sName) + c_sInstructionsSuffix;
}