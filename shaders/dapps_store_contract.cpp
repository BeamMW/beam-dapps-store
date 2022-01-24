#include "dapps_store_contract.h"

namespace DAppsStore
{
    BEAM_EXPORT void Ctor(const Method::Create& /*args*/)
    {
    }

    BEAM_EXPORT void Dtor(void*)
    {
    }

    BEAM_EXPORT void Method_2(const Method::AddPublisher& args)
    {
        Env::Halt_if(args.m_LabelSize > Publisher::LABEL_MAX_SIZE);

        Env::AddSig(args.m_Publisher);

        Publisher::Key key;
        _POD_(key.m_PubKey) = args.m_Publisher;

        Publisher publisher;

        Env::Memset(publisher.m_Label, 0, Publisher::LABEL_MAX_SIZE + 1);
        Env::Memcpy(publisher.m_Label, args.m_Label, args.m_LabelSize);

        Env::SaveVar_T(key, publisher);
    }

    BEAM_EXPORT void Method_3(const Method::UpdatePublisher& args)
    {
        Env::Halt_if(args.m_LabelSize > Publisher::LABEL_MAX_SIZE);

        Env::AddSig(args.m_Publisher);

        Publisher::Key key;
        _POD_(key.m_PubKey) = args.m_Publisher;

        Publisher publisher;
        Env::Halt_if(!Env::LoadVar_T(key, publisher));
        Env::Memset(publisher.m_Label, 0, Publisher::LABEL_MAX_SIZE + 1);
        Env::Memcpy(publisher.m_Label, args.m_Label, args.m_LabelSize);

        Env::SaveVar_T(key, publisher);
    }

    BEAM_EXPORT void Method_4(const Method::DeletePublisher& /*args*/)
    {}

    BEAM_EXPORT void Method_5(const Method::AddDApp& args)
    {
        Env::Halt_if(args.m_LabelSize > DApp::LABEL_MAX_SIZE);

        Env::AddSig(args.m_Publisher);

        // check publisher
        Publisher::Key publisherKey;
        _POD_(publisherKey.m_PubKey) = args.m_Publisher;
        Publisher publisher;
        Env::Halt_if(!Env::LoadVar_T(publisherKey, publisher));

        DApp dapp;
        Env::Memcpy(dapp.m_IPFSId, args.m_IPFSId, sizeof(dapp.m_IPFSId));
        _POD_(dapp.m_Publisher) = args.m_Publisher;
        Env::Memset(dapp.m_Label, 0, DApp::LABEL_MAX_SIZE + 1);
        Env::Memcpy(dapp.m_Label, args.m_Label, args.m_LabelSize);

        uint64_t id = 0;
        Env::LoadVar_T(Tags::STATE, id);

        DApp::Key key;
        key.m_IdInBE = Utils::FromBE(++id);

        Env::SaveVar_T(key, dapp);
        Env::SaveVar_T(Tags::STATE, id);
    }

    BEAM_EXPORT void Method_6(const Method::UpdateDApp& args)
    {
        Env::Halt_if(args.m_LabelSize > DApp::LABEL_MAX_SIZE);

        DApp::Key key;
        key.m_IdInBE = Utils::FromBE(args.m_Id);

        DApp dapp;
        Env::Halt_if(!Env::LoadVar_T(key, dapp));

        Env::AddSig(dapp.m_Publisher);

        Env::Memcpy(dapp.m_IPFSId, args.m_IPFSId, sizeof(dapp.m_IPFSId));
        Env::Memset(dapp.m_Label, 0, DApp::LABEL_MAX_SIZE + 1);
        Env::Memcpy(dapp.m_Label, args.m_Label, args.m_LabelSize);

        Env::SaveVar_T(key, dapp);
    }

    BEAM_EXPORT void Method_7(const Method::DeleteDApp& args)
    {
        DApp::Key key;
        key.m_IdInBE = Utils::FromBE(args.m_Id);

        DApp dapp;
        Env::Halt_if(!Env::LoadVar_T(key, dapp));

        Env::AddSig(dapp.m_Publisher);
        Env::DelVar_T(key);
    }
} // namespace DAppsStore