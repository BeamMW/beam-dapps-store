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

    void OnError(const char* sz)
    {
        Env::DocAddText("error", sz);
    }
} // namespace

namespace manager
{
    void Create()
    {
        DAppsStore::Create args;

        Env::GenerateKernel(nullptr, args.s_iMethod, &args, sizeof(args), nullptr, 0, nullptr, 0, "create DApps Store contract", 0);
    }

    void View()
    {
        EnumAndDumpContracts(DAppsStore::s_SID);
    }

    void GetPk()
    {
        ContractID cid;
        Env::DocGet(CONTRACT_ID, cid);

        PubKey pk;
        Env::DerivePk(pk, &cid, sizeof(cid));
        Env::DocAddBlob_T("pk", pk);
    }

    void AddPublisher()
    {
        ContractID cid;
        Env::DocGet(CONTRACT_ID, cid);

        DAppsStore::AddPublisher args;
        Env::DocGet(PUBLISHER, args.m_Publisher);
        args.m_LabelSize = Env::DocGetText(LABEL, args.m_Label, DAppsStore::Publisher::LABEL_MAX_SIZE);

        // TODO check size of label

        Env::GenerateKernel(&cid, args.s_iMethod, &args, sizeof(args), nullptr, 0, nullptr, 0, "add publisher to store", 0);
    }

    void ViewPublishers()
    {
        ContractID cid;
        Env::DocGet(CONTRACT_ID, cid);

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
        }
    }

    void AddDApp()
    {
        ContractID cid;
        Env::DocGet(CONTRACT_ID, cid);

        DAppsStore::AddDApp args;
        Env::DocGet(PUBLISHER, args.m_Publisher);
        Env::DocGetBlobEx(IPFS_ID, &args.m_IPFSId, sizeof(args.m_IPFSId));
        args.m_LabelSize = Env::DocGetText(LABEL, args.m_Label, DAppsStore::DApp::LABEL_MAX_SIZE);

        // TODO check size of label

        Env::GenerateKernel(&cid, args.s_iMethod, &args, sizeof(args), nullptr, 0, nullptr, 0, "add dapp to store", 0);
    }

    void ViewDApps()
    {
        ContractID cid;
        Env::DocGet(CONTRACT_ID, cid);

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
            Env::DocAddNum("id", Utils::FromBE(k0.m_KeyInContract.m_IdInBE));
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
        Env::DocGroup grMethod("create");
    }
    {
        Env::DocGroup grMethod("view");
    }
    {
        Env::DocGroup grMethod("get_pk");
        Env::DocAddText(CONTRACT_ID, "ContractID");
    }
    {
        Env::DocGroup grMethod("add_publisher");
        Env::DocAddText(CONTRACT_ID, "ContractID");
        Env::DocAddText(PUBLISHER, "PubKey");
        Env::DocAddText(LABEL, "string");
    }
    {
        Env::DocGroup grMethod("view_publisher");
        Env::DocAddText(CONTRACT_ID, "ContractID");
    }
    {
        Env::DocGroup grMethod("add_dapp");
        Env::DocAddText(CONTRACT_ID, "ContractID");
        Env::DocAddText(PUBLISHER, "PubKey");
        Env::DocAddText(IPFS_ID, "IPFSCID");
        Env::DocAddText(LABEL, "string");
    }
    {
        Env::DocGroup grMethod("view_dapps");
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

    if (!Env::Strcmp(szAction, "create"))
    {
        manager::Create();
    }
    else if (!Env::Strcmp(szAction, "view"))
    {
        manager::View();
    }
    else if (!Env::Strcmp(szAction, "get_pk"))
    {
        manager::GetPk();
    }
    else if (!Env::Strcmp(szAction, "add_publisher"))
    {
        manager::AddPublisher();
    }
    else if (!Env::Strcmp(szAction, "view_publishers"))
    {
        manager::ViewPublishers();
    }
    else if (!Env::Strcmp(szAction, "add_dapp"))
    {
        manager::AddDApp();
    }
    else if (!Env::Strcmp(szAction, "view_dapps"))
    {
        manager::ViewDApps();
    }
    else
    {
        OnError("invalid Action.");
    }
}