// C++ port of niflib.net, added for the 2026 migration; licensed under GPL-2.0-or-later.
#pragma once

#include "niflib/ni_scene.h"
#include "niflib/nif_types.h"

#include <cstdint>

namespace niflib
{
    class NiProperty : public NiObjectNET
    {
    public:
        using NiObjectNET::NiObjectNET;
    };

    class NiAlphaProperty : public NiProperty
    {
    public:
        std::uint16_t flags = 0;
        std::uint8_t threshold = 0;

        NiAlphaProperty(std::uint32_t version, NifBinaryReader& reader);
    };

    class NiMaterialProperty : public NiProperty
    {
    public:
        std::uint16_t flags = 0;
        Vec3 ambient_color{};
        Vec3 diffuse_color{};
        Vec3 specular_color{};
        Vec3 emissive_color{};
        float glossiness = 0.0f;
        float alpha = 0.0f;

        NiMaterialProperty(std::uint32_t version, NifBinaryReader& reader);
    };

    class NiVertexColorProperty : public NiProperty
    {
    public:
        std::uint16_t flags = 0;
        std::uint32_t vertex_mode = 0;
        std::uint32_t lighting_mode = 0;

        NiVertexColorProperty(std::uint32_t version, NifBinaryReader& reader);
    };

    class NiSpecularProperty : public NiProperty
    {
    public:
        std::uint16_t flags = 0;
        NiSpecularProperty(std::uint32_t version, NifBinaryReader& reader);
    };

    class NiShadeProperty : public NiProperty
    {
    public:
        std::uint16_t flags = 0;
        NiShadeProperty(std::uint32_t version, NifBinaryReader& reader);
    };

    class NiDitherProperty : public NiProperty
    {
    public:
        std::uint16_t flags = 0;
        NiDitherProperty(std::uint32_t version, NifBinaryReader& reader);
    };

    class NiWireframeProperty : public NiProperty
    {
    public:
        std::uint16_t flags = 0;
        NiWireframeProperty(std::uint32_t version, NifBinaryReader& reader);
    };

    class NiZBufferProperty : public NiProperty
    {
    public:
        std::uint16_t flags = 0;
        std::uint32_t z_compare_mode = 0;
        NiZBufferProperty(std::uint32_t version, NifBinaryReader& reader);
    };

    class NiStencilProperty : public NiProperty
    {
    public:
        std::uint16_t flags = 0;
        bool is_stencil_enabled = false;
        eStencilCompareMode stencil_function = eStencilCompareMode::TEST_NEVER;
        std::uint32_t stencil_ref = 0;
        std::uint32_t stencil_mask = 0;
        eStencilAction fail_action = eStencilAction::ACTION_KEEP;
        eStencilAction z_fail_action = eStencilAction::ACTION_KEEP;
        eStencilAction pass_action = eStencilAction::ACTION_KEEP;
        eFaceDrawMode face_draw_mode = eFaceDrawMode::DRAW_CCW_OR_BOTH;

        NiStencilProperty(std::uint32_t version, NifBinaryReader& reader);
    };

    class NiFogProperty : public NiProperty
    {
    public:
        std::uint16_t flags = 0;
        float depth = 0.0f;
        Vec3 color{};

        NiFogProperty(std::uint32_t version, NifBinaryReader& reader);
    };
}