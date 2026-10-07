#include "BookMarkerState.h"

#include <algorithm>

bool BookMarkerState::Toggle(FormId formId)
{
    if (formId == 0) {
        return false;
    }

    const auto [_, inserted] = markedForms_.insert(formId);
    if (inserted) {
        return true;
    }

    markedForms_.erase(formId);
    return false;
}

bool BookMarkerState::IsMarked(FormId formId) const
{
    return formId != 0 && markedForms_.contains(formId);
}

std::vector<BookMarkerState::FormId> BookMarkerState::MarkedForms() const
{
    std::vector<FormId> forms(markedForms_.begin(), markedForms_.end());
    std::sort(forms.begin(), forms.end());
    return forms;
}

void BookMarkerState::Restore(const std::vector<FormId>& formIds)
{
    markedForms_.clear();
    for (const FormId formId : formIds) {
        if (formId != 0) {
            markedForms_.insert(formId);
        }
    }
}

void BookMarkerState::Clear()
{
    markedForms_.clear();
}
