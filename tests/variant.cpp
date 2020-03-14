#include <gtest/gtest.h>
#include <Variant.h>
#include <open62541/server.h>


TEST(variant, baseTypes)
{
    UA_Variant v;
    UA_Boolean b = UA_TRUE;
    UA_Variant_setScalar(&v, &b, &UA_TYPES[UA_TYPES_BOOLEAN]);
    opc::Variant var{&v};
    
    ASSERT_EQ(var.get<bool>(), true);

    UA_Int32 i = 42;
    UA_Variant_setScalar(&v, &i, &UA_TYPES[UA_TYPES_INT32]);
    var = opc::Variant(&v);
    ASSERT_EQ(var.get<int>(), 42);
}

TEST(variant, arrays) {
    UA_Variant v;
    UA_Int32 arr[3] = {1, 2, 3};
    UA_Variant_setArray(&v, &arr, 3, &UA_TYPES[UA_TYPES_INT32]);
    opc::Variant var{&v};
    auto vec = var.get<std::vector<int>>();
    ASSERT_EQ(vec.size(), 3);
    ASSERT_EQ(vec[0], 1);
    ASSERT_EQ(vec[1], 2);
    ASSERT_EQ(vec[2], 3);
}

opc::Variant getVariant()
{
    UA_Variant* v=UA_Variant_new();
    UA_Int32 arr[3] = {1, 2, 3};
    UA_Variant_setArrayCopy(v, &arr, 3, &UA_TYPES[UA_TYPES_INT32]);
    opc::Variant var {v, true};
    return var;
}

TEST(variant, ownership)
{
    UA_Variant* v = UA_Variant_new();
    opc::Variant var{v, true};
}

TEST(variant, ownership2) {
    opc::Variant var = getVariant();
    auto vec = var.get<std::vector<int>>();
    ASSERT_EQ(vec.size(), 3);
    ASSERT_EQ(vec[0], 1);
    ASSERT_EQ(vec[1], 2);
    ASSERT_EQ(vec[2], 3);
}