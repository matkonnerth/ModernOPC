#include <gtest/gtest.h>
#include <iostream>
#include <modernopc/StatusCode.h>

using modernopc::StatusCode;

TEST(statusCode, basics)
{
    const StatusCode status{UA_STATUSCODE_GOOD};
    ASSERT_TRUE(status);
    ASSERT_TRUE(status.IsGood());
    ASSERT_TRUE(!status.IsBad());
    const StatusCode status2{UA_STATUSCODE_BADALREADYEXISTS};
    ASSERT_FALSE(status==status2);
    ASSERT_FALSE(status2);
    ASSERT_TRUE(status2.IsBad());
    const StatusCode status3{UA_STATUSCODE_BADALREADYEXISTS};
    ASSERT_TRUE(status2==status3);
}
