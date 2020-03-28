#include <Variant.h>
#include <gtest/gtest.h>
#include <open62541/server.h>
#include <vector>

TEST(variant, baseTypes)
{
    UA_Variant v;
    UA_Boolean b = UA_TRUE;
    UA_Variant_setScalar(&v, &b, &UA_TYPES[UA_TYPES_BOOLEAN]);
    opc::Variant var{&v};
    ASSERT_TRUE(var.isScalar() && var.is_a<bool>());
}

TEST(variant, arrays)
{
    UA_Variant v;
    UA_Int32 arr[3] = {1, 2, 3};
    UA_Variant_setArray(&v, &arr, 3, &UA_TYPES[UA_TYPES_INT32]);
    opc::Variant var{&v};
    ASSERT_TRUE(!var.isScalar() && var.is_a<int32_t>());
    ASSERT_TRUE(var.is_a<std::vector<int32_t>>());
}
