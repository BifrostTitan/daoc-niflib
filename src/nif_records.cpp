// C++ port of niflib.net, added for the 2026 migration; licensed under GPL-2.0-or-later.
#include "niflib/nif_records.h"

namespace niflib
{
    namespace
    {
        constexpr std::uint32_t maximum_mip_map_count = 4096;
    }

    ChannelData::ChannelData(NifBinaryReader& reader)
        : type(static_cast<eChannelType>(reader.read_u32())),
          convention(static_cast<eChannelConvention>(reader.read_u32())),
          bits_per_channel(reader.read_u8()),
          unknown_byte(reader.read_u8())
    {
    }

    MipMap::MipMap(NifBinaryReader& reader)
        : width(reader.read_u32()),
          height(reader.read_u32()),
          offset(reader.read_u32())
    {
    }

    NiPalette::NiPalette(std::uint32_t version, NifBinaryReader& reader)
        : NiObject(version),
          unknown_byte(reader.read_u8())
    {
        const std::uint32_t color_count = reader.read_u32();
        if (color_count > 1'000'000)
        {
            throw NifFormatError("NIF palette size exceeds the supported limit.");
        }
        palette.reserve(color_count);
        for (std::uint32_t index = 0; index < color_count; ++index)
        {
            palette.push_back(reader.read_color4_byte());
        }
    }

    ATextureRenderData::ATextureRenderData(std::uint32_t version, NifBinaryReader& reader)
        : NiObject(version),
          pixel_format(static_cast<ePixelFormat>(reader.read_u32()))
    {
        if (version <= version_value(eNifVersion::VER_10_2_0_0))
        {
            red_mask = reader.read_u32();
            green_mask = reader.read_u32();
            blue_mask = reader.read_u32();
            alpha_mask = reader.read_u32();
            bits_per_pixel = reader.read_u8();
            for (auto& value : unknown_3_bytes)
            {
                value = reader.read_u8();
            }
            for (auto& value : unknown_8_bytes)
            {
                value = reader.read_u8();
            }
        }

        if (version >= version_value(eNifVersion::VER_10_0_1_0) &&
            version <= version_value(eNifVersion::VER_10_2_0_0))
        {
            unknown_int = reader.read_u32();
        }

        if (version >= version_value(eNifVersion::VER_20_0_0_4))
        {
            bits_per_pixel = reader.read_u8();
            unknown_int_2 = reader.read_u32();
            unknown_int_3 = reader.read_u32();
            flags = reader.read_u8();
            unknown_int_4 = reader.read_u32();
        }
        if (version >= version_value(eNifVersion::VER_20_3_0_6))
        {
            unknown_byte_1 = reader.read_u8();
        }
        if (version >= version_value(eNifVersion::VER_20_0_0_4))
        {
            for (auto& channel : channel_data)
            {
                channel = ChannelData(reader);
            }
        }

        palette = NiRef<NiPalette>(reader);
        num_mip_maps = reader.read_u32();
        if (num_mip_maps > maximum_mip_map_count)
        {
            throw NifFormatError("NIF mip-map count exceeds the supported limit.");
        }
        bytes_per_pixel = reader.read_u32();
        mip_maps.reserve(num_mip_maps);
        for (std::uint32_t index = 0; index < num_mip_maps; ++index)
        {
            mip_maps.emplace_back(reader);
        }
    }

    NiPixelData::NiPixelData(std::uint32_t version, NifBinaryReader& reader)
        : ATextureRenderData(version, reader),
          num_pixels(reader.read_u32())
    {
        constexpr std::uint32_t maximum_pixel_data_size = 256 * 1024 * 1024;
        if (num_pixels > maximum_pixel_data_size)
        {
            throw NifFormatError("NIF pixel payload exceeds the supported size.");
        }

        if (version >= version_value(eNifVersion::VER_20_0_0_4))
        {
            num_faces = reader.read_u32();
            if (num_faces > 4096 ||
                static_cast<std::uint64_t>(num_faces) * num_pixels > maximum_pixel_data_size)
            {
                throw NifFormatError("NIF pixel face data exceeds the supported size.");
            }
            pixel_data.resize(num_faces, std::vector<std::uint8_t>(num_pixels));
            for (auto& face : pixel_data)
            {
                for (auto& byte : face)
                {
                    byte = reader.read_u8();
                }
            }
        }
        if (version <= version_value(eNifVersion::VER_10_2_0_0))
        {
            num_faces = 1;
            auto& face = pixel_data.emplace_back();
            face.reserve(num_pixels);
            for (std::uint32_t index = 0; index < num_pixels; ++index)
            {
                face.push_back(reader.read_u8());
            }
        }
    }

    ByteKey::ByteKey(NifBinaryReader& reader, eKeyType type)
        : time(reader.read_f32())
    {
        if (type != eKeyType::LINEAR_KEY)
        {
            throw NifFormatError("Byte keys only support the linear key type.");
        }

        value = reader.read_u8();
        if (type == eKeyType::TBC_KEY)
        {
            tbc = reader.read_vec3();
        }
    }

    Color4Key::Color4Key(NifBinaryReader& reader, eKeyType type)
        : BaseKey(reader, type),
          time(reader.read_f32())
    {
        const auto key_type = static_cast<std::uint32_t>(type);
        if (key_type < static_cast<std::uint32_t>(eKeyType::LINEAR_KEY) ||
            key_type > static_cast<std::uint32_t>(eKeyType::TBC_KEY))
        {
            throw NifFormatError("Invalid color key type.");
        }

        value = reader.read_color4();
        if (type == eKeyType::QUADRATIC_KEY)
        {
            forward = reader.read_color4();
            backward = reader.read_color4();
        }
    }

    FloatKey::FloatKey(NifBinaryReader& reader, eKeyType type)
        : BaseKey(reader, type),
          time(reader.read_f32()),
          value(reader.read_f32())
    {
        const auto key_type = static_cast<std::uint32_t>(type);
        if (key_type < static_cast<std::uint32_t>(eKeyType::LINEAR_KEY) ||
            key_type > static_cast<std::uint32_t>(eKeyType::TBC_KEY))
        {
            throw NifFormatError("Invalid float key type.");
        }
        if (type == eKeyType::QUADRATIC_KEY)
        {
            forward = reader.read_f32();
            backward = reader.read_f32();
        }
        if (type == eKeyType::TBC_KEY)
        {
            tbc = reader.read_vec3();
        }
    }

    VecKey::VecKey(NifBinaryReader& reader, eKeyType type)
        : BaseKey(reader, type),
          time(reader.read_f32())
    {
        const auto key_type = static_cast<std::uint32_t>(type);
        if (key_type < static_cast<std::uint32_t>(eKeyType::LINEAR_KEY) ||
            key_type > static_cast<std::uint32_t>(eKeyType::TBC_KEY))
        {
            throw NifFormatError("Invalid vector key type.");
        }

        if (type == eKeyType::LINEAR_KEY)
        {
            value = reader.read_vec3();
        }
        if (type == eKeyType::QUADRATIC_KEY)
        {
            value = reader.read_vec3();
            forward = reader.read_vec3();
            backward = reader.read_vec3();
        }
        if (type == eKeyType::TBC_KEY)
        {
            value = reader.read_vec3();
            tbc = reader.read_vec3();
        }
    }

    QuatKey::QuatKey(NifBinaryReader& reader, eKeyType type)
        : time(reader.read_f32())
    {
        const auto key_type = static_cast<std::uint32_t>(type);
        if (key_type < static_cast<std::uint32_t>(eKeyType::LINEAR_KEY) ||
            key_type > static_cast<std::uint32_t>(eKeyType::TBC_KEY))
        {
            throw NifFormatError("Invalid quaternion key type.");
        }
        value = reader.read_vec4();
        if (type == eKeyType::TBC_KEY)
        {
            tbc = reader.read_vec3();
        }
    }

    LODRange::LODRange(std::uint32_t version, NifBinaryReader& reader)
        : near_extent(reader.read_f32()),
          far_extent(reader.read_f32())
    {
        if (version <= version_value(eNifVersion::VER_3_1))
        {
            unknown_ints.emplace();
            for (auto& value : *unknown_ints)
            {
                value = reader.read_u32();
            }
        }
    }
}