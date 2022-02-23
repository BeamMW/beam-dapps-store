#include "dapps_store_contract.h"
#include "Shaders/app_common_impl.h"

namespace DAppsStore
{
#include "dapps_store_contract_sid.i"
} // namespace DAppsStore

namespace
{
    const char* CONTRACT_ID = "cid";
    const char* PUBLISHER = "publisher";
    const char* PUBLISHERS = "publishers";
    const char* PUBKEY = "pubkey";
    const char* IPFS_ID = "ipfs_id";
    const char* NAME = "name";
    const char* DESCRIPTION = "description";
    const char* API_VERSION = "api_ver";
    const char* MIN_API_VERSION = "min_api_ver";
    const char* DAPP_ID = "id";
    const char* VERSION = "version";
    const char* MAJOR = "major";
    const char* MINOR = "minor";
    const char* RELEASE = "release";
    const char* BUILD = "build";
    const char* SHORT_TITLE = "short_title";
    const char* ABOUT_ME = "about_me";
    const char* WEBSITE = "website";
    const char* TWITTER = "twitter";
    const char* LINKEDIN = "linkedin";
    const char* INSTAGRAM = "instagram";
    const char* TELEGRAM = "telegram";
    const char* DISCORD = "discord";
    const char* TIMESTAMP = "timestamp";
    const char* CATEGORY = "category";
    const char* ICON = "icon";

    namespace Actions
    {
        const char* CREATE = "create";
        const char* VIEW = "view";
        const char* GET_PK = "get_pk";
        const char* ADD_PUBLISHER = "add_publisher";
        const char* UPDATE_PUBLISHER = "update_publisher";
        const char* VIEW_PUBLISHERS = "view_publishers";
        const char* MY_PUBLISHER_INFO = "my_publsiher_info";
        const char* ADD_DAPP = "add_dapp";
        const char* UPDATE_DAPP = "update_dapp";
        const char* DELETE_DAPP = "delete_dapp";
        const char* VIEW_DAPPS = "view_dapps";
    } // namespace Actions

    uint8_t Ch2Hex(uint8_t c)
    {
        if (c >= 'a' && c <= 'f')
            return 0xa + (c - 'a');

        if (c >= 'A' && c <= 'F')
            return 0xa + (c - 'A');

        return c - '0';
    }

    uint32_t Scan(uint8_t* pDst, const char* sz, uint32_t nTxtLen)
    {
        uint32_t ret = 0;
        for (; ret < nTxtLen; ret++)
        {
            uint8_t x = Ch2Hex(sz[ret]);
            if (x > 0xf)
                break;

            if (1 & ret)
                *pDst++ |= x;
            else
                *pDst = (x << 4);
        }

        return ret;
    }

    void OnError(const char* sz)
    {
        Env::DocAddText("error", sz);
    }

    template<typename Type>
    Type* AllocateForPublisher()
    {
        uint32_t size = sizeof(Type) + DAppsStore::Publisher::NAME_MAX_SIZE + DAppsStore::Publisher::SHORT_TITLE_MAX_SIZE +
            DAppsStore::Publisher::ABOUT_ME_MAX_SIZE + DAppsStore::Publisher::WEBSITE_MAX_SIZE + 5 * DAppsStore::Publisher::SOCIAL_NICK_MAX_SIZE;

        return static_cast<Type*>(Env::Heap_Alloc(size));
    }

    template<typename Type>
    bool GetPublisherMetadata(Type* args, uint32_t& argsSize)
    {
        uint8_t* pointer = reinterpret_cast<uint8_t*>(args + 1);
        args->m_NameSize = Env::DocGetBlob(NAME, pointer, DAppsStore::Publisher::NAME_MAX_SIZE);

        if (args->m_NameSize == 0)
        {
            OnError("name should be specified");
            return false;
        }
        pointer += args->m_NameSize;
        Env::DocAddNum32("nameSize", args->m_NameSize);

        args->m_ShortTitleSize = Env::DocGetBlob(SHORT_TITLE, pointer, DAppsStore::Publisher::SHORT_TITLE_MAX_SIZE);
        pointer += args->m_ShortTitleSize;

        args->m_AboutMeSize = Env::DocGetBlob(ABOUT_ME, pointer, DAppsStore::Publisher::ABOUT_ME_MAX_SIZE);
        pointer += args->m_AboutMeSize;

        args->m_WebsiteSize = Env::DocGetBlob(WEBSITE, pointer, DAppsStore::Publisher::WEBSITE_MAX_SIZE);
        pointer += args->m_WebsiteSize;

        args->m_TwitterSize = Env::DocGetBlob(TWITTER, pointer, DAppsStore::Publisher::SOCIAL_NICK_MAX_SIZE);
        pointer += args->m_TwitterSize;

        args->m_LinkedinSize = Env::DocGetBlob(LINKEDIN, pointer, DAppsStore::Publisher::SOCIAL_NICK_MAX_SIZE);
        pointer += args->m_LinkedinSize;

        args->m_InstagramSize = Env::DocGetBlob(INSTAGRAM, pointer, DAppsStore::Publisher::SOCIAL_NICK_MAX_SIZE);
        pointer += args->m_InstagramSize;

        args->m_TelegramSize = Env::DocGetBlob(TELEGRAM, pointer, DAppsStore::Publisher::SOCIAL_NICK_MAX_SIZE);
        pointer += args->m_TelegramSize;

        args->m_DiscordSize = Env::DocGetBlob(DISCORD, pointer, DAppsStore::Publisher::SOCIAL_NICK_MAX_SIZE);
        pointer += args->m_DiscordSize;

        argsSize = pointer - reinterpret_cast<uint8_t*>(args);

        return true;
    }

    template<typename Type>
    uint32_t EstimateDAppSize()
    {
        uint32_t size = sizeof(Type);

        size += Env::DocGetBlob(NAME, nullptr, 0);
        size += Env::DocGetBlob(DESCRIPTION, nullptr, 0);
        size += Env::DocGetBlob(API_VERSION, nullptr, 0);
        size += Env::DocGetBlob(MIN_API_VERSION, nullptr, 0);
        size += Env::DocGetBlob(ICON, nullptr, 0);

        return size;
    }

    template<typename Type>
    Type* AllocateDApp(uint32_t& size)
    {
        Type tmp;

        tmp.m_NameSize = Env::DocGetBlob(NAME, nullptr, 0);
        if (tmp.m_NameSize > DAppsStore::DApp::NAME_MAX_SIZE)
        {
            OnError("");
            return nullptr;
        }

        tmp.m_DescriptionSize = Env::DocGetBlob(DESCRIPTION, nullptr, 0);
        if (tmp.m_DescriptionSize > DAppsStore::DApp::DESCRIPTION_MAX_SIZE)
        {
            OnError("");
            return nullptr;
        }

        tmp.m_ApiVersionSize = Env::DocGetBlob(API_VERSION, nullptr, 0);
        if (tmp.m_ApiVersionSize > DAppsStore::DApp::API_VERSION_MAX_SIZE)
        {
            OnError("");
            return nullptr;
        }

        tmp.m_MinApiVersionSize = Env::DocGetBlob(MIN_API_VERSION, nullptr, 0);
        if (tmp.m_MinApiVersionSize > DAppsStore::DApp::API_VERSION_MAX_SIZE)
        {
            OnError("");
            return nullptr;
        }

        tmp.m_IconSize = Env::DocGetBlob(ICON, nullptr, 0);

        size = sizeof(Type) + tmp.m_NameSize + tmp.m_DescriptionSize + tmp.m_ApiVersionSize +
            tmp.m_MinApiVersionSize + tmp.m_IconSize;

        auto* dapp = static_cast<Type*>(Env::Heap_Alloc(size));
        _POD_(*dapp) = tmp;

        return dapp;
    }

    template<typename Type>
    bool GetDAppMetadata(Type* args)
    {
        if (!Env::DocGetBlobEx(DAPP_ID, &args->m_Id, sizeof(args->m_Id)))
        {
            OnError("id should be specified");
            return false;
        }

        if (!Env::DocGetText(IPFS_ID, args->m_IPFSId, sizeof(args->m_IPFSId)))
        {
            OnError("ipfs id should be specified");
            return false;
        }

        if (!Env::DocGetNum32(MAJOR, &args->m_Version.m_Major))
        {
            OnError("major should be specified");
            return false;
        }

        if (!Env::DocGetNum32(MINOR, &args->m_Version.m_Minor))
        {
            OnError("minor should be specified");
            return false;
        }

        if (!Env::DocGetNum32(RELEASE, &args->m_Version.m_Release))
        {
            OnError("release should be specified");
            return false;
        }

        if (!Env::DocGetNum32(BUILD, &args->m_Version.m_Build))
        {
            OnError("build should be specified");
            return false;
        }

        if (!Env::DocGetNum32(CATEGORY, &args->m_Category))
        {
            OnError("category should be specified");
            return false;
        }

        auto* pointer = reinterpret_cast<uint8_t*>(args + 1);
        if (!args->m_NameSize || !Env::DocGetBlobEx(NAME, pointer, args->m_NameSize))
        {
            OnError("dapp name should be specified");
            return false;
        }
        pointer += args->m_NameSize;

        if (!args->m_DescriptionSize || !Env::DocGetBlobEx(DESCRIPTION, pointer, args->m_DescriptionSize))
        {
            OnError("description should be specified");
            return false;
        }
        pointer += args->m_DescriptionSize;

        if (!args->m_ApiVersionSize || !Env::DocGetBlobEx(API_VERSION, pointer, args->m_ApiVersionSize))
        {
            OnError("api_ver should be specified");
            return false;
        }
        pointer += args->m_ApiVersionSize;

        if (!args->m_MinApiVersionSize || !Env::DocGetBlobEx(MIN_API_VERSION, pointer, args->m_MinApiVersionSize))
        {
            OnError("min_api_ver should be specified");
            return false;
        }
        pointer += args->m_MinApiVersionSize;

        Env::DocGetBlobEx(ICON, pointer, args->m_IconSize);

        return true;
    }

    bool IsExistPublisher(const ContractID& cid, const PubKey& pubkey)
    {
        Env::Key_T<DAppsStore::Publisher::Key> k;
        _POD_(k.m_Prefix.m_Cid) = cid;
        _POD_(k.m_KeyInContract.m_PubKey) = pubkey;
        Env::VarReader reader(k, k);
        uint32_t sizeKey = sizeof(k);
        uint32_t size = 0;
        return reader.MoveNext(&k, sizeKey, nullptr, size, 0);
    }

    bool IsExistDApp(const ContractID& cid, const DAppsStore::DAppId& id)
    {
        Env::Key_T<DAppsStore::DApp::Key> k;
        _POD_(k.m_Prefix.m_Cid) = cid;
        _POD_(k.m_KeyInContract.m_Id) = id;
        Env::VarReader reader(k, k);
        uint32_t sizeKey = sizeof(k);
        uint32_t size = 0;
        return reader.MoveNext(&k, sizeKey, nullptr, size, 0);
    }

    struct Publishers
    {
        ~Publishers()
        {
            if (m_PubKeys)
            {
                Env::Heap_Free(m_PubKeys);
            }
        }

        bool Init()
        {
            const uint32_t CHUNK_SIZE = 67;
            uint32_t size = Env::DocGetText(PUBLISHERS, nullptr, 0);

            if (size == 0)
            {
                return true;
            }

            if (size % CHUNK_SIZE != 0)
            {
                OnError("wrong size of buffer");
                return false;
            }

            char* buffer = static_cast<char*>(Env::Heap_Alloc(size));

            Env::DocGetText(PUBLISHERS, buffer, size);

            m_Amount = size / CHUNK_SIZE;
            m_PubKeys = static_cast<PubKey*>(Env::Heap_Alloc(m_Amount * sizeof(PubKey)));

            for (uint32_t i = 0; i < m_Amount; i++)
            {
                const char separator = buffer[(i + 1) * CHUNK_SIZE - 1];

                if (i == m_Amount - 1)
                {
                    if (separator != '\0')
                    {
                        OnError("last sympol wrong");
                        return false;
                    }
                }
                else
                {
                    if (separator != ';')
                    {
                        OnError("separator is wrong");
                        return false;
                    }
                }

                char tmp[CHUNK_SIZE - 1];

                Env::Memcpy(tmp, &buffer[i * CHUNK_SIZE], CHUNK_SIZE - 1);

                Scan((uint8_t*)&m_PubKeys[i], tmp, CHUNK_SIZE - 1);
            }

            Env::Heap_Free(buffer);
            return true;
        }

        bool NeedFilter() const
        {
            return m_Amount > 0;
        }

        bool HasPubKey(const PubKey& pk)
        {
            for (uint32_t i = 0; i < m_Amount; i++)
            {
                if (_POD_(pk) == m_PubKeys[i])
                {
                    return true;
                }
            }
            return false;
        }

        PubKey* m_PubKeys = nullptr;
        uint32_t m_Amount = 0;
    };

    void PrintPublisher(uint8_t* pointer, const DAppsStore::Publisher& publisher)
    {
        Env::DocAddBlob(NAME, pointer, publisher.m_NameSize);
        pointer += publisher.m_NameSize;
        Env::DocAddBlob(SHORT_TITLE, pointer, publisher.m_ShortTitleSize);
        pointer += publisher.m_ShortTitleSize;
        Env::DocAddBlob(ABOUT_ME, pointer, publisher.m_AboutMeSize);
        pointer += publisher.m_AboutMeSize;
        Env::DocAddBlob(WEBSITE, pointer, publisher.m_WebsiteSize);
        pointer += publisher.m_WebsiteSize;
        Env::DocAddBlob(TWITTER, pointer, publisher.m_TwitterSize);
        pointer += publisher.m_TwitterSize;
        Env::DocAddBlob(LINKEDIN, pointer, publisher.m_LinkedinSize);
        pointer += publisher.m_LinkedinSize;
        Env::DocAddBlob(INSTAGRAM, pointer, publisher.m_InstagramSize);
        pointer += publisher.m_InstagramSize;
        Env::DocAddBlob(TELEGRAM, pointer, publisher.m_TelegramSize);
        pointer += publisher.m_TelegramSize;
        Env::DocAddBlob(DISCORD, pointer, publisher.m_DiscordSize);
    }
} // namespace

namespace manager
{
    ContractID cid;

    void GetPk()
    {
        PubKey pk;
        Env::DerivePk(pk, &cid, sizeof(cid));
        Env::DocAddBlob_T(PUBKEY, pk);
    }

    void AddPublisher()
    {
        auto* args = AllocateForPublisher<DAppsStore::Method::AddPublisher>();

        Env::DerivePk(args->m_Publisher, &cid, sizeof(cid));

        uint32_t argsSize = 0;
        if (IsExistPublisher(cid, args->m_Publisher))
        {
            OnError("the publisher exists");
        }
        else if (GetPublisherMetadata(args, argsSize))
        {
            SigRequest sig;
            sig.m_pID = &cid;
            sig.m_nID = sizeof(cid);

            Env::GenerateKernel(&cid, DAppsStore::Method::AddPublisher::METHOD_ID, args, argsSize, nullptr, 0, &sig, 1, "add publisher to store", 1000000);
        }

        Env::Heap_Free(args);
    }

    void UpdatePublisher()
    {
        auto* args = AllocateForPublisher<DAppsStore::Method::UpdatePublisher>();
        Env::DerivePk(args->m_Publisher, &cid, sizeof(cid));

        uint32_t argsSize = 0;
        if (!IsExistPublisher(cid, args->m_Publisher))
        {
            OnError("the publisher is missing");
        }
        else if (GetPublisherMetadata(args, argsSize))
        {
            SigRequest sig;
            sig.m_pID = &cid;
            sig.m_nID = sizeof(cid);

            Env::GenerateKernel(&cid, DAppsStore::Method::UpdatePublisher::METHOD_ID, args, argsSize, nullptr, 0, &sig, 1, "update publisher", 0);
        }

        Env::Heap_Free(args);
    }

    void ViewPublishers()
    {
        Env::Key_T<DAppsStore::Publisher::Key> k0, k1;
        _POD_(k0.m_Prefix.m_Cid) = cid;
        _POD_(k0.m_KeyInContract.m_PubKey).SetZero();
        _POD_(k1.m_Prefix.m_Cid) = cid;
        _POD_(k1.m_KeyInContract.m_PubKey).SetObject(0xff);

        Env::VarReader reader(k0, k1);
        Env::DocArray arr("publishers");
        while (true)
        {
            uint32_t keySize = sizeof(k0);
            uint32_t size = 0;

            if (!reader.MoveNext(&k0, keySize, nullptr, size, 0))
                break;

            if ((sizeof(k0) != keySize) || (size < sizeof(DAppsStore::Publisher)))
                continue;

            auto* publisher = static_cast<DAppsStore::Publisher*>(Env::Heap_Alloc(size));

            reader.MoveNext(&k0, keySize, publisher, size, 1);

            auto* pointer = reinterpret_cast<uint8_t*>(publisher + 1);

            Env::DocGroup gr("");
            Env::DocAddBlob_T(PUBKEY, k0.m_KeyInContract.m_PubKey);
            PrintPublisher(pointer, *publisher);
        }
    }

    void MyPublisherInfo()
    {
        PubKey pk;
        Env::DerivePk(pk, &cid, sizeof(cid));

        Env::Key_T<DAppsStore::Publisher::Key> k;
        _POD_(k.m_Prefix.m_Cid) = cid;
        _POD_(k.m_KeyInContract.m_PubKey) = pk;

        Env::VarReader reader(k, k);
        Env::DocGroup gr(Actions::MY_PUBLISHER_INFO);

        uint32_t keySize = sizeof(k);
        uint32_t size = 0;

        if (!reader.MoveNext(&k, keySize, nullptr, size, 0))
            return;

        if ((sizeof(k) != keySize) || (size < sizeof(DAppsStore::Publisher)))
            return;

        auto* publisher = static_cast<DAppsStore::Publisher*>(Env::Heap_Alloc(size));

        reader.MoveNext(&k, keySize, publisher, size, 1);

        auto* pointer = reinterpret_cast<uint8_t*>(publisher + 1);

        Env::DocAddBlob_T(PUBKEY, k.m_KeyInContract.m_PubKey);
        PrintPublisher(pointer, *publisher);
    }

    void AddDApp()
    {
        uint32_t argsSize = 0;
        auto* args = AllocateDApp<DAppsStore::Method::AddDApp>(argsSize);

        if (!GetDAppMetadata(args))
        {
            return;
        }
        
        if (IsExistDApp(cid, args->m_Id))
        {
            OnError("the dapp exists");
            return;
        }

        Env::DerivePk(args->m_Publisher, &cid, sizeof(cid));

        if (!IsExistPublisher(cid, args->m_Publisher))
        {
            OnError("the publisher is missing");
            return;
        }

        SigRequest sig;
        sig.m_pID = &cid;
        sig.m_nID = sizeof(cid);

        Env::GenerateKernel(&cid, DAppsStore::Method::AddDApp::METHOD_ID, args, argsSize, nullptr, 0, &sig, 1, "add dapp to store", 170000);

        Env::Heap_Free(args);
    }

    void UpdateDApp()
    {
        uint32_t argsSize = 0;
        auto* args = AllocateDApp<DAppsStore::Method::UpdateDApp>(argsSize);

        if (!GetDAppMetadata(args))
        {
            return;
        }

        if (!IsExistDApp(cid, args->m_Id))
        {
            OnError("the dapp is missing");
            return;
        }

        SigRequest sig;
        sig.m_pID = &cid;
        sig.m_nID = sizeof(cid);

        Env::GenerateKernel(&cid, DAppsStore::Method::UpdateDApp::METHOD_ID, args, argsSize, nullptr, 0, &sig, 1, "update dapp", 0);

        Env::Heap_Free(args);
    }

    void DeleteDApp()
    {
        DAppsStore::Method::DeleteDApp args;

        Env::DocGetBlobEx(DAPP_ID, &args.m_Id, sizeof(args.m_Id));

        if (!IsExistDApp(cid, args.m_Id))
        {
            OnError("the dapp is missing");
            return;
        }

        SigRequest sig;
        sig.m_pID = &cid;
        sig.m_nID = sizeof(cid);

        Env::GenerateKernel(&cid, args.METHOD_ID, &args, sizeof(args), nullptr, 0, &sig, 1, "delete dapp", 0);
    }

    void ViewDApps()
    {
        Publishers publishers;
        if (!publishers.Init())
        {
            return;
        }

        Env::Key_T<DAppsStore::DApp::Key> k0, k1;
        _POD_(k0.m_Prefix.m_Cid) = cid;
        _POD_(k0.m_KeyInContract.m_Id).SetZero();
        _POD_(k1.m_Prefix.m_Cid) = cid;
        _POD_(k1.m_KeyInContract.m_Id).SetObject(0xff);

        Env::VarReader reader(k0, k1);

        Env::DocArray arr("dapps");
        while (true)
        {
            uint32_t keySize = sizeof(k0);
            uint32_t size = 0;

            if (!reader.MoveNext(&k0, keySize, nullptr, size, 0))
                break;

            if ((sizeof(k0) != keySize) || (size < sizeof(DAppsStore::DApp)))
                continue;

            auto* dapp = static_cast<DAppsStore::DApp*>(Env::Heap_Alloc(size));

            reader.MoveNext(&k0, keySize, dapp, size, 1);

            if (publishers.NeedFilter() && !publishers.HasPubKey(dapp->m_Publisher))
                continue;

            Env::DocGroup gr("");
            Env::DocAddBlob_T(DAPP_ID, k0.m_KeyInContract.m_Id);

            auto* pointer = reinterpret_cast<uint8_t*>(dapp + 1);

            Env::DocAddBlob(NAME, pointer, dapp->m_NameSize);
            pointer += dapp->m_NameSize;
            Env::DocAddBlob(DESCRIPTION, pointer, dapp->m_DescriptionSize);
            pointer += dapp->m_DescriptionSize;
            Env::DocAddBlob(API_VERSION, pointer, dapp->m_ApiVersionSize);
            pointer += dapp->m_ApiVersionSize;
            Env::DocAddBlob(MIN_API_VERSION, pointer, dapp->m_MinApiVersionSize);
            pointer += dapp->m_MinApiVersionSize;

            Env::DocAddBlob_T(PUBLISHER, dapp->m_Publisher);
            Env::DocAddText(IPFS_ID, dapp->m_IPFSId);
            Env::DocAddNum64(TIMESTAMP, dapp->m_Timestamp);
            Env::DocAddNum32(CATEGORY, dapp->m_Category);

            Env::DocAddBlob(ICON, pointer, dapp->m_IconSize);

            {
                Env::DocGroup version(VERSION);
                Env::DocAddNum32(MAJOR, dapp->m_Version.m_Major);
                Env::DocAddNum32(MINOR, dapp->m_Version.m_Minor);
                Env::DocAddNum32(RELEASE, dapp->m_Version.m_Release);
                Env::DocAddNum32(BUILD, dapp->m_Version.m_Build);
            }

            Env::Heap_Free(dapp);
        }
    }
} // namespace manager

BEAM_EXPORT void Method_0()
{
    // scheme
    Env::DocGroup root("");
    {
        Env::DocGroup grMethod(Actions::GET_PK);
        Env::DocAddText(CONTRACT_ID, "ContractID");
    }
    {
        Env::DocGroup grMethod(Actions::ADD_PUBLISHER);
        Env::DocAddText(CONTRACT_ID, "ContractID");
        Env::DocAddText(NAME, "string");
        Env::DocAddText(NAME, "string");
    }
    {
        Env::DocGroup grMethod(Actions::UPDATE_PUBLISHER);
        Env::DocAddText(CONTRACT_ID, "ContractID");
        Env::DocAddText(NAME, "string");
    }
    {
        Env::DocGroup grMethod(Actions::VIEW_PUBLISHERS);
        Env::DocAddText(CONTRACT_ID, "ContractID");
    }
    {
        Env::DocGroup grMethod(Actions::MY_PUBLISHER_INFO);
        Env::DocAddText(CONTRACT_ID, "ContractID");
    }
    {
        Env::DocGroup grMethod(Actions::ADD_DAPP);
        Env::DocAddText(CONTRACT_ID, "ContractID");
        Env::DocAddText(DAPP_ID, "DAppId");
        Env::DocAddText(IPFS_ID, "IPFSCID");
        Env::DocAddText(NAME, "string");
        Env::DocAddText(DESCRIPTION, "string");
        Env::DocAddText(API_VERSION, "string");
        Env::DocAddText(MIN_API_VERSION, "string");
        // version
        Env::DocAddText(MAJOR, "uint32_t");
        Env::DocAddText(MINOR, "uint32_t");
        Env::DocAddText(RELEASE, "uint32_t");
        Env::DocAddText(BUILD, "uint32_t");
        // icon
        Env::DocAddText(ICON, "icon");
    }
    {
        Env::DocGroup grMethod(Actions::UPDATE_DAPP);
        Env::DocAddText(CONTRACT_ID, "ContractID");
        Env::DocAddText(DAPP_ID, "DAppId");
        Env::DocAddText(IPFS_ID, "IPFSCID");
        Env::DocAddText(NAME, "string");
        Env::DocAddText(DESCRIPTION, "string");
        Env::DocAddText(API_VERSION, "string");
        Env::DocAddText(MIN_API_VERSION, "string");
        // version
        Env::DocAddText(MAJOR, "uint32_t");
        Env::DocAddText(MINOR, "uint32_t");
        Env::DocAddText(RELEASE, "uint32_t");
        Env::DocAddText(BUILD, "uint32_t");
        // icon
        Env::DocAddText(ICON, "icon");
    }
    {
        Env::DocGroup grMethod(Actions::DELETE_DAPP);
        Env::DocAddText(CONTRACT_ID, "ContractID");
        Env::DocAddText(DAPP_ID, "DAppId");
    }
    {
        Env::DocGroup grMethod(Actions::VIEW_DAPPS);
        Env::DocAddText(CONTRACT_ID, "ContractID");
        Env::DocAddText(PUBLISHERS, "string");
    }
}

BEAM_EXPORT void Method_1()
{
    Env::DocGroup root("");

    char szAction[20];

    if (!Env::DocGetText("action", szAction, sizeof(szAction)))
    {
        OnError("action should be specified");
        return;
    }
    
    if (!Env::DocGet(CONTRACT_ID, manager::cid))
    {
        OnError("cid should be specified");
        return;
    }

    if (!Env::Strcmp(szAction, Actions::GET_PK))
    {
        manager::GetPk();
    }
    else if (!Env::Strcmp(szAction, Actions::ADD_PUBLISHER))
    {
        manager::AddPublisher();
    }
    else if (!Env::Strcmp(szAction, Actions::UPDATE_PUBLISHER))
    {
        manager::UpdatePublisher();
    }
    else if (!Env::Strcmp(szAction, Actions::VIEW_PUBLISHERS))
    {
        manager::ViewPublishers();
    }
    else if (!Env::Strcmp(szAction, Actions::MY_PUBLISHER_INFO))
    {
        manager::MyPublisherInfo();
    }
    else if (!Env::Strcmp(szAction, Actions::ADD_DAPP))
    {
        manager::AddDApp();
    }
    else if (!Env::Strcmp(szAction, Actions::UPDATE_DAPP))
    {
        manager::UpdateDApp();
    }
    else if (!Env::Strcmp(szAction, Actions::DELETE_DAPP))
    {
        manager::DeleteDApp();
    }
    else if (!Env::Strcmp(szAction, Actions::VIEW_DAPPS))
    {
        manager::ViewDApps();
    }
    else
    {
        OnError("invalid Action.");
    }
}