// C++ port of niflib.net, added for the 2026 migration; licensed under GPL-2.0-or-later.
#include "niflib/ni_particles.h"

namespace niflib
{
    NiParticleSystemController::NiParticleSystemController(
        std::uint32_t version,
        NifBinaryReader& reader)
        : NiTimeController(version, reader)
    {
        if (version <= version_value(eNifVersion::VER_3_1))
        {
            old_speed = reader.read_u32();
        }
        if (version >= version_value(eNifVersion::VER_3_3_0_13))
        {
            speed = reader.read_f32();
        }
        random_speed = reader.read_f32();
        vertical_direction = reader.read_f32();
        vertical_angle = reader.read_f32();
        horizontal_direction = reader.read_f32();
        horizontal_angle = reader.read_f32();
        unknown_normal = reader.read_vec3();
        unknown_color = reader.read_color4();
        size = reader.read_f32();
        emit_start_time = reader.read_f32();
        emit_stop_time = reader.read_f32();
        if (version >= version_value(eNifVersion::VER_4_0_0_2))
        {
            unknown_byte = reader.read_u8();
        }
        if (version <= version_value(eNifVersion::VER_3_1))
        {
            old_emit_rate = reader.read_u32();
        }
        if (version >= version_value(eNifVersion::VER_3_3_0_13))
        {
            emit_rate = reader.read_f32();
        }
        lifetime = reader.read_f32();
        lifetime_random = reader.read_f32();
        if (version >= version_value(eNifVersion::VER_4_0_0_2))
        {
            emit_flags = reader.read_u16();
        }
        start_random = reader.read_vec3();
        emitter = NiRef<NiObject>(reader.read_u32());

        if (version >= version_value(eNifVersion::VER_4_0_0_2))
        {
            reader.read_u16();
            reader.read_f32();
            reader.read_u32();
            reader.read_u32();
            reader.read_u16();
        }
        if (version <= version_value(eNifVersion::VER_3_1))
        {
            particle_velocity = reader.read_vec3();
            particle_unknown_vector = reader.read_vec3();
            particle_lifetime = reader.read_f32();
            particle_link = NiRef<NiObject>(reader.read_u32());
            particle_timestamp = reader.read_u32();
            particle_unknown_short = reader.read_u16();
            particle_vertex_id = reader.read_u16();
        }
        if (version >= version_value(eNifVersion::VER_4_0_0_2))
        {
            num_particles = reader.read_u16();
            num_valid = reader.read_u16();
            if (num_particles > 65535)
            {
                throw NifFormatError("NIF particle count exceeds the supported limit.");
            }
            particles.reserve(num_particles);
            for (std::uint16_t index = 0; index < num_particles; ++index)
            {
                particles.emplace_back(reader);
            }
            unknown_ref = NiRef<NiObject>(reader.read_u32());
        }
        particle_extra = NiRef<NiParticleModifier>(reader.read_u32());
        unknown_ref_2 = NiRef<NiObject>(reader.read_u32());
        if (version >= version_value(eNifVersion::VER_4_0_0_2))
        {
            trailer = reader.read_u8();
        }
        if (version <= version_value(eNifVersion::VER_3_1))
        {
            color_data = NiRef<NiColorData>(reader.read_u32());
            unknown_float_1 = reader.read_f32();
            unknown_floats_2.reserve(particle_unknown_short);
            for (std::uint16_t index = 0; index < particle_unknown_short; ++index)
            {
                unknown_floats_2.push_back(reader.read_f32());
            }
        }
    }
}