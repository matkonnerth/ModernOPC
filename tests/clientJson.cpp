#include <chrono>
#include <future>
#include <gtest/gtest.h>
#include <iostream>
#include <modernopc/BrowseResult.h>
#include <modernopc/Server.h>
#include <modernopc/Variant.h>
#include <modernopc/client/Client.h>
#include <modernopc/nodes/ObjectNode.h>
#include <modernopc/nodes/VariableNode.h>
#include <modernopc/types/NodeId.h>
#include <modernopc/types/QualifiedName.h>
#include <string>
#include <vector>
#include <chrono>

using modernopc::Client;
using modernopc::NodeId;
using modernopc::Server;
using modernopc::UnresolvedNodeId;
using namespace std::string_literals;
using modernopc::BrowseResult;
using modernopc::DataSource;
using modernopc::ObjectNode;
using modernopc::QualifiedName;
using modernopc::Variant;

std::string path = "";

UA_StatusCode getValue(const modernopc::NodeId &id, modernopc::Variant &var)
{
    var(42);
    return UA_STATUSCODE_GOOD;
}

UA_StatusCode setValue(const modernopc::NodeId &id,
                       const modernopc::Variant &var)
{
    auto test = var.get<int32_t>();
    std::cout << "setValue: " << test << "\n";
    return UA_STATUSCODE_GOOD;
}

class TestServer
{
  public:
    explicit TestServer(uint16_t port) : m_port{port}, s{port}
    {
        char hostname[HOST_NAME_MAX + 1];
        gethostname(hostname, HOST_NAME_MAX + 1);
        // m_hostname = hostname;
        m_hostname = "localhost"s;
        m_endpointUri =
            "opc.tcp://"s + m_hostname + ":"s + std::to_string(m_port);

        auto root = s.getObjectsFolder();
        auto var = root->addVariable(NodeId(1, "demoInt"),
                                     NodeId(0, UA_NS0ID_BASEDATAVARIABLETYPE),
                                     QualifiedName(1, "demoInt"), 27);
        var->connectCallback(
            std::make_unique<DataSource>("simpleVal", getValue, setValue));

        test = []() { return "hello"s; };

        auto objectsFolder = s.getObject(NodeId(0, 85));
        objectsFolder->addMethod(NodeId(1, "test"s), QualifiedName(1, "open"),
                                 test);
        f = std::async(std::launch::async, [&] { s.run(); });

        using namespace std::chrono_literals;
        std::this_thread::sleep_for(100ms);
    }
    ~TestServer()
    {
        s.stop();
        f.wait();
    }

    const std::string &Hostname() { return m_hostname; };
    const std::string &EndpointUri() { return m_endpointUri; }

  private:
    Server s;
    uint16_t m_port;
    std::future<void> f;
    std::function<std::string()> test;
    std::string m_hostname;
    std::string m_endpointUri;
};

TEST(ClientTest, import)
{
    TestServer s{4844};

    Client c{s.EndpointUri()};

    ASSERT_TRUE(c.loadNodeset(path + "/" + "struct.xml"));

    auto v = c.createVariantFromJson("{\"X\": 1, \"Y\":2, \"Z\": 3}", NodeId(2, 3002));

    struct Point
    {
        int32_t X;
        int32_t Y;
        int32_t Z;
    };

    Point p {1,2,3};

    ASSERT_EQ(0, memcmp(&p, v.getUAVariant()->data, c.findCustomDataType(UA_NODEID_NUMERIC(2, 3002))->memSize));
}

int main(int argc, char **argv)
{

    testing::InitGoogleTest(&argc, argv);

    if (!(argc > 1))
        return 1;
    path = argv[1];

    return RUN_ALL_TESTS();
}
