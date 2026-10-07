// C++ port of niflib.net, added for the 2026 migration; licensed under GPL-2.0-or-later.
#pragma once

#include "niflib/nif_binary_reader.h"
#include "niflib/ni_object.h"

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace niflib
{
    class NifObjectFactory
    {
    public:
        using Reader = std::function<std::shared_ptr<NiObject>(
            std::uint32_t,
            std::uint32_t,
            NifBinaryReader&)>;

        void register_type(std::string type_name, Reader reader);
        std::shared_ptr<NiObject> create(
            const std::string& type_name,
            std::uint32_t version,
            std::uint32_t user_version,
            NifBinaryReader& reader) const;

        static NifObjectFactory with_builtin_types();

    private:
        std::unordered_map<std::string, Reader> readers_;
    };
}