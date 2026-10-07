#include "BookMarkerState.h"
#include "BookContext.h"

#include <cstdlib>
#include <iostream>

namespace {

void Check(bool condition, const char* message)
{
    if (!condition) {
        std::cerr << "FAIL: " << message << '\n';
        std::exit(EXIT_FAILURE);
    }
}

void ToggleMarksAndUnmarks()
{
    BookMarkerState state;
    Check(state.Toggle(0x1234), "first toggle marks the book");
    Check(state.IsMarked(0x1234), "marked book is reported marked");
    Check(!state.Toggle(0x1234), "second toggle unmarks the book");
    Check(!state.IsMarked(0x1234), "unmarked book is reported unmarked");
}

void FormsAreIndependent()
{
    BookMarkerState state;
    state.Toggle(0x1234);
    state.Toggle(0x5678);
    state.Toggle(0x1234);
    Check(!state.IsMarked(0x1234), "toggling one form does not change its neighbor");
    Check(state.IsMarked(0x5678), "other form remains marked");
}

void RestoreDropsInvalidAndDuplicateIds()
{
    BookMarkerState state;
    state.Restore({0x1234, 0, 0x1234, 0x5678});
    Check(state.IsMarked(0x1234), "valid form restored");
    Check(state.IsMarked(0x5678), "second valid form restored");
    Check(state.MarkedForms().size() == 2, "restore removes duplicate and zero ids");
}

void ClearRemovesAllMarkers()
{
    BookMarkerState state;
    state.Toggle(0x1234);
    state.Clear();
    Check(state.MarkedForms().empty(), "clear empties marker state");
}

void OpenBookTakesPrecedenceOverInventorySelection()
{
    const auto selected = BookContext::Resolve(true, 0x1234, true, 0x5678);
    Check(selected == 0x1234, "open book takes precedence over inventory selection");
}

void InventoryContextRequiresASelectedBook()
{
    const auto selected = BookContext::Resolve(false, 0, true, 0x5678);
    Check(selected == 0x5678, "selected inventory book is resolved");
    Check(!BookContext::Resolve(false, 0, true, 0), "empty inventory selection is ignored");
}

void MissingBookContextDoesNothing()
{
    Check(!BookContext::Resolve(false, 0, false, 0), "no active book context resolves to no form");
    Check(!BookContext::Resolve(true, 0, false, 0), "open book without a resolvable form fails safely");
}

}

int main()
{
    ToggleMarksAndUnmarks();
    FormsAreIndependent();
    RestoreDropsInvalidAndDuplicateIds();
    ClearRemovesAllMarkers();
    OpenBookTakesPrecedenceOverInventorySelection();
    InventoryContextRequiresASelectedBook();
    MissingBookContextDoesNothing();
    std::cout << "7 marker-state and context tests passed\n";
}
