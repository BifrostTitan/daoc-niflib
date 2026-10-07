// C++ port of niflib.net, added for the 2026 migration; licensed under GPL-2.0-or-later.
#pragma once

#include "niflib/ni_animation.h"

#include <cstdint>
#include <string>
#include <vector>

namespace niflib
{
    struct Morph
    {
        std::string frame_name;
        KeyGroup<FloatKey> keys;
        std::uint32_t unknown_int = 0;
        std::vector<Vec3> vectors;

        Morph(std::uint32_t version, std::uint32_t vertex_count, NifBinaryReader& reader);
    };

    class NiMorphData : public NiObject
    {
    public:
        std::uint32_t num_morphs = 0;
        std::uint32_t num_vertices = 0;
        std::uint8_t relative_targets = 0;
        std::vector<Morph> morphs;

        NiMorphData(std::uint32_t version, NifBinaryReader& reader);
    };

    class NiGeomMorpherController : public NiInterpController
    {
    public:
        std::uint16_t extra_flags = 0;
        std::uint8_t unknown_2 = 0;
        NiRef<NiMorphData> data;
        bool always_update = false;
        std::uint32_t num_interpolators = 0;
        std::vector<NiRef<NiInterpolator>> interpolators;

        NiGeomMorpherController(std::uint32_t version, NifBinaryReader& reader);
    };
}