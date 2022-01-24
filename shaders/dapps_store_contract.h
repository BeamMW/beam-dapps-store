#pragma once
#include "Shaders/common.h"

namespace DAppsStore
{
    using IPFSCID = char[46];
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

        char m_Label[LABEL_MAX_SIZE + 1];
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

        char m_Label[LABEL_MAX_SIZE + 1];
    };

    namespace Method
    {
        struct Create
        {
            static const uint32_t METHOD_ID = 0;
        };

        struct AddPublisher
        {
            static const uint32_t METHOD_ID = 2;

            PubKey m_Publisher;
            uint32_t m_LabelSize;
            char m_Label[Publisher::LABEL_MAX_SIZE + 1];
        };

        struct UpdatePublisher
        {
            static const uint32_t METHOD_ID = 3;

            PubKey m_Publisher;
            uint32_t m_LabelSize;
            char m_Label[Publisher::LABEL_MAX_SIZE + 1];
        };

        struct DeletePublisher
        {
            static const uint32_t METHOD_ID = 4;

            PubKey m_Publisher;
        };

        struct AddDApp
        {
            static const uint32_t METHOD_ID = 5;

            PubKey m_Publisher;
            IPFSCID m_IPFSId;
            uint32_t m_LabelSize;
            char m_Label[DApp::LABEL_MAX_SIZE + 1];
        };

        struct UpdateDApp
        {
            static const uint32_t METHOD_ID = 6;

            uint64_t m_Id;
            IPFSCID m_IPFSId;
            uint32_t m_LabelSize;
            char m_Label[DApp::LABEL_MAX_SIZE + 1];
        };

        struct DeleteDApp
        {
            static const uint32_t METHOD_ID = 7;

            uint64_t m_Id;
        };
    } // namespace Method
#pragma pack (pop)
} // namespace DappsStore