#include <open62541/server.h>
#include <iostream>
#include <TypeConverter.h>
#include <vector>
#include <Variant.h>

void dataSource_readValueTemplated(opc::Variant& var)
{
    var(45);
}

void
dataSource_writeValueTemplated(opc::Variant &var) {
    std::vector<int> vec = var.get<std::vector<int>>();
    for(const auto & elem : vec)
    {
        std::cout << elem << std::endl;
    }
}

int main()
{
    UA_Variant v;
    UA_Int32 i = 42;
    UA_Variant_setScalar(&v, &i, &UA_TYPES[UA_TYPES_INT32]);

    opc::Variant t {&v};

    dataSource_readValueTemplated(t);
    std::cout << t.get<int>() << std::endl;

    UA_Variant varVec;
    UA_Int32 arr[3] = {1,2,3};
    UA_Variant_setArrayCopy(&varVec, arr, 3, &UA_TYPES[UA_TYPES_INT32]);

    opc::Variant tVec {&varVec};
    dataSource_writeValueTemplated(tVec);

    opc::Variant testVariant{};
    testVariant(std::vector<float>{1.0f, 2.0f, 3.0f});


    std::vector<opc::Variant> vectorOfVar;
    //vectorOfVar.push_back(testVariant);


        return 0;
}