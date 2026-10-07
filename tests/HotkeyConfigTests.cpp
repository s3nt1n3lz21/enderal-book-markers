#include "HotkeyConfig.h"

#include <cassert>
#include <cstdint>
#include <iostream>

int main()
{
    constexpr std::uint32_t defaultKey = 0x40;
    assert(HotkeyConfig::ParseToggleKey("[Input]\nToggleKey=0x41\n", defaultKey) == 0x41);
    assert(HotkeyConfig::ParseToggleKey("[Input]\nToggleKey=65\n", defaultKey) == 65);
    assert(HotkeyConfig::ParseToggleKey("[Other]\nToggleKey=66\n", defaultKey) == defaultKey);
    assert(HotkeyConfig::ParseToggleKey("[Input]\nToggleKey=0\n", defaultKey) == defaultKey);
    assert(HotkeyConfig::ParseToggleKey("[Input]\nToggleKey=not-a-key\n", defaultKey) == defaultKey);
    assert(HotkeyConfig::ParseToggleKey("", defaultKey) == defaultKey);
    std::cout << "6 hotkey configuration tests passed\n";
}
