/**
 * Using OPC UA Methods
 * -----------------------
 * This example shows how to add method nodes and connect it to the implementation.
 * Free Functions, member functions and lambdas are supported. */

#include <functional>
#include <iostream>
#include <modernopc/client/Client.h>
#include <modernopc/types/NodeId.h>

using modernopc::QualifiedName;
using modernopc::Client;
using modernopc::NodeId;

void browseRecursive(Client& client, const NodeId& root, std::vector<NodeId>& results)
{
    auto refs = client.browse(root);
    for (const auto &ref : refs)
    {
        if (ref.Type() == modernopc::NodeType::VARIABLE)
        {
            results.push_back(ref.Id());
        }
        browseRecursive(client, ref.Id(), results);
    }
}

std::vector<NodeId> getAllVariables(Client& client, const NodeId& root)
{
    std::vector<NodeId> vars{};
    browseRecursive(client, root, vars);
    return vars;
}

void monitorVars(Client& client, const std::vector<NodeId>& ids)
{
    for (const auto &var : ids)
    {
        client.createMonitoredItem(var);
    }
}

int main()
{ 
	Client client{"opc.tcp://localhost:48010"};
    client.connect();

    /*
	auto nsUriPlcAction =
        client.resolveNamespaceUri("http://engelglobal.com/PLCActionModel/");

            std::cout
        << "nsIdx: " << std::to_string(nsUriPlcAction) << "\n";
        */


    auto vars = getAllVariables(client, NodeId(3, "PSTV"));

    client.createSubscription();

    int loopCount=0;

    monitorVars(client, vars);
    while(true)
    {
        client.doComm();
        if(loopCount>=10)
        {
            client.clearMonitoredItems();
            monitorVars(client, vars);
            loopCount=0;
        }
        loopCount++;
    }


}