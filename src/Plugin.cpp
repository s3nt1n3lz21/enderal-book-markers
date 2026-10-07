#include "BookContext.h"
#include "DIIICompatibility.h"
#include "HotkeyConfig.h"
#include "BookMarkerState.h"

#define NOMINMAX
#include <Windows.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <limits>
#include <optional>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

namespace
{
    constexpr std::uint32_t kSerializationID = 0x45424D4B;  // "KMBE"
    constexpr std::uint32_t kRecordType = 0x4D524B42;       // "BKRM"
    constexpr std::uint32_t kRecordVersion = 1;
    constexpr std::uint32_t kMaximumSavedForms = 100000;
    constexpr std::uint32_t kDefaultHotkey = 0x40;          // F6 keyboard scan code
    std::uint32_t g_hotkey = kDefaultHotkey;

    BookMarkerState g_markerState;

    void LoadHotkey()
    {
        std::array<wchar_t, 32768> executablePath{};
        const auto pathLength = GetModuleFileNameW(
            nullptr, executablePath.data(), static_cast<DWORD>(executablePath.size()));
        if (pathLength == 0 || pathLength >= executablePath.size()) {
            return;
        }

        const std::filesystem::path executable(executablePath.data());
        const auto iniPath = executable.parent_path() /
            L"Data" / L"SKSE" / L"Plugins" / L"EnderalBookMarkers.ini";
        std::ifstream ini(iniPath);
        if (!ini) {
            return;
        }

        const std::string contents(
            std::istreambuf_iterator<char>(ini),
            std::istreambuf_iterator<char>());
        g_hotkey = HotkeyConfig::ParseToggleKey(contents, kDefaultHotkey);
    }

    void SaveCallback(SKSE::SerializationInterface* serialization)
    {
        const auto forms = g_markerState.MarkedForms();
        if (forms.size() > kMaximumSavedForms) {
            return;
        }

        const auto count = static_cast<std::uint32_t>(forms.size());
        if (!serialization->OpenRecord(kRecordType, kRecordVersion) ||
            !serialization->WriteRecordData(count)) {
            return;
        }

        if (!forms.empty()) {
            serialization->WriteRecordData(forms.data(),
                static_cast<std::uint32_t>(forms.size() * sizeof(BookMarkerState::FormId)));
        }
    }

    void LoadCallback(SKSE::SerializationInterface* serialization)
    {
        g_markerState.Clear();

        std::uint32_t type = 0;
        std::uint32_t version = 0;
        std::uint32_t length = 0;
        while (serialization->GetNextRecordInfo(type, version, length)) {
            if (type != kRecordType) {
                continue;
            }
            if (version != kRecordVersion || length < sizeof(std::uint32_t)) {
                continue;
            }

            std::uint32_t count = 0;
            if (serialization->ReadRecordData(count) != sizeof(count) ||
                count > kMaximumSavedForms ||
                count > (length - sizeof(count)) / sizeof(BookMarkerState::FormId)) {
                continue;
            }

            std::vector<BookMarkerState::FormId> forms(count);
            if (count > 0 &&
                serialization->ReadRecordData(forms.data(),
                    count * sizeof(BookMarkerState::FormId)) != count * sizeof(BookMarkerState::FormId)) {
                continue;
            }

            std::vector<BookMarkerState::FormId> resolved;
            resolved.reserve(forms.size());
            for (const auto oldFormId : forms) {
                RE::FormID newFormId = 0;
                if (!serialization->ResolveFormID(oldFormId, newFormId) || newFormId == 0) {
                    continue;
                }
                const auto* form = RE::TESForm::LookupByID(newFormId);
                if (form && form->As<RE::TESObjectBOOK>()) {
                    resolved.push_back(newFormId);
                }
            }
            g_markerState.Restore(resolved);
        }
    }

    void RevertCallback(SKSE::SerializationInterface*)
    {
        g_markerState.Clear();
    }

    std::optional<BookMarkerState::FormId> GetSelectedInventoryBook()
    {
        const auto movie = RE::UI::GetSingleton()->GetMovieView(RE::InventoryMenu::MENU_NAME);
        if (!movie) {
            return std::nullopt;
        }

        RE::GFxValue value;
        constexpr auto path = "_root.Menu_mc.inventoryLists.itemList.selectedEntry.formId";
        if (!movie->GetVariable(&value, path) ||
            value.GetType() != RE::GFxValue::ValueType::kNumber) {
            return std::nullopt;
        }

        const double rawFormId = value.GetNumber();
        if (!std::isfinite(rawFormId) || rawFormId <= 0.0 ||
            rawFormId > static_cast<double>(std::numeric_limits<RE::FormID>::max()) ||
            std::floor(rawFormId) != rawFormId) {
            return std::nullopt;
        }

        const auto formId = static_cast<RE::FormID>(rawFormId);
        const auto* form = RE::TESForm::LookupByID(formId);
        if (!form || !form->As<RE::TESObjectBOOK>()) {
            return std::nullopt;
        }
        return formId;
    }

    void ToggleCurrentBook()
    {
        auto* ui = RE::UI::GetSingleton();
        if (!ui) {
            return;
        }

        const bool bookMenuOpen = ui->IsMenuOpen(RE::BookMenu::MENU_NAME);
        const auto* openBook = bookMenuOpen ? RE::BookMenu::GetTargetForm() : nullptr;
        const auto selectedInventoryBook = ui->IsMenuOpen(RE::InventoryMenu::MENU_NAME)
            ? GetSelectedInventoryBook()
            : std::nullopt;

        const auto currentBook = BookContext::Resolve(
            bookMenuOpen,
            openBook ? openBook->GetFormID() : 0,
            ui->IsMenuOpen(RE::InventoryMenu::MENU_NAME),
            selectedInventoryBook.value_or(0));
        if (!currentBook) {
            return;
        }

        const bool marked = g_markerState.Toggle(*currentBook);
        RE::DebugNotification(marked ? "Book marked manually." : "Book marker removed.");
    }


    class PersonalReadCondition final : public DIII::ICondition
    {
    public:
        bool Match(RE::InventoryEntryData* entry) const override
        {
            if (!entry) {
                return false;
            }
            const auto* object = entry->GetObject();
            return object && object->As<RE::TESObjectBOOK>() &&
                g_markerState.IsMarked(object->GetFormID());
        }
    };

    void OnDIIIMessage(SKSE::MessagingInterface::Message* message)
    {
        if (!message || message->type != DIII::kMessage_GetAPI || !message->data) {
            return;
        }

        auto* api = static_cast<DIII::IAPI*>(message->data);
        if (api->GetVersion() < 1) {
            return;
        }
        api->RegisterCondition(
            "personallyRead",
            [](const Json::Value&, RE::FormType) -> std::unique_ptr<DIII::ICondition> {
                return std::make_unique<PersonalReadCondition>();
            });
    }

    class HotkeyListener final : public RE::BSTEventSink<RE::InputEvent*>
    {
    public:
        RE::BSEventNotifyControl ProcessEvent(
            RE::InputEvent* const* event,
            RE::BSTEventSource<RE::InputEvent*>*) override
        {
            if (!event || !*event || (*event)->GetDevice() != RE::INPUT_DEVICE::kKeyboard) {
                return RE::BSEventNotifyControl::kContinue;
            }

            const auto* button = (*event)->AsButtonEvent();
            if (button && button->GetIDCode() == g_hotkey && button->IsDown()) {
                ToggleCurrentBook();
            }
            return RE::BSEventNotifyControl::kContinue;
        }
    };

    HotkeyListener g_hotkeyListener;

    void OnSKSEMessage(SKSE::MessagingInterface::Message* message)
    {
        if (message && message->type == SKSE::MessagingInterface::kInputLoaded) {
            if (auto* input = RE::BSInputDeviceManager::GetSingleton()) {
                input->AddEventSink(&g_hotkeyListener);
            }
        }
    }
}

SKSEPluginLoad(const SKSE::LoadInterface* skse)
{
    SKSE::Init(skse);
    LoadHotkey();

    if (const auto* serialization = SKSE::GetSerializationInterface()) {
        serialization->SetUniqueID(kSerializationID);
        serialization->SetSaveCallback(SaveCallback);
        serialization->SetLoadCallback(LoadCallback);
        serialization->SetRevertCallback(RevertCallback);
    }

    if (const auto* messaging = SKSE::GetMessagingInterface()) {
        messaging->RegisterListener(OnSKSEMessage);
        messaging->RegisterListener("DynamicInventoryIconInjector", OnDIIIMessage);
    }
    return true;
}
