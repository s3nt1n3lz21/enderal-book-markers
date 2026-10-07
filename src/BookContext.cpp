#include "BookContext.h"

namespace BookContext {

std::optional<FormId> Resolve(
    bool bookMenuOpen,
    FormId openBookForm,
    bool inventoryMenuOpen,
    FormId selectedInventoryForm)
{
    if (bookMenuOpen) {
        return openBookForm == 0 ? std::nullopt : std::optional<FormId>{openBookForm};
    }

    if (inventoryMenuOpen && selectedInventoryForm != 0) {
        return selectedInventoryForm;
    }

    return std::nullopt;
}

}
