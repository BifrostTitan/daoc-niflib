// C++ port of niflib.net, added for the 2026 migration; licensed under GPL-2.0-or-later.
#pragma once

#include "niflib/ni_object.h"
#include "niflib/ni_ref.h"
#include "niflib/nif_header.h"
#include "niflib/nif_object_factory.h"

#include <cstdint>
#include <istream>
#include <memory>
#include <unordered_map>
#include <vector>

namespace niflib
{
    struct NiFooter
    {
        std::vector<NiRef<NiObject>> root_nodes;
    };

    class NiFile
    {
    public:
        explicit NiFile(std::istream& input);
        NiFile(std::istream& input, const NifObjectFactory& factory);

        const NifHeader& header() const noexcept;
        const NiFooter& footer() const noexcept;
        const std::unordered_map<std::uint32_t, std::shared_ptr<NiObject>>& objects() const noexcept;
        const std::vector<NiRef<NiObject>>& root_nodes() const noexcept;
        std::shared_ptr<NiObject> object(std::uint32_t ref_id) const;

    private:
        void read_objects(NifBinaryReader& reader, const NifObjectFactory& factory);
        void read_footer(NifBinaryReader& reader);
        void resolve_object_refs();

        NifHeader header_;
        NiFooter footer_;
        std::unordered_map<std::uint32_t, std::shared_ptr<NiObject>> objects_;
    };
}