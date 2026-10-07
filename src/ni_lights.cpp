// C++ port of niflib.net, added for the 2026 migration; licensed under GPL-2.0-or-later.
#include "niflib/ni_lights.h"
#include "niflib/ni_textures.h"

namespace niflib
{
    namespace
    {
        constexpr std::uint32_t maximum_affected_node_count = 1'000'000;
    }

    NiDynamicEffect::NiDynamicEffect(std::uint32_t version, NifBinaryReader& reader)
        : NiAVObject(version, reader)
    {
        if (version >= version_value(eNifVersion::VER_10_1_0_106))
        {
            switch_state = reader.read_bool(static_cast<eNifVersion>(version));
        }
        if (version <= version_value(eNifVersion::VER_4_0_0_2) ||
            version >= version_value(eNifVersion::VER_10_0_1_0))
        {
            const std::uint32_t count = reader.read_u32();
            if (count > maximum_affected_node_count)
            {
                throw NifFormatError("NIF affected-node count exceeds the supported limit.");
            }
            affected_nodes.reserve(count);
            for (std::uint32_t index = 0; index < count; ++index)
            {
                affected_nodes.emplace_back(reader.read_u32());
            }
        }
    }

    NiLight::NiLight(std::uint32_t version, NifBinaryReader& reader)
        : NiDynamicEffect(version, reader),
          dimmer(reader.read_f32()),
          ambient_color(reader.read_vec3()),
          diffuse_color(reader.read_vec3()),
          specular_color(reader.read_vec3())
    {
    }

    NiPointLight::NiPointLight(std::uint32_t version, NifBinaryReader& reader)
        : NiLight(version, reader),
          constant_attenuation(reader.read_f32()),
          linear_attenuation(reader.read_f32()),
          quadratic_attenuation(reader.read_f32())
    {
    }

    NiSpotLight::NiSpotLight(std::uint32_t version, NifBinaryReader& reader)
        : NiPointLight(version, reader),
          cutoff_angle(reader.read_f32())
    {
        if (version >= version_value(eNifVersion::VER_20_2_0_7))
        {
            unknown_float = reader.read_f32();
        }
        exponent = reader.read_f32();
    }

    NiTextureEffect::NiTextureEffect(std::uint32_t version, NifBinaryReader& reader)
        : NiDynamicEffect(version, reader),
          model_projection_matrix(reader.read_matrix33()),
          model_projection_transform(reader.read_vec3()),
          texture_filtering(static_cast<eTexFilterMode>(reader.read_u32())),
          texture_clamping(static_cast<eTexClampMode>(reader.read_u32())),
          effect_type(static_cast<eEffectType>(reader.read_u32())),
          coord_gen_type(static_cast<eCoordGenType>(reader.read_u32()))
    {
        if (version <= version_value(eNifVersion::VER_3_1))
        {
            throw NifFormatError("NIF texture effects before version 4.0 are unsupported.");
        }
        if (version >= version_value(eNifVersion::VER_4_0_0_0))
        {
            source_texture = NiRef<NiSourceTexture>(reader.read_u32());
        }
        clipping_plane = reader.read_bool(static_cast<eNifVersion>(version));
        unknown_vector = reader.read_vec3();
        unknown_2 = reader.read_f32();
        if (version <= version_value(eNifVersion::VER_10_2_0_0))
        {
            ps2_l = static_cast<std::int16_t>(reader.read_u16());
            ps2_k = static_cast<std::int16_t>(reader.read_u16());
        }
        if (version <= version_value(eNifVersion::VER_4_1_0_12))
        {
            unknown_3 = reader.read_u16();
        }
    }
}