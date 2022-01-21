#pragma once
#include "Shaders/common.h"

namespace DAppsStore
{
    typedef Opaque<34> IPFSCID;
    typedef uint8_t TagType;

    namespace Tags
    {
        static const TagType STATE = 1;
        static const TagType PUBLISHER = 2;
        static const TagType DAPP = 3;
    }; // namespace Tags

#pragma pack (push, 1) // the following structures will be stored in the node in binary form
    struct Publisher
    {
        struct Key
        {
            TagType m_Type = Tags::PUBLISHER;
            PubKey m_PubKey;
        };

        static const uint32_t LABEL_MAX_SIZE = 100;

        char m_Label[LABEL_MAX_SIZE];
    };

    struct DApp
    {
        struct Key
        {
            TagType m_Type = Tags::DAPP;
            // big-endian, for simpler enumeration by app shader
            uint64_t m_IdInBE;
        };

        static const uint32_t LABEL_MAX_SIZE = 100;

        PubKey m_Publisher;
        IPFSCID m_IPFSId;

        char m_Label[LABEL_MAX_SIZE];
    };

    struct Create
    {
        static const uint32_t s_iMethod = 0;
    };

    struct AddPublisher
    {
        static const uint32_t s_iMethod = 2;

        PubKey m_Publisher;
        uint32_t m_LabelSize;
        char m_Label[Publisher::LABEL_MAX_SIZE];
    };

    struct UpdatePublisher
    {
        static const uint32_t s_iMethod = 3;

        PubKey m_Publisher;
    };

    struct DeletePublisher
    {
        static const uint32_t s_iMethod = 4;

        PubKey m_Publisher;
    };

    struct AddDApp
    {
        static const uint32_t s_iMethod = 5;

        PubKey m_Publisher;
        IPFSCID m_IPFSId;
        uint32_t m_LabelSize;
        char m_Label[DApp::LABEL_MAX_SIZE];
    };

    struct UpdateDApp
    {
        static const uint32_t s_iMethod = 6;
    };

    struct DeleteDApp
    {
        static const uint32_t s_iMethod = 7;
    };

#pragma pack (pop)
} // namespace DappsStore