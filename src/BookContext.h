#pragma once

#include <cstdint>
#include <optional>

namespace BookContext {

using FormId = std::uint32_t;

// An active book menu is authoritative: an unresolved open book must not
// fall through to a possibly stale inventory selection.
[[nodiscard]] std::optional<FormId> Resolve(
    bool bookMenuOpen,
    FormId openBookForm,
    bool inventoryMenuOpen,
    FormId selectedInventoryForm);

}
