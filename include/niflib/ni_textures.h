// C++ port of niflib.net, added for the 2026 migration; licensed under GPL-2.0-or-later.
#pragma once

#include "niflib/ni_properties.h"
#include "niflib/ni_ref.h"
#include "niflib/nif_types.h"

#include <cstdint>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace niflib
{
    struct ATextureRenderData;

    class NiTexture : public NiObjectNET
    {
    public:
        using NiObjectNET::NiObjectNET;
    };

    class NiSourceTexture : public NiTexture
    {
    public:
        bool use_external = false;
        std::string file_name;
        ePixelLayout pixel_layout = ePixelLayout::PIX_LAY_DEFAULT;
        eMipMapFormat use_mipmaps = eMipMapFormat::MIP_FMT_DEFAULT;
        eAlphaFormat alpha_format = eAlphaFormat::ALPHA_DEFAULT;
        bool is_static = true;
        bool direct_render = false;
        bool persistent_render_data = false;
        NiRef<ATextureRenderData> internal_texture;

        NiSourceTexture(std::uint32_t version, NifBinaryReader& reader);
    };

    struct TexDesc
    {
        NiRef<NiSourceTexture> source;
        eTexClampMode clamp_mode = eTexClampMode::CLAMP_S_CLAMP_T;
        eTexFilterMode filter_mode = eTexFilterMode::FILTER_NEAREST;
        std::uint16_t flags = 0;
        std::uint32_t uv_set_index = 0;
        std::int16_t ps2_l = 0;
        std::int16_t ps2_k = 0;
        bool has_texture_transform = false;
        Vec2 translation{};
        Vec2 tiling{};
        float w_rotation = 0.0f;
        std::uint32_t transform_type = 0;
        Vec2 center_offset{};

        TexDesc(std::uint32_t version, NifBinaryReader& reader);
    };

    struct ShaderTexture
    {
        TexDesc descriptor;
        std::uint32_t unknown_value = 0;

        ShaderTexture(std::uint32_t version, NifBinaryReader& reader);
    };

    class NiTexturingProperty : public NiProperty
    {
    public:
        std::uint16_t flags = 0;
        std::uint32_t apply_mode = 0;
        std::uint32_t texture_count = 0;
        std::optional<TexDesc> base_texture;
        std::optional<TexDesc> dark_texture;
        std::optional<TexDesc> detail_texture;
        std::optional<TexDesc> gloss_texture;
        std::optional<TexDesc> glow_texture;
        std::optional<TexDesc> bump_map_texture;
        std::optional<TexDesc> decal_0_texture;
        float bump_map_luma_scale = 0.0f;
        float bump_map_luma_offset = 0.0f;
        Vec3 bump_map_matrix{};
        std::uint32_t num_shader_textures = 0;
        std::vector<ShaderTexture> shader_textures;

        NiTexturingProperty(std::uint32_t version, NifBinaryReader& reader);
    };
}