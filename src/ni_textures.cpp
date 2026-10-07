// C++ port of niflib.net, added for the 2026 migration; licensed under GPL-2.0-or-later.
#include "niflib/ni_textures.h"

#include "niflib/nif_records.h"

namespace niflib
{
    namespace
    {
        constexpr std::uint32_t maximum_shader_texture_count = 65536;

        std::string read_texture_string(NifBinaryReader& reader)
        {
            const std::uint32_t length = reader.read_u32();
            if (length > 16384)
            {
                throw NifFormatError("NIF texture path exceeds the supported length.");
            }
            return reader.read_string(length);
        }

        std::optional<TexDesc> read_optional_descriptor(
            std::uint32_t version,
            NifBinaryReader& reader)
        {
            if (!reader.read_bool(static_cast<eNifVersion>(version)))
            {
                return std::nullopt;
            }
            return TexDesc(version, reader);
        }
    }

    NiSourceTexture::NiSourceTexture(std::uint32_t version, NifBinaryReader& reader)
        : NiTexture(version, reader)
    {
        use_external = reader.read_bool(static_cast<eNifVersion>(version));
        if (use_external)
        {
            file_name = read_texture_string(reader);
            if (version >= version_value(eNifVersion::VER_10_1_0_0))
            {
                reader.read_u32();
            }
        }
        else
        {
            if (version <= version_value(eNifVersion::VER_10_0_1_0))
            {
                reader.read_u8();
            }
            if (version >= version_value(eNifVersion::VER_10_1_0_0))
            {
                file_name = read_texture_string(reader);
            }
            internal_texture = NiRef<ATextureRenderData>(reader.read_u32());
        }

        pixel_layout = static_cast<ePixelLayout>(reader.read_u32());
        use_mipmaps = static_cast<eMipMapFormat>(reader.read_u32());
        alpha_format = static_cast<eAlphaFormat>(reader.read_u32());
        is_static = reader.read_bool(static_cast<eNifVersion>(version));
        if (version >= version_value(eNifVersion::VER_10_1_0_106))
        {
            direct_render = reader.read_bool(static_cast<eNifVersion>(version));
        }
        if (version >= version_value(eNifVersion::VER_20_2_0_7))
        {
            persistent_render_data = reader.read_bool(static_cast<eNifVersion>(version));
        }
    }

    TexDesc::TexDesc(std::uint32_t version, NifBinaryReader& reader)
        : source(reader.read_u32())
    {
        if (version <= version_value(eNifVersion::VER_20_0_0_5))
        {
            clamp_mode = static_cast<eTexClampMode>(reader.read_u32());
            filter_mode = static_cast<eTexFilterMode>(reader.read_u32());
        }
        if (version >= version_value(eNifVersion::VER_20_1_0_3))
        {
            flags = reader.read_u16();
        }
        if (version <= version_value(eNifVersion::VER_20_0_0_5))
        {
            uv_set_index = reader.read_u32();
        }
        if (version <= version_value(eNifVersion::VER_10_4_0_1))
        {
            ps2_l = static_cast<std::int16_t>(reader.read_u16());
            ps2_k = static_cast<std::int16_t>(reader.read_u16());
        }
        if (version <= version_value(eNifVersion::VER_4_1_0_12))
        {
            reader.read_u16();
        }
        if (version >= version_value(eNifVersion::VER_10_1_0_0))
        {
            has_texture_transform = reader.read_bool(static_cast<eNifVersion>(version));
            if (has_texture_transform)
            {
                translation = reader.read_vec2();
                tiling = reader.read_vec2();
                w_rotation = reader.read_f32();
                transform_type = reader.read_u32();
                center_offset = reader.read_vec2();
            }
        }
    }

    ShaderTexture::ShaderTexture(std::uint32_t version, NifBinaryReader& reader)
        : descriptor(version, reader),
          unknown_value(reader.read_u32())
    {
    }

    NiTexturingProperty::NiTexturingProperty(std::uint32_t version, NifBinaryReader& reader)
        : NiProperty(version, reader)
    {
        if (version <= version_value(eNifVersion::VER_10_0_1_2) ||
            version >= version_value(eNifVersion::VER_20_1_0_3))
        {
            flags = reader.read_u16();
        }
        if (version <= version_value(eNifVersion::VER_20_0_0_5))
        {
            apply_mode = reader.read_u32();
        }
        texture_count = reader.read_u32();
        base_texture = read_optional_descriptor(version, reader);
        dark_texture = read_optional_descriptor(version, reader);
        detail_texture = read_optional_descriptor(version, reader);
        gloss_texture = read_optional_descriptor(version, reader);
        glow_texture = read_optional_descriptor(version, reader);
        if (reader.read_bool(static_cast<eNifVersion>(version)))
        {
            bump_map_texture.emplace(version, reader);
            bump_map_luma_scale = reader.read_f32();
            bump_map_luma_offset = reader.read_f32();
            bump_map_matrix = reader.read_vec3();
            reader.read_f32();
        }
        decal_0_texture = read_optional_descriptor(version, reader);

        if (version >= version_value(eNifVersion::VER_10_0_1_0))
        {
            num_shader_textures = reader.read_u32();
            if (num_shader_textures > maximum_shader_texture_count)
            {
                throw NifFormatError("NIF shader texture count exceeds the supported limit.");
            }
            for (std::uint32_t index = 0; index < num_shader_textures; ++index)
            {
                if (reader.read_bool(static_cast<eNifVersion>(version)))
                {
                    shader_textures.emplace_back(version, reader);
                }
            }
        }
    }
}