// C++ port of niflib.net, added for the 2026 migration; licensed under GPL-2.0-or-later.
#pragma once

#include "niflib/ni_geometry.h"
#include "niflib/ni_animation.h"
#include "niflib/ni_collision.h"

#include <cstdint>
#include <vector>

namespace niflib
{
    struct Particle
    {
        Vec3 velocity{};
        Vec3 unknown_vector{};
        float lifetime = 0.0f;
        float lifespan = 0.0f;
        float timestamp = 0.0f;
        std::uint16_t unknown_short = 0;
        std::uint16_t vertex_id = 0;

        explicit Particle(NifBinaryReader& reader);
    };

    class NiParticlesData : public NiGeometryData
    {
    public:
        std::uint16_t num_particles = 0;
        float particle_radius = 0.0f;
        bool has_radii = false;
        std::vector<float> radii;
        std::uint16_t num_active = 0;
        bool has_sizes = false;
        std::vector<float> sizes;
        bool has_rotations = false;
        std::vector<Vec4> rotations;

        NiParticlesData(std::uint32_t version, std::uint32_t user_version, NifBinaryReader& reader);
    };

    class NiAutoNormalParticlesData : public NiParticlesData
    {
    public:
        using NiParticlesData::NiParticlesData;
    };

    class NiRotatingParticlesData : public NiParticlesData
    {
    public:
        bool has_rotations_2 = false;
        std::vector<Vec4> rotations_2;

        NiRotatingParticlesData(std::uint32_t version, std::uint32_t user_version, NifBinaryReader& reader);
    };

    class NiParticleMeshesData : public NiRotatingParticlesData
    {
    public:
        NiRef<NiAVObject> unknown_link;
        NiParticleMeshesData(std::uint32_t version, std::uint32_t user_version, NifBinaryReader& reader);
    };

    class NiParticles : public NiGeometry
    {
    public:
        using NiGeometry::NiGeometry;
    };

    class NiAutoNormalParticles : public NiParticles
    {
    public:
        using NiParticles::NiParticles;
    };

    class NiRotatingParticles : public NiParticles
    {
    public:
        using NiParticles::NiParticles;
    };

    class NiParticleMeshes : public NiParticles
    {
    public:
        using NiParticles::NiParticles;
    };

    class NiParticleSystemController : public NiTimeController
    {
    public:
        std::uint32_t old_speed = 0;
        float speed = 0.0f;
        float random_speed = 0.0f;
        float vertical_direction = 0.0f;
        float vertical_angle = 0.0f;
        float horizontal_direction = 0.0f;
        float horizontal_angle = 0.0f;
        Vec3 unknown_normal{};
        Color4 unknown_color{};
        float size = 0.0f;
        float emit_start_time = 0.0f;
        float emit_stop_time = 0.0f;
        std::uint8_t unknown_byte = 0;
        std::uint32_t old_emit_rate = 0;
        float emit_rate = 0.0f;
        float lifetime = 0.0f;
        float lifetime_random = 0.0f;
        std::uint16_t emit_flags = 0;
        Vec3 start_random{};
        NiRef<NiObject> emitter;
        Vec3 particle_velocity{};
        Vec3 particle_unknown_vector{};
        float particle_lifetime = 0.0f;
        NiRef<NiObject> particle_link;
        std::uint32_t particle_timestamp = 0;
        std::uint16_t particle_unknown_short = 0;
        std::uint16_t particle_vertex_id = 0;
        std::uint16_t num_particles = 0;
        std::uint16_t num_valid = 0;
        std::vector<Particle> particles;
        NiRef<NiObject> unknown_ref;
        NiRef<NiParticleModifier> particle_extra;
        NiRef<NiObject> unknown_ref_2;
        std::uint8_t trailer = 0;
        NiRef<NiColorData> color_data;
        float unknown_float_1 = 0.0f;
        std::vector<float> unknown_floats_2;

        NiParticleSystemController(std::uint32_t version, NifBinaryReader& reader);
    };
}