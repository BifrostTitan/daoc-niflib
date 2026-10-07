// C++ port of niflib.net, added for the 2026 migration; licensed under GPL-2.0-or-later.
#include "niflib/ni_particles.h"

namespace niflib
{
    namespace
    {
        constexpr std::uint32_t maximum_particle_count = 1'000'000;

        std::vector<float> read_float_array(NifBinaryReader& reader, std::uint32_t count)
        {
            if (count > maximum_particle_count)
            {
                throw NifFormatError("NIF particle array exceeds the supported limit.");
            }
            std::vector<float> values;
            values.reserve(count);
            for (std::uint32_t index = 0; index < count; ++index)
            {
                values.push_back(reader.read_f32());
            }
            return values;
        }
    }

    Particle::Particle(NifBinaryReader& reader)
        : velocity(reader.read_vec3()),
          unknown_vector(reader.read_vec3()),
          lifetime(reader.read_f32()),
          lifespan(reader.read_f32()),
          timestamp(reader.read_f32()),
          unknown_short(reader.read_u16()),
          vertex_id(reader.read_u16())
    {
    }

    NiParticlesData::NiParticlesData(
        std::uint32_t version,
        std::uint32_t user_version,
        NifBinaryReader& reader)
        : NiGeometryData(version, user_version, reader)
    {
        if (version <= version_value(eNifVersion::VER_4_0_0_2))
        {
            num_particles = reader.read_u16();
        }
        if (version <= version_value(eNifVersion::VER_10_0_1_0))
        {
            particle_radius = reader.read_f32();
        }
        if (version >= version_value(eNifVersion::VER_10_1_0_0))
        {
            has_radii = reader.read_bool(static_cast<eNifVersion>(version));
            if (has_radii)
            {
                radii = read_float_array(reader, num_vertices);
            }
        }
        num_active = reader.read_u16();
        has_sizes = reader.read_bool(static_cast<eNifVersion>(version));
        if (has_sizes)
        {
            sizes = read_float_array(reader, num_vertices);
        }
        if (version >= version_value(eNifVersion::VER_10_0_1_0))
        {
            has_rotations = reader.read_bool(static_cast<eNifVersion>(version));
            if (has_rotations)
            {
                rotations.reserve(num_vertices);
                for (std::uint32_t index = 0; index < num_vertices; ++index)
                {
                    rotations.push_back(reader.read_vec4());
                }
            }
        }
    }

    NiRotatingParticlesData::NiRotatingParticlesData(
        std::uint32_t version,
        std::uint32_t user_version,
        NifBinaryReader& reader)
        : NiParticlesData(version, user_version, reader)
    {
        if (version <= version_value(eNifVersion::VER_4_2_2_0))
        {
            has_rotations_2 = reader.read_bool(static_cast<eNifVersion>(version));
            rotations_2.reserve(num_vertices);
            for (std::uint32_t index = 0; index < num_vertices; ++index)
            {
                rotations_2.push_back(reader.read_vec4());
            }
        }
    }

    NiParticleMeshesData::NiParticleMeshesData(
        std::uint32_t version,
        std::uint32_t user_version,
        NifBinaryReader& reader)
        : NiRotatingParticlesData(version, user_version, reader),
          unknown_link(reader.read_u32())
    {
    }
}