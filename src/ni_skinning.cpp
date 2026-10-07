// C++ port of niflib.net, added for the 2026 migration; licensed under GPL-2.0-or-later.
#include "niflib/ni_skinning.h"

#include <limits>

namespace niflib
{
    namespace
    {
        constexpr std::uint32_t maximum_skin_array_count = 1'000'000;

        void check_count(std::uint32_t count)
        {
            if (count > maximum_skin_array_count)
            {
                throw NifFormatError("NIF skin array exceeds the supported limit.");
            }
        }

        std::vector<std::uint16_t> read_u16_array(NifBinaryReader& reader, std::uint32_t count)
        {
            check_count(count);
            std::vector<std::uint16_t> values;
            values.reserve(count);
            for (std::uint32_t index = 0; index < count; ++index)
            {
                values.push_back(reader.read_u16());
            }
            return values;
        }
    }

    SkinTransform::SkinTransform(NifBinaryReader& reader)
        : rotation(reader.read_matrix33()),
          translation(reader.read_vec3()),
          scale(reader.read_f32())
    {
    }

    SkinWeight::SkinWeight(NifBinaryReader& reader)
        : index(reader.read_u16()),
          weight(reader.read_f32())
    {
    }

    SkinData::SkinData(
        std::uint32_t version,
        std::uint32_t user_version,
        bool has_weights,
        NifBinaryReader& reader)
        : transform(reader),
          bounding_sphere_offset(reader.read_vec3()),
          bounding_sphere_radius(reader.read_f32())
    {
        if (version == version_value(eNifVersion::VER_20_3_0_9) && user_version == 131072)
        {
            for (auto& value : unknown_13_shorts)
            {
                value = reader.read_u16();
            }
        }
        num_vertices = reader.read_u16();
        if (has_weights)
        {
            vertex_weights.reserve(num_vertices);
            for (std::uint16_t index = 0; index < num_vertices; ++index)
            {
                vertex_weights.emplace_back(reader);
            }
        }
    }

    SkinPartitionUnknownItem::SkinPartitionUnknownItem(NifBinaryReader& reader)
        : flags(reader.read_u32())
    {
        for (auto& value : unknown_values)
        {
            value = reader.read_f32();
        }
    }

    SkinPartition::SkinPartition(
        std::uint32_t version,
        std::uint32_t user_version,
        NifBinaryReader& reader)
        : num_vertices(reader.read_u16()),
          num_triangles(reader.read_u16()),
          num_bones(reader.read_u16()),
          num_strips(reader.read_u16()),
          num_weights_per_vertex(reader.read_u16()),
          bones(read_u16_array(reader, num_bones))
    {
        if (version >= version_value(eNifVersion::VER_10_1_0_0))
        {
            has_vertex_map = reader.read_bool(static_cast<eNifVersion>(version));
        }
        if (has_vertex_map)
        {
            vertex_map = read_u16_array(reader, num_vertices);
        }
        if (version >= version_value(eNifVersion::VER_10_1_0_0))
        {
            has_vertex_weights = reader.read_bool(static_cast<eNifVersion>(version));
        }
        if (has_vertex_weights)
        {
            vertex_weights.resize(num_vertices);
            for (auto& weights : vertex_weights)
            {
                weights.reserve(num_weights_per_vertex);
                for (std::uint16_t index = 0; index < num_weights_per_vertex; ++index)
                {
                    weights.push_back(reader.read_f32());
                }
            }
        }

        strip_lengths = read_u16_array(reader, num_strips);
        if (version >= version_value(eNifVersion::VER_10_1_0_0))
        {
            has_faces = reader.read_bool(static_cast<eNifVersion>(version));
        }
        if (has_faces && num_strips != 0)
        {
            strips.resize(num_strips);
            for (std::size_t strip = 0; strip < strips.size(); ++strip)
            {
                strips[strip] = read_u16_array(reader, strip_lengths[strip]);
            }
        }
        else if (has_faces)
        {
            check_count(num_triangles);
            triangles.reserve(num_triangles);
            for (std::uint16_t index = 0; index < num_triangles; ++index)
            {
                triangles.emplace_back(reader);
            }
        }

        has_bone_indices = reader.read_bool(static_cast<eNifVersion>(version));
        if (has_bone_indices)
        {
            bone_indices.resize(num_vertices);
            for (auto& indices : bone_indices)
            {
                indices.reserve(num_weights_per_vertex);
                for (std::uint16_t index = 0; index < num_weights_per_vertex; ++index)
                {
                    indices.push_back(reader.read_u8());
                }
            }
        }
        if (user_version >= 12)
        {
            unknown_short = reader.read_u16();
        }
        if (version == version_value(eNifVersion::VER_10_2_0_0) && user_version == 1)
        {
            for (auto& value : unknown_shorts)
            {
                value = reader.read_u16();
            }
            num_vertices_2 = unknown_shorts[2];
            check_count(num_vertices_2);
            unknown_items.reserve(num_vertices_2);
            for (std::uint16_t index = 0; index < num_vertices_2; ++index)
            {
                unknown_items.emplace_back(reader);
            }
        }
    }

    NiSkinPartition::NiSkinPartition(
        std::uint32_t version,
        std::uint32_t user_version,
        NifBinaryReader& reader)
        : NiObject(version)
    {
        const std::uint32_t count = reader.read_u32();
        check_count(count);
        partitions.reserve(count);
        for (std::uint32_t index = 0; index < count; ++index)
        {
            partitions.emplace_back(version, user_version, reader);
        }
    }

    NiSkinData::NiSkinData(std::uint32_t version, std::uint32_t user_version, NifBinaryReader& reader)
        : NiObject(version),
          transform(reader)
    {
        const std::uint32_t bone_count = reader.read_u32();
        check_count(bone_count);
        if (version >= version_value(eNifVersion::VER_4_0_0_2) &&
            version <= version_value(eNifVersion::VER_10_1_0_0))
        {
            partition = NiRef<NiSkinPartition>(reader.read_u32());
        }
        if (version >= version_value(eNifVersion::VER_4_2_1_0))
        {
            has_vertex_weights = reader.read_bool(static_cast<eNifVersion>(version));
        }
        if (has_vertex_weights)
        {
            bone_list.reserve(bone_count);
            for (std::uint32_t index = 0; index < bone_count; ++index)
            {
                bone_list.emplace_back(version, user_version, true, reader);
            }
        }
    }

    NiSkinInstance::NiSkinInstance(std::uint32_t version, NifBinaryReader& reader)
        : NiObject(version),
          data(reader.read_u32())
    {
        if (version >= version_value(eNifVersion::VER_10_2_0_0))
        {
            partition = NiRef<NiSkinPartition>(reader.read_u32());
        }
        skeleton_root = NiRef<NiNode>(reader.read_u32());
        const std::uint32_t bone_count = reader.read_u32();
        check_count(bone_count);
        bones.reserve(bone_count);
        for (std::uint32_t index = 0; index < bone_count; ++index)
        {
            bones.emplace_back(reader.read_u32());
        }
    }
}