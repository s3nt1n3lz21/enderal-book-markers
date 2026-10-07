#pragma once

#include <cstdint>
#include <unordered_set>
#include <vector>

class BookMarkerState {
public:
    using FormId = std::uint32_t;

    // Returns true when the form is marked after the toggle.
    bool Toggle(FormId formId);
    [[nodiscard]] bool IsMarked(FormId formId) const;
    [[nodiscard]] std::vector<FormId> MarkedForms() const;
    void Restore(const std::vector<FormId>& formIds);
    void Clear();

private:
    std::unordered_set<FormId> markedForms_;
};
