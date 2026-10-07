// C++ port of niflib.net, added for the 2026 migration; licensed under GPL-2.0-or-later.
#pragma once

#include "niflib/nif_types.h"

#include <cstddef>
#include <cstdint>
#include <istream>
#include <stdexcept>
#include <string>

namespace niflib
{
    class NifFormatError : public std::runtime_error
    {
    public:
        using std::runtime_error::runtime_error;
    };

    class NifBinaryReader
    {
    public:
        explicit NifBinaryReader(std::istream& input) noexcept;

        std::uint8_t read_u8();
        std::uint16_t read_u16();
        std::uint32_t read_u32();
        std::int32_t read_i32();
        float read_f32();
        bool read_bool(eNifVersion version);
        Vec2 read_vec2();
        Vec3 read_vec3();
        Vec4 read_vec4();
        Color4 read_color4();
        Color4 read_color4_byte();
        std::array<float, 9> read_matrix33();
        std::string read_string(std::size_t length);
        std::string read_line(std::size_t maximum_length);
        void read_exact(char* destination, std::size_t size);

    private:
        std::istream& input_;
    };
}