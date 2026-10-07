// C++ port of niflib.net, added for the 2026 migration; licensed under GPL-2.0-or-later.
#pragma once

#include "niflib/nif_binary_reader.h"

#include <cstdint>
#include <memory>

namespace niflib
{
    template <typename T>
    class NiRef
    {
    public:
        static constexpr std::uint32_t invalid_ref = 0xFFFFFFFF;

        NiRef() noexcept = default;

        explicit NiRef(std::uint32_t ref_id) noexcept
            : ref_id_(ref_id)
        {
        }

        explicit NiRef(NifBinaryReader& reader)
            : ref_id_(reader.read_u32())
        {
        }

        std::uint32_t ref_id() const noexcept
        {
            return ref_id_;
        }

        const std::shared_ptr<T>& object() const noexcept
        {
            return object_;
        }

        void set_object(std::shared_ptr<T> object) noexcept
        {
            object_ = std::move(object);
        }

        bool is_valid() const noexcept
        {
            return ref_id_ != invalid_ref;
        }

    private:
        std::uint32_t ref_id_ = invalid_ref;
        std::shared_ptr<T> object_;
    };
}