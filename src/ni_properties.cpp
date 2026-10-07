// C++ port of niflib.net, added for the 2026 migration; licensed under GPL-2.0-or-later.
#include "niflib/ni_properties.h"

namespace niflib
{
    NiAlphaProperty::NiAlphaProperty(std::uint32_t version, NifBinaryReader& reader)
        : NiProperty(version, reader),
          flags(reader.read_u16()),
          threshold(reader.read_u8())
    {
    }

    NiMaterialProperty::NiMaterialProperty(std::uint32_t version, NifBinaryReader& reader)
        : NiProperty(version, reader)
    {
        if (version <= version_value(eNifVersion::VER_10_0_1_2))
        {
            flags = reader.read_u16();
        }
        ambient_color = reader.read_vec3();
        diffuse_color = reader.read_vec3();
        specular_color = reader.read_vec3();
        emissive_color = reader.read_vec3();
        glossiness = reader.read_f32();
        alpha = reader.read_f32();
    }

    NiVertexColorProperty::NiVertexColorProperty(std::uint32_t version, NifBinaryReader& reader)
        : NiProperty(version, reader),
          flags(reader.read_u16())
    {
        if (version > version_value(eNifVersion::VER_20_0_0_5))
        {
            throw NifFormatError("Unsupported NiVertexColorProperty version.");
        }
        vertex_mode = reader.read_u32();
        lighting_mode = reader.read_u32();
    }

    NiSpecularProperty::NiSpecularProperty(std::uint32_t version, NifBinaryReader& reader)
        : NiProperty(version, reader),
          flags(reader.read_u16())
    {
    }

    NiShadeProperty::NiShadeProperty(std::uint32_t version, NifBinaryReader& reader)
        : NiProperty(version, reader),
          flags(reader.read_u16())
    {
    }

    NiDitherProperty::NiDitherProperty(std::uint32_t version, NifBinaryReader& reader)
        : NiProperty(version, reader),
          flags(reader.read_u16())
    {
    }

    NiWireframeProperty::NiWireframeProperty(std::uint32_t version, NifBinaryReader& reader)
        : NiProperty(version, reader),
          flags(reader.read_u16())
    {
    }

    NiZBufferProperty::NiZBufferProperty(std::uint32_t version, NifBinaryReader& reader)
        : NiProperty(version, reader),
          flags(reader.read_u16())
    {
        if (version >= version_value(eNifVersion::VER_4_1_0_12) &&
            version <= version_value(eNifVersion::VER_20_0_0_5))
        {
            z_compare_mode = reader.read_u32();
        }
    }

    NiStencilProperty::NiStencilProperty(std::uint32_t version, NifBinaryReader& reader)
        : NiProperty(version, reader)
    {
        if (version <= version_value(eNifVersion::VER_10_0_1_2))
        {
            flags = reader.read_u16();
        }
        if (version <= version_value(eNifVersion::VER_20_0_0_5))
        {
            is_stencil_enabled = reader.read_bool(static_cast<eNifVersion>(version));
            stencil_function = static_cast<eStencilCompareMode>(reader.read_u32());
            stencil_ref = reader.read_u32();
            stencil_mask = reader.read_u32();
            fail_action = static_cast<eStencilAction>(reader.read_u32());
            z_fail_action = static_cast<eStencilAction>(reader.read_u32());
            pass_action = static_cast<eStencilAction>(reader.read_u32());
            face_draw_mode = static_cast<eFaceDrawMode>(reader.read_u32());
        }
        if (version >= version_value(eNifVersion::VER_20_1_0_3))
        {
            flags = reader.read_u16();
            stencil_ref = reader.read_u32();
            stencil_mask = reader.read_u32();
        }
    }

    NiFogProperty::NiFogProperty(std::uint32_t version, NifBinaryReader& reader)
        : NiProperty(version, reader),
          flags(reader.read_u16()),
          depth(reader.read_f32()),
          color(reader.read_vec3())
    {
    }
}