
#ifndef VALUE_H
#define VALUE_H
#include <open62541/types.h>

struct TNode;
enum VALUE_STATE
{
    VALUE_STATE_DATA,
    VALUE_STATE_FINISHED,
    VALUE_STATE_BUILTIN,
    VALUE_STATE_NODEID,
    VALUE_STATE_LOCALIZEDTEXT,
    VALUE_STATE_QUALIFIEDNAME,
    VALUE_STATE_ERROR
};

namespace opc
{
    class TypeList;
}

struct Value
{
    bool isArray;
    enum VALUE_STATE state;
    void *value;
    size_t arrayCnt;
    opc::TypeList *typestack;
    size_t offset;
    const char *name;
    const UA_DataType *datatype;
};

namespace opc
{

struct Value *Value_new(const TNode *node);
void Value_start(Value *val, const char *localname);
void Value_end(Value *val, const char *localname, char *value);
void Value_finish(Value *val);
void Value_delete(Value **val);

} // namespace opc

#endif
