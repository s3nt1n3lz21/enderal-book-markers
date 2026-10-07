#include "BookMarkerState.h"

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

}

int main()
{
    ToggleMarksAndUnmarks();
    FormsAreIndependent();
    RestoreDropsInvalidAndDuplicateIds();
    ClearRemovesAllMarkers();
    std::cout << "4 marker-state tests passed\n";
}
