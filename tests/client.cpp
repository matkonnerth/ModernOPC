#include <chrono>
#include <future>
#include <gtest/gtest.h>
#include <iostream>
#include <modernOpc/Server.h>
#include <modernOpc/Variant.h>
#include <modernOpc/client/Client.h>
#include <modernOpc/nodes/ObjectNode.h>
#include <modernOpc/nodes/VariableNode.h>
#include <modernOpc/types/NodeId.h>
#include <string>
#include <vector>

using modernopc::NodeId;
using modernopc::UnresolvedNodeId;
using namespace std::string_literals;
using modernopc::ObjectNode;

UA_StatusCode getValue(const modernopc::NodeId &id, modernopc::Variant &var)
{
    var(42);
    return UA_STATUSCODE_GOOD;
}

UA_StatusCode setValue(const modernopc::NodeId &id, const modernopc::Variant &var)
{
    auto test = var.get<int32_t>();
    std::cout << "setValue: " << test << "\n";
    return UA_STATUSCODE_GOOD;
}

class ClientTest : public ::testing::Test {
 protected:
  void SetUp() override {
      auto root = s.getObjectsFolder();
    auto var = root->addVariable(modernopc::NodeId(1, "demoInt"),
                                 modernopc::NodeId(0, UA_NS0ID_BASEDATAVARIABLETYPE),
                                 modernopc::QualifiedName(1, "demoInt"), 27);
    var->connectCallback(
        std::make_unique<modernopc::DataSource>("simpleVal", getValue, setValue));
     f=std::async(std::launch::async, [&] { s.run(); });
  }

  void TearDown() override {
      s.stop();
      f.wait();

  }

  modernopc::Server s;
  std::future<void> f;
};

TEST_F(ClientTest, read)
{
    modernopc::Client c{"opc.tcp://localhost:4840"};
    c.connect();

    auto id =
        c.resolve(UnresolvedNodeId("http://opcfoundation.org/UA/", "i=2255"));
    auto var = c.read(id);
    ASSERT_TRUE(var.is_a<std::vector<std::string>>());
}

TEST_F(ClientTest, write)
{
    modernopc::Client c{"opc.tcp://localhost:4840"};
    c.connect();

    c.write(modernopc::NodeId(1, "demoInt"), modernopc::Variant(20));
    s.stop();
}
