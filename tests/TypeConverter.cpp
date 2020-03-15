#include <TypeConverter.h>
#include <gtest/gtest.h>

TEST(TypeConverter, boolean) {
    auto t = opc::getDataType<bool>();
    ASSERT_EQ(t, &UA_TYPES[UA_TYPES_BOOLEAN]);
}

TEST(TypeConverter, integer) {
    auto t = opc::getDataType<int32_t>();
    ASSERT_EQ(t, &UA_TYPES[UA_TYPES_INT32]);
    t = opc::getDataType<int64_t>();
    ASSERT_EQ(t, &UA_TYPES[UA_TYPES_INT64]);
    t = opc::getDataType<char>();
    ASSERT_EQ(t, &UA_TYPES[UA_TYPES_SBYTE]);
    t = opc::getDataType<int_fast64_t>();
    ASSERT_EQ(t, &UA_TYPES[UA_TYPES_INT64]);
}

TEST(TypeConverter, floatingPoint) {
    auto t = opc::getDataType<double>();
    ASSERT_EQ(t, &UA_TYPES[UA_TYPES_DOUBLE]);
    t = opc::getDataType<float>();
    ASSERT_EQ(t, &UA_TYPES[UA_TYPES_FLOAT]);
}