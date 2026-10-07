// C++ port of niflib.net, added for the 2026 migration; licensed under GPL-2.0-or-later.
#pragma once

#include "niflib/ni_geometry.h"
#include "niflib/ni_object.h"
#include "niflib/ni_ref.h"
#include "niflib/nif_types.h"

#include <array>
#include <cstdint>
#include <vector>

namespace niflib
{
    class NiSkinPartition;

    struct SkinTransform
    {
        std::array<float, 9> rotation{};
        Vec3 translation{};
        float scale = 0.0f;

        explicit SkinTransform(NifBinaryReader& reader);
    };

    struct SkinWeight
    {
        std::uint16_t index = 0;
        float weight = 0.0f;

        explicit SkinWeight(NifBinaryReader& reader);
    };

    struct SkinData
    {
        SkinTransform transform;
        Vec3 bounding_sphere_offset{};
        float bounding_sphere_radius = 0.0f;
        std::array<std::uint16_t, 13> unknown_13_shorts{};
        std::uint16_t num_vertices = 0;
        std::vector<SkinWeight> vertex_weights;

        SkinData(std::uint32_t version, std::uint32_t user_version, bool has_vertex_weights, NifBinaryReader& reader);
    };

    struct SkinPartitionUnknownItem
    {
        std::uint32_t flags = 0;
        std::array<float, 5> unknown_values{};

        explicit SkinPartitionUnknownItem(NifBinaryReader& reader);
    };

    using SkinPartitionUnkownItem1 = SkinPartitionUnknownItem;

    struct SkinPartition
    {
        std::uint16_t num_vertices = 0;
        std::uint16_t num_triangles = 0;
        std::uint16_t num_bones = 0;
        std::uint16_t num_strips = 0;
        std::uint16_t num_weights_per_vertex = 0;
        std::vector<std::uint16_t> bones;
        bool has_vertex_map = true;
        std::vector<std::uint16_t> vertex_map;
        bool has_vertex_weights = true;
        std::vector<std::vector<float>> vertex_weights;
        std::vector<std::uint16_t> strip_lengths;
        bool has_faces = true;
        std::vector<std::vector<std::uint16_t>> strips;
        std::vector<Triangle> triangles;
        bool has_bone_indices = false;
        std::vector<std::vector<std::uint8_t>> bone_indices;
        std::uint16_t unknown_short = 0;
        std::array<std::uint16_t, 6> unknown_shorts{};
        std::uint16_t num_vertices_2 = 0;
        std::vector<SkinPartitionUnknownItem> unknown_items;

        SkinPartition(std::uint32_t version, std::uint32_t user_version, NifBinaryReader& reader);
    };

    class NiSkinPartition : public NiObject
    {
    public:
        std::vector<SkinPartition> partitions;

        NiSkinPartition(std::uint32_t version, std::uint32_t user_version, NifBinaryReader& reader);
    };

    class NiSkinData : public NiObject
    {
    public:
        SkinTransform transform;
        NiRef<NiSkinPartition> partition;
        bool has_vertex_weights = true;
        std::vector<SkinData> bone_list;

        NiSkinData(std::uint32_t version, std::uint32_t user_version, NifBinaryReader& reader);
    };

    class NiSkinInstance : public NiObject
    {
    public:
        NiRef<NiSkinData> data;
        NiRef<NiSkinPartition> partition;
        NiRef<NiNode> skeleton_root;
        std::vector<NiRef<NiNode>> bones;

        NiSkinInstance(std::uint32_t version, NifBinaryReader& reader);
    };
}