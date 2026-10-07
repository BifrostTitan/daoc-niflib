// C++ port of niflib.net, added for the 2026 migration; licensed under GPL-2.0-or-later.
#include "niflib/ni_animation.h"

namespace niflib
{
    NiTimeController::NiTimeController(std::uint32_t version, NifBinaryReader& reader)
        : NiObject(version),
          next_controller(reader.read_u32()),
          flags(reader.read_u16()),
          frequency(reader.read_f32()),
          phase(reader.read_f32()),
          start_time(reader.read_f32()),
          stop_time(reader.read_f32())
    {
        if (version >= version_value(eNifVersion::VER_3_3_0_13))
        {
            target = NiRef<NiObjectNET>(reader.read_u32());
        }
        if (version <= version_value(eNifVersion::VER_3_1))
        {
            unknown_int = reader.read_u32();
        }
    }

    NiSingleInterpController::NiSingleInterpController(std::uint32_t version, NifBinaryReader& reader)
        : NiInterpController(version, reader)
    {
        if (version >= version_value(eNifVersion::VER_10_2_0_0))
        {
            interpolator = NiRef<NiInterpolator>(reader.read_u32());
        }
    }

    NiAlphaController::NiAlphaController(std::uint32_t version, NifBinaryReader& reader)
        : NiFloatInterpController(version, reader)
    {
        if (version <= version_value(eNifVersion::VER_10_1_0_0))
        {
            data = NiRef<NiFloatData>(reader.read_u32());
        }
    }

    NiKeyframeController::NiKeyframeController(std::uint32_t version, NifBinaryReader& reader)
        : NiSingleInterpController(version, reader)
    {
        if (version <= version_value(eNifVersion::VER_10_1_0_0))
        {
            data = NiRef<NiKeyframeData>(reader.read_u32());
        }
    }

    NiPoint3InterpController::NiPoint3InterpController(std::uint32_t version, NifBinaryReader& reader)
        : NiSingleInterpController(version, reader)
    {
        if (version >= version_value(eNifVersion::VER_10_1_0_0))
        {
            target_color = static_cast<eTargetColor>(reader.read_u16());
        }
        if (version <= version_value(eNifVersion::VER_10_1_0_0))
        {
            data = NiRef<NiPosData>(reader.read_u32());
        }
    }

    NiUVController::NiUVController(std::uint32_t version, NifBinaryReader& reader)
        : NiTimeController(version, reader),
          unknown_short_1(reader.read_u16()),
          data(reader.read_u32())
    {
    }

    NiVisController::NiVisController(std::uint32_t version, NifBinaryReader& reader)
        : NiBoolInterpController(version, reader)
    {
        if (version <= version_value(eNifVersion::VER_10_1_0_0))
        {
            data = NiRef<NiVisData>(reader.read_u32());
        }
    }

    NiLookAtController::NiLookAtController(std::uint32_t version, NifBinaryReader& reader)
        : NiTimeController(version, reader)
    {
        if (version >= version_value(eNifVersion::VER_10_1_0_0))
        {
            unknown_1 = reader.read_u16();
        }
        camera_target_node = NiRef<NiNode>(reader.read_u32());
    }

    NiTextureTransformController::NiTextureTransformController(
        std::uint32_t version,
        NifBinaryReader& reader)
        : NiFloatInterpController(version, reader),
          unknown_2(reader.read_u8()),
          texture_slot(static_cast<eTexType>(reader.read_u32())),
          operation(static_cast<eTexTransform>(reader.read_u32()))
    {
        if (version <= version_value(eNifVersion::VER_10_1_0_0))
        {
            data = NiRef<NiFloatData>(reader.read_u32());
        }
    }

    NiPathController::NiPathController(std::uint32_t version, NifBinaryReader& reader)
        : NiTimeController(version, reader)
    {
        if (version >= version_value(eNifVersion::VER_10_1_0_0))
        {
            unknown_1 = reader.read_u16();
        }
        unknown_2 = reader.read_u32();
        unknown_3 = reader.read_f32();
        unknown_4 = reader.read_f32();
        unknown_5 = reader.read_u16();
        position_data = NiRef<NiPosData>(reader.read_u32());
        float_data = NiRef<NiFloatData>(reader.read_u32());
    }

    NiInterpolator::NiInterpolator(std::uint32_t version) noexcept
        : NiObject(version)
    {
    }

    NiFloatData::NiFloatData(std::uint32_t version, NifBinaryReader& reader)
        : NiObject(version),
          data(reader)
    {
    }

    NiPosData::NiPosData(std::uint32_t version, NifBinaryReader& reader)
        : NiObject(version),
          data(reader)
    {
    }

    NiColorData::NiColorData(std::uint32_t version, NifBinaryReader& reader)
        : NiObject(version),
          data(reader)
    {
    }

    NiVisData::NiVisData(std::uint32_t version, NifBinaryReader& reader)
        : NiObject(version)
    {
        const std::uint32_t key_count = reader.read_u32();
        if (key_count > 1'000'000)
        {
            throw NifFormatError("NIF visibility key count exceeds the supported limit.");
        }
        keys.reserve(key_count);
        for (std::uint32_t index = 0; index < key_count; ++index)
        {
            keys.emplace_back(reader, eKeyType::LINEAR_KEY);
        }
    }

    NiKeyframeData::NiKeyframeData(std::uint32_t version, NifBinaryReader& reader)
                : NiObject(version)
    {
        const std::uint32_t rotation_count = reader.read_u32();
        if (rotation_count > 1'000'000)
        {
            throw NifFormatError("NIF quaternion key count exceeds the supported limit.");
        }
        if (rotation_count != 0)
        {
            key_type = static_cast<eKeyType>(reader.read_u32());
        }

        if (key_type != eKeyType::XYZ_ROTATION_KEY)
        {
            quaternion_keys.reserve(rotation_count);
            for (std::uint32_t index = 0; index < rotation_count; ++index)
            {
                quaternion_keys.emplace_back(reader, key_type);
            }
        }
        if (version <= version_value(eNifVersion::VER_10_1_0_0) &&
            key_type == eKeyType::XYZ_ROTATION_KEY)
        {
            unknown_float = reader.read_f32();
        }
        if (key_type == eKeyType::XYZ_ROTATION_KEY)
        {
            rotations.reserve(3);
            for (int axis = 0; axis < 3; ++axis)
            {
                rotations.emplace_back(reader);
            }
        }

        translations = KeyGroup<VecKey>(reader);
        scales = KeyGroup<FloatKey>(reader);
    }

    NiUVData::NiUVData(std::uint32_t version, NifBinaryReader& reader)
        : NiObject(version),
          u_translation(reader),
          v_translation(reader),
          u_scaling_and_tiling(reader),
          v_scaling_and_tiling(reader)
    {
    }
}