#pragma once
#include "Shaders/common.h"

namespace DAppsStore
{
    static const uint8_t APPS_AMOUNT_KEY = 5;
#pragma pack (push, 1) // the following structures will be stored in the node in binary form
    struct Create
    {
        static const uint32_t s_iMethod = 0;
    };

    struct AddDApp
    {
        static const uint32_t s_iMethod = 2;

        PubKey m_Publisher;
        HashValue m_IPFSId;
    };

    struct UpdateDApp
    {
        static const uint32_t s_iMethod = 3;
    };

    struct DeleteDApp
    {
        static const uint32_t s_iMethod = 4;
    };

    struct DApp
    {
        struct Key
        {
            uint8_t m_Type = 0;
            // big-endian, for simpler enumeration by app shader
            uint64_t m_MsgId_BE;
        };
        PubKey m_Publisher;
        HashValue m_IPFSId;
    };
} // namespace DappsStore