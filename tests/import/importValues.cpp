#include <Server.h>
#include <Variant.h>
#include <gtest/gtest.h>

std::string path = "";

TEST(import, namespaceZeroValues) {

    opc::Server server;
    ASSERT_TRUE(server.loadNodeset(path + "/" + "namespaceZeroValues.xml"));

    opc::Variant var;
    ASSERT_TRUE(server.readValue(opc::NodeId(2,1003), var));
    ASSERT_TRUE(var.get<double>()-3.14 < 0.01);
    ASSERT_TRUE(server.readValue(opc::NodeId(2, 1004), var));
    ASSERT_TRUE(var.get<std::vector<uint>>()[2]==140);

    /*

        
        // extension object with nested struct
        retval = UA_Server_readValue(server, UA_NODEID_NUMERIC(nsIdx, 1005), &var);
        ck_assert_uint_eq(retval, UA_STATUSCODE_GOOD);
        ck_assert(var.type->typeIndex == UA_TYPES_SERVERSTATUSDATATYPE);
        ck_assert_int_eq(((UA_ServerStatusDataType *)var.data)->state, 5);
        UA_Variant_clear(&var);
        // array of extension object with nested struct
        retval = UA_Server_readValue(server, UA_NODEID_NUMERIC(nsIdx, 1006), &var);
        ck_assert_uint_eq(retval, UA_STATUSCODE_GOOD);
        ck_assert(var.type->typeIndex == UA_TYPES_SERVERSTATUSDATATYPE);
        ck_assert_int_eq(((UA_ServerStatusDataType *)var.data)[1].state, 3);
        UA_Variant_clear(&var);
        // LocalizedText
        retval = UA_Server_readValue(server, UA_NODEID_NUMERIC(nsIdx, 1007), &var);
        ck_assert_uint_eq(retval, UA_STATUSCODE_GOOD);
        ck_assert(var.type->typeIndex == UA_TYPES_LOCALIZEDTEXT);
        UA_String s = UA_STRING("en");
        ck_assert(UA_String_equal(&((UA_LocalizedText *)var.data)->locale, &s));
        UA_Variant_clear(&var);
        // LocalizedTextArray
        retval = UA_Server_readValue(server, UA_NODEID_NUMERIC(nsIdx, 1008), &var);
        ck_assert_uint_eq(retval, UA_STATUSCODE_GOOD);
        ck_assert(var.type->typeIndex == UA_TYPES_LOCALIZEDTEXT);
        ck_assert_int_eq(var.arrayLength, 2);
        s = UA_STRING("griasEich!");
        ck_assert(UA_String_equal(&((UA_LocalizedText *)var.data)->text, &s));
        s = UA_STRING("Hi!");
        ck_assert(UA_String_equal(&((UA_LocalizedText *)var.data)[1].text, &s));
        UA_Variant_clear(&var);
        // QualifiedName
        retval = UA_Server_readValue(server, UA_NODEID_NUMERIC(nsIdx, 1009), &var);
        ck_assert_uint_eq(retval, UA_STATUSCODE_GOOD);
        ck_assert(var.type->typeIndex == UA_TYPES_QUALIFIEDNAME);
        ck_assert_uint_eq(((UA_QualifiedName *)var.data)->namespaceIndex, 2);
        UA_Variant_clear(&var);
    */
}

int
main(int argc, char **argv) {

    testing::InitGoogleTest(&argc, argv);

    if(!(argc > 1))
        return 1;
    std::cout << argv[1] << std::endl;
    path = argv[1];

    return RUN_ALL_TESTS();
}