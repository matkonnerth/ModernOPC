#include <DataSource.h>
#include <Variant.h>
#include <array>
#include <chrono>
#include <functional>
#include <iostream>
#include <thread>
#include <vector>
#include "MyDataSource.h"
#include "NodeId.h"
#include "Server.h"

auto
add(int a, int b, double c) {
    return a + b + c;
}

static int test = 0;

void
getValue(const opc::NodeId &id, opc::Variant &var) {
    test++;
    var(test);
}

void
setValue(const opc::NodeId &id, opc::Variant &var) {
    auto test = var.get<double>();
    (void)test;
}

void
getVectorValue(const opc::NodeId &id, opc::Variant &var) {
    std::vector<int> vec;
    vec.push_back(33);
    vec.push_back(423);
    vec.push_back(5323);
    var(std::move(vec));
}

void
setVectorValue(const opc::NodeId &id, opc::Variant &var) {
    auto vec = var.get<std::vector<int>>();
    for(auto &elem : vec) {
        std::cout << elem << std::endl;
    }
}

std::string
NodesetLoader_unload(std::vector<std::string> s1) {
    for(auto &s : s1) {
        std::cout << s;
        std::cout << std::endl;
    }
    //std::cout << s2 << s3 << f << std::endl;
    return "test";
}

opc::Server *s;

void
createServer() {
    s = new opc::Server();
}

void
run() {
    s->run();
}

class MethodObject {
  public:
    void
    call(int a) {
        std::cout << "called " << a << std::endl;
    }
};

int
main() {
    createServer();
    s->addVariableNode(opc::NodeId(0, 85), opc::NodeId(1, 233),
                       "demoNode" + std::to_string(233), 42);

    // wild west
    // server is accessed from multiple threads
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(0s);
    std::vector<float> fVector{1.1f, 2.2f, 3.3f};

    MyDataSource source1{"source1"};

    s->registerDataSource("simpleVal", getValue, setValue);
    s->registerDataSource("vectorDataSource", getVectorValue,
                          [](const opc::NodeId &, opc::Variant &) {});

    s->registerDataSource(
        source1.getKey(),
        [&](const opc::NodeId &id, opc::Variant &var) { source1.read(id, var); },
        [&](const opc::NodeId &id, opc::Variant &var) { source1.write(id, var); });

    // adding of variable nodes
    s->addVariableNode(opc::NodeId(0, 85), opc::NodeId(1, "demoVector"), "demoVector",
                       fVector, std::make_unique<opc::NodeMetaInfo>("vectorDataSource"));
    s->addVariableNode(opc::NodeId(0, 85), opc::NodeId(1, "demoArray"), "demoArray",
                       std::array<int, 3>{12, 13, 14});
    s->addVariableNode(opc::NodeId(0, 85), opc::NodeId(1, "demoInt"), "demoInt", 23,
                       std::make_unique<opc::NodeMetaInfo>("simpleVal"));
    s->addVariableNode(opc::NodeId(0, 85), opc::NodeId(1, "IntVector"), "IntVector",
                       test);
    s->addVariableNode(opc::NodeId(0, 85), opc::NodeId(1, "source1Var"), "source1Var", 12,
                       std::make_unique<opc::NodeMetaInfo>("source1"));

    // loading of a xml nodeset
    s->loadNodeset("../models/serviceobject.xml");
    //s->loadNodeset("../../nodesetLoader/nodesets/testNodeset100nodes.xml");
    s->loadNodeset("/home/matzy/Dokumente/opc_ua/models/types.xml");
    s->loadNodeset("../tests/import/namespaceZeroValues.xml");
    //s->loadNodeset("../../open62541/deps/ua-nodeset/Di/Opc.Ua.Di.NodeSet2.xml");
    std::function load = [&](std::string path) {
        s->loadNodeset(path);
    };

    // bind opc ua methods to business logic
    s->bindMethodNode(opc::NodeId(2, 7003), load);
    s->bindMethodNode(
        opc::NodeId(2, 7004),
        std::function{NodesetLoader_unload});

    std::function f = [](int a, int b) { return std::vector<int>{a, b}; };
    s->addMethod("addMethod2", f);

    // not really useful now, lacks parent node id
    s->addMethod("addMethod", std::function{add});

    MethodObject obj1;

    std::function m=[&](int a){obj1.call(a);};
    s->addMethod("ob1", m);


    std::thread serverThread{run};
    serverThread.join();
}