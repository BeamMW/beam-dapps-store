#pragma once
#include "Shaders/common.h"

namespace DAppsStore
{
    // size of CID is 46 + 1 of '\0'
    using IPFSCID = char[47];
    typedef uint8_t TagType;
    typedef Opaque<16> DAppId;
    typedef uint16_t ShaderVersionType;

    static const ShaderVersionType SHADER_VERSION = 0;

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
            ShaderVersionType m_Version = SHADER_VERSION;
            TagType m_Type = Tags::PUBLISHER;
            PubKey m_PubKey;
        };

        static const uint32_t NAME_MAX_SIZE = 30;
        static const uint32_t SHORT_TITLE_MAX_SIZE = 50;
        static const uint32_t ABOUT_ME_MAX_SIZE = 150;
        static const uint32_t WEBSITE_MAX_SIZE = 100;
        static const uint32_t SOCIAL_NICK_MAX_SIZE = 50;

        uint32_t m_NameSize;
        uint32_t m_ShortTitleSize;
        uint32_t m_AboutMeSize;
        uint32_t m_WebsiteSize;
        uint32_t m_TwitterSize;
        uint32_t m_LinkedinSize;
        uint32_t m_InstagramSize;
        uint32_t m_TelegramSize;
        uint32_t m_DiscordSize;
        // followed by data
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
            ShaderVersionType m_Version = SHADER_VERSION;
            TagType m_Type = Tags::DAPP;
            DAppId m_Id;
        };

        static const uint32_t NAME_MAX_SIZE = 30;
        static const uint32_t DESCRIPTION_MAX_SIZE = 1024;
        static const uint32_t API_VERSION_MAX_SIZE = 10;
        static const uint32_t ICON_MAX_SIZE = 10240; // 10kb

        PubKey m_Publisher;
        IPFSCID m_IPFSId;
        Version m_Version;
        Timestamp m_Timestamp;
        uint32_t m_Category;

        uint32_t m_NameSize;
        uint32_t m_DescriptionSize;
        uint32_t m_ApiVersionSize;
        uint32_t m_MinApiVersionSize;
        uint32_t m_IconSize;
        // followed by data
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

            uint32_t m_NameSize;
            uint32_t m_ShortTitleSize;
            uint32_t m_AboutMeSize;
            uint32_t m_WebsiteSize;
            uint32_t m_TwitterSize;
            uint32_t m_LinkedinSize;
            uint32_t m_InstagramSize;
            uint32_t m_TelegramSize;
            uint32_t m_DiscordSize;
        };

        struct AddPublisher: public PublisherBase
        {
            static const uint32_t METHOD_ID = 3;
            // followed by data
        };

        struct UpdatePublisher: public PublisherBase
        {
            static const uint32_t METHOD_ID = 4;
            // followed by data
        };

        struct Base
        {
            DAppId m_Id;
            IPFSCID m_IPFSId;
            Version m_Version;
            uint32_t m_Category;

            uint32_t m_NameSize;
            uint32_t m_DescriptionSize;
            uint32_t m_ApiVersionSize;
            uint32_t m_MinApiVersionSize;
            uint32_t m_IconSize;
        };

        struct AddDApp: public Base
        {
            static const uint32_t METHOD_ID = 5;

            PubKey m_Publisher;
            // followed by data
        };

        struct UpdateDApp: public Base
        {
            static const uint32_t METHOD_ID = 6;
            // followed by data
        };

        struct DeleteDApp
        {
            static const uint32_t METHOD_ID = 7;

            DAppId m_Id;
        };
    } // namespace Method
#pragma pack (pop)

    uint32_t calcPublisherSize(const Method::PublisherBase& args)
    {
        return sizeof(Publisher) + args.m_NameSize + args.m_ShortTitleSize + args.m_AboutMeSize +
            args.m_WebsiteSize + args.m_TwitterSize + args.m_LinkedinSize + args.m_InstagramSize +
            args.m_TelegramSize + args.m_DiscordSize;
    };

    uint32_t calcDAppSize(const Method::Base& args)
    {
        return sizeof(DApp) + args.m_NameSize + args.m_DescriptionSize +
            args.m_ApiVersionSize + args.m_MinApiVersionSize + args.m_IconSize;
    };
} // namespace DappsStore