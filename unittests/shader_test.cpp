
#define HOST_BUILD

#include "../beam/bvm/bvm2.h"
#include "../beam/bvm/bvm2_impl.h"
#include <cmath>

namespace Shaders
{
#ifdef _MSC_VER
#    pragma warning (disable : 4200 4702) // unreachable code
#endif // _MSC_VER

#define BEAM_EXPORT

#include "Shaders/common.h"

#include "../shaders/dapps_store_contract.h"

    template <bool bToShader> void Convert(DAppsStore::Method::Create& x)
    {
        //ConvertOrd<bToShader>(x.m_MetadataSize);
    }

    template <bool bToShader> void Convert(DAppsStore::Method::AddPublisher& x)
    {
        ConvertOrd<bToShader>(x.m_NameSize);
        ConvertOrd<bToShader>(x.m_ShortTitleSize);
        ConvertOrd<bToShader>(x.m_AboutMeSize);
        ConvertOrd<bToShader>(x.m_WebsiteSize);
        ConvertOrd<bToShader>(x.m_TwitterSize);
        ConvertOrd<bToShader>(x.m_LinkedinSize);
        ConvertOrd<bToShader>(x.m_InstagramSize);
        ConvertOrd<bToShader>(x.m_TelegramSize);
        ConvertOrd<bToShader>(x.m_DiscordSize);
    }

    template <bool bToShader> void Convert(DAppsStore::Method::UpdatePublisher& x)
    {
        ConvertOrd<bToShader>(x.m_NameSize);
        ConvertOrd<bToShader>(x.m_ShortTitleSize);
        ConvertOrd<bToShader>(x.m_AboutMeSize);
        ConvertOrd<bToShader>(x.m_WebsiteSize);
        ConvertOrd<bToShader>(x.m_TwitterSize);
        ConvertOrd<bToShader>(x.m_LinkedinSize);
        ConvertOrd<bToShader>(x.m_InstagramSize);
        ConvertOrd<bToShader>(x.m_TelegramSize);
        ConvertOrd<bToShader>(x.m_DiscordSize);
    }

    template <bool bToShader> void Convert(DAppsStore::Method::AddDApp& x)
    {
        ConvertOrd<bToShader>(x.m_NameSize);
        ConvertOrd<bToShader>(x.m_DescriptionSize);
        ConvertOrd<bToShader>(x.m_ApiVersionSize);
        ConvertOrd<bToShader>(x.m_MinApiVersionSize);
        ConvertOrd<bToShader>(x.m_IconSize);

        ConvertOrd<bToShader>(x.m_Version.m_Build);
        ConvertOrd<bToShader>(x.m_Version.m_Major);
        ConvertOrd<bToShader>(x.m_Version.m_Minor);
        ConvertOrd<bToShader>(x.m_Version.m_Release);

        ConvertOrd<bToShader>(x.m_Category);
    }

    template <bool bToShader> void Convert(DAppsStore::Method::UpdateDApp& x)
    {
        ConvertOrd<bToShader>(x.m_NameSize);
        ConvertOrd<bToShader>(x.m_DescriptionSize);
        ConvertOrd<bToShader>(x.m_ApiVersionSize);
        ConvertOrd<bToShader>(x.m_MinApiVersionSize);
        ConvertOrd<bToShader>(x.m_IconSize);

        ConvertOrd<bToShader>(x.m_Version.m_Build);
        ConvertOrd<bToShader>(x.m_Version.m_Major);
        ConvertOrd<bToShader>(x.m_Version.m_Minor);
        ConvertOrd<bToShader>(x.m_Version.m_Release);

        ConvertOrd<bToShader>(x.m_Category);
    }

    namespace Env
    {
        void CallFarN(const ContractID& cid, uint32_t iMethod, void* pArgs, uint32_t nArgs, uint8_t bInheritContext);

        template <typename T>
        void CallFar_T(const ContractID& cid, T& args, uint8_t bInheritContext = 0)
        {
            Convert<true>(args);
            CallFarN(cid, args.s_iMethod, &args, sizeof(args), bInheritContext);
            Convert<false>(args);
        }
    }

    namespace DAppsStore
    {
#include "../shaders/dapps_store_contract_sid.i"
#include "../shaders/dapps_store_contract.cpp"
    } // namespace Token

#ifdef _MSC_VER
#    pragma warning (default : 4200 4702)
#endif // _MSC_VER
} // namespace Shaders

int g_TestsFailed = 0;

void TestFailed(const char* szExpr, uint32_t nLine)
{
    printf("Test failed! Line=%u, Expression: %s\n", nLine, szExpr);
    g_TestsFailed++;
    fflush(stdout);
}

#define verify_test(x) \
    do { \
        if (!(x)) \
            TestFailed(#x, __LINE__); \
    } while (false)

#define fail_test(msg) TestFailed(msg, __LINE__)

using namespace beam;
using namespace beam::bvm2;

#include "unittest/contract_test_processor.h"

namespace beam
{
    namespace bvm2
    {
        struct MyProcessor
            : public ContractTestProcessor
        {
            struct Code
            {
                ByteBuffer m_DAppsStore;
            } m_Code;

            ContractID m_cidDAppsStore;

            void CallFar(const ContractID& cid, uint32_t iMethod, Wasm::Word pArgs, uint8_t bInheritContext) override
            {

                if (cid == m_cidDAppsStore)
                {
                    TempFrame f(*this, cid);
                    switch (iMethod)
                    {
                    //case 0: Shaders::Pipe::Ctor(CastArg<Shaders::Pipe::Create>(pArgs)); return;
                    //case 3: Shaders::DAppsStore::DAppsStore::Method_3(CastArg<Shaders::DAppsStore::Method::AddPublisher>(pArgs)); return;
                    //case 6: Shaders::DAppsStore::DAppsStore::Method_6(CastArg<Shaders::DAppsStore::Method::UpdateDApp>(pArgs)); return;
                    }
                }

                ProcessorContract::CallFar(cid, iMethod, pArgs, bInheritContext);
            }

            template<class Key, class Value>
            Value ReadValue(ContractID cid, const Key& key, uint8_t tag = VarKey::Tag::Internal)
            {
                VarKey varKey;
                varKey.Set(cid);
                varKey.Append(tag, Blob(&key, sizeof(Key)));

                Blob buffer;

                LoadVar(Blob(varKey.m_p, varKey.m_Size), buffer);

                auto result = static_cast<const Value*>(buffer.p);

                return *result;
            }

            void TestContractCreation();
            void TestAddPublisher();
            void TestAddPublisherAgain();
            void TestAddPublisherBigData();
            void TestUpdatePublisher();
            void TestUpdateUnknownPublisher();
            void TestAddDApp();
            void TestAddDAppAgain();
            void TestAddDAppUnknownPublisher();
            void TestAddDAppBigData();
            void TestUpdateDApp();
            void TestUpdateUnknownDApp();
            void TestUpdateDAppWithWrongVersion();

            void TestAll();
        };

        template <>
        struct MyProcessor::Converter<beam::Zero_>
            :public Blob
        {
            Converter(beam::Zero_&)
            {
                p = nullptr;
                n = 0;
            }
        };

        void MyProcessor::TestContractCreation()
        {
            Shaders::DAppsStore::Method::Create args;

            verify_test(ContractCreate_T(m_cidDAppsStore, m_Code.m_DAppsStore, args));
        }

        struct AddPublisher : Shaders::DAppsStore::Method::AddPublisher
        {
            char m_Name[Shaders::DAppsStore::Publisher::NAME_MAX_SIZE];
        };

        struct UpdatePublisher : Shaders::DAppsStore::Method::UpdatePublisher
        {
            char m_Name[Shaders::DAppsStore::Publisher::NAME_MAX_SIZE];
        };

        template<typename PublisherType>
        PublisherType GeneratePublisherArgs(uint32_t nameSize = 10)
        {
            PublisherType args;

            args.m_NameSize = nameSize;
            args.m_ShortTitleSize = 0;
            args.m_AboutMeSize = 0;
            args.m_WebsiteSize = 0;
            args.m_TwitterSize = 0;
            args.m_LinkedinSize = 0;
            args.m_InstagramSize = 0;
            args.m_TelegramSize = 0;
            args.m_DiscordSize = 0;
            return args;
        }

        void MyProcessor::TestAddPublisher()
        {
            auto args = GeneratePublisherArgs<AddPublisher>();

            args.m_Publisher.m_X = 1u;

            verify_test(RunGuarded_T(m_cidDAppsStore, args.METHOD_ID, args));
        }

        void MyProcessor::TestAddPublisherAgain()
        {
            auto args = GeneratePublisherArgs<AddPublisher>();

            args.m_Publisher.m_X = 1u;

            verify_test(!RunGuarded_T(m_cidDAppsStore, args.METHOD_ID, args));
        }

        void MyProcessor::TestAddPublisherBigData()
        {
            {
                auto args = GeneratePublisherArgs<AddPublisher>();

                args.m_Publisher.m_X = 2u;
                args.m_NameSize = Shaders::DAppsStore::Publisher::NAME_MAX_SIZE + 1;

                verify_test(!RunGuarded_T(m_cidDAppsStore, args.METHOD_ID, args));
            }

            {
                auto args = GeneratePublisherArgs<AddPublisher>();

                args.m_Publisher.m_X = 2u;
                args.m_ShortTitleSize = Shaders::DAppsStore::Publisher::SHORT_TITLE_MAX_SIZE + 1;

                verify_test(!RunGuarded_T(m_cidDAppsStore, args.METHOD_ID, args));
            }

            {
                auto args = GeneratePublisherArgs<AddPublisher>();

                args.m_Publisher.m_X = 2u;
                args.m_AboutMeSize = Shaders::DAppsStore::Publisher::ABOUT_ME_MAX_SIZE + 1;

                verify_test(!RunGuarded_T(m_cidDAppsStore, args.METHOD_ID, args));
            }

            {
                auto args = GeneratePublisherArgs<AddPublisher>();

                args.m_Publisher.m_X = 2u;
                args.m_WebsiteSize = Shaders::DAppsStore::Publisher::WEBSITE_MAX_SIZE + 1;

                verify_test(!RunGuarded_T(m_cidDAppsStore, args.METHOD_ID, args));
            }

            {
                auto args = GeneratePublisherArgs<AddPublisher>();

                args.m_Publisher.m_X = 2u;
                args.m_TwitterSize = Shaders::DAppsStore::Publisher::SOCIAL_NICK_MAX_SIZE + 1;

                verify_test(!RunGuarded_T(m_cidDAppsStore, args.METHOD_ID, args));
            }

            {
                auto args = GeneratePublisherArgs<AddPublisher>();

                args.m_Publisher.m_X = 2u;
                args.m_LinkedinSize = Shaders::DAppsStore::Publisher::SOCIAL_NICK_MAX_SIZE + 1;

                verify_test(!RunGuarded_T(m_cidDAppsStore, args.METHOD_ID, args));
            }

            {
                auto args = GeneratePublisherArgs<AddPublisher>();

                args.m_Publisher.m_X = 2u;
                args.m_InstagramSize = Shaders::DAppsStore::Publisher::SOCIAL_NICK_MAX_SIZE + 1;

                verify_test(!RunGuarded_T(m_cidDAppsStore, args.METHOD_ID, args));
            }

            {
                auto args = GeneratePublisherArgs<AddPublisher>();

                args.m_Publisher.m_X = 2u;
                args.m_TelegramSize = Shaders::DAppsStore::Publisher::SOCIAL_NICK_MAX_SIZE + 1;

                verify_test(!RunGuarded_T(m_cidDAppsStore, args.METHOD_ID, args));
            }

            {
                auto args = GeneratePublisherArgs<AddPublisher>();

                args.m_Publisher.m_X = 2u;
                args.m_DiscordSize = Shaders::DAppsStore::Publisher::SOCIAL_NICK_MAX_SIZE + 1;

                verify_test(!RunGuarded_T(m_cidDAppsStore, args.METHOD_ID, args));
            }
        }

        void MyProcessor::TestUpdatePublisher()
        {
            auto args = GeneratePublisherArgs<UpdatePublisher>();

            args.m_Publisher.m_X = 1u;

            verify_test(RunGuarded_T(m_cidDAppsStore, args.METHOD_ID, args));
        }

        void MyProcessor::TestUpdateUnknownPublisher()
        {
            auto args = GeneratePublisherArgs<UpdatePublisher>();

            args.m_Publisher.m_X = 2u;

            verify_test(!RunGuarded_T(m_cidDAppsStore, args.METHOD_ID, args));
        }

        struct AddDApp : Shaders::DAppsStore::Method::AddDApp
        {
            char m_Name[Shaders::DAppsStore::DApp::NAME_MAX_SIZE];
        };

        struct UpdateDApp : Shaders::DAppsStore::Method::UpdateDApp
        {
            char m_Name[Shaders::DAppsStore::DApp::NAME_MAX_SIZE];
        };

        template<typename DAppType>
        DAppType GenerateDAppArgs(uint32_t nameSize = 10)
        {
            DAppType args;

            args.m_NameSize = nameSize;
            args.m_DescriptionSize = 0;
            args.m_ApiVersionSize = 0;
            args.m_MinApiVersionSize = 0;
            args.m_IconSize = 0;

            args.m_Version.m_Major = 1;
            args.m_Version.m_Minor = 0;
            args.m_Version.m_Release = 0;
            args.m_Version.m_Build = 0;

            args.m_Category = 0;

            return args;
        }

        void MyProcessor::TestAddDApp()
        {
            auto args = GenerateDAppArgs<AddDApp>();

            memset(args.m_Id.m_pData, 1, args.m_Id.nBytes);
            args.m_Publisher.m_X = 1u;
            verify_test(RunGuarded_T(m_cidDAppsStore, args.METHOD_ID, args));
        }

        void MyProcessor::TestAddDAppAgain()
        {
            auto args = GenerateDAppArgs<AddDApp>();

            memset(args.m_Id.m_pData, 1, args.m_Id.nBytes);
            args.m_Publisher.m_X = 1u;
            verify_test(!RunGuarded_T(m_cidDAppsStore, args.METHOD_ID, args));
        }

        void MyProcessor::TestAddDAppUnknownPublisher()
        {
            auto args = GenerateDAppArgs<AddDApp>();

            memset(args.m_Id.m_pData, 2, args.m_Id.nBytes);
            args.m_Publisher.m_X = 2u;
            verify_test(!RunGuarded_T(m_cidDAppsStore, args.METHOD_ID, args));
        }

        void MyProcessor::TestAddDAppBigData()
        {
            {
                auto args = GenerateDAppArgs<AddDApp>();

                memset(args.m_Id.m_pData, 2, args.m_Id.nBytes);
                args.m_Publisher.m_X = 1u;
                args.m_ApiVersionSize = Shaders::DAppsStore::DApp::API_VERSION_MAX_SIZE + 1;
                verify_test(!RunGuarded_T(m_cidDAppsStore, args.METHOD_ID, args));
            }
            {
                auto args = GenerateDAppArgs<AddDApp>();

                memset(args.m_Id.m_pData, 2, args.m_Id.nBytes);
                args.m_Publisher.m_X = 1u;
                args.m_DescriptionSize = Shaders::DAppsStore::DApp::DESCRIPTION_MAX_SIZE + 1;
                verify_test(!RunGuarded_T(m_cidDAppsStore, args.METHOD_ID, args));
            }
            {
                auto args = GenerateDAppArgs<AddDApp>();

                memset(args.m_Id.m_pData, 2, args.m_Id.nBytes);
                args.m_Publisher.m_X = 1u;
                args.m_IconSize = Shaders::DAppsStore::DApp::ICON_MAX_SIZE + 1;
                verify_test(!RunGuarded_T(m_cidDAppsStore, args.METHOD_ID, args));
            }
            {
                auto args = GenerateDAppArgs<AddDApp>();

                memset(args.m_Id.m_pData, 2, args.m_Id.nBytes);
                args.m_Publisher.m_X = 1u;
                args.m_MinApiVersionSize = Shaders::DAppsStore::DApp::API_VERSION_MAX_SIZE + 1;
                verify_test(!RunGuarded_T(m_cidDAppsStore, args.METHOD_ID, args));
            }
            {
                auto args = GenerateDAppArgs<AddDApp>();

                memset(args.m_Id.m_pData, 2, args.m_Id.nBytes);
                args.m_Publisher.m_X = 1u;
                args.m_NameSize = Shaders::DAppsStore::DApp::NAME_MAX_SIZE + 1;
                verify_test(!RunGuarded_T(m_cidDAppsStore, args.METHOD_ID, args));
            }
        }

        void MyProcessor::TestUpdateDApp()
        {
            auto args = GenerateDAppArgs<UpdateDApp>();

            memset(args.m_Id.m_pData, 1, args.m_Id.nBytes);
            args.m_Version.m_Major = 2;
            verify_test(RunGuarded_T(m_cidDAppsStore, args.METHOD_ID, args));
        }

        void MyProcessor::TestUpdateUnknownDApp()
        {
            auto args = GenerateDAppArgs<UpdateDApp>();

            memset(args.m_Id.m_pData, 2, args.m_Id.nBytes);
            args.m_Version.m_Major = 2;
            verify_test(!RunGuarded_T(m_cidDAppsStore, args.METHOD_ID, args));
        }

        void MyProcessor::TestUpdateDAppWithWrongVersion()
        {
            auto args = GenerateDAppArgs<UpdateDApp>();

            memset(args.m_Id.m_pData, 1, args.m_Id.nBytes);
            args.m_Version.m_Major = 0;
            verify_test(!RunGuarded_T(m_cidDAppsStore, args.METHOD_ID, args));
        }

        void MyProcessor::TestAll()
        {
            AddCode(m_Code.m_DAppsStore, "dapps_store_contract.wasm");

            TestContractCreation();

            TestAddPublisher();
            TestAddPublisherAgain();
            TestAddPublisherBigData();
            TestUpdatePublisher();
            TestUpdateUnknownPublisher();

            TestAddDApp();
            TestAddDAppAgain();
            TestAddDAppUnknownPublisher();
            TestAddDAppBigData();
            TestUpdateDApp();
            TestUpdateUnknownDApp();
            TestUpdateDAppWithWrongVersion();
        }

        struct CidTxt
        {
            char m_szBuf[Shaders::ContractID::nBytes * 5];

            void Set(const Shaders::ContractID& x)
            {
                char* p = m_szBuf;
                for (uint32_t i = 0; i < x.nBytes; i++)
                {
                    if (i)
                        *p++ = ',';

                    *p++ = '0';
                    *p++ = 'x';

                    uintBigImpl::_Print(x.m_pData + i, 1, p);
                    p += 2;
                }

                assert(p - m_szBuf < (long int)_countof(m_szBuf));
                *p = 0;
            }
        };

        static void VerifyId(const ContractID& cidExp, const ContractID& cid, const char* szName)
        {
            if (cidExp != cid)
            {
                CidTxt ct;
                ct.Set(cid);

                printf("Incorrect %s. Actual value: %s\n", szName, ct.m_szBuf);
                g_TestsFailed++;
                fflush(stdout);
            }
        }

#define VERIFY_ID(exp, actual) VerifyId(exp, actual, #exp)
    } // namespace bvm2
} // namespace beam

void Shaders::Env::CallFarN(const ContractID& cid, uint32_t iMethod, void* pArgs, uint32_t nArgs, uint8_t bInheritContext)
{
    Cast::Up<beam::bvm2::MyProcessor>(g_pEnv)->CallFarN(cid, iMethod, pArgs, nArgs, bInheritContext);
}

int main()
{
    try
    {
        ECC::PseudoRandomGenerator prg;
        ECC::PseudoRandomGenerator::Scope scope(&prg);

        MyProcessor proc;

        proc.TestAll();
    }
    catch (const std::exception& ex)
    {
        printf("Expression: %s\n", ex.what());
        g_TestsFailed++;
    }

    return g_TestsFailed ? -1 : 0;
}
