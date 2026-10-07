// C++ port of niflib.net, added for the 2026 migration; licensed under GPL-2.0-or-later.
#pragma once

#include "niflib/nif_binary_reader.h"
#include "niflib/ni_object.h"
#include "niflib/ni_ref.h"
#include "niflib/nif_types.h"

#include <array>
#include <cstdint>
#include <optional>
#include <type_traits>
#include <vector>

namespace niflib
{
    class NiPalette : public NiObject
    {
    public:
        std::uint8_t unknown_byte = 0;
        std::vector<Color4> palette;

        NiPalette(std::uint32_t version, NifBinaryReader& reader);
    };

    struct ChannelData
    {
        eChannelType type = eChannelType::CHNL_RED;
        eChannelConvention convention = eChannelConvention::CC_FIXED;
        std::uint8_t bits_per_channel = 0;
        std::uint8_t unknown_byte = 0;

        ChannelData() = default;
        explicit ChannelData(NifBinaryReader& reader);
    };

    struct MipMap
    {
        std::uint32_t width = 0;
        std::uint32_t height = 0;
        std::uint32_t offset = 0;

        MipMap() = default;
        explicit MipMap(NifBinaryReader& reader);
    };

    struct ATextureRenderData : NiObject
    {
        ePixelFormat pixel_format = ePixelFormat::PX_FMT_RGB8;
        std::uint32_t red_mask = 0;
        std::uint32_t green_mask = 0;
        std::uint32_t blue_mask = 0;
        std::uint32_t alpha_mask = 0;
        std::uint8_t bits_per_pixel = 0;
        std::array<std::uint8_t, 3> unknown_3_bytes{};
        std::array<std::uint8_t, 8> unknown_8_bytes{};
        std::uint32_t unknown_int = 0;
        std::uint32_t unknown_int_2 = 0;
        std::uint32_t unknown_int_3 = 0;
        std::uint32_t unknown_int_4 = 0;
        std::uint8_t flags = 0;
        std::uint8_t unknown_byte_1 = 0;
        std::array<ChannelData, 4> channel_data{};
        NiRef<NiPalette> palette;
        std::uint32_t num_mip_maps = 0;
        std::uint32_t bytes_per_pixel = 0;
        std::vector<MipMap> mip_maps;

        ATextureRenderData(std::uint32_t version, NifBinaryReader& reader);
    };

    class NiPixelData : public ATextureRenderData
    {
    public:
        std::uint32_t num_pixels = 0;
        std::uint32_t num_faces = 0;
        std::vector<std::vector<std::uint8_t>> pixel_data;

        NiPixelData(std::uint32_t version, NifBinaryReader& reader);
    };

    class BaseKey
    {
    public:
        BaseKey(NifBinaryReader&, eKeyType) noexcept
        {
        }
    };

    struct ByteKey
    {
        float time = 0.0f;
        std::uint8_t value = 0;
        Vec3 tbc{};

        ByteKey(NifBinaryReader& reader, eKeyType type);
    };

    struct Color4Key : BaseKey
    {
        float time = 0.0f;
        Color4 value{};
        Color4 forward{};
        Color4 backward{};

        Color4Key(NifBinaryReader& reader, eKeyType type);
    };

    struct FloatKey : BaseKey
    {
        float time = 0.0f;
        float value = 0.0f;
        float forward = 0.0f;
        float backward = 0.0f;
        Vec3 tbc{};

        FloatKey(NifBinaryReader& reader, eKeyType type);
    };

    struct VecKey : BaseKey
    {
        float time = 0.0f;
        Vec3 value{};
        Vec3 forward{};
        Vec3 backward{};
        Vec3 tbc{};

        VecKey(NifBinaryReader& reader, eKeyType type);
    };

    struct QuatKey
    {
        float time = 0.0f;
        Vec4 value{};
        Vec3 tbc{};

        QuatKey(NifBinaryReader& reader, eKeyType type);
    };

    template <typename T>
    class KeyGroup
    {
        static_assert(std::is_base_of<BaseKey, T>::value, "KeyGroup values must derive from BaseKey");

    public:
        eKeyType interpolation = static_cast<eKeyType>(0);
        std::vector<T> values;

        KeyGroup() = default;

        explicit KeyGroup(NifBinaryReader& reader)
        {
            const std::uint32_t count = reader.read_u32();
            if (count > maximum_key_count)
            {
                throw NifFormatError("NIF key count exceeds the supported limit.");
            }
            if (count == 0)
            {
                return;
            }

            interpolation = static_cast<eKeyType>(reader.read_u32());
            values.reserve(count);
            for (std::uint32_t index = 0; index < count; ++index)
            {
                values.emplace_back(reader, interpolation);
            }
        }

    private:
        static constexpr std::uint32_t maximum_key_count = 1'000'000;
    };

    struct LODRange
    {
        float near_extent = 0.0f;
        float far_extent = 0.0f;
        std::optional<std::array<std::uint32_t, 3>> unknown_ints;

        LODRange(std::uint32_t version, NifBinaryReader& reader);
    };
}