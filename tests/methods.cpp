#include <opc/Server.h>
#include <gtest/gtest.h>
#include <opc/Variant.h>

using opc::Variant;
TEST(Methods, std_function)
{
    using namespace std::string_literals;
    std::function test = []() { return "hello"s; };
    opc::Server s;
    s.addMethod(opc::NodeId(0, 85), opc::NodeId(1, "doIt"), "open", test);

    auto server = s.getUAServer();
    UA_StatusCode retval = UA_Server_run_startup(server);

    /*
    UA_Variant var;
    UA_Variant_init(&var);
    UA_Int32 i = 1230;
    UA_Variant_setScalar(&var, &i, &UA_TYPES[UA_TYPES_INT32]);
    */

    UA_CallMethodRequest req;
    UA_CallMethodRequest_init(&req);
    req.objectId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    req.methodId = UA_NODEID_STRING(1, (char*)"doIt");
    req.inputArguments = nullptr;
    req.inputArgumentsSize = 0;

    UA_CallMethodResult res = UA_Server_call(server, &req);
    ASSERT_EQ(res.statusCode, UA_STATUSCODE_GOOD);
    ASSERT_EQ(res.outputArgumentsSize, 1);
    ASSERT_EQ(res.outputArguments->type, &UA_TYPES[UA_TYPES_STRING]);
    Variant var { res.outputArguments, true };
    ASSERT_TRUE(var.is_a<std::string>());
    ASSERT_EQ(var.get<std::string>(), "hello"s);
}

TEST(Methods, memberFunction)
{
    using namespace std::string_literals;
    struct Callable
    {
        int run(){return 12;}
    };
    Callable c;
    opc::Server s;
    std::function fn = [&]() { return c.run(); };
    s.addMethod(opc::NodeId(0, 85), opc::NodeId(1, "doIt"), "open", fn);

    std::function<int(Callable*)> memberFn = &Callable::run;
    s.addMethod(opc::NodeId(0, 85), opc::NodeId(1, "doIt"), "run",
                &Callable::run);
}

void freeVoidVoid()
{

}

TEST(Methods, freeVoidVoid)
{
    opc::Server s;
    s.addMethod(opc::NodeId(0, 85), opc::NodeId(1, "doIt"), "open",
                &freeVoidVoid);

    auto server = s.getUAServer();
    UA_StatusCode retval = UA_Server_run_startup(server);

    /*
    UA_Variant var;
    UA_Variant_init(&var);
    UA_Int32 i = 1230;
    UA_Variant_setScalar(&var, &i, &UA_TYPES[UA_TYPES_INT32]);
    */

    UA_CallMethodRequest req;
    UA_CallMethodRequest_init(&req);
    req.objectId = UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER);
    req.methodId = UA_NODEID_STRING(1, (char *)"doIt");
    req.inputArguments = nullptr;
    req.inputArgumentsSize = 0;

    UA_CallMethodResult res = UA_Server_call(server, &req);
    ASSERT_EQ(res.statusCode, UA_STATUSCODE_GOOD);
    ASSERT_EQ(res.outputArgumentsSize, 0);
}

void freeVoidConstStdString(const std::string& test)
{
    (void)test;
}

TEST(Methods, freeVoidConstString)
{
    opc::Server s;
    s.addMethod(opc::NodeId(0, 85), opc::NodeId(1, "doIt"), "open",
                &freeVoidConstStdString);
}
