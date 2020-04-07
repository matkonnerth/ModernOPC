#include <gtest/gtest.h>
#include <memory>
#include <opc/Server.h>
#include <opc/Variant.h>
#include <opc/methods/MethodWrapper.h>
#include <vector>

using opc::Variant;
using namespace std::string_literals;

TEST(Methods, std_function)
{
    std::function test = []() { return "hello"s; };
    opc::Server s;
    s.addMethod(opc::NodeId(0, 85), opc::NodeId(1, 2222), "open", test);

    auto server = s.getUAServer();
    UA_StatusCode retval = UA_Server_run_startup(server);

    UA_CallMethodRequest req;
    UA_CallMethodRequest_init(&req);
    req.objectId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    req.methodId = UA_NODEID_NUMERIC(1, 2222);
    req.inputArguments = nullptr;
    req.inputArgumentsSize = 0;

    UA_CallMethodResult res = UA_Server_call(server, &req);
    ASSERT_EQ(res.statusCode, UA_STATUSCODE_GOOD);
    ASSERT_EQ(res.outputArgumentsSize, 1);
    ASSERT_EQ(res.outputArguments->type, &UA_TYPES[UA_TYPES_STRING]);
    Variant var{res.outputArguments, true};
    ASSERT_TRUE(var.is_a<std::string>());
    ASSERT_EQ(var.get<std::string>(), "hello"s);

    UA_Server_run_shutdown(server);
}

TEST(Methods, memberFunction)
{

    struct Callable
    {
        int32_t run(int a) { return a + 12; }
    };
    Callable c;
    opc::Server s;
    std::function fn = [&](int32_t a) { return c.run(a); };
    s.addMethod(opc::NodeId(0, 85), opc::NodeId(1, 2222), "open", fn);

    std::function<int32_t(Callable *, int)> memberFn = &Callable::run;
    s.addMethod(opc::NodeId(0, 85), opc::NodeId(1, 2223), "run",
                &Callable::run);

    auto server = s.getUAServer();
    UA_StatusCode retval = UA_Server_run_startup(server);

    UA_Variant var;
    UA_Variant_init(&var);
    UA_Int32 i = 1230;
    UA_Variant_setScalar(&var, &i, &UA_TYPES[UA_TYPES_INT32]);

    UA_CallMethodRequest req;
    UA_CallMethodRequest_init(&req);
    req.objectId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    req.methodId = UA_NODEID_NUMERIC(1, 2222);
    req.inputArguments = &var;
    req.inputArgumentsSize = 1;

    UA_CallMethodResult res = UA_Server_call(server, &req);
    ASSERT_EQ(res.statusCode, UA_STATUSCODE_GOOD);
    ASSERT_EQ(res.outputArgumentsSize, 1);
    Variant v{res.outputArguments, true};
    ASSERT_TRUE(v.is_a<int32_t>());
    ASSERT_EQ(v.get<int32_t>(), 1242);

    UA_Server_run_shutdown(server);
}

void freeVoidVoid() {}

TEST(Methods, freeVoidVoid)
{
    opc::Server s;
    s.addMethod(opc::NodeId(0, 85), opc::NodeId(1, 2222), "open",
                &freeVoidVoid);

    auto server = s.getUAServer();
    UA_StatusCode retval = UA_Server_run_startup(server);

    UA_CallMethodRequest req;
    UA_CallMethodRequest_init(&req);
    req.objectId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    req.methodId = UA_NODEID_NUMERIC(1, 2222);
    req.inputArguments = nullptr;
    req.inputArgumentsSize = 0;

    UA_CallMethodResult res = UA_Server_call(server, &req);
    ASSERT_EQ(res.statusCode, UA_STATUSCODE_GOOD);
    ASSERT_EQ(res.outputArgumentsSize, 0);

    UA_Server_run_shutdown(server);
}

void freeVoidConstStdString(const std::string &test) { (void)test; }

TEST(Methods, freeVoidConstString)
{
    opc::Server s;
    s.addMethod(opc::NodeId(0, 85), opc::NodeId(1, 2222), "open",
                &freeVoidConstStdString);
}

std::vector<std::string> freeVectorOfString()
{
    return std::vector{"hello"s, "world"s};
}

TEST(Methods, freeVectorString)
{
    opc::Server s;
    s.addMethod(opc::NodeId(0, 85), opc::NodeId(1, 2222), "open",
                &freeVectorOfString);

    auto server = s.getUAServer();
    UA_StatusCode retval = UA_Server_run_startup(server);

    UA_CallMethodRequest req;
    UA_CallMethodRequest_init(&req);
    req.objectId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    req.methodId = UA_NODEID_NUMERIC(1, 2222);
    req.inputArguments = nullptr;
    req.inputArgumentsSize = 0;

    UA_CallMethodResult res = UA_Server_call(server, &req);
    ASSERT_EQ(res.statusCode, UA_STATUSCODE_GOOD);
    ASSERT_EQ(res.outputArgumentsSize, 1);
    Variant v{res.outputArguments, true};
    ASSERT_TRUE(v.is_a<std::vector<std::string>>());
    ASSERT_EQ(v.get<std::vector<std::string>>().size(), 2);

    UA_Server_run_shutdown(server);
}

int32_t testOrder(int a0, int a1, int a2, int a3, int a4, int a5, int a6,
                  int a7, int a8, int a9, int a10, int a11, int a12, int a13,
                  int a14, int a15, int a16, int a17, int a18, int a19, int a20,
                  int a21, int a22, int a23, int a24, int a25, int a26, int a27,
                  int a28, int a29, int a30, int a31, int a32, int a33, int a34,
                  int a35, int a36, int a37, int a38, int a39, int a40, int a41,
                  int a42, int a43, int a44, int a45, int a46, int a47, int a48,
                  int a49, int a50, int a51, int a52, int a53, int a54, int a55,
                  int a56, int a57, int a58, int a59, int a60, int a61, int a62,
                  int a63, int a64, int a65, int a66, int a67, int a68, int a69,
                  int a70, int a71, int a72, int a73, int a74, int a75, int a76,
                  int a77, int a78, int a79, int a80, int a81, int a82, int a83,
                  int a84, int a85, int a86, int a87, int a88, int a89, int a90,
                  int a91, int a92, int a93, int a94, int a95, int a96, int a97,
                  int a98, int a99)
{
    return a99;
}

TEST(Methods, testOrder)
{
    opc::Server s;
    s.addMethod(opc::NodeId(0, 85), opc::NodeId(1, 2222), "open", &testOrder);

    auto server = s.getUAServer();
    UA_StatusCode retval = UA_Server_run_startup(server);

    const auto cnt = 100;
    UA_Variant var[cnt];
    for (auto i = 0; i < cnt; i++)
    {
        UA_Variant_setScalarCopy(&var[i], &i, &UA_TYPES[UA_TYPES_INT32]);
    }

    UA_CallMethodRequest req;
    UA_CallMethodRequest_init(&req);
    req.objectId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    req.methodId = UA_NODEID_NUMERIC(1, 2222);
    req.inputArguments = var;
    req.inputArgumentsSize = cnt;

    UA_CallMethodResult res = UA_Server_call(server, &req);
    ASSERT_EQ(res.statusCode, UA_STATUSCODE_GOOD);
    ASSERT_EQ(res.outputArgumentsSize, 1);
    Variant v{res.outputArguments, true};
    ASSERT_TRUE(v.is_a<int32_t>());
    ASSERT_EQ(v.get<int32_t>(), cnt - 1);

    for (auto i = 0; i < cnt; i++)
    {
        UA_Variant_clear(&var[i]);
    }

    UA_Server_run_shutdown(server);
}

TEST(Methods, opc_Call_int_int)
{
    std::function<int32_t(int, int, int)> fn = [](int a, int b, int c) {
        return a + b + c;
    };

    std::unique_ptr<opc::ICallable> c =
        std::make_unique<opc::Call<decltype(fn)>>(opc::Call<decltype(fn)>{fn});

    std::vector<Variant> in{};
    in.emplace_back(Variant(23));
    in.emplace_back(Variant(2));
    in.emplace_back(Variant(2));
    std::vector<Variant> out{};
    c->call(nullptr, in, out);
    ASSERT_EQ(out.size(), 1);
    ASSERT_EQ(out[0].get<int32_t>(), 27);
}

TEST(Methods, opc_Call_void_int)
{
    std::function<void(int, int, int)> fn = [](int a, int b, int c) {

    };

    std::unique_ptr<opc::ICallable> c =
        std::make_unique<opc::Call<decltype(fn)>>(opc::Call<decltype(fn)>{fn});

    std::vector<Variant> in{};
    in.emplace_back(Variant(23));
    in.emplace_back(Variant(2));
    in.emplace_back(Variant(2));
    std::vector<Variant> out{};
    c->call(nullptr, in, out);
    ASSERT_EQ(out.size(), 0);
}



TEST(Methods, opc_Call_int_ptr_int)
{
    struct Callable23
    {
        int32_t calc(int a, int b, int c) { return a + b + c; }
    };

    Callable23 callable1;

    std::function<int32_t(Callable23*, int, int, int)> fn = &Callable23::calc;

    std::unique_ptr<opc::ICallable> c =
        std::make_unique<opc::Call<decltype(fn)>>(opc::Call<decltype(fn)>{fn});

    std::vector<Variant> in{};
    in.emplace_back(Variant(23));
    in.emplace_back(Variant(2));
    in.emplace_back(Variant(2));
    std::vector<Variant> out{};
    c->call(&callable1, in, out);
    ASSERT_EQ(out.size(), 1);
    ASSERT_EQ(out[0].get<int32_t>(), 27);
}

TEST(Methods, opc_Call_void_ptr_int)
{
    struct Callable23
    {
        void calc(int a, int b, int c) { }
    };

    Callable23 callable1;

    std::function<void(Callable23 *, int, int, int)> fn = &Callable23::calc;

    std::unique_ptr<opc::ICallable> c =
        std::make_unique<opc::Call<decltype(fn)>>(opc::Call<decltype(fn)>{fn});

    std::vector<Variant> in{};
    in.emplace_back(Variant(23));
    in.emplace_back(Variant(2));
    in.emplace_back(Variant(2));
    std::vector<Variant> out{};
    c->call(&callable1, in, out);
    ASSERT_EQ(out.size(), 0);
}