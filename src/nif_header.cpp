// C++ port of niflib.net, added for the 2026 migration; licensed under GPL-2.0-or-later.
#include "niflib/nif_header.h"
#include "niflib/nif_binary_reader.h"
#include "niflib/nif_types.h"

#include <cstdint>

namespace niflib
{
    namespace
    {
        constexpr std::uint32_t maximum_version_line_length = 256;
        constexpr std::uint32_t maximum_string_length = 16384;
        constexpr std::uint32_t maximum_block_count = 1'000'000;
    }

    NifHeader NifHeader::read(std::istream& input)
    {
        NifBinaryReader reader(input);
        NifHeader header;
        header.version_text = reader.read_line(maximum_version_line_length);
        header.version = reader.read_u32();

        if (header.version >= version_value(eNifVersion::VER_10_1_0_0))
        {
            header.user_version = reader.read_u32();
        }

        if (header.version >= version_value(eNifVersion::VER_3_3_0_13))
        {
            header.block_count = reader.read_u32();
            if (header.block_count > maximum_block_count)
            {
                throw NifFormatError("NIF block count exceeds the supported limit.");
            }
        }

        if (header.version >= version_value(eNifVersion::VER_10_1_0_0) &&
            (header.user_version == 10 || header.user_version == 11))
        {
            header.user_version_2 = reader.read_u32();
        }

        if (header.version >= version_value(eNifVersion::VER_20_0_0_4))
        {
            throw NifFormatError("NIF versions 20.0.0.4 and newer are not supported yet.");
        }
        if (header.version == version_value(eNifVersion::VER_10_0_1_2))
        {
            throw NifFormatError("NIF version 10.0.1.2 is not supported.");
        }
        if (header.version >= version_value(eNifVersion::VER_10_1_0_0) &&
            (header.user_version == 10 || header.user_version == 11))
        {
            throw NifFormatError("NIF user versions 10 and 11 are not supported yet.");
        }

        if (header.version >= version_value(eNifVersion::VER_10_0_1_0))
        {
            const std::uint16_t block_type_count = reader.read_u16();
            header.block_types.reserve(block_type_count);
            for (std::uint16_t index = 0; index < block_type_count; ++index)
            {
                const std::uint32_t length = reader.read_u32();
                if (length > maximum_string_length)
                {
                    throw NifFormatError("NIF header string exceeds the supported length.");
                }
                header.block_types.push_back(reader.read_string(length));
            }

            header.block_type_indices.reserve(header.block_count);
            for (std::uint32_t index = 0; index < header.block_count; ++index)
            {
                const std::uint16_t block_type_index = reader.read_u16();
                if (block_type_index >= block_type_count)
                {
                    throw NifFormatError("NIF block references an invalid block type.");
                }
                header.block_type_indices.push_back(block_type_index);
            }
        }

        if (header.version >= version_value(eNifVersion::VER_10_0_1_0))
        {
            header.unknown_header_value = reader.read_u32();
        }

        return header;
    }
}