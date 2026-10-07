#pragma once

#include <cstdint>
#include <functional>
#include <memory>

namespace Json
{
    class Value;
}

namespace RE
{
    class InventoryEntryData;
    enum class FormType;
}

namespace DIII
{
    // Minimal declarations of DIII's documented extension ABI.
    class ICondition
    {
    public:
        virtual ~ICondition() = default;
        virtual bool Match(RE::InventoryEntryData* entry) const = 0;
    };

    using ConditionBuilder = std::function<
        std::unique_ptr<ICondition>(const Json::Value& value, RE::FormType type)>;

    class IAPI
    {
    public:
        virtual ~IAPI() = default;
        virtual bool RegisterCondition(const char* name, ConditionBuilder builder) = 0;
        virtual std::uint32_t GetVersion() const = 0;
    };

    inline constexpr std::uint32_t kMessage_GetAPI = 0xD111;
}
