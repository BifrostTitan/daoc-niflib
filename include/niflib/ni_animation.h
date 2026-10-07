// C++ port of niflib.net, added for the 2026 migration; licensed under GPL-2.0-or-later.
#pragma once

#include "niflib/ni_object.h"
#include "niflib/ni_ref.h"
#include "niflib/nif_binary_reader.h"
#include "niflib/nif_records.h"
#include "niflib/ni_scene.h"

#include <cstdint>

namespace niflib
{
    class NiFloatData;
    class NiInterpolator;
    class NiKeyframeData;
    class NiPosData;
    class NiUVData;
    class NiVisData;

    class NiTimeController : public NiObject
    {
    public:
        NiRef<NiTimeController> next_controller;
        std::uint16_t flags = 0;
        float frequency = 0.0f;
        float phase = 0.0f;
        float start_time = 0.0f;
        float stop_time = 0.0f;
        NiRef<NiObjectNET> target;
        std::uint32_t unknown_int = 0;

        NiTimeController(std::uint32_t version, NifBinaryReader& reader);
    };

    class NiInterpController : public NiTimeController
    {
    public:
        using NiTimeController::NiTimeController;
    };

    class NiSingleInterpController : public NiInterpController
    {
    public:
        NiRef<NiInterpolator> interpolator;

        NiSingleInterpController(std::uint32_t version, NifBinaryReader& reader);
    };

    class NiFloatInterpController : public NiSingleInterpController
    {
    public:
        using NiSingleInterpController::NiSingleInterpController;
    };

    class NiAlphaController : public NiFloatInterpController
    {
    public:
        NiRef<NiFloatData> data;

        NiAlphaController(std::uint32_t version, NifBinaryReader& reader);
    };

    class NiKeyframeController : public NiSingleInterpController
    {
    public:
        NiRef<NiKeyframeData> data;
        NiKeyframeController(std::uint32_t version, NifBinaryReader& reader);
    };

    class NiBoolInterpController : public NiSingleInterpController
    {
    public:
        using NiSingleInterpController::NiSingleInterpController;
    };

    class NiPoint3InterpController : public NiSingleInterpController
    {
    public:
        eTargetColor target_color = eTargetColor::TC_AMBIENT;
        NiRef<NiPosData> data;

        NiPoint3InterpController(std::uint32_t version, NifBinaryReader& reader);
    };

    class NiMaterialColorController : public NiPoint3InterpController
    {
    public:
        using NiPoint3InterpController::NiPoint3InterpController;
    };

    class NiLightColorController : public NiPoint3InterpController
    {
    public:
        using NiPoint3InterpController::NiPoint3InterpController;
    };

    class NiUVController : public NiTimeController
    {
    public:
        std::uint16_t unknown_short_1 = 0;
        NiRef<NiUVData> data;

        NiUVController(std::uint32_t version, NifBinaryReader& reader);
    };

    class NiVisController : public NiBoolInterpController
    {
    public:
        NiRef<NiVisData> data;
        NiVisController(std::uint32_t version, NifBinaryReader& reader);
    };

    class NiLookAtController : public NiTimeController
    {
    public:
        std::uint16_t unknown_1 = 0;
        NiRef<NiNode> camera_target_node;
        NiLookAtController(std::uint32_t version, NifBinaryReader& reader);
    };

    class NiTextureTransformController : public NiFloatInterpController
    {
    public:
        std::uint8_t unknown_2 = 0;
        eTexType texture_slot = eTexType::BASE_MAP;
        eTexTransform operation = eTexTransform::TT_TRANSLATE_U;
        NiRef<NiFloatData> data;

        NiTextureTransformController(std::uint32_t version, NifBinaryReader& reader);
    };

    class NiPathController : public NiTimeController
    {
    public:
        std::uint16_t unknown_1 = 0;
        std::uint32_t unknown_2 = 0;
        float unknown_3 = 0.0f;
        float unknown_4 = 0.0f;
        std::uint16_t unknown_5 = 0;
        NiRef<NiPosData> position_data;
        NiRef<NiFloatData> float_data;

        NiPathController(std::uint32_t version, NifBinaryReader& reader);
    };

    class NiInterpolator : public NiObject
    {
    public:
        explicit NiInterpolator(std::uint32_t version) noexcept;
    };

    class NiFloatData : public NiObject
    {
    public:
        KeyGroup<FloatKey> data;

        NiFloatData(std::uint32_t version, NifBinaryReader& reader);
    };

    class NiPosData : public NiObject
    {
    public:
        KeyGroup<VecKey> data;
        NiPosData(std::uint32_t version, NifBinaryReader& reader);
    };

    class NiColorData : public NiObject
    {
    public:
        KeyGroup<Color4Key> data;
        NiColorData(std::uint32_t version, NifBinaryReader& reader);
    };

    class NiVisData : public NiObject
    {
    public:
        std::vector<ByteKey> keys;
        NiVisData(std::uint32_t version, NifBinaryReader& reader);
    };

    class NiKeyframeData : public NiObject
    {
    public:
        eKeyType key_type = static_cast<eKeyType>(0);
        std::vector<QuatKey> quaternion_keys;
        float unknown_float = 0.0f;
        std::vector<KeyGroup<FloatKey>> rotations;
        KeyGroup<VecKey> translations;
        KeyGroup<FloatKey> scales;

        NiKeyframeData(std::uint32_t version, NifBinaryReader& reader);
    };

    class NiUVData : public NiObject
    {
    public:
        KeyGroup<FloatKey> u_translation;
        KeyGroup<FloatKey> v_translation;
        KeyGroup<FloatKey> u_scaling_and_tiling;
        KeyGroup<FloatKey> v_scaling_and_tiling;

        NiUVData(std::uint32_t version, NifBinaryReader& reader);
    };
}