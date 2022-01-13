#include "dapps_store_contract.h"

BEAM_EXPORT void Ctor(const DAppsStore::Create& /*args*/)
{
}

BEAM_EXPORT void Dtor(void*)
{
}

BEAM_EXPORT void Method_2(const DAppsStore::AddDApp& args)
{
    DAppsStore::DApp dapp;

    _POD_(dapp.m_IPFSId) = args.m_IPFSId;
    _POD_(dapp.m_Publisher) = args.m_Publisher;

    uint64_t id = 0;

    Env::LoadVar_T(DAppsStore::APPS_AMOUNT_KEY, id);

    DAppsStore::DApp::Key key;
    key.m_MsgId_BE = Utils::FromBE(++id);

    Env::SaveVar_T(key, dapp);
    Env::SaveVar_T(DAppsStore::APPS_AMOUNT_KEY, id);
}

BEAM_EXPORT void Method_3(const DAppsStore::UpdateDApp& /*args*/)
{
}

BEAM_EXPORT void Method_4(const DAppsStore::DeleteDApp& /*args*/)
{
}