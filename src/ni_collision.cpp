// C++ port of niflib.net, added for the 2026 migration; licensed under GPL-2.0-or-later.
#include "niflib/ni_collision.h"

namespace niflib
{
    NiCollisionObject::NiCollisionObject(std::uint32_t version, NifBinaryReader& reader)
        : NiObject(version),
          target(reader.read_u32())
    {
    }

    NiParticleModifier::NiParticleModifier(std::uint32_t version, NifBinaryReader& reader)
        : NiObject(version),
          next(reader.read_u32())
    {
        if (version >= version_value(eNifVersion::VER_4_0_0_2))
        {
            controller = NiRef<NiParticleSystemController>(reader.read_u32());
        }
    }

    NiGravity::NiGravity(std::uint32_t version, NifBinaryReader& reader)
        : NiParticleModifier(version, reader)
    {
        if (version >= version_value(eNifVersion::VER_4_0_0_2))
        {
            unknown_float_1 = reader.read_f32();
        }
        force = reader.read_f32();
        type = reader.read_u32();
        position = reader.read_vec3();
        direction = reader.read_vec3();
    }

    NiPlanarCollider::NiPlanarCollider(std::uint32_t version, NifBinaryReader& reader)
        : NiParticleModifier(version, reader)
    {
        if (version >= version_value(eNifVersion::VER_10_0_1_0))
        {
            unknown_short_1 = reader.read_u16();
        }
        unknown_floats[0] = reader.read_f32();
        unknown_floats[1] = reader.read_f32();
        if (version == version_value(eNifVersion::VER_4_2_2_0))
        {
            unknown_short_2 = reader.read_u16();
        }
        for (std::size_t index = 2; index < unknown_floats.size(); ++index)
        {
            unknown_floats[index] = reader.read_f32();
        }
    }

    NiSphericalCollider::NiSphericalCollider(std::uint32_t version, NifBinaryReader& reader)
        : NiParticleModifier(version, reader),
          unknown_float_1(reader.read_f32()),
          unknown_short_1(reader.read_u16()),
          unknown_float_2(reader.read_f32())
    {
        if (version <= version_value(eNifVersion::VER_4_2_0_2))
        {
            unknown_short_2 = reader.read_u16();
        }
        else
        {
            unknown_float_3 = reader.read_f32();
        }
        unknown_float_4 = reader.read_f32();
        unknown_float_5 = reader.read_f32();
    }

    NiParticleColorModifier::NiParticleColorModifier(std::uint32_t version, NifBinaryReader& reader)
        : NiParticleModifier(version, reader),
          data(reader.read_u32())
    {
    }

    NiParticleBomb::NiParticleBomb(std::uint32_t version, NifBinaryReader& reader)
        : NiParticleModifier(version, reader),
          decay(reader.read_f32()),
          duration(reader.read_f32()),
          delta_v(reader.read_f32()),
          start(reader.read_f32()),
          decay_type(static_cast<eDecayType>(reader.read_u32()))
    {
        if (version >= version_value(eNifVersion::VER_4_1_0_12))
        {
            symmetry_type = static_cast<eSymmetryType>(reader.read_u32());
        }
        position = reader.read_vec3();
        direction = reader.read_vec3();
    }

    NiParticleGrowFade::NiParticleGrowFade(std::uint32_t version, NifBinaryReader& reader)
        : NiParticleModifier(version, reader),
          grow(reader.read_f32()),
          fade(reader.read_f32())
    {
    }

    NiParticleRotation::NiParticleRotation(std::uint32_t version, NifBinaryReader& reader)
        : NiParticleModifier(version, reader),
          random_initial_axis(reader.read_bool(static_cast<eNifVersion>(version))),
          initial_axis(reader.read_vec3()),
          speed(reader.read_f32())
    {
    }

    NiParticleMeshModifier::NiParticleMeshModifier(std::uint32_t version, NifBinaryReader& reader)
        : NiParticleModifier(version, reader)
    {
        const std::uint32_t count = reader.read_u32();
        if (count > 1'000'000)
        {
            throw NifFormatError("NIF particle mesh count exceeds the supported limit.");
        }
        particle_meshes.reserve(count);
        for (std::uint32_t index = 0; index < count; ++index)
        {
            particle_meshes.emplace_back(reader.read_u32());
        }
    }
}