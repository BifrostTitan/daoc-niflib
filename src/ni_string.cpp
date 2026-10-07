// C++ port of niflib.net, added for the 2026 migration; licensed under GPL-2.0-or-later.
#include "niflib/ni_string.h"

namespace niflib
{
    NiString::NiString(NifBinaryReader& reader)
    {
        const std::uint32_t length = reader.read_u32();
        if (length > 16384)
        {
            throw NifFormatError("NIF string exceeds the supported length.");
        }
        value = reader.read_string(length);
    }
}