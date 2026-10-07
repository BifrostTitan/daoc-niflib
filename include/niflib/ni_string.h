// C++ port of niflib.net, added for the 2026 migration; licensed under GPL-2.0-or-later.
#pragma once

#include "niflib/nif_binary_reader.h"

#include <string>

namespace niflib
{
    struct NiString
    {
        std::string value;

        explicit NiString(NifBinaryReader& reader);
    };
}