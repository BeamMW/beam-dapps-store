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
    const char* IPFS_ID = "ipfs_id";
    const char* LABEL = "label";
    const char* DAPP_ID = "id";

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
        Env::DocAddBlob_T("pk", pk);
    }

    void AddPublisher()
    {
        DAppsStore::Method::AddPublisher args;
        Env::DerivePk(args.m_Publisher, &cid, sizeof(cid));
        args.m_LabelSize = Env::DocGetText(LABEL, args.m_Label, DAppsStore::Publisher::LABEL_MAX_SIZE);

        // check size of label
        if (args.m_LabelSize > DAppsStore::Publisher::LABEL_MAX_SIZE)
        {
            OnError("too large label!!!");
            return;
        }

        SigRequest sig;
        sig.m_pID = &cid;
        sig.m_nID = sizeof(cid);

        Env::GenerateKernel(&cid, args.METHOD_ID, &args, sizeof(args), nullptr, 0, &sig, 1, "add publisher to store", 0);
    }

    void UpdatePublisher()
    {
        // TODO check to exist
        DAppsStore::Method::UpdatePublisher args;
        Env::DerivePk(args.m_Publisher, &cid, sizeof(cid));
        args.m_LabelSize = Env::DocGetText(LABEL, args.m_Label, DAppsStore::Publisher::LABEL_MAX_SIZE);

        // check size of label
        if (args.m_LabelSize > DAppsStore::Publisher::LABEL_MAX_SIZE)
        {
            OnError("too large label!!!");
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

        while (reader.MoveNext_T(k0, publisher))
        {
            Env::DocGroup gr("");
            Env::DocAddBlob_T(PUBLISHER, k0.m_KeyInContract.m_PubKey);
            Env::DocAddText(LABEL, publisher.m_Label);
        }
    }

    void AddDApp()
    {
        DAppsStore::Method::AddDApp args;
        Env::DerivePk(args.m_Publisher, &cid, sizeof(cid));
        Env::DocGetBlobEx(IPFS_ID, &args.m_IPFSId, sizeof(args.m_IPFSId));
        args.m_LabelSize = Env::DocGetText(LABEL, args.m_Label, DAppsStore::DApp::LABEL_MAX_SIZE);

        // check size of label
        if (args.m_LabelSize > DAppsStore::DApp::LABEL_MAX_SIZE)
        {
            OnError("too large label!!!");
            return;
        }

        SigRequest sig;
        sig.m_pID = &cid;
        sig.m_nID = sizeof(cid);

        Env::GenerateKernel(&cid, args.METHOD_ID, &args, sizeof(args), nullptr, 0, &sig, 1, "add dapp to store", 0);
    }

    void UpdateDApp()
    {
        // TODO check to exist
        DAppsStore::Method::UpdateDApp args;

        Env::DocGetNum64(DAPP_ID, &args.m_Id);
        Env::DocGetBlobEx(IPFS_ID, &args.m_IPFSId, sizeof(args.m_IPFSId));
        args.m_LabelSize = Env::DocGetText(LABEL, args.m_Label, DAppsStore::DApp::LABEL_MAX_SIZE);

        // check size of label
        if (args.m_LabelSize > DAppsStore::DApp::LABEL_MAX_SIZE)
        {
            OnError("too large label!!!");
            return;
        }

        SigRequest sig;
        sig.m_pID = &cid;
        sig.m_nID = sizeof(cid);

        Env::GenerateKernel(&cid, args.METHOD_ID, &args, sizeof(args), nullptr, 0, &sig, 1, "update dapp", 0);
    }

    void DeleteDApp()
    {
        // TODO check to exist
        DAppsStore::Method::DeleteDApp args;

        Env::DocGetNum64(DAPP_ID, &args.m_Id);

        SigRequest sig;
        sig.m_pID = &cid;
        sig.m_nID = sizeof(cid);

        Env::GenerateKernel(&cid, args.METHOD_ID, &args, sizeof(args), nullptr, 0, &sig, 1, "delete dapp", 0);
    }

    void ViewDApps()
    {
        Env::Key_T<DAppsStore::DApp::Key> k0, k1;
        _POD_(k0.m_Prefix.m_Cid) = cid;
        k0.m_KeyInContract.m_IdInBE = 0;
        _POD_(k1.m_Prefix.m_Cid) = cid;
        k1.m_KeyInContract.m_IdInBE = -1;

        Env::VarReader reader(k0, k1);
        DAppsStore::DApp dapp;

        while (reader.MoveNext_T(k0, dapp))
        {
            Env::DocGroup gr("");
            Env::DocAddNum(DAPP_ID, Utils::FromBE(k0.m_KeyInContract.m_IdInBE));
            Env::DocAddText(LABEL, dapp.m_Label);
            Env::DocAddBlob_T(PUBLISHER, dapp.m_Publisher);
            Env::DocAddBlob_T(IPFS_ID, dapp.m_IPFSId);
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
        Env::DocAddText(LABEL, "string");
    }
    {
        Env::DocGroup grMethod(Actions::UPDATE_PUBLISHER);
        Env::DocAddText(CONTRACT_ID, "ContractID");
        Env::DocAddText(LABEL, "string");
    }
    {
        Env::DocGroup grMethod(Actions::VIEW_PUBLISHERS);
        Env::DocAddText(CONTRACT_ID, "ContractID");
    }
    {
        Env::DocGroup grMethod(Actions::ADD_DAPP);
        Env::DocAddText(CONTRACT_ID, "ContractID");
        Env::DocAddText(IPFS_ID, "IPFSCID");
        Env::DocAddText(LABEL, "string");
    }
    {
        Env::DocGroup grMethod(Actions::UPDATE_DAPP);
        Env::DocAddText(CONTRACT_ID, "ContractID");
        Env::DocAddText(DAPP_ID, "uint64_t");
        Env::DocAddText(IPFS_ID, "IPFSCID");
        Env::DocAddText(LABEL, "string");
    }
    {
        Env::DocGroup grMethod(Actions::DELETE_DAPP);
        Env::DocAddText(CONTRACT_ID, "ContractID");
        Env::DocAddText(DAPP_ID, "uint64_t");
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