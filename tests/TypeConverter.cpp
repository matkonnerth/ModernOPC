#include <TypeConverter.h>
#include <gtest/gtest.h>

TEST(TypeConverter, boolean) {
    auto t = TypeConverter::getDataType<bool>();
    ASSERT_EQ(t, &UA_TYPES[UA_TYPES_BOOLEAN]);
}

TEST(TypeConverter, integer) {
    auto t = TypeConverter::getDataType<int32_t>();
    ASSERT_EQ(t, &UA_TYPES[UA_TYPES_INT32]);
    t = TypeConverter::getDataType<int64_t>();
    ASSERT_EQ(t, &UA_TYPES[UA_TYPES_INT64]);
    t = TypeConverter::getDataType<char>();
    ASSERT_EQ(t, &UA_TYPES[UA_TYPES_SBYTE]);
    t = TypeConverter::getDataType<int_fast64_t>();
    ASSERT_EQ(t, &UA_TYPES[UA_TYPES_INT64]);
}

TEST(TypeConverter, floatingPoint) {
    auto t = TypeConverter::getDataType<double>();
    ASSERT_EQ(t, &UA_TYPES[UA_TYPES_DOUBLE]);
    t = TypeConverter::getDataType<float>();
    ASSERT_EQ(t, &UA_TYPES[UA_TYPES_FLOAT]);
}