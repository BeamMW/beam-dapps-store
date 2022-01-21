#include "dapps_store_contract.h"

BEAM_EXPORT void Ctor(const DAppsStore::Create& /*args*/)
{
}

BEAM_EXPORT void Dtor(void*)
{
}

BEAM_EXPORT void Method_2(const DAppsStore::AddPublisher& args)
{
    Env::Halt_if(args.m_LabelSize >= DAppsStore::Publisher::LABEL_MAX_SIZE);

    DAppsStore::Publisher::Key key;
    _POD_(key.m_PubKey) = args.m_Publisher;

    DAppsStore::Publisher publisher;

    Env::Memset(publisher.m_Label, 0, DAppsStore::Publisher::LABEL_MAX_SIZE);
    Env::Memcpy(publisher.m_Label, args.m_Label, args.m_LabelSize);

    Env::SaveVar_T(key, publisher);
}

BEAM_EXPORT void Method_3(const DAppsStore::UpdatePublisher& /*args*/)
{}

BEAM_EXPORT void Method_4(const DAppsStore::DeletePublisher& /*args*/)
{}

BEAM_EXPORT void Method_5(const DAppsStore::AddDApp& args)
{
    Env::Halt_if(args.m_LabelSize >= DAppsStore::DApp::LABEL_MAX_SIZE);

    DAppsStore::DApp dapp;
    _POD_(dapp.m_IPFSId) = args.m_IPFSId;
    _POD_(dapp.m_Publisher) = args.m_Publisher;
    Env::Memset(dapp.m_Label, 0, DAppsStore::DApp::LABEL_MAX_SIZE);
    Env::Memcpy(dapp.m_Label, args.m_Label, args.m_LabelSize);

    uint64_t id = 0;
    Env::LoadVar_T(DAppsStore::Tags::STATE, id);

    DAppsStore::DApp::Key key;
    key.m_IdInBE = Utils::FromBE(++id);

    Env::SaveVar_T(key, dapp);
    Env::SaveVar_T(DAppsStore::Tags::STATE, id);
}

BEAM_EXPORT void Method_6(const DAppsStore::UpdateDApp& /*args*/)
{
}

BEAM_EXPORT void Method_7(const DAppsStore::DeleteDApp& /*args*/)
{
}