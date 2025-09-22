#pragma once
#include "BaseJson.h"
#include <rcommon/rstring.h>
#include <rcommon/LanguageManager.h>


constexpr LPCSTR c_sJsonSect_HowToPlay = "how_to_play";	// UTF-8
constexpr LPCSTR c_sJsonSect_HowToUseApp = "how_to_use_app";	// UTF-8




class HelpJson :
    public BaseJson
{
 public:
    HelpJson() = default;
    ~HelpJson() = default;

    bool load(const LanguageManager& a_lang);


    std::vector<std::pair<std::string, tstring>> getSectionTitles() const;
    std::vector<tstring> getHowToPlay() const;
    std::vector<tstring> getHowToUseApp() const;
    std::vector<tstring> getInstructions(const std::string& a_sSection);




private:
    static tstring GetInstructionFileNameFromLangID(const LanguageManager& a_lang);
};

