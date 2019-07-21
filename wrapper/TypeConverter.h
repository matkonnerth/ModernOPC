#include <open62541/server.h>
#include <typeinfo>


namespace TypeConverter
{
  template <typename T> UA_NodeId uaTypeNodeIdFromCpp() {

    std::cout << std::is_same<T, int>::value << std::endl;


    if (std::is_same<T, bool>::value) {
      return UA_NODEID_NUMERIC(0, UA_NS0ID_BOOLEAN);
    } else if (std::is_same<T, char>::value) {
      return UA_NODEID_NUMERIC(0, UA_NS0ID_SBYTE);
    } else if (std::is_same<T, u_char>::value) {
      return UA_NODEID_NUMERIC(0, UA_NS0ID_BYTE);
    } else if (std::is_same<T, int16_t>::value) {
      return UA_NODEID_NUMERIC(0, UA_NS0ID_INT16);
    } else if (std::is_same<T, uint16_t>::value) {
      return UA_NODEID_NUMERIC(0, UA_NS0ID_UINT16);
    } else if (std::is_same<T, int>::value) {
      return UA_NODEID_NUMERIC(0, UA_NS0ID_INT32);
    } else if (std::is_same<T, int32_t>::value) {
      return UA_NODEID_NUMERIC(0, UA_NS0ID_INT32);
    } else if (std::is_same<T, uint32_t>::value) {
      return UA_NODEID_NUMERIC(0, UA_NS0ID_UINT32);
    } else if (std::is_same<T, int64_t>::value) {
      return UA_NODEID_NUMERIC(0, UA_NS0ID_INT64);
    } else if (std::is_same<T, uint64_t>::value) {
      return UA_NODEID_NUMERIC(0, UA_NS0ID_UINT64);
    } else if (std::is_same<T, float>::value) {
      return UA_NODEID_NUMERIC(0, UA_NS0ID_FLOAT);
    } else if (std::is_same<T, double>::value) {
      return UA_NODEID_NUMERIC(0, UA_NS0ID_DOUBLE);
    } else if (std::is_same<T, std::string>::value ||
              std::is_same<T, const char *>::value) {
      return UA_NODEID_NUMERIC(0, UA_NS0ID_STRING);
    }
    std::cout << typeid(T).name() << std::endl;
    //bool x = T::nothing;
    //static_assert(false, "uaTypeNodeIdFromCpp ");
    return UA_NodeId();
  }
}