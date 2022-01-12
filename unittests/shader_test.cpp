
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

    namespace Env
    {
        void CallFarN(const ContractID& cid, uint32_t iMethod, void* pArgs, uint32_t nArgs, uint8_t bInheritContext);

        template <typename T>
        void CallFar_T(const ContractID& cid, T& args, uint8_t bInheritContext = 0)
        {
            //Convert<true>(args);
            CallFarN(cid, args.s_iMethod, &args, sizeof(args), bInheritContext);
            //Convert<false>(args);
        }
    }

    namespace DappsStore
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
                ByteBuffer m_DappsStore;
            } m_Code;

            ContractID m_cidDappsStore;

            void CallFar(const ContractID& cid, uint32_t iMethod, Wasm::Word pArgs, uint8_t bInheritContext) override
            {

                //if (cid == m_cidPipe)
                //{
                //	TempFrame f(*this, cid);
                //	switch (iMethod)
                //	{
                //	case 0: Shaders::Pipe::Ctor(CastArg<Shaders::Pipe::Create>(pArgs)); return;
                //	//case 2: Shaders::Pipe::Method_2(CastArg<Shaders::Pipe::PushLocal>(pArgs)); return;
                //	}
                //}

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


        void MyProcessor::TestAll()
        {
            AddCode(m_Code.m_DappsStore, "dapps_store_contract.wasm");
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
