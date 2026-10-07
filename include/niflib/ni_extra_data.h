// C++ port of niflib.net, added for the 2026 migration; licensed under GPL-2.0-or-later.
#pragma once

#include "niflib/ni_object.h"
#include "niflib/ni_ref.h"
#include "niflib/nif_binary_reader.h"
#include "niflib/nif_types.h"

#include <cstdint>
#include <string>
#include <vector>

namespace niflib
{
    class NiExtraData : public NiObject
    {
    public:
        std::string name;
        NiRef<NiExtraData> next_extra_data;

        NiExtraData(std::uint32_t version, NifBinaryReader& reader);
    };

    class NiStringExtraData : public NiExtraData
    {
    public:
        std::uint32_t bytes_remaining = 0;
        std::string string_data;

        NiStringExtraData(std::uint32_t version, NifBinaryReader& reader);
    };

    class NiBinaryExtraData : public NiExtraData
    {
    public:
        std::vector<std::uint8_t> data;
        NiBinaryExtraData(std::uint32_t version, NifBinaryReader& reader);
    };

    class NiIntegerExtraData : public NiExtraData
    {
    public:
        std::uint32_t data = 0;

        NiIntegerExtraData(std::uint32_t version, NifBinaryReader& reader);
    };

    class NiBooleanExtraData : public NiExtraData
    {
    public:
        bool data = false;

        NiBooleanExtraData(std::uint32_t version, NifBinaryReader& reader);
    };

    class NiFloatExtraData : public NiExtraData
    {
    public:
        float data = 0.0f;

        NiFloatExtraData(std::uint32_t version, NifBinaryReader& reader);
    };

    class NiColorExtraData : public NiExtraData
    {
    public:
        Color4 data{};

        NiColorExtraData(std::uint32_t version, NifBinaryReader& reader);
    };

    class NiVectorExtraData : public NiExtraData
    {
    public:
        Vec3 data{};
        float unknown_float = 0.0f;

        NiVectorExtraData(std::uint32_t version, NifBinaryReader& reader);
    };

    class NiIntegersExtraData : public NiExtraData
    {
    public:
        std::vector<std::uint32_t> extra_int_data;
        NiIntegersExtraData(std::uint32_t version, NifBinaryReader& reader);
    };

    class NiStringsExtraData : public NiExtraData
    {
    public:
        std::vector<std::string> extra_string_data;
        NiStringsExtraData(std::uint32_t version, NifBinaryReader& reader);
    };

    struct StringKey
    {
        float time = 0.0f;
        std::string value;

        explicit StringKey(NifBinaryReader& reader);
    };

    class NiTextKeyExtraData : public NiExtraData
    {
    public:
        std::uint32_t unknown_int_1 = 0;
        std::vector<StringKey> text_keys;

        NiTextKeyExtraData(std::uint32_t version, NifBinaryReader& reader);
    };
}