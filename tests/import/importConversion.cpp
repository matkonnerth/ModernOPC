#include <gtest/gtest.h>
#include <src/import/conversion.h>
#include <open62541/types_generated.h>

TEST(conversion, isTrue)
{
    ASSERT_TRUE(isTrue("true"));
    ASSERT_FALSE(isTrue(nullptr));
    ASSERT_FALSE(isTrue("false"));
    ASSERT_FALSE(isTrue("asdfasf"));
}

TEST(conversion, extractNodeId_numeric) 
{
    char i10[]="i=10";
    UA_NodeId id = extractNodeId(&i10[0]);
    UA_NodeId exp = UA_NODEID_NUMERIC(0, 10);

    ASSERT_TRUE(UA_NodeId_equal(&exp, &id));
}

TEST(conversion, extractNodeId_string)
{
    char sTest[] = "s=test";
    UA_NodeId id = extractNodeId(&sTest[0]);
    char test[] = "test";
    UA_NodeId exp = UA_NODEID_STRING(0, &test[0]);
    ASSERT_TRUE(UA_NodeId_equal(&id, &exp));
    UA_clear(&id, &UA_TYPES[UA_TYPES_NODEID]);
}

TEST(conversion, extractNodeId_notNs0_string)
{
    char sTest[] = "ns=234;s=test";
    UA_NodeId id = extractNodeId(&sTest[0]);
    char test[] = "test";
    UA_NodeId exp = UA_NODEID_STRING(234, &test[0]);
    ASSERT_TRUE(UA_NodeId_equal(&id, &exp));
    UA_clear(&id, &UA_TYPES[UA_TYPES_NODEID]);
}

TEST(conversion, extractNodeId_ns333_numeric)
{
    char i10[] = "ns=333;i=10";
    UA_NodeId id = extractNodeId(&i10[0]);
    UA_NodeId exp = UA_NODEID_NUMERIC(333, 10);

    ASSERT_TRUE(UA_NodeId_equal(&exp, &id));
}

TEST(conversion, extractNodeId_wrongInputs)
{
    UA_NodeId id = extractNodeId(nullptr);
    ASSERT_TRUE(UA_NodeId_equal(&id, &UA_NODEID_NULL));

    id = extractNodeId((char*)";");
    ASSERT_TRUE(UA_NodeId_equal(&id, &UA_NODEID_NULL));
}