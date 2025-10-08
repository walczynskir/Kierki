#include "pch.h"
#include <sstream>
#include <iomanip>

std::string escapeUnicode(const std::wstring& input)
{
    std::ostringstream oss;
    for (wchar_t ch : input)
    {
        if (ch >= 32 && ch <= 126) // printable ASCII
        {
            oss << static_cast<char>(ch);
        }
        else
        {
            oss << "\\u" << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(ch);
        }
    }
    return oss.str();
}
