#include "HotkeyConfig.h"

#include <charconv>
#include <cctype>

namespace
{
    std::string_view Trim(std::string_view text)
    {
        while (!text.empty() && std::isspace(static_cast<unsigned char>(text.front()))) {
            text.remove_prefix(1);
        }
        while (!text.empty() && std::isspace(static_cast<unsigned char>(text.back()))) {
            text.remove_suffix(1);
        }
        return text;
    }

    bool EqualsIgnoreCase(std::string_view left, std::string_view right)
    {
        if (left.size() != right.size()) {
            return false;
        }
        for (std::size_t i = 0; i < left.size(); ++i) {
            if (std::tolower(static_cast<unsigned char>(left[i])) !=
                std::tolower(static_cast<unsigned char>(right[i]))) {
                return false;
            }
        }
        return true;
    }

    bool ParseScanCode(std::string_view text, std::uint32_t& result)
    {
        text = Trim(text);
        int base = 10;
        if (text.size() > 2 && text[0] == '0' && (text[1] == 'x' || text[1] == 'X')) {
            text.remove_prefix(2);
            base = 16;
        }
        if (text.empty()) {
            return false;
        }

        std::uint32_t value = 0;
        const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), value, base);
        if (error != std::errc{} || end != text.data() + text.size() || value == 0 || value > 0xFF) {
            return false;
        }
        result = value;
        return true;
    }
}

namespace HotkeyConfig
{

std::uint32_t ParseToggleKey(std::string_view iniText, std::uint32_t defaultKey)
{
    bool inInputSection = false;
    while (!iniText.empty()) {
        const auto newline = iniText.find('\n');
        auto line = Trim(iniText.substr(0, newline));
        iniText = newline == std::string_view::npos ? std::string_view{} : iniText.substr(newline + 1);
        if (const auto comment = line.find_first_of(";#"); comment != std::string_view::npos) {
            line = Trim(line.substr(0, comment));
        }
        if (line.empty()) {
            continue;
        }
        if (line.front() == '[' && line.back() == ']') {
            inInputSection = EqualsIgnoreCase(Trim(line.substr(1, line.size() - 2)), "Input");
            continue;
        }
        if (!inInputSection) {
            continue;
        }

        const auto equals = line.find('=');
        if (equals == std::string_view::npos || !EqualsIgnoreCase(Trim(line.substr(0, equals)), "ToggleKey")) {
            continue;
        }
        std::uint32_t key = 0;
        return ParseScanCode(line.substr(equals + 1), key) ? key : defaultKey;
    }
    return defaultKey;
}

}
