#include "dapps_store_contract.h"

namespace DAppsStore
{
    namespace
    {
        void checkPublisherSize(const Method::PublisherBase& args)
        {
            Env::Halt_if(args.m_NameSize > Publisher::NAME_MAX_SIZE);
            Env::Halt_if(args.m_ShortTitleSize > Publisher::SHORT_TITLE_MAX_SIZE);
            Env::Halt_if(args.m_AboutMeSize > Publisher::ABOUT_ME_MAX_SIZE);
            Env::Halt_if(args.m_WebsiteSize > Publisher::WEBSITE_MAX_SIZE);
            Env::Halt_if(args.m_TwitterSize > Publisher::SOCIAL_NICK_MAX_SIZE);
            Env::Halt_if(args.m_LinkedinSize > Publisher::SOCIAL_NICK_MAX_SIZE);
            Env::Halt_if(args.m_InstagramSize > Publisher::SOCIAL_NICK_MAX_SIZE);
            Env::Halt_if(args.m_TelegramSize > Publisher::SOCIAL_NICK_MAX_SIZE);
            Env::Halt_if(args.m_DiscordSize > Publisher::SOCIAL_NICK_MAX_SIZE);
        }

        void checkDAppSize(const Method::Base& args)
        {
            Env::Halt_if(args.m_NameSize > DApp::NAME_MAX_SIZE);
            Env::Halt_if(args.m_DescriptionSize > DApp::DESCRIPTION_MAX_SIZE);
            Env::Halt_if(args.m_ApiVersionSize > DApp::API_VERSION_MAX_SIZE);
            Env::Halt_if(args.m_MinApiVersionSize > DApp::API_VERSION_MAX_SIZE);
            Env::Halt_if(args.m_IconSize > DApp::ICON_MAX_SIZE);
        }

        void processPublisher(const Method::PublisherBase& args, bool update)
        {
            checkPublisherSize(args);

            Env::AddSig(args.m_Publisher);

            Publisher::Key key;
            _POD_(key.m_PubKey) = args.m_Publisher;

            if (update)
            {
                // if the publisher is missing then abort
                Env::Halt_if(Env::LoadVar(&key, sizeof(key), nullptr, 0, KeyTag::Internal) < sizeof(Publisher));
            }
            else
            {
                // if the publisher exists then abort
                Env::Halt_if(Env::LoadVar(&key, sizeof(key), nullptr, 0, KeyTag::Internal) > 0);
            }
            const uint32_t publisherSize = calcPublisherSize(args);
            auto* publisher = static_cast<Publisher*>(Env::Heap_Alloc(publisherSize));

            publisher->m_NameSize = args.m_NameSize;
            publisher->m_ShortTitleSize = args.m_ShortTitleSize;
            publisher->m_AboutMeSize = args.m_AboutMeSize;
            publisher->m_WebsiteSize = args.m_WebsiteSize;
            publisher->m_TwitterSize = args.m_TwitterSize;
            publisher->m_LinkedinSize = args.m_LinkedinSize;
            publisher->m_InstagramSize = args.m_InstagramSize;
            publisher->m_TelegramSize = args.m_TelegramSize;
            publisher->m_DiscordSize = args.m_DiscordSize;

            Env::Memcpy(static_cast<void*>(publisher + 1), static_cast<const void*>(&args + 1), publisherSize - sizeof(Publisher));

            Env::SaveVar(&key, sizeof(key), publisher, publisherSize, KeyTag::Internal);
            Env::Heap_Free(publisher);
        }

        void processDApp(const Method::Base& args, const PubKey& pubkey)
        {
            checkDAppSize(args);

            const uint32_t dappSize = calcDAppSize(args);
            auto* dapp = static_cast<DApp*>(Env::Heap_Alloc(dappSize));
            DApp::Key key;
            _POD_(key.m_Id) = args.m_Id;

            _POD_(dapp->m_Publisher) = pubkey;
            _POD_(dapp->m_Version) = args.m_Version;
            dapp->m_NameSize = args.m_NameSize;
            dapp->m_DescriptionSize = args.m_DescriptionSize;
            dapp->m_ApiVersionSize = args.m_ApiVersionSize;
            dapp->m_MinApiVersionSize = args.m_MinApiVersionSize;
            dapp->m_IconSize = args.m_IconSize;
            Env::Memcpy(dapp->m_IPFSId, args.m_IPFSId, sizeof(dapp->m_IPFSId));
            dapp->m_Category = args.m_Category;

            BlockHeader::Info hdr;
            hdr.m_Height = Env::get_Height();
            Env::get_HdrInfo(hdr);

            dapp->m_Timestamp = hdr.m_Timestamp;

            Env::Memcpy(static_cast<void*>(dapp + 1), static_cast<const void*>(&args + 1), dappSize - sizeof(DApp));

            Env::SaveVar(&key, sizeof(key), dapp, dappSize, KeyTag::Internal);
            Env::Heap_Free(dapp);
        }

        bool checkVersion(const Version& version1, const Version& version2)
        {
            if (version1.m_Major < version2.m_Major)
                return true;

            if (version1.m_Major == version2.m_Major)
            {
                if (version1.m_Minor < version2.m_Minor)
                    return true;

                if (version1.m_Minor == version2.m_Minor)
                {
                    if (version1.m_Release < version2.m_Release)
                        return true;

                    if (version1.m_Release == version2.m_Release)
                    {
                        if (version1.m_Build < version2.m_Build)
                            return true;
                    }
                }
            }

            return false;
        }
    } // namespace

    BEAM_EXPORT void Ctor(const Method::Create& /*args*/)
    {
    }

    BEAM_EXPORT void Dtor(void*)
    {
    }

    BEAM_EXPORT void Method_2(void*)
    {
        // for upgrade
    }

    BEAM_EXPORT void Method_3(const Method::AddPublisher& args)
    {
        processPublisher(args, false);
    }

    BEAM_EXPORT void Method_4(const Method::UpdatePublisher& args)
    {
        processPublisher(args, true);
    }

    BEAM_EXPORT void Method_5(const Method::AddDApp& args)
    {
        Env::AddSig(args.m_Publisher);

        Publisher::Key publisherKey;
        _POD_(publisherKey.m_PubKey) = args.m_Publisher;
        // if the publisher is missing then abort
        Env::Halt_if(Env::LoadVar(&publisherKey, sizeof(publisherKey), nullptr, 0, KeyTag::Internal) < sizeof(Publisher));

        processDApp(args, args.m_Publisher);
    }

    BEAM_EXPORT void Method_6(const Method::UpdateDApp& args)
    {
        DApp::Key key;
        _POD_(key.m_Id) = args.m_Id;

        DApp oldVersionOfDapp;
        // if the dapp is missing then abort
        Env::Halt_if(Env::LoadVar(&key, sizeof(key), &oldVersionOfDapp, sizeof(oldVersionOfDapp), KeyTag::Internal) < sizeof(DApp));
        Env::AddSig(oldVersionOfDapp.m_Publisher);

        // check version
        Env::Halt_if(!checkVersion(oldVersionOfDapp.m_Version, args.m_Version));

        processDApp(args, oldVersionOfDapp.m_Publisher);
    }

    BEAM_EXPORT void Method_7(const Method::DeleteDApp& args)
    {
        DApp::Key key;
        _POD_(key.m_Id) = args.m_Id;

        DApp dapp;
        // if the dapp is missing then abort
        Env::Halt_if(Env::LoadVar(&key, sizeof(key), &dapp, sizeof(dapp), KeyTag::Internal) < sizeof(DApp));
        Env::AddSig(dapp.m_Publisher);

        Env::DelVar_T(key);
    }
} // namespace DAppsStore