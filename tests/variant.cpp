#include <opc/Variant.h>
#include <gtest/gtest.h>
#include <open62541/server.h>
#include <opc/types/Types.h>
#include <opc/types/NodeId.h>


using namespace std::string_literals;

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

TEST(variant, arrays)
{   
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
    UA_Variant *v = UA_Variant_new();
    UA_Int32 arr[3] = {1, 2, 3};
    UA_Variant_setArrayCopy(v, &arr, 3, &UA_TYPES[UA_TYPES_INT32]);
    opc::Variant var{v, true};
    return var;
}

TEST(variant, ownership)
{
    UA_Variant *v = UA_Variant_new();
    opc::Variant var{v, true};
}

TEST(variant, ownership2)
{
    opc::Variant var = getVariant();
    auto vec = var.get<std::vector<int>>();
    ASSERT_EQ(vec.size(), 3);
    ASSERT_EQ(vec[0], 1);
    ASSERT_EQ(vec[1], 2);
    ASSERT_EQ(vec[2], 3);
}

TEST(variant, setPerReference)
{
    opc::Variant var;
    std::string s{"myString"};
    const std::string& sref=s;
    var(sref);
}

TEST(variant, setAndGet)
{
    opc::Variant var{std::string{"myString"}};
    auto s2 = var.get<std::string>();
}

TEST(variant, localizedText)
{
    opc::LocalizedText lt{"de", "Hallo"};
    opc::Variant var;
    var(std::move(lt));
    ASSERT_TRUE(var.is_a<opc::LocalizedText>());
    std::cout << lt << "\n";
}

TEST(variant, rvalueVector)
{
    std::vector v{1,2,3};
    opc::Variant var{std::move(v)};
}

TEST(variant, stringVector)
{
    std::vector v{"1"s, "2"s, "3"s};
    opc::Variant var{v};
}

TEST(variant, stringVectorRValue)
{
    std::vector v{"1"s, "2"s, "3"s};
    opc::Variant var{std::move(v)};
}

TEST(variant, primitiveTypes)
{
    opc::Variant var;
    var = true;
    ASSERT_TRUE(var.is_a<bool>());
    ASSERT_EQ(var.get<bool>(), true);
    var = static_cast<int8_t>(127);
    ASSERT_TRUE(var.is_a<int8_t>());
    ASSERT_EQ(var.get<int8_t>(),127);
    var = static_cast<uint8_t>(255);
    ASSERT_TRUE(var.is_a<uint8_t>());
    ASSERT_EQ(var.get<uint8_t>(), 255);
    var = static_cast<int16_t>(123);
    ASSERT_TRUE(var.is_a<int16_t>());
    ASSERT_EQ(var.get<int16_t>(), 123);
    var = static_cast<uint16_t>(2134);
    ASSERT_TRUE(var.is_a<uint16_t>());
    ASSERT_EQ(var.get<uint16_t>(), 2134);
    var = static_cast<int32_t>(2342);
    ASSERT_TRUE(var.is_a<int32_t>());
    ASSERT_EQ(var.get<int32_t>(), 2342);
    var = static_cast<uint32_t>(234234);
    ASSERT_TRUE(var.is_a<uint32_t>());
    ASSERT_EQ(var.get<uint32_t>(), 234234);
    var = static_cast<int64_t>(23423);
    ASSERT_TRUE(var.is_a<int64_t>());
    ASSERT_EQ(var.get<int64_t>(), 23423);
    var = static_cast<uint64_t>(234234);
    ASSERT_EQ(var.get<uint64_t>(), 234234);
    ASSERT_TRUE(var.is_a<uint64_t>());
    var = 1.34f;
    ASSERT_TRUE(var.is_a<float>());
    ASSERT_EQ(var.get<float>(), 1.34f);
    var = 3.1415;
    ASSERT_TRUE(var.is_a<double>());
    ASSERT_EQ(var.get<double>(), 3.1415);
}

TEST(variant, string)
{
    opc::Variant var {"helloWorld"s};
    ASSERT_TRUE(var.is_a<std::string>());
    std::string s = var.get<std::string>();
    ASSERT_EQ("helloWorld"s, s);
}

TEST(variant, qualifiedName)
{
    opc::QualifiedName qn {9, "name1"};
    opc::Variant var{qn};
    ASSERT_TRUE(var.is_a<opc::QualifiedName>());
    opc::QualifiedName q2 = var.get<opc::QualifiedName>();
    std::cout << q2 << "\n";
}

TEST(variant, NodeId)
{
    opc::NodeId id{2, "stringId"};
    opc::Variant var {id};
    ASSERT_TRUE(var.is_a<opc::NodeId>());
    auto id2 = var.get<opc::NodeId>();
    std::cout << id2 << "\n";
}
