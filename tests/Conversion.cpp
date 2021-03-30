#include <gtest/gtest.h>
#include <modernOpc/DataType.h>
#include <open62541/types_generated.h>

TEST(TypeConverter, boolean) {
    auto t = modernopc::getDataType<bool>();
    ASSERT_EQ(t, &UA_TYPES[UA_TYPES_BOOLEAN]);
}

TEST(TypeConverter, integer) {
    auto t = modernopc::getDataType<int32_t>();
    ASSERT_EQ(t, &UA_TYPES[UA_TYPES_INT32]);
    t = modernopc::getDataType<int64_t>();
    ASSERT_EQ(t, &UA_TYPES[UA_TYPES_INT64]);
    t = modernopc::getDataType<char>();
    ASSERT_EQ(t, &UA_TYPES[UA_TYPES_SBYTE]);
    t = modernopc::getDataType<int_fast64_t>();
    ASSERT_EQ(t, &UA_TYPES[UA_TYPES_INT64]);
}

TEST(TypeConverter, floatingPoint) {
    auto t = modernopc::getDataType<double>();
    ASSERT_EQ(t, &UA_TYPES[UA_TYPES_DOUBLE]);
    t = modernopc::getDataType<float>();
    ASSERT_EQ(t, &UA_TYPES[UA_TYPES_FLOAT]);
}