// C++ port of niflib.net, added for the 2026 migration; licensed under GPL-2.0-or-later.
#include "niflib/ni_scene_variants.h"

#include "niflib/ni_geometry.h"

namespace niflib
{
    namespace
    {
        constexpr std::uint32_t maximum_lod_count = 1'000'000;

        std::vector<LODRange> read_lod_ranges(std::uint32_t version, NifBinaryReader &reader)
        {
            const std::uint32_t count = reader.read_u32();
            if (count > maximum_lod_count)
            {
                throw NifFormatError("NIF LOD level count exceeds the supported limit.");
            }

            std::vector<LODRange> ranges;
            ranges.reserve(count);
            for (std::uint32_t index = 0; index < count; ++index)
            {
                ranges.emplace_back(version, reader);
            }
            return ranges;
        }
    }

    NiLODData::NiLODData(std::uint32_t version) noexcept
        : NiObject(version)
    {
    }

    NiCamera::NiCamera(std::uint32_t version, NifBinaryReader &reader)
        : NiAVObject(version, reader)
    {
        if (version >= version_value(eNifVersion::VER_10_1_0_0))
        {
            unknown_1 = reader.read_u16();
        }
        frustum_left = reader.read_f32();
        frustum_right = reader.read_f32();
        frustum_top = reader.read_f32();
        frustum_bottom = reader.read_f32();
        frustum_near = reader.read_f32();
        frustum_far = reader.read_f32();
        if (version >= version_value(eNifVersion::VER_10_1_0_0))
        {
            use_orthographic_projection = reader.read_bool(static_cast<eNifVersion>(version));
        }
        viewport_left = reader.read_f32();
        viewport_right = reader.read_f32();
        viewport_top = reader.read_f32();
        viewport_bottom = reader.read_f32();
        lod_adjust = reader.read_f32();
        unknown_link = NiRef<NiObject>(reader.read_u32());
        unknown_2 = reader.read_u32();
        if (version >= version_value(eNifVersion::VER_4_2_1_0))
        {
            unknown_3 = reader.read_u32();
        }
        if (version <= version_value(eNifVersion::VER_3_1))
        {
            unknown_4 = reader.read_u32();
        }
    }

    NiSwitchNode::NiSwitchNode(std::uint32_t version, NifBinaryReader &reader)
        : NiNode(version, reader)
    {
        if (version >= version_value(eNifVersion::VER_10_0_1_0))
        {
            unknown_flags = reader.read_u16();
        }
        unknown_int = reader.read_i32();
    }

    NiBillboardNode::NiBillboardNode(std::uint32_t version, NifBinaryReader &reader)
        : NiNode(version, reader)
    {
        if (version >= version_value(eNifVersion::VER_10_1_0_0))
        {
            billboard_mode = static_cast<eBillboardMode>(reader.read_u16());
        }
    }

    NiLODNode::NiLODNode(std::uint32_t version, NifBinaryReader &reader)
        : NiSwitchNode(version, reader)
    {
        if (version >= version_value(eNifVersion::VER_4_0_0_2) &&
            version <= version_value(eNifVersion::VER_10_0_1_0))
        {
            lod_center = reader.read_vec3();
        }
        if (version <= version_value(eNifVersion::VER_10_0_1_0))
        {
            lod_levels = read_lod_ranges(version, reader);
        }
        if (version >= version_value(eNifVersion::VER_10_0_1_0))
        {
            lod_level_data = NiRef<NiLODData>(reader.read_u32());
        }
    }

    NiRangeLODData::NiRangeLODData(std::uint32_t version, NifBinaryReader &reader)
        : NiLODData(version),
          lod_center(reader.read_vec3()),
          lod_levels(read_lod_ranges(version, reader))
    {
    }

    NiScreenLODData::NiScreenLODData(std::uint32_t version, NifBinaryReader &reader)
        : NiLODData(version),
          bound_center(reader.read_vec3()),
          bound_radius(reader.read_f32()),
          world_center(reader.read_vec3()),
          world_radius(reader.read_f32())
    {
        const std::uint32_t count = reader.read_u32();
        if (count > maximum_lod_count)
        {
            throw NifFormatError("NIF screen LOD level count exceeds the supported limit.");
        }
        proportion_levels.reserve(count);
        for (std::uint32_t index = 0; index < count; ++index)
        {
            proportion_levels.push_back(reader.read_f32());
        }
    }
}