// C++ port of niflib.net, added for the 2026 migration; licensed under GPL-2.0-or-later.
#pragma once

#include "niflib/ni_object.h"
#include "niflib/nif_binary_reader.h"
#include "niflib/ni_ref.h"
#include "niflib/ni_scene.h"
#include "niflib/nif_types.h"

#include <cstdint>
#include <vector>

namespace niflib
{
    class NiGeometryData;
    class NiSkinInstance;

    struct Triangle
    {
        std::uint16_t x = 0;
        std::uint16_t y = 0;
        std::uint16_t z = 0;

        Triangle() = default;
        explicit Triangle(NifBinaryReader& reader);
    };

    class NiGeometryData : public NiObject
    {
    public:
        std::uint32_t unknown_1 = 0;
        std::uint8_t keep_flags = 0;
        std::uint8_t compress_flags = 0;
        bool has_vertices = false;
        std::vector<Vec3> vertices;
        std::uint8_t t_space_flag = 0;
        bool has_normals = false;
        std::vector<Vec3> normals;
        bool has_vertex_colors = false;
        bool has_uv = false;
        std::uint16_t consistency_flags = 0;
        Vec3 center{};
        float radius = 0.0f;
        std::vector<Color4> vertex_colors;
        std::vector<std::vector<Vec2>> uv_sets;
        std::uint32_t additional_data_id = 0;
        std::vector<Vec3> binormals;
        std::vector<Vec3> tangents;
        std::uint32_t num_vertices = 0;

        NiGeometryData(std::uint32_t version, std::uint32_t user_version, NifBinaryReader& reader);
    };

    class NiTriBasedGeomData : public NiGeometryData
    {
    public:
        std::uint16_t num_triangles = 0;

        NiTriBasedGeomData(std::uint32_t version, std::uint32_t user_version, NifBinaryReader& reader);
    };

    class NiTriShapeData : public NiTriBasedGeomData
    {
    public:
        std::uint32_t num_triangle_points = 0;
        bool has_triangles = false;
        std::vector<Triangle> triangles;
        std::vector<std::vector<std::uint16_t>> match_groups;

        NiTriShapeData(std::uint32_t version, std::uint32_t user_version, NifBinaryReader& reader);
    };

    class NiTriStripsData : public NiTriBasedGeomData
    {
    public:
        bool has_points = false;
        std::vector<std::vector<std::uint16_t>> points;

        NiTriStripsData(std::uint32_t version, std::uint32_t user_version, NifBinaryReader& reader);
    };

    class NiGeometry : public NiAVObject
    {
    public:
        NiRef<NiGeometryData> data;
        NiRef<NiSkinInstance> skin_instance;
        std::vector<std::string> material_names;
        std::vector<std::int32_t> material_extra_data;
        std::int32_t active_material = 0;
        bool has_shader = false;
        std::string shader_name;
        std::uint32_t unknown_integer = 0;

        NiGeometry(std::uint32_t version, std::uint32_t user_version, NifBinaryReader& reader);
    };

    class NiTriBasedGeometry : public NiGeometry
    {
    public:
        using NiGeometry::NiGeometry;
    };

    class NiTriShape : public NiTriBasedGeometry
    {
    public:
        using NiTriBasedGeometry::NiTriBasedGeometry;
    };

    class NiTriStrips : public NiTriBasedGeometry
    {
    public:
        using NiTriBasedGeometry::NiTriBasedGeometry;
    };
}