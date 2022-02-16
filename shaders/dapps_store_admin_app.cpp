#include "dapps_store_contract.h"
#include "Shaders/app_common_impl.h"
#include "Shaders/upgradable2/contract.h"
#include "Shaders/upgradable2/app_common_impl.h"

namespace DAppsStore
{
#include "dapps_store_contract_sid.i"
} // namespace DAppsStore

namespace
{
    const char* CID_VERSION = "cidVersion";
    const char* UPGRADE_DELAY = "hUpgradeDelay";
    const char* MIN_APPROVERS = "nMinApprovers";
    const char* PUBKEY = "pubkey";
    const char* CONTRACT_ID = "cid";
    const char* TARGET = "target";

    namespace Actions
    {
        const char* DEPLOY_VERSION = "deploy_version";
        const char* VIEW = "view";
        const char* GET_ADMIN_PK = "get_admin_pk";
        const char* DEPLOY_CONTRACT = "deploy_contract";
        const char* UPGRADE = "upgrade";
        const char* SCHEDULE_UPGRADE = "schedule_upgrade";
    } // namespace Actions

    void OnError(const char* sz)
    {
        Env::DocAddText("error", sz);
    }

    const char ADMIN_SEED[] = "upgr2-dapps-store";

    struct MyAdminKeyID :public Env::KeyID {
        MyAdminKeyID() : Env::KeyID(&ADMIN_SEED, sizeof(ADMIN_SEED)) 
        {}
    };
} // namespace

namespace manager
{
    void DeployVersion()
    {
        Env::GenerateKernel(nullptr, 0, nullptr, 0, nullptr, 0, nullptr, 0, "Deploy DappsStore bytecode", 0);
    }

    void View()
    {
        static const ShaderID s_pSid[] = {
            {0xac,0x50,0x8d,0x67,0x17,0xb0,0x10,0xed,0x13,0x0c,0x18,0xfb,0x24,0xd5,0x7c,0xeb,0x4c,0x6f,0xbd,0xd5,0xfc,0x80,0x3f,0xd2,0x85,0x63,0xd4,0x77,0x2d,0xed,0xc9,0x35},
            DAppsStore::s_SID,
        };

        ContractID pVerCid[_countof(s_pSid)];
        Height pVerDeploy[_countof(s_pSid)];

        ManagerUpgadable2::Walker wlk;
        wlk.m_VerInfo.m_Count = _countof(s_pSid);
        wlk.m_VerInfo.s_pSid = s_pSid;
        wlk.m_VerInfo.m_pCid = pVerCid;
        wlk.m_VerInfo.m_pHeight = pVerDeploy;

        MyAdminKeyID kid;
        wlk.ViewAll(&kid);
    }

    void GetAdminPk()
    {
        MyAdminKeyID keyId;
        PubKey pk;
        keyId.get_Pk(pk);
        Env::DocAddBlob_T(PUBKEY, pk);
    }

    void DeployContract()
    {
        MyAdminKeyID kid;
        PubKey pk;
        kid.get_Pk(pk);

        Upgradable2::Create arg;
        if (!ManagerUpgadable2::FillDeployArgs(arg, &pk))
            return;

        Env::GenerateKernel(nullptr, 0, &arg, sizeof(arg), nullptr, 0, nullptr, 0, "Deploy DappsStore contract", 2005980);
    }

    void ScheduleUpgrade()
    {
        ContractID cid;
        if (!Env::DocGet(CONTRACT_ID, cid))
        {
            OnError("cid should be specified");
            return;
        }

        ContractID cidVersion;
        if (!Env::DocGet(CID_VERSION, cidVersion))
        {
            OnError("cidVersion should be specified");
            return;
        }

        Height hTarget;
        if (!Env::DocGetNum64(TARGET, &hTarget))
        {
            OnError("target should be specified");
            return;
        }

        MyAdminKeyID kid;
        ManagerUpgadable2::MultiSigRitual::Perform_ScheduleUpgrade(cid, kid, cidVersion, hTarget);
    }

    void Upgrade()
    {
        ContractID cid;
        if (!Env::DocGet(CONTRACT_ID, cid))
        {
            OnError("cid should be specified");
            return;
        }

        ManagerUpgadable2::MultiSigRitual::Perform_ExplicitUpgrade(cid);
    }
} // namespace manager

BEAM_EXPORT void Method_0()
{
    Env::DocGroup root("");
    {
        Env::DocGroup grMethod(Actions::DEPLOY_VERSION);
    }
    {
        Env::DocGroup grMethod(Actions::VIEW);
    }
    {
        Env::DocGroup grMethod(Actions::GET_ADMIN_PK);
    }
    {
        Env::DocGroup grMethod(Actions::DEPLOY_CONTRACT);
        Env::DocAddText(CID_VERSION, "ContractID");
        Env::DocAddText(UPGRADE_DELAY, "Height");
        Env::DocAddText(MIN_APPROVERS, "uint32_t");
    }
    {
        Env::DocGroup grMethod(Actions::SCHEDULE_UPGRADE);
        Env::DocAddText(CONTRACT_ID, "ContractID");
        Env::DocAddText(CID_VERSION, "ContractID");
        Env::DocAddText(TARGET, "Height");
    }
    {
        Env::DocGroup grMethod(Actions::UPGRADE);
        Env::DocAddText(CONTRACT_ID, "ContractID");
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

    if (!Env::Strcmp(szAction, Actions::DEPLOY_VERSION))
    {
        manager::DeployVersion();
    }
    else if (!Env::Strcmp(szAction, Actions::VIEW))
    {
        manager::View();
    }
    else if (!Env::Strcmp(szAction, Actions::GET_ADMIN_PK))
    {
        manager::GetAdminPk();
    }
    else if (!Env::Strcmp(szAction, Actions::DEPLOY_CONTRACT))
    {
        manager::DeployContract();
    }
    else if (!Env::Strcmp(szAction, Actions::SCHEDULE_UPGRADE))
    {
        manager::ScheduleUpgrade();
    }
    else if (!Env::Strcmp(szAction, Actions::UPGRADE))
    {
        manager::Upgrade();
    }
    else
    {
        OnError("invalid Action.");
    }
}