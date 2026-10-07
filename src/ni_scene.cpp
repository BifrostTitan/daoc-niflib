// C++ port of niflib.net, added for the 2026 migration; licensed under GPL-2.0-or-later.
#include "niflib/ni_scene.h"

#include "niflib/nif_binary_reader.h"

namespace niflib
{
    namespace
    {
        constexpr std::uint32_t maximum_array_count = 1'000'000;

        template <typename T>
        std::vector<NiRef<T>> read_refs(NifBinaryReader& reader, std::uint32_t count)
        {
            if (count > maximum_array_count)
            {
                throw NifFormatError("NIF reference count exceeds the supported limit.");
            }

            std::vector<NiRef<T>> refs;
            refs.reserve(count);
            for (std::uint32_t index = 0; index < count; ++index)
            {
                refs.emplace_back(reader.read_u32());
            }
            return refs;
        }
    }

    NiObjectNET::NiObjectNET(std::uint32_t version, NifBinaryReader& reader)
        : NiObject(version)
    {
        const std::uint32_t name_length = reader.read_u32();
        name = reader.read_string(name_length);

        if (version <= version_value(eNifVersion::VER_2_3))
        {
            throw NifFormatError("NIF versions through 2.3 are unsupported by NiObjectNET.");
        }
        if (version >= version_value(eNifVersion::VER_3_0) &&
            version <= version_value(eNifVersion::VER_4_2_2_0))
        {
            extra_data.emplace_back(reader.read_u32());
        }
        if (version >= version_value(eNifVersion::VER_10_0_1_0))
        {
            extra_data = read_refs<NiExtraData>(reader, reader.read_u32());
        }
        if (version >= version_value(eNifVersion::VER_3_0))
        {
            controller = NiRef<NiTimeController>(reader.read_u32());
        }
    }

    NiAVObject::NiAVObject(std::uint32_t version, NifBinaryReader& reader)
        : NiObjectNET(version, reader)
    {
        if (version >= version_value(eNifVersion::VER_3_0))
        {
            flags = reader.read_u16();
        }
        if (version >= version_value(eNifVersion::VER_20_2_0_7))
        {
            throw NifFormatError("NIF versions 20.2.0.7 and newer are unsupported by NiAVObject.");
        }

        translation = reader.read_vec3();
        rotation = reader.read_matrix33();
        scale = reader.read_f32();
        if (version <= version_value(eNifVersion::VER_4_2_2_0))
        {
            velocity = reader.read_vec3();
        }
        if (version <= version_value(eNifVersion::VER_20_2_0_7))
        {
            properties = read_refs<NiProperty>(reader, reader.read_u32());
        }
        if (version <= version_value(eNifVersion::VER_2_3))
        {
            for (auto& value : unknown_ints_1)
            {
                value = reader.read_u32();
            }
            unknown_byte = reader.read_u8();
        }
        if (version >= version_value(eNifVersion::VER_3_0) &&
            version <= version_value(eNifVersion::VER_4_2_2_0))
        {
            has_bounding_box = reader.read_bool(static_cast<eNifVersion>(version));
            if (has_bounding_box)
            {
                throw NifFormatError("NIF bounding boxes are not supported yet.");
            }
        }
        if (version >= version_value(eNifVersion::VER_10_0_1_0))
        {
            collision_object = NiRef<NiCollisionObject>(reader.read_u32());
        }
    }

    NiNode::NiNode(std::uint32_t version, NifBinaryReader& reader)
        : NiAVObject(version, reader),
          children(read_refs<NiAVObject>(reader, reader.read_u32())),
          effects(read_refs<NiDynamicEffect>(reader, reader.read_u32()))
    {
    }
}