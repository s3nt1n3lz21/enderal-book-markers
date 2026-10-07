#pragma once

#include <cstdint>
#include <string_view>

namespace HotkeyConfig
{

// Reads [Input] ToggleKey as a decimal or 0x-prefixed keyboard scan code.
[[nodiscard]] std::uint32_t ParseToggleKey(std::string_view iniText, std::uint32_t defaultKey);

}
