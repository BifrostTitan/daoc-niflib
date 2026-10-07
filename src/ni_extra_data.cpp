// C++ port of niflib.net, added for the 2026 migration; licensed under GPL-2.0-or-later.
#include "niflib/ni_extra_data.h"

namespace niflib
{
    namespace
    {
        std::string read_nif_string(NifBinaryReader& reader)
        {
            const std::uint32_t length = reader.read_u32();
            if (length > 16384)
            {
                throw NifFormatError("NIF string exceeds the supported length.");
            }
            return reader.read_string(length);
        }
    }

    NiExtraData::NiExtraData(std::uint32_t version, NifBinaryReader& reader)
        : NiObject(version)
    {
        if (version >= version_value(eNifVersion::VER_10_0_1_0))
        {
            name = read_nif_string(reader);
        }
        if (version <= version_value(eNifVersion::VER_4_2_2_0))
        {
            next_extra_data = NiRef<NiExtraData>(reader.read_u32());
        }
    }

    NiStringExtraData::NiStringExtraData(std::uint32_t version, NifBinaryReader& reader)
        : NiExtraData(version, reader)
    {
        if (version <= version_value(eNifVersion::VER_4_2_2_0))
        {
            bytes_remaining = reader.read_u32();
        }
        string_data = read_nif_string(reader);
    }

    NiBinaryExtraData::NiBinaryExtraData(std::uint32_t version, NifBinaryReader& reader)
        : NiExtraData(version, reader)
    {
        const std::uint32_t count = reader.read_u32();
        if (count > 64 * 1024 * 1024)
        {
            throw NifFormatError("NIF binary extra-data payload exceeds the supported limit.");
        }
        data.reserve(count);
        for (std::uint32_t index = 0; index < count; ++index)
        {
            data.push_back(reader.read_u8());
        }
    }

    NiIntegerExtraData::NiIntegerExtraData(std::uint32_t version, NifBinaryReader& reader)
        : NiExtraData(version, reader),
          data(reader.read_u32())
    {
    }

    NiBooleanExtraData::NiBooleanExtraData(std::uint32_t version, NifBinaryReader& reader)
        : NiExtraData(version, reader),
          data(reader.read_bool(static_cast<eNifVersion>(version)))
    {
    }

    NiFloatExtraData::NiFloatExtraData(std::uint32_t version, NifBinaryReader& reader)
        : NiExtraData(version, reader),
          data(reader.read_f32())
    {
    }

    NiColorExtraData::NiColorExtraData(std::uint32_t version, NifBinaryReader& reader)
        : NiExtraData(version, reader),
          data(reader.read_color4())
    {
    }

    NiVectorExtraData::NiVectorExtraData(std::uint32_t version, NifBinaryReader& reader)
        : NiExtraData(version, reader),
          data(reader.read_vec3()),
          unknown_float(reader.read_f32())
    {
    }

    NiIntegersExtraData::NiIntegersExtraData(std::uint32_t version, NifBinaryReader& reader)
        : NiExtraData(version, reader)
    {
        const std::uint32_t count = reader.read_u32();
        if (count > 1'000'000)
        {
            throw NifFormatError("NIF integer extra-data count exceeds the supported limit.");
        }
        extra_int_data.reserve(count);
        for (std::uint32_t index = 0; index < count; ++index)
        {
            extra_int_data.push_back(reader.read_u32());
        }
    }

    NiStringsExtraData::NiStringsExtraData(std::uint32_t version, NifBinaryReader& reader)
        : NiExtraData(version, reader)
    {
        const std::uint32_t count = reader.read_u32();
        if (count > 1'000'000)
        {
            throw NifFormatError("NIF string extra-data count exceeds the supported limit.");
        }
        extra_string_data.reserve(count);
        for (std::uint32_t index = 0; index < count; ++index)
        {
            extra_string_data.push_back(read_nif_string(reader));
        }
    }

    StringKey::StringKey(NifBinaryReader& reader)
        : time(reader.read_f32()),
          value(read_nif_string(reader))
    {
    }

    NiTextKeyExtraData::NiTextKeyExtraData(std::uint32_t version, NifBinaryReader& reader)
        : NiExtraData(version, reader)
    {
        if (version <= version_value(eNifVersion::VER_4_2_2_0))
        {
            unknown_int_1 = reader.read_u32();
        }
        const std::uint32_t count = reader.read_u32();
        if (count > 1'000'000)
        {
            throw NifFormatError("NIF text-key count exceeds the supported limit.");
        }
        text_keys.reserve(count);
        for (std::uint32_t index = 0; index < count; ++index)
        {
            text_keys.emplace_back(reader);
        }
    }
}