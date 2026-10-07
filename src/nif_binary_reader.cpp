// C++ port of niflib.net, added for the 2026 migration; licensed under GPL-2.0-or-later.
#include "niflib/nif_binary_reader.h"

#include <cstring>
#include <limits>

namespace niflib
{
    namespace
    {
        constexpr std::size_t maximum_string_length = 64 * 1024 * 1024;
    }

    NifBinaryReader::NifBinaryReader(std::istream& input) noexcept
        : input_(input)
    {
    }

    std::uint8_t NifBinaryReader::read_u8()
    {
        char value = 0;
        read_exact(&value, 1);
        return static_cast<std::uint8_t>(static_cast<unsigned char>(value));
    }

    std::uint16_t NifBinaryReader::read_u16()
    {
        const std::uint16_t low = read_u8();
        const std::uint16_t high = read_u8();
        return static_cast<std::uint16_t>(low | (high << 8));
    }

    std::uint32_t NifBinaryReader::read_u32()
    {
        const std::uint32_t byte_0 = read_u8();
        const std::uint32_t byte_1 = read_u8();
        const std::uint32_t byte_2 = read_u8();
        const std::uint32_t byte_3 = read_u8();
        return byte_0 | (byte_1 << 8) | (byte_2 << 16) | (byte_3 << 24);
    }

    std::int32_t NifBinaryReader::read_i32()
    {
        const std::uint32_t bits = read_u32();
        std::int32_t value = 0;
        static_assert(sizeof(value) == sizeof(bits));
        std::memcpy(&value, &bits, sizeof(value));
        return value;
    }

    float NifBinaryReader::read_f32()
    {
        const std::uint32_t bits = read_u32();
        float value = 0.0f;
        static_assert(sizeof(value) == sizeof(bits));
        std::memcpy(&value, &bits, sizeof(value));
        return value;
    }

    bool NifBinaryReader::read_bool(eNifVersion version)
    {
        if (version_value(version) < version_value(eNifVersion::VER_4_1_0_1))
        {
            return read_u32() != 0;
        }
        return read_u8() != 0;
    }

    Vec2 NifBinaryReader::read_vec2()
    {
        return {read_f32(), read_f32()};
    }

    Vec3 NifBinaryReader::read_vec3()
    {
        return {read_f32(), read_f32(), read_f32()};
    }

    Vec4 NifBinaryReader::read_vec4()
    {
        return {read_f32(), read_f32(), read_f32(), read_f32()};
    }

    Color4 NifBinaryReader::read_color4()
    {
        return {read_f32(), read_f32(), read_f32(), read_f32()};
    }

    Color4 NifBinaryReader::read_color4_byte()
    {
        constexpr float byte_to_unit = 1.0f / 255.0f;
        return {
            static_cast<float>(read_u8()) * byte_to_unit,
            static_cast<float>(read_u8()) * byte_to_unit,
            static_cast<float>(read_u8()) * byte_to_unit,
            static_cast<float>(read_u8()) * byte_to_unit};
    }

    std::array<float, 9> NifBinaryReader::read_matrix33()
    {
        std::array<float, 9> values{};
        for (std::size_t row = 0; row < 3; ++row)
        {
            for (std::size_t column = 0; column < 3; ++column)
            {
                values[column * 3 + row] = read_f32();
            }
        }
        return values;
    }

    std::string NifBinaryReader::read_string(std::size_t length)
    {
        if (length > maximum_string_length)
        {
            throw NifFormatError("NIF string exceeds the supported length.");
        }

        std::string value(length, '\0');
        if (length != 0)
        {
            read_exact(value.data(), length);
        }
        return value;
    }

    std::string NifBinaryReader::read_line(std::size_t maximum_length)
    {
        std::string value;
        value.reserve(maximum_length);
        while (value.size() < maximum_length)
        {
            const char next = static_cast<char>(read_u8());
            if (next == '\n')
            {
                return value;
            }
            value.push_back(next);
        }

        throw NifFormatError("NIF line exceeds the supported length.");
    }

    void NifBinaryReader::read_exact(char* destination, std::size_t size)
    {
        if (size > static_cast<std::size_t>(std::numeric_limits<std::streamsize>::max()))
        {
            throw NifFormatError("Requested NIF read exceeds the stream limit.");
        }

        const std::streampos start_position = input_.tellg();
        input_.read(destination, static_cast<std::streamsize>(size));
        if (input_.gcount() != static_cast<std::streamsize>(size))
        {
            const auto offset = static_cast<std::streamoff>(start_position);
            throw NifFormatError(
                "Unexpected end of NIF data at byte offset " + std::to_string(offset) +
                " (wanted " + std::to_string(size) + " bytes, got " +
                std::to_string(input_.gcount()) + ").");
        }
    }
}