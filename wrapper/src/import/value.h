
#ifndef VALUE_H
#define VALUE_H
#include <open62541/types.h>


struct TNode;
enum VALUE_STATE {
    VALUE_STATE_DATA,
    VALUE_STATE_FINISHED,
    VALUE_STATE_BUILTIN,
    VALUE_STATE_NODEID,
    VALUE_STATE_LOCALIZEDTEXT,
    VALUE_STATE_QUALIFIEDNAME,
    VALUE_STATE_ERROR
};

// typedef struct TypeList TypeList;
struct TypeList {
    const UA_DataType *type;
    size_t memberIndex;
    TypeList *next;
};

struct Value {
    bool isArray;
    enum VALUE_STATE state;
    void *value;
    size_t arrayCnt;
    TypeList *typestack;
    size_t offset;
    const char *name;
    const UA_DataType *datatype;
};
struct Value *
Value_new(const TNode *node);
void
Value_start(Value *val, const char *localname);
void
Value_end(Value *val, const char *localname, char *value);
void
Value_finish(Value *val);
void Value_delete(Value** val);

#endif
