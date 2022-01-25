#pragma once
#include "Shaders/common.h"

namespace DAppsStore
{
    using IPFSCID = char[46];
    typedef uint8_t TagType;
    typedef Opaque<16> DAppId;

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

        static const uint32_t NAME_MAX_SIZE = 30;

        char m_Name[NAME_MAX_SIZE];
    };

    struct DApp
    {
        struct Key
        {
            TagType m_Type = Tags::DAPP;
            DAppId m_Id;
        };

        static const uint32_t NAME_MAX_SIZE = 30;
        static const uint32_t DESCRIPTION_MAX_SIZE = 1024;
        static const uint32_t API_VERSION_MAX_SIZE = 10;

        PubKey m_Publisher;
        IPFSCID m_IPFSId;

        char m_Name[NAME_MAX_SIZE];
        char m_Description[DESCRIPTION_MAX_SIZE];
        char m_ApiVersion[API_VERSION_MAX_SIZE];
        char m_MinApiVersion[API_VERSION_MAX_SIZE];
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
            char m_Name[Publisher::NAME_MAX_SIZE];
        };

        struct UpdatePublisher
        {
            static const uint32_t METHOD_ID = 3;

            PubKey m_Publisher;
            char m_Name[Publisher::NAME_MAX_SIZE];
        };

        struct DeletePublisher
        {
            static const uint32_t METHOD_ID = 4;

            PubKey m_Publisher;
        };

        struct AddDApp
        {
            static const uint32_t METHOD_ID = 5;

            DAppId m_Id;
            PubKey m_Publisher;
            IPFSCID m_IPFSId;
            char m_Name[DApp::NAME_MAX_SIZE];
            char m_Description[DApp::DESCRIPTION_MAX_SIZE];
            char m_ApiVersion[DApp::API_VERSION_MAX_SIZE];
            char m_MinApiVersion[DApp::API_VERSION_MAX_SIZE];
        };

        struct UpdateDApp
        {
            static const uint32_t METHOD_ID = 6;

            DAppId m_Id;
            IPFSCID m_IPFSId;
            char m_Name[DApp::NAME_MAX_SIZE];
            char m_Description[DApp::DESCRIPTION_MAX_SIZE];
            char m_ApiVersion[DApp::API_VERSION_MAX_SIZE];
            char m_MinApiVersion[DApp::API_VERSION_MAX_SIZE];
        };

        struct DeleteDApp
        {
            static const uint32_t METHOD_ID = 7;

            DAppId m_Id;
        };
    } // namespace Method
#pragma pack (pop)
} // namespace DappsStore