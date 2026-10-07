// C++ port of niflib.net, added for the 2026 migration; licensed under GPL-2.0-or-later.
#pragma once

#include "niflib/ni_object.h"
#include "niflib/ni_ref.h"
#include "niflib/nif_types.h"

#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace niflib
{
    class NiCollisionObject;
    class NiDynamicEffect;
    class NiExtraData;
    class NiNode;
    class NiProperty;
    class NiTimeController;

    class NiObjectNET : public NiObject
    {
    public:
        std::string name;
        std::vector<NiRef<NiExtraData>> extra_data;
        NiRef<NiTimeController> controller;

        NiObjectNET(std::uint32_t version, NifBinaryReader& reader);
    };

    class NiAVObject : public NiObjectNET
    {
    public:
        std::uint16_t flags = 0;
        std::uint16_t unknown_short_1 = 0;
        Vec3 translation{};
        std::array<float, 9> rotation{};
        float scale = 0.0f;
        Vec3 velocity{};
        std::vector<NiRef<NiProperty>> properties;
        std::array<std::uint32_t, 4> unknown_ints_1{};
        std::uint8_t unknown_byte = 0;
        bool has_bounding_box = false;
        NiRef<NiCollisionObject> collision_object;
        std::weak_ptr<NiNode> parent;

        NiAVObject(std::uint32_t version, NifBinaryReader& reader);
    };

    class NiNode : public NiAVObject
    {
    public:
        std::vector<NiRef<NiAVObject>> children;
        std::vector<NiRef<NiDynamicEffect>> effects;

        NiNode(std::uint32_t version, NifBinaryReader& reader);
    };
}