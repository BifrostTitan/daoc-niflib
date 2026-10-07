// C++ port of niflib.net, added for the 2026 migration; licensed under GPL-2.0-or-later.
#pragma once

#include "niflib/ni_scene.h"

#include <cstdint>
#include <vector>

namespace niflib
{
    class NiDynamicEffect : public NiAVObject
    {
    public:
        bool switch_state = true;
        std::vector<NiRef<NiAVObject>> affected_nodes;

        NiDynamicEffect(std::uint32_t version, NifBinaryReader& reader);
    };

    class NiLight : public NiDynamicEffect
    {
    public:
        float dimmer = 0.0f;
        Vec3 ambient_color{};
        Vec3 diffuse_color{};
        Vec3 specular_color{};

        NiLight(std::uint32_t version, NifBinaryReader& reader);
    };

    class NiAmbientLight : public NiLight
    {
    public:
        using NiLight::NiLight;
    };

    class NiDirectionalLight : public NiLight
    {
    public:
        using NiLight::NiLight;
    };

    class NiPointLight : public NiLight
    {
    public:
        float constant_attenuation = 0.0f;
        float linear_attenuation = 0.0f;
        float quadratic_attenuation = 0.0f;

        NiPointLight(std::uint32_t version, NifBinaryReader& reader);
    };

    class NiSpotLight : public NiPointLight
    {
    public:
        float cutoff_angle = 0.0f;
        float unknown_float = 0.0f;
        float exponent = 0.0f;

        NiSpotLight(std::uint32_t version, NifBinaryReader& reader);
    };

    class NiSourceTexture;

    class NiTextureEffect : public NiDynamicEffect
    {
    public:
        std::array<float, 9> model_projection_matrix{};
        Vec3 model_projection_transform{};
        eTexFilterMode texture_filtering = eTexFilterMode::FILTER_NEAREST;
        eTexClampMode texture_clamping = eTexClampMode::CLAMP_S_CLAMP_T;
        std::uint16_t unknown_1 = 0;
        eEffectType effect_type = eEffectType::EFFECT_PROJECTED_LIGHT;
        eCoordGenType coord_gen_type = eCoordGenType::CG_WORLD_PARALLEL;
        NiRef<NiSourceTexture> source_texture;
        bool clipping_plane = false;
        Vec3 unknown_vector{};
        float unknown_2 = 0.0f;
        std::int16_t ps2_l = 0;
        std::int16_t ps2_k = 0;
        std::uint16_t unknown_3 = 0;

        NiTextureEffect(std::uint32_t version, NifBinaryReader& reader);
    };
}