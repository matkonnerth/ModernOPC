#include <chrono>
#include <future>
#include <gtest/gtest.h>
#include <iostream>
#include <modernOpc/BrowseResult.h>
#include <modernOpc/Server.h>
#include <modernOpc/Variant.h>
#include <modernOpc/client/Client.h>
#include <modernOpc/nodes/ObjectNode.h>
#include <modernOpc/nodes/VariableNode.h>
#include <modernOpc/types/NodeId.h>
#include <modernOpc/types/QualifiedName.h>
#include <string>
#include <vector>

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

TEST(ClientTest, read)
{
    TestServer s{4840};

    Client c{s.EndpointUri()};
    c.connect();

    auto id =
        c.resolve(UnresolvedNodeId("http://opcfoundation.org/UA/", "i=2255"));
    auto var = c.read(id);
    ASSERT_TRUE(var.is_a<std::vector<std::string>>());
}

TEST(ClientTest, write)
{
    TestServer s{4841};

    Client c{s.EndpointUri()};
    c.connect();

    c.write(NodeId(1, "demoInt"), Variant(20));
}

TEST(ClientTest, call)
{
    TestServer s{4842};

    Client c{s.EndpointUri()};

    c.connect();

    auto output =
        c.call(NodeId(0, 85), NodeId(1, "test"s), std::vector<Variant>{});
    ASSERT_TRUE(output.size() == 1);
    ASSERT_TRUE(output[0].is_a<std::string>());
    std::cout << output[0].get<std::string>() << "\n";
}

TEST(ClientTest, browse)
{
    TestServer s{4843};

    Client c{s.EndpointUri()};

    c.connect();

    auto results = c.browse(NodeId(0, 85));
    for(const auto& r : results)
    {
        std::cout << "id: " << r.Id() << " name: " << r.Name() << "\n";
    }
}
