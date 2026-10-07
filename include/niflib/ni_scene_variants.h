// C++ port of niflib.net, added for the 2026 migration; licensed under GPL-2.0-or-later.
#pragma once

#include "niflib/ni_ref.h"
#include "niflib/ni_scene.h"
#include "niflib/nif_records.h"
#include "niflib/nif_types.h"

#include <cstdint>
#include <vector>

namespace niflib
{
    class NiCamera : public NiAVObject
    {
    public:
        std::uint16_t unknown_1 = 0;
        float frustum_left = 0.0f;
        float frustum_right = 0.0f;
        float frustum_top = 0.0f;
        float frustum_bottom = 0.0f;
        float frustum_near = 0.0f;
        float frustum_far = 0.0f;
        bool use_orthographic_projection = false;
        float viewport_left = 0.0f;
        float viewport_right = 0.0f;
        float viewport_top = 0.0f;
        float viewport_bottom = 0.0f;
        float lod_adjust = 0.0f;
        NiRef<NiObject> unknown_link;
        std::uint32_t unknown_2 = 0;
        std::uint32_t unknown_3 = 0;
        std::uint32_t unknown_4 = 0;

        NiCamera(std::uint32_t version, NifBinaryReader& reader);
    };

    class NiLODData : public NiObject
    {
    public:
        explicit NiLODData(std::uint32_t version) noexcept;
    };

    class NiSwitchNode : public NiNode
    {
    public:
        std::uint16_t unknown_flags = 0;
        std::int32_t unknown_int = 0;

        NiSwitchNode(std::uint32_t version, NifBinaryReader& reader);
    };

    class NiBillboardNode : public NiNode
    {
    public:
        eBillboardMode billboard_mode = eBillboardMode::ALWAYS_FACE_CAMERA;

        NiBillboardNode(std::uint32_t version, NifBinaryReader& reader);
    };

    class NiLODNode : public NiSwitchNode
    {
    public:
        Vec3 lod_center{};
        std::vector<LODRange> lod_levels;
        NiRef<NiLODData> lod_level_data;

        NiLODNode(std::uint32_t version, NifBinaryReader& reader);
    };

    class NiRangeLODData : public NiLODData
    {
    public:
        Vec3 lod_center{};
        std::vector<LODRange> lod_levels;

        NiRangeLODData(std::uint32_t version, NifBinaryReader& reader);
    };

    class NiScreenLODData : public NiLODData
    {
    public:
        Vec3 bound_center{};
        float bound_radius = 0.0f;
        Vec3 world_center{};
        float world_radius = 0.0f;
        std::vector<float> proportion_levels;

        NiScreenLODData(std::uint32_t version, NifBinaryReader& reader);
    };
}