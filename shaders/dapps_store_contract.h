#pragma once
#include "Shaders/common.h"

namespace DappsStore
{
#pragma pack (push, 1) // the following structures will be stored in the node in binary form
    struct Create
    {
        static const uint32_t s_iMethod = 0;

        uint32_t m_MetadataSize;
    };
} // namespace DappsStore