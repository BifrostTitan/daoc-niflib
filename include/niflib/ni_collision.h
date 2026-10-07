// C++ port of niflib.net, added for the 2026 migration; licensed under GPL-2.0-or-later.
#pragma once

#include "niflib/ni_object.h"
#include "niflib/ni_ref.h"
#include "niflib/ni_scene.h"

#include <cstdint>

namespace niflib
{
    class NiColorData;

    class NiCollisionObject : public NiObject
    {
    public:
        NiRef<NiAVObject> target;

        NiCollisionObject(std::uint32_t version, NifBinaryReader& reader);
    };

    class NiParticleSystemController;

    class NiParticleModifier : public NiObject
    {
    public:
        NiRef<NiParticleModifier> next;
        NiRef<NiParticleSystemController> controller;

        NiParticleModifier(std::uint32_t version, NifBinaryReader& reader);
    };

    class NiGravity : public NiParticleModifier
    {
    public:
        float unknown_float_1 = 0.0f;
        float force = 0.0f;
        std::uint32_t type = 0;
        Vec3 position{};
        Vec3 direction{};

        NiGravity(std::uint32_t version, NifBinaryReader& reader);
    };

    class NiPlanarCollider : public NiParticleModifier
    {
    public:
        std::uint16_t unknown_short_1 = 0;
        std::uint16_t unknown_short_2 = 0;
        std::array<float, 16> unknown_floats{};

        NiPlanarCollider(std::uint32_t version, NifBinaryReader& reader);
    };

    class NiSphericalCollider : public NiParticleModifier
    {
    public:
        float unknown_float_1 = 0.0f;
        std::uint16_t unknown_short_1 = 0;
        float unknown_float_2 = 0.0f;
        std::uint16_t unknown_short_2 = 0;
        float unknown_float_3 = 0.0f;
        float unknown_float_4 = 0.0f;
        float unknown_float_5 = 0.0f;

        NiSphericalCollider(std::uint32_t version, NifBinaryReader& reader);
    };

    class NiParticleColorModifier : public NiParticleModifier
    {
    public:
        NiRef<NiColorData> data;
        NiParticleColorModifier(std::uint32_t version, NifBinaryReader& reader);
    };

    class NiParticleBomb : public NiParticleModifier
    {
    public:
        float decay = 0.0f;
        float duration = 0.0f;
        float delta_v = 0.0f;
        float start = 0.0f;
        eDecayType decay_type = eDecayType::DECAY_NONE;
        eSymmetryType symmetry_type = eSymmetryType::SPHERICAL_SYMMETRY;
        Vec3 position{};
        Vec3 direction{};

        NiParticleBomb(std::uint32_t version, NifBinaryReader& reader);
    };

    class NiParticleGrowFade : public NiParticleModifier
    {
    public:
        float grow = 0.0f;
        float fade = 0.0f;
        NiParticleGrowFade(std::uint32_t version, NifBinaryReader& reader);
    };

    class NiParticleRotation : public NiParticleModifier
    {
    public:
        bool random_initial_axis = false;
        Vec3 initial_axis{};
        float speed = 0.0f;
        NiParticleRotation(std::uint32_t version, NifBinaryReader& reader);
    };

    class NiParticleMeshModifier : public NiParticleModifier
    {
    public:
        std::vector<NiRef<NiAVObject>> particle_meshes;
        NiParticleMeshModifier(std::uint32_t version, NifBinaryReader& reader);
    };
}