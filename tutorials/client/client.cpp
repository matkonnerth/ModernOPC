/**
 * Using OPC UA Methods
 * -----------------------
 * This example shows how to add method nodes and connect it to the implementation.
 * Free Functions, member functions and lambdas are supported. */

#include <functional>
#include <iostream>
#include <modernopc/client/Client.h>

using modernopc::QualifiedName;
using modernopc::Client;



int main()
{ 
	Client client{"opc.tcp://192.168.110.10:4860"};
    client.connect();

	auto nsUriPlcAction =
        client.resolveNamespaceUri("http://engelglobal.com/PLCActionModel/");

            std::cout
        << "nsIdx: " << std::to_string(nsUriPlcAction) << "\n";

}