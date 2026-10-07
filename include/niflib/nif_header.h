// C++ port of niflib.net, added for the 2026 migration; licensed under GPL-2.0-or-later.
#pragma once

#include <cstdint>
#include <iosfwd>
#include <string>
#include <vector>

namespace niflib
{
    struct NifHeader
    {
        std::string version_text;
        std::uint32_t version = 0;
        std::uint32_t user_version = 0;
        std::uint32_t user_version_2 = 0;
        std::uint32_t block_count = 0;
        std::vector<std::string> block_types;
        std::vector<std::uint16_t> block_type_indices;
        std::uint32_t unknown_header_value = 0;

        static NifHeader read(std::istream& input);
    };

    using NiHeader = NifHeader;
}