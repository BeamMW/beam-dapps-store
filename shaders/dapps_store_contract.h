#pragma once
#include "Shaders/common.h"

namespace DAppsStore
{
    // size of CID is 46 + 1 of '\0'
    using IPFSCID = char[47];
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
        static const uint32_t SHORT_TITLE_MAX_SIZE = 50;
        static const uint32_t ABOUT_ME_MAX_SIZE = 150;
        static const uint32_t WEBSITE_MAX_SIZE = 100;
        static const uint32_t SOCIAL_NICK_MAX_SIZE = 50;

        char m_Name[NAME_MAX_SIZE];
        char m_ShortTitle[SHORT_TITLE_MAX_SIZE];
        char m_AboutMe[ABOUT_ME_MAX_SIZE];
        char m_Website[WEBSITE_MAX_SIZE];
        char m_Twitter[SOCIAL_NICK_MAX_SIZE];
        char m_Linkedin[SOCIAL_NICK_MAX_SIZE];
        char m_Instagram[SOCIAL_NICK_MAX_SIZE];
        char m_Telegram[SOCIAL_NICK_MAX_SIZE];
        char m_Discord[SOCIAL_NICK_MAX_SIZE];
    };

    struct Version
    {
        uint32_t m_Major;
        uint32_t m_Minor;
        uint32_t m_Release;
        uint32_t m_Build;
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
        Version m_Version;
        Timestamp m_Timestamp;
        uint32_t m_Category;
        // followed by icon
    };

    namespace Method
    {
        struct Create
        {
            static const uint32_t METHOD_ID = 0;
        };

        struct PublisherBase
        {
            PubKey m_Publisher;

            char m_Name[Publisher::NAME_MAX_SIZE];
            char m_ShortTitle[Publisher::SHORT_TITLE_MAX_SIZE];
            char m_AboutMe[Publisher::ABOUT_ME_MAX_SIZE];
            char m_Website[Publisher::WEBSITE_MAX_SIZE];
            char m_Twitter[Publisher::SOCIAL_NICK_MAX_SIZE];
            char m_Linkedin[Publisher::SOCIAL_NICK_MAX_SIZE];
            char m_Instagram[Publisher::SOCIAL_NICK_MAX_SIZE];
            char m_Telegram[Publisher::SOCIAL_NICK_MAX_SIZE];
            char m_Discord[Publisher::SOCIAL_NICK_MAX_SIZE];
        };

        struct AddPublisher: public PublisherBase
        {
            static const uint32_t METHOD_ID = 3;
        };

        struct UpdatePublisher: public PublisherBase
        {
            static const uint32_t METHOD_ID = 4;
        };

        struct Base
        {
            DAppId m_Id;
            IPFSCID m_IPFSId;
            char m_Name[DApp::NAME_MAX_SIZE];
            char m_Description[DApp::DESCRIPTION_MAX_SIZE];
            char m_ApiVersion[DApp::API_VERSION_MAX_SIZE];
            char m_MinApiVersion[DApp::API_VERSION_MAX_SIZE];
            Version m_Version;
            uint32_t m_Category;
            uint32_t m_IconSize;
        };

        struct AddDApp: public Base
        {
            static const uint32_t METHOD_ID = 5;

            PubKey m_Publisher;
            // followed by icon
        };

        struct UpdateDApp: public Base
        {
            static const uint32_t METHOD_ID = 6;
            // followed by icon
        };

        struct DeleteDApp
        {
            static const uint32_t METHOD_ID = 7;

            DAppId m_Id;
        };
    } // namespace Method
#pragma pack (pop)
} // namespace DappsStore