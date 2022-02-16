#include "dapps_store_contract.h"

namespace DAppsStore
{
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
        Env::AddSig(args.m_Publisher);

        Publisher::Key key;
        _POD_(key.m_PubKey) = args.m_Publisher;

        Publisher publisher;
        // if the publisher exists then abort
        Env::Halt_if(Env::LoadVar_T(key, publisher));
        Env::Memcpy(publisher.m_Name, args.m_Name, Publisher::NAME_MAX_SIZE);

        Env::SaveVar_T(key, publisher);
    }

    BEAM_EXPORT void Method_4(const Method::UpdatePublisher& args)
    {
        Env::AddSig(args.m_Publisher);

        Publisher::Key key;
        _POD_(key.m_PubKey) = args.m_Publisher;

        Publisher publisher;
        // if the publisher is missing then abort
        Env::Halt_if(!Env::LoadVar_T(key, publisher));
        Env::Memcpy(publisher.m_Name, args.m_Name, Publisher::NAME_MAX_SIZE);

        Env::SaveVar_T(key, publisher);
    }

    BEAM_EXPORT void Method_5(const Method::AddDApp& args)
    {
        Env::AddSig(args.m_Publisher);

        Publisher::Key publisherKey;
        _POD_(publisherKey.m_PubKey) = args.m_Publisher;
        Publisher publisher;
        // if the publisher is missing then abort
        Env::Halt_if(!Env::LoadVar_T(publisherKey, publisher));

        DApp::Key key;
        _POD_(key.m_Id) = args.m_Id;

        DApp dapp;
        // if the dapp exists then abort
        Env::Halt_if(Env::LoadVar_T(key, dapp));

        _POD_(dapp.m_Publisher) = args.m_Publisher;
        Env::Memcpy(dapp.m_IPFSId, args.m_IPFSId, sizeof(dapp.m_IPFSId));
        Env::Memcpy(dapp.m_Name, args.m_Name, DApp::NAME_MAX_SIZE);
        Env::Memcpy(dapp.m_Description, args.m_Description, DApp::DESCRIPTION_MAX_SIZE);
        Env::Memcpy(dapp.m_ApiVersion, args.m_ApiVersion, DApp::API_VERSION_MAX_SIZE);
        Env::Memcpy(dapp.m_MinApiVersion, args.m_MinApiVersion, DApp::API_VERSION_MAX_SIZE);
        _POD_(dapp.m_Version) = args.m_Version;

        Env::SaveVar_T(key, dapp);
    }

    BEAM_EXPORT void Method_6(const Method::UpdateDApp& args)
    {
        DApp::Key key;
        _POD_(key.m_Id) = args.m_Id;

        DApp dapp;
        // if the dapp is missing then abort
        Env::Halt_if(!Env::LoadVar_T(key, dapp));
        Env::AddSig(dapp.m_Publisher);

        Env::Memcpy(dapp.m_IPFSId, args.m_IPFSId, sizeof(dapp.m_IPFSId));
        Env::Memcpy(dapp.m_Name, args.m_Name, DApp::NAME_MAX_SIZE);
        Env::Memcpy(dapp.m_Description, args.m_Description, DApp::DESCRIPTION_MAX_SIZE);
        Env::Memcpy(dapp.m_ApiVersion, args.m_ApiVersion, DApp::API_VERSION_MAX_SIZE);
        Env::Memcpy(dapp.m_MinApiVersion, args.m_MinApiVersion, DApp::API_VERSION_MAX_SIZE);
        _POD_(dapp.m_Version) = args.m_Version;

        Env::SaveVar_T(key, dapp);
    }

    BEAM_EXPORT void Method_7(const Method::DeleteDApp& args)
    {
        DApp::Key key;
        _POD_(key.m_Id) = args.m_Id;

        DApp dapp;
        // if the dapp is missing then abort
        Env::Halt_if(!Env::LoadVar_T(key, dapp));
        Env::AddSig(dapp.m_Publisher);

        Env::DelVar_T(key);
    }
} // namespace DAppsStore