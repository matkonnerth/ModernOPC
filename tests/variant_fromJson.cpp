#include <gtest/gtest.h>
#include <modernopc/Variant.h>
#include <modernopc/types/NodeId.h>
#include <iostream>

using namespace std::string_literals;
using modernopc::NodeId;
using modernopc::Variant;

TEST(variant, decode_NodeId)
{
    auto var = modernopc::fromJson("{\"Id\":42,\"Namespace\":123}",
                                    &UA_TYPES[UA_TYPES_NODEID]);

    ASSERT_TRUE(var.is_a<NodeId>());

    Variant var4{NodeId{123, 42}};
    ASSERT_EQ(var4, var);
}

TEST(variant, decode_DiagnosticInfo)
{
    auto var = modernopc::fromJson(
        "{\"SymbolicId\":13,"
        "\"LocalizedText\":14,"
        "\"Locale\":12,"
        "\"AdditionalInfo\":\"additionalInfo\","
        "\"InnerStatusCode\":2155216896,"
        "\"InnerDiagnosticInfo\":{\"AdditionalInfo\":\"INNER ADDITION INFO\"}}",
        &UA_TYPES[UA_TYPES_DIAGNOSTICINFO]);
}

