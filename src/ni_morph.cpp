// C++ port of niflib.net, added for the 2026 migration; licensed under GPL-2.0-or-later.
#include "niflib/ni_morph.h"

namespace niflib
{
    namespace
    {
        constexpr std::uint32_t maximum_morph_count = 65536;
        constexpr std::uint32_t maximum_morph_vertex_count = 1'000'000;

        std::string read_morph_string(NifBinaryReader& reader)
        {
            const std::uint32_t length = reader.read_u32();
            if (length > 16384)
            {
                throw NifFormatError("NIF morph name exceeds the supported length.");
            }
            return reader.read_string(length);
        }
    }

    Morph::Morph(std::uint32_t version, std::uint32_t vertex_count, NifBinaryReader& reader)
    {
        if (version >= version_value(eNifVersion::VER_10_1_0_106))
        {
            frame_name = read_morph_string(reader);
        }
        if (version <= version_value(eNifVersion::VER_10_1_0_0))
        {
            keys = KeyGroup<FloatKey>(reader);
        }
        if ((version >= version_value(eNifVersion::VER_10_1_0_106) &&
             version <= version_value(eNifVersion::VER_10_2_0_0)) ||
            (version >= version_value(eNifVersion::VER_20_0_0_4) &&
             version <= version_value(eNifVersion::VER_20_1_0_3)))
        {
            unknown_int = reader.read_u32();
        }
        if (vertex_count > maximum_morph_vertex_count)
        {
            throw NifFormatError("NIF morph vertex count exceeds the supported limit.");
        }
        vectors.reserve(vertex_count);
        for (std::uint32_t index = 0; index < vertex_count; ++index)
        {
            vectors.push_back(reader.read_vec3());
        }
    }

    NiMorphData::NiMorphData(std::uint32_t version, NifBinaryReader& reader)
        : NiObject(version),
          num_morphs(reader.read_u32()),
          num_vertices(reader.read_u32()),
          relative_targets(reader.read_u8())
    {
        if (num_morphs > maximum_morph_count || num_vertices > maximum_morph_vertex_count)
        {
            throw NifFormatError("NIF morph data dimensions exceed the supported limits.");
        }
        morphs.reserve(num_morphs);
        for (std::uint32_t index = 0; index < num_morphs; ++index)
        {
            morphs.emplace_back(version, num_vertices, reader);
        }
    }

    NiGeomMorpherController::NiGeomMorpherController(std::uint32_t version, NifBinaryReader& reader)
        : NiInterpController(version, reader)
    {
        if (version >= version_value(eNifVersion::VER_10_0_1_2))
        {
            extra_flags = reader.read_u16();
        }
        if (version == version_value(eNifVersion::VER_10_1_0_106))
        {
            unknown_2 = reader.read_u8();
        }
        data = NiRef<NiMorphData>(reader.read_u32());
        always_update = reader.read_bool(static_cast<eNifVersion>(version));
        if (version >= version_value(eNifVersion::VER_10_1_0_106))
        {
            num_interpolators = reader.read_u32();
            if (num_interpolators > maximum_morph_count)
            {
                throw NifFormatError("NIF morph interpolator count exceeds the supported limit.");
            }
            if (version < version_value(eNifVersion::VER_20_2_0_7))
            {
                interpolators.reserve(num_interpolators);
                for (std::uint32_t index = 0; index < num_interpolators; ++index)
                {
                    interpolators.emplace_back(reader.read_u32());
                }
            }
        }
        if (version >= version_value(eNifVersion::VER_20_0_0_4))
        {
            throw NifFormatError("NIF morph controllers 20.0.0.4 and newer are unsupported.");
        }
    }
}