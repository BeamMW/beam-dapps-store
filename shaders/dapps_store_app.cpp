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
    const char* PUBKEY = "pubkey";
    const char* IPFS_ID = "ipfs_id";
    const char* NAME = "name";
    const char* DESCRIPTION = "description";
    const char* API_VERSION = "api_ver";
    const char* MIN_API_VERSION = "min_api_ver";
    const char* DAPP_ID = "id";
    const char* VERSION = "version";

    namespace Actions
    {
        const char* CREATE = "create";
        const char* VIEW = "view";
        const char* GET_PK = "get_pk";
        const char* ADD_PUBLISHER = "add_publisher";
        const char* UPDATE_PUBLISHER = "update_publisher";
        const char* VIEW_PUBLISHERS = "view_publishers";
        const char* ADD_DAPP = "add_dapp";
        const char* UPDATE_DAPP = "update_dapp";
        const char* DELETE_DAPP = "delete_dapp";
        const char* VIEW_DAPPS = "view_dapps";
    } // namespace Actions

    void OnError(const char* sz)
    {
        Env::DocAddText("error", sz);
    }

    bool ReadDAppMetadata(DAppsStore::Method::Base& args)
    {
        if (!Env::DocGetBlobEx(DAPP_ID, &args.m_Id, sizeof(args.m_Id)))
        {
            OnError("id should be specified");
            return false;
        }

        if (!Env::DocGetText(IPFS_ID, args.m_IPFSId, sizeof(args.m_IPFSId)))
        {
            OnError("ipfs id should be specified");
            return false;
        }

        if (!Env::DocGetText(NAME, args.m_Name, DAppsStore::DApp::NAME_MAX_SIZE))
        {
            OnError("dapp name should be specified");
            return false;
        }

        if (!Env::DocGetText(VERSION, args.m_Version, DAppsStore::DApp::VERSION_MAX_SIZE))
        {
            OnError("dapp version should be specified");
            return false;
        }

        if (!Env::DocGetText(DESCRIPTION, args.m_Description, DAppsStore::DApp::DESCRIPTION_MAX_SIZE))
        {
            OnError("description should be specified");
            return false;
        }

        if (!Env::DocGetText(API_VERSION, args.m_ApiVersion, DAppsStore::DApp::API_VERSION_MAX_SIZE))
        {
            OnError("api_ver should be specified");
            return false;
        }

        if (!Env::DocGetText(MIN_API_VERSION, args.m_MinApiVersion, DAppsStore::DApp::API_VERSION_MAX_SIZE))
        {
            OnError("min_api_ver should be specified");
            return false;
        }

        return true;
    }

    bool IsExistPublisher(const ContractID& cid, const PubKey& pubkey)
    {
        Env::Key_T<DAppsStore::Publisher::Key> k;
        _POD_(k.m_Prefix.m_Cid) = cid;
        _POD_(k.m_KeyInContract.m_PubKey) = pubkey;

        DAppsStore::Publisher publisher;
        if (Env::VarReader::Read_T(k, publisher))
        {
            return true;
        }

        return false;
    }

    bool IsExistDApp(const ContractID& cid, const DAppsStore::DAppId& id)
    {
        Env::Key_T<DAppsStore::DApp::Key> k;
        _POD_(k.m_Prefix.m_Cid) = cid;
        _POD_(k.m_KeyInContract.m_Id) = id;
        DAppsStore::DApp dapp;
        if (Env::VarReader::Read_T(k, dapp))
        {
            return true;
        }

        return false;
    }
} // namespace

namespace manager
{
    ContractID cid;

    void Create()
    {
        DAppsStore::Method::Create args;

        Env::GenerateKernel(nullptr, args.METHOD_ID, &args, sizeof(args), nullptr, 0, nullptr, 0, "create DApps Store contract", 0);
    }

    void View()
    {
        EnumAndDumpContracts(DAppsStore::s_SID);
    }

    void GetPk()
    {
        PubKey pk;
        Env::DerivePk(pk, &cid, sizeof(cid));
        Env::DocAddBlob_T(PUBKEY, pk);
    }

    void AddPublisher()
    {
        DAppsStore::Method::AddPublisher args;
        Env::DerivePk(args.m_Publisher, &cid, sizeof(cid));

        if (IsExistPublisher(cid, args.m_Publisher))
        {
            OnError("publisher is exist");
            return;
        }

        if (!Env::DocGetText(NAME, args.m_Name, DAppsStore::Publisher::NAME_MAX_SIZE))
        {
            OnError("name should be specified");
            return;
        }

        SigRequest sig;
        sig.m_pID = &cid;
        sig.m_nID = sizeof(cid);

        Env::GenerateKernel(&cid, args.METHOD_ID, &args, sizeof(args), nullptr, 0, &sig, 1, "add publisher to store", 0);
    }

    void UpdatePublisher()
    {
        DAppsStore::Method::UpdatePublisher args;
        Env::DerivePk(args.m_Publisher, &cid, sizeof(cid));

        if (!IsExistPublisher(cid, args.m_Publisher))
        {
            OnError("publisher is absent");
            return;
        }

        if (!Env::DocGetText(NAME, args.m_Name, DAppsStore::Publisher::NAME_MAX_SIZE))
        {
            OnError("name should be specified");
            return;
        }

        SigRequest sig;
        sig.m_pID = &cid;
        sig.m_nID = sizeof(cid);

        Env::GenerateKernel(&cid, args.METHOD_ID, &args, sizeof(args), nullptr, 0, &sig, 1, "update publisher", 0);
    }

    void ViewPublishers()
    {
        Env::Key_T<DAppsStore::Publisher::Key> k0, k1;
        _POD_(k0.m_Prefix.m_Cid) = cid;
        _POD_(k0.m_KeyInContract.m_PubKey).SetZero();
        _POD_(k1.m_Prefix.m_Cid) = cid;
        _POD_(k1.m_KeyInContract.m_PubKey).SetObject(0xff);

        Env::VarReader reader(k0, k1);
        DAppsStore::Publisher publisher;

        Env::DocArray arr("publishers");
        while (reader.MoveNext_T(k0, publisher))
        {
            Env::DocGroup gr("");
            Env::DocAddBlob_T(PUBKEY, k0.m_KeyInContract.m_PubKey);
            Env::DocAddText(NAME, publisher.m_Name);
        }
    }

    void AddDApp()
    {
        DAppsStore::Method::AddDApp args;

        if (!ReadDAppMetadata(args))
        {
            return;
        }
        
        if (IsExistDApp(cid, args.m_Id))
        {
            OnError("dapp is exist");
            return;
        }

        Env::DerivePk(args.m_Publisher, &cid, sizeof(cid));

        if (!IsExistPublisher(cid, args.m_Publisher))
        {
            OnError("publisher is absent");
            return;
        }

        SigRequest sig;
        sig.m_pID = &cid;
        sig.m_nID = sizeof(cid);

        Env::GenerateKernel(&cid, args.METHOD_ID, &args, sizeof(args), nullptr, 0, &sig, 1, "add dapp to store", 170000);
    }

    void UpdateDApp()
    {
        DAppsStore::Method::UpdateDApp args;

        if (!ReadDAppMetadata(args))
        {
            return;
        }

        if (!IsExistDApp(cid, args.m_Id))
        {
            OnError("dapp is absent");
            return;
        }

        SigRequest sig;
        sig.m_pID = &cid;
        sig.m_nID = sizeof(cid);

        Env::GenerateKernel(&cid, args.METHOD_ID, &args, sizeof(args), nullptr, 0, &sig, 1, "update dapp", 0);
    }

    void DeleteDApp()
    {
        DAppsStore::Method::DeleteDApp args;

        Env::DocGetBlobEx(DAPP_ID, &args.m_Id, sizeof(args.m_Id));

        if (!IsExistDApp(cid, args.m_Id))
        {
            OnError("dapp is absent");
            return;
        }

        SigRequest sig;
        sig.m_pID = &cid;
        sig.m_nID = sizeof(cid);

        Env::GenerateKernel(&cid, args.METHOD_ID, &args, sizeof(args), nullptr, 0, &sig, 1, "delete dapp", 0);
    }

    void ViewDApps()
    {
        PubKey pk;
        bool filter = Env::DocGet(PUBLISHER, pk);
        Env::Key_T<DAppsStore::DApp::Key> k0, k1;
        _POD_(k0.m_Prefix.m_Cid) = cid;
        _POD_(k0.m_KeyInContract.m_Id).SetZero();
        _POD_(k1.m_Prefix.m_Cid) = cid;
        _POD_(k1.m_KeyInContract.m_Id).SetObject(0xff);

        Env::VarReader reader(k0, k1);
        DAppsStore::DApp dapp;

        Env::DocArray arr("dapps");
        while (reader.MoveNext_T(k0, dapp))
        {
            if (filter && _POD_(dapp.m_Publisher) != pk)
                continue;

            Env::DocGroup gr("");
            Env::DocAddBlob_T(DAPP_ID, k0.m_KeyInContract.m_Id);
            Env::DocAddText(NAME, dapp.m_Name);
            Env::DocAddText(DESCRIPTION, dapp.m_Description);
            Env::DocAddText(VERSION, dapp.m_Version);
            Env::DocAddText(API_VERSION, dapp.m_ApiVersion);
            Env::DocAddText(MIN_API_VERSION, dapp.m_MinApiVersion);
            Env::DocAddBlob_T(PUBLISHER, dapp.m_Publisher);
            Env::DocAddText(IPFS_ID, dapp.m_IPFSId);
        }
    }
} // namespace manager

BEAM_EXPORT void Method_0()
{
    // scheme
    Env::DocGroup root("");
    {
        Env::DocGroup grMethod(Actions::CREATE);
    }
    {
        Env::DocGroup grMethod(Actions::VIEW);
    }
    {
        Env::DocGroup grMethod(Actions::GET_PK);
        Env::DocAddText(CONTRACT_ID, "ContractID");
    }
    {
        Env::DocGroup grMethod(Actions::ADD_PUBLISHER);
        Env::DocAddText(CONTRACT_ID, "ContractID");
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
        Env::DocGroup grMethod(Actions::ADD_DAPP);
        Env::DocAddText(CONTRACT_ID, "ContractID");
        Env::DocAddText(DAPP_ID, "DAppId");
        Env::DocAddText(IPFS_ID, "IPFSCID");
        Env::DocAddText(NAME, "string");
        Env::DocAddText(VERSION, "string");
        Env::DocAddText(DESCRIPTION, "string");
        Env::DocAddText(API_VERSION, "string");
        Env::DocAddText(MIN_API_VERSION, "string");
    }
    {
        Env::DocGroup grMethod(Actions::UPDATE_DAPP);
        Env::DocAddText(CONTRACT_ID, "ContractID");
        Env::DocAddText(DAPP_ID, "DAppId");
        Env::DocAddText(IPFS_ID, "IPFSCID");
        Env::DocAddText(NAME, "string");
        Env::DocAddText(VERSION, "string");
        Env::DocAddText(DESCRIPTION, "string");
        Env::DocAddText(API_VERSION, "string");
        Env::DocAddText(MIN_API_VERSION, "string");
    }
    {
        Env::DocGroup grMethod(Actions::DELETE_DAPP);
        Env::DocAddText(CONTRACT_ID, "ContractID");
        Env::DocAddText(DAPP_ID, "DAppId");
    }
    {
        Env::DocGroup grMethod(Actions::VIEW_DAPPS);
        Env::DocAddText(CONTRACT_ID, "ContractID");
    }
}

BEAM_EXPORT void Method_1()
{
    Env::DocGroup root("");

    char szAction[20];

    if (!Env::DocGetText("action", szAction, sizeof(szAction)))
    {
        OnError("Action not specified");
        return;
    }

    if (!Env::Strcmp(szAction, Actions::CREATE))
    {
        manager::Create();
    }
    else if (!Env::Strcmp(szAction, Actions::VIEW))
    {
        manager::View();
    }
    else
    {
        Env::DocGet(CONTRACT_ID, manager::cid);

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
}