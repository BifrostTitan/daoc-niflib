// C++ port of niflib.net, added for the 2026 migration; licensed under GPL-2.0-or-later.
#pragma once

#include <cstdint>

namespace niflib
{
    class NiObject
    {
    public:
        explicit NiObject(std::uint32_t version) noexcept
            : version_(version)
        {
        }

        virtual ~NiObject() = default;

        std::uint32_t version() const noexcept
        {
            return version_;
        }

    private:
        std::uint32_t version_;
    };
}