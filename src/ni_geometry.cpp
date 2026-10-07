// C++ port of niflib.net, added for the 2026 migration; licensed under GPL-2.0-or-later.
#include "niflib/ni_geometry.h"

#include <limits>

namespace niflib
{
    namespace
    {
        constexpr std::uint32_t maximum_geometry_array_count = 1'000'000;

        void check_count(std::uint32_t count)
        {
            if (count > maximum_geometry_array_count)
            {
                throw NifFormatError("NIF geometry array exceeds the supported limit.");
            }
        }
    }

    Triangle::Triangle(NifBinaryReader& reader)
        : x(reader.read_u16()),
          y(reader.read_u16()),
          z(reader.read_u16())
    {
    }

    NiGeometryData::NiGeometryData(
        std::uint32_t version,
        std::uint32_t user_version,
        NifBinaryReader& reader)
        : NiObject(version)
    {
        if (version >= version_value(eNifVersion::VER_10_2_0_0))
        {
            unknown_1 = reader.read_u32();
        }

        num_vertices = reader.read_u16();
        check_count(num_vertices);
        if (version >= version_value(eNifVersion::VER_10_1_0_0))
        {
            keep_flags = reader.read_u8();
            compress_flags = reader.read_u8();
        }

        has_vertices = reader.read_bool(static_cast<eNifVersion>(version));
        if (has_vertices)
        {
            vertices.reserve(num_vertices);
            for (std::uint32_t index = 0; index < num_vertices; ++index)
            {
                vertices.push_back(reader.read_vec3());
            }
        }

        std::uint8_t num_uv_sets = 0;
        if (version >= version_value(eNifVersion::VER_10_0_1_0))
        {
            num_uv_sets = reader.read_u8();
            t_space_flag = reader.read_u8();
        }

        has_normals = reader.read_bool(static_cast<eNifVersion>(version));
        if (has_normals)
        {
            normals.reserve(num_vertices);
            for (std::uint32_t index = 0; index < num_vertices; ++index)
            {
                normals.push_back(reader.read_vec3());
            }
        }

        if (version >= version_value(eNifVersion::VER_10_1_0_0))
        {
            binormals.resize(num_vertices);
            tangents.resize(num_vertices);
            if (has_normals && (t_space_flag & 0xF0) != 0)
            {
                for (auto& binormal : binormals)
                {
                    binormal = reader.read_vec3();
                }
                for (auto& tangent : tangents)
                {
                    tangent = reader.read_vec3();
                }
            }
        }

        center = reader.read_vec3();
        radius = reader.read_f32();

        has_vertex_colors = reader.read_bool(static_cast<eNifVersion>(version));
        if (has_vertex_colors)
        {
            vertex_colors.reserve(num_vertices);
            for (std::uint32_t index = 0; index < num_vertices; ++index)
            {
                vertex_colors.push_back(reader.read_color4());
            }
        }

        if (version <= version_value(eNifVersion::VER_4_2_2_0))
        {
            num_uv_sets = reader.read_u8();
            t_space_flag = reader.read_u8();
        }
        if (version <= version_value(eNifVersion::VER_4_0_0_2))
        {
            has_uv = reader.read_bool(static_cast<eNifVersion>(version));
        }

        const std::uint8_t uv_set_count =
            version < version_value(eNifVersion::VER_20_2_0_7) || user_version != 1
                ? static_cast<std::uint8_t>(num_uv_sets & 0x3F)
                : static_cast<std::uint8_t>(num_uv_sets & 0x01);
        uv_sets.resize(uv_set_count);
        for (auto& uv_set : uv_sets)
        {
            uv_set.reserve(num_vertices);
            for (std::uint32_t index = 0; index < num_vertices; ++index)
            {
                uv_set.push_back(reader.read_vec2());
            }
        }

        if (version >= version_value(eNifVersion::VER_10_0_1_0))
        {
            consistency_flags = reader.read_u16();
        }
        if (version >= version_value(eNifVersion::VER_20_0_0_4))
        {
            additional_data_id = reader.read_u32();
        }
    }

    NiTriBasedGeomData::NiTriBasedGeomData(
        std::uint32_t version,
        std::uint32_t user_version,
        NifBinaryReader& reader)
        : NiGeometryData(version, user_version, reader),
          num_triangles(reader.read_u16())
    {
    }

    NiTriShapeData::NiTriShapeData(
        std::uint32_t version,
        std::uint32_t user_version,
        NifBinaryReader& reader)
        : NiTriBasedGeomData(version, user_version, reader),
          num_triangle_points(reader.read_u32())
    {
        check_count(num_triangle_points);
        if (version >= version_value(eNifVersion::VER_10_1_0_0))
        {
            has_triangles = reader.read_bool(static_cast<eNifVersion>(version));
        }

        if (version <= version_value(eNifVersion::VER_10_0_1_2) || has_triangles ||
            version >= version_value(eNifVersion::VER_10_0_1_3))
        {
            check_count(num_triangles);
            triangles.reserve(num_triangles);
            for (std::uint16_t index = 0; index < num_triangles; ++index)
            {
                triangles.emplace_back(reader);
            }
            has_triangles = !triangles.empty();
        }

        if (version >= version_value(eNifVersion::VER_3_1))
        {
            const std::uint16_t group_count = reader.read_u16();
            match_groups.resize(group_count);
            for (auto& group : match_groups)
            {
                const std::uint16_t item_count = reader.read_u16();
                check_count(item_count);
                group.reserve(item_count);
                for (std::uint16_t index = 0; index < item_count; ++index)
                {
                    group.push_back(reader.read_u16());
                }
            }
        }
    }

    NiTriStripsData::NiTriStripsData(
        std::uint32_t version,
        std::uint32_t user_version,
        NifBinaryReader& reader)
        : NiTriBasedGeomData(version, user_version, reader)
    {
        const std::uint16_t strip_count = reader.read_u16();
        std::vector<std::uint16_t> point_counts;
        point_counts.reserve(strip_count);
        for (std::uint16_t index = 0; index < strip_count; ++index)
        {
            point_counts.push_back(reader.read_u16());
        }

        if (version >= version_value(eNifVersion::VER_10_0_1_3))
        {
            has_points = reader.read_bool(static_cast<eNifVersion>(version));
        }
        else
        {
            has_points = !point_counts.empty();
        }

        if (version < version_value(eNifVersion::VER_10_0_1_3) || has_points)
        {
            points.resize(strip_count);
            for (std::size_t strip = 0; strip < point_counts.size(); ++strip)
            {
                const std::uint16_t point_count = point_counts[strip];
                points[strip].reserve(point_count);
                for (std::uint16_t index = 0; index < point_count; ++index)
                {
                    points[strip].push_back(reader.read_u16());
                }
            }
        }
    }

    NiGeometry::NiGeometry(
        std::uint32_t version,
        std::uint32_t user_version,
        NifBinaryReader& reader)
        : NiAVObject(version, reader),
          data(reader.read_u32())
    {
        if (version >= version_value(eNifVersion::VER_3_3_0_13))
        {
            skin_instance = NiRef<NiSkinInstance>(reader.read_u32());
        }

        if (version >= version_value(eNifVersion::VER_20_2_0_7))
        {
            const std::uint32_t material_count = reader.read_u32();
            check_count(material_count);
            material_names.reserve(material_count);
            for (std::uint32_t index = 0; index < material_count; ++index)
            {
                const std::uint32_t name_length = reader.read_u32();
                material_names.push_back(reader.read_string(name_length));
            }
            material_extra_data.reserve(material_count);
            for (std::uint32_t index = 0; index < material_count; ++index)
            {
                material_extra_data.push_back(reader.read_i32());
            }
            active_material = reader.read_i32();
        }

        if (version >= version_value(eNifVersion::VER_10_0_1_0) &&
            version <= version_value(eNifVersion::VER_20_1_0_3))
        {
            has_shader = reader.read_bool(static_cast<eNifVersion>(version));
            if (has_shader)
            {
                const std::uint32_t name_length = reader.read_u32();
                shader_name = reader.read_string(name_length);
                unknown_integer = reader.read_u32();
            }
        }
        if (version == version_value(eNifVersion::VER_10_4_0_1))
        {
            reader.read_u32();
        }
        if (version >= version_value(eNifVersion::VER_20_2_0_7))
        {
            throw NifFormatError("NIF geometry versions 20.2.0.7 and newer are unsupported.");
        }
        static_cast<void>(user_version);
    }
}