/**
 * Using OPC UA Methods
 * -----------------------
 * This example shows how to add method nodes and connect it to the
 * implementation. Free Functions, member functions and lambdas are supported.
 */

#include <functional>
#include <iostream>
#include <modernopc/client/Client.h>
#include <modernopc/types/NodeId.h>
#include <open62541/client_highlevel.h>
#include <thread>

using modernopc::Client;
using modernopc::NodeId;
using modernopc::QualifiedName;

bool tryRead(Client &client, const NodeId &id)
{
    UA_StatusCode status = UA_STATUSCODE_BAD;
    client.read(id, status);
    return status == UA_STATUSCODE_GOOD;
}

void browseRecursive(Client &client, const NodeId &root,
                     std::vector<NodeId> &results, int maxResults)
{

    auto refs = client.browse(root);
    for (const auto &ref : refs)
    {
        if (ref.Type() == modernopc::NodeType::VARIABLE)
        {
            if (!tryRead(client, ref.Id()))
            {
                std::cout << "reading: " << ref.Id() << " failed"
                          << "\n";
                continue;
            }
            if (results.size() >= maxResults)
            {
                return;
            }

            results.push_back(ref.Id());
        }
        browseRecursive(client, ref.Id(), results, maxResults);
    }
}

std::vector<NodeId> filterNodeIds(const std::vector<NodeId> &ids,
                                  const std::string &toFind)
{
    std::vector<NodeId> result{};
    for (const auto &id : ids)
    {
        if (std::holds_alternative<std::string>(id.getIdentifier()))
        {
            auto idString = std::get<std::string>(id.getIdentifier());
            if (idString.find(toFind) != std::string::npos)
            {
                result.push_back(id);
            }
        }
    }
    return result;
}

std::vector<NodeId> getAllVariables(Client &client, const NodeId &root)
{
    std::vector<NodeId> vars{};
    browseRecursive(client, root, vars, 100);
    //return filterNodeIds(vars, ".Activated");
    return vars;
}

std::vector<NodeId> getSingleVariable(int nsIdx, const std::string &idString)
{
    std::vector<NodeId> vars{};
    vars.push_back(NodeId(24, idString.c_str()));
    return vars;
}

void monitorVars(Client &client, const std::vector<NodeId> &ids)
{
    for (const auto &var : ids)
    {
        client.createMonitoredItem(var, UA_ATTRIBUTEID_USERACCESSLEVEL);
    }
}

class Container
{

    const std::vector<Container> &children() const { return m_children; }

  private:
    int id{};
    std::vector<Container> m_children{};
};

static bool bCancel = false;

void cyclicActivate(Client *client)
{
    while (!bCancel)
    {
        client->activateSession("de");
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        client->activateSession("en");
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}

int main(int argc, char *argv[])
{

    std::string uri{"opc.tcp://localhost:48010"};
    if (argc >= 2)
    {
        uri = argv[1];
    }
    std::cout << "uri: " << uri << "\n";

    Client client{uri};
    // Client client{"opc.tcp://10.11.65.189:4840"};
    // Client client{"opc.tcp://192.168.110.10:4850"};
    client.connect();

    /*
        auto nsUriPlcAction =
        client.resolveNamespaceUri("http://engelglobal.com/IMM/Mold1");
    */

    bool resolved = false;
    int nsUriPlcAction = 0;
    while (!resolved)
    {
        nsUriPlcAction =
            client.resolveNamespaceUri("http://engelglobal.com/IMM/Mold1/");
        if (nsUriPlcAction != 0)
        {
            resolved = true;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    // auto nsUriPlcAction =
    //     client.resolveNamespaceUri("http://engelglobal.com/PLCActionModel/");

    //        std::cout
    //    << "nsIdx: " << std::to_string(nsUriPlcAction) << "\n";

    // auto vars = getAllVariables(client, NodeId(nsUriPlcAction,
    // "PushButtonDevices"));
    auto vars =
        getAllVariables(client, NodeId(nsUriPlcAction, "Mold1"));
    // auto vars = getSingleVariable(nsUriPlcAction,
    // "_global.EqRobot1.uiRobotCycleCounter");
    //  auto vars = getAllVariables(client, NodeId(0, 85));
    //  auto vars = getAllVariables(client, NodeId(nsUriPlcAction,
    //  "IMM.Project"));

    for (const auto &var : vars)
    {
        std::cout << var << "\n";
    }

    // client.disconnect();
    client.createSubscription();

    //std::thread cyclicThread(cyclicActivate, &client);

    int cnt = 0;

    while (true)
    {
        if (cnt == 0)
        {
            monitorVars(client, vars);
            client.activateSession("de");
        }
        if (cnt == 50)
        {
            client.clearMonitoredItems();
            client.activateSession("en");
            cnt = 0;
        }
        else
        {
            cnt++;
        }

        client.doComm();
    }
    bCancel = true; 

    //cyclicThread.join();
    client.disconnect();
}