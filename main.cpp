#include <array>
#include <functional>
#include <iostream>
#include <vector>
#include "DataSource.h"
#include "Variant.h"
#include "NodeId.h"
#include "Server.h"

void
add(int a, int b, double c) {
    std::cout << a + b << std::endl;
}


void
getValue(Variant& var) {
    static int test = 0;
    test++;
    //val(std::make_unique<int>(test));
    var(test);
}

/*
void
getVectorValue(DataValue &val) {
    std::vector<int> vec;
    vec.push_back(3);
    vec.push_back(4);
    vec.push_back(5);
    val(vec);
}

void
floatSourceFunc(DataValue &val) {
    static float f = 100.0;
    f += 0.7;
    val(&f, true);
}
*/

int
main() {

    opc::Server s;
    s.addMethod("addMethod", &add);

    NodeId id{3, 1234};

    std::array<int, 3> test{12, 13, 14};

    std::vector<float> fVector{1.1f, 2.2f, 3.3f};

    DataSource ds{getValue};
    //DataSource d2{getVectorValue};

    float fval = 223.2f;
    //DataSource floatSource{floatSourceFunc};

    s.addVariableNode(NodeId(0, 85), NodeId(1, "demoVector"), "demoVector", fVector);
    s.addVariableNode(NodeId(0, 85), NodeId(1, "demoArray"), "demoArray", test);
    s.addVariableNode(NodeId(0, 85), NodeId(1, "demoInt"), "demoInt", 23);

    s.addVariableNode(NodeId(0, 85), NodeId(1, "IntVector"), "IntVector", 0);

    //s.setDataSource(NodeId(1, "IntVector"), d2);
    s.setDataSource(NodeId(1, "demoInt"), ds);

    s.addVariableNode(NodeId(0, 85), "demoFloat", 23.0f);
    s.addVariableNode(NodeId(0, 85), NodeId(1, "myStringId"), "demoFloat", 23.0f);
    //s.setDataSource(NodeId(1, "myStringId"), floatSource);
    s.run();
}