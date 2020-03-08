#include "value.h"
#include <open62541/types_generated.h>
#include <cassert>
#include <nodesetLoader.h>
#include "conversion.h"

using ConversionFn = void (*)(uintptr_t adr, char *value);

static UA_StatusCode
getMem(Value *val) {
    if(!val->typestack->type) {
        return UA_STATUSCODE_BADDATATYPEIDUNKNOWN;
    }
    void *newVal = UA_calloc(val->arrayCnt + 1, val->typestack->type->memSize);
    if(!newVal) {
        return UA_STATUSCODE_BADOUTOFMEMORY;
    }
    if(val->value) {
        memcpy(newVal, val->value, val->typestack->type->memSize * val->arrayCnt);
        UA_free(val->value);
    }
    val->value = newVal;
    val->offset = val->typestack->type->memSize * val->arrayCnt;
    val->arrayCnt++;
    return UA_STATUSCODE_GOOD;
}

static TypeList *
TypeList_push(TypeList *stack, const UA_DataType *newType) {
    auto *newStack = static_cast<TypeList*>(UA_calloc(1, sizeof(TypeList)));
    newStack->next = stack;
    newStack->type = newType;
    return newStack;
}

static TypeList *
TypeList_pop(TypeList *stack) {
    TypeList *tmp = stack->next;
    UA_free(stack);
    return tmp;
}

Value *
Value_new(const TNode *node) {
    if(!(node->nodeClass == NODECLASS_VARIABLE)) {
        assert(false && "Value_new on node that is not a variable node\n");
    }
    auto *val = static_cast<Value*>(UA_calloc(1, sizeof(Value)));
    val->state = VALUE_STATE_DATA;
    val->typestack = (TypeList *)UA_calloc(1, sizeof(TypeList));

    // looks only in UA_TYPES, should also look in custom types
    UA_NodeId dataTypeId = getNodeIdFromChars(((TVariableNode *)node)->datatype);

    val->typestack->type = UA_findDataType(&dataTypeId);
    val->datatype = val->typestack->type;
    if(!val->typestack->type) {
        printf("could not determine type, value processing stopped\n");
        val->state = VALUE_STATE_ERROR;
        return val;
    }
    val->name = val->typestack->type->typeName;
    val->typestack->memberIndex = 0;
    if(getMem(val) != UA_STATUSCODE_GOOD) {
        printf("getMem failed, value processing stopped\n");
        val->state = VALUE_STATE_ERROR;
    }

    return val;
}

static void
isBuiltinSpecialType(Value *val) {
    switch(val->typestack->type->typeKind) {
        case UA_DATATYPEKIND_NODEID:
            val->state = VALUE_STATE_NODEID;
            break;
        case UA_DATATYPEKIND_LOCALIZEDTEXT:
            val->state = VALUE_STATE_LOCALIZEDTEXT;
            break;
        case UA_DATATYPEKIND_QUALIFIEDNAME:
            val->state = VALUE_STATE_QUALIFIEDNAME;
        default:
            break;
    }
}

void
Value_start(Value *val, const char *localname) {
    switch(val->state) {
        case VALUE_STATE_ERROR:
            break;
        case VALUE_STATE_FINISHED:
        case VALUE_STATE_DATA:
            if(val->state == VALUE_STATE_FINISHED) {
                val->typestack = (TypeList *)UA_calloc(1, sizeof(TypeList));
                val->typestack->type = val->datatype;
                val->typestack->memberIndex = 0;
                if(getMem(val) != UA_STATUSCODE_GOOD) {
                    printf("getMem failed, value processing stopped\n");
                    val->state = VALUE_STATE_ERROR;
                    return;
                }
                if(val->typestack->type->members) {
                    val->name = val->typestack->type->members[val->typestack->memberIndex]
                                    .memberName;
                } else {
                    val->name = val->typestack->type->typeName;
                }
                val->state = VALUE_STATE_DATA;
            }
            if(!strncmp(localname, "ListOf", strlen("ListOf"))) {
                val->isArray = true;
                break;
            }
            if(!strcmp(val->name, localname)) {

                size_t idx = val->typestack->memberIndex;
                if(val->typestack->type->members) {
                    val->name = val->typestack->type->members[idx].memberName;
                    val->offset =
                        val->offset + val->typestack->type->members[idx].padding;

                    val->typestack = TypeList_push(
                        val->typestack,
                        &UA_TYPES[val->typestack->type->members[idx].memberTypeIndex]);
                    val->state = VALUE_STATE_DATA;
                    if(val->typestack->type->members) {
                        val->name = val->typestack->type->members[0].memberName;
                    }
                }
                if(!val->typestack->type->members) {
                    val->state = VALUE_STATE_BUILTIN;
                    isBuiltinSpecialType(val);
                }
            }
            break;
        case VALUE_STATE_BUILTIN:
            break;
        case VALUE_STATE_NODEID:
            break;
        case VALUE_STATE_LOCALIZEDTEXT:
            break;
        case VALUE_STATE_QUALIFIEDNAME:
            break;
        default:
            break;
    }
}

static void
setBoolean(uintptr_t adr, char *value) {
    *(UA_Boolean *)adr = isTrue(value);
}

static void
setSByte(uintptr_t adr, char *value) {
    *(UA_SByte *)adr = (UA_SByte)atoi(value);
}

static void
setByte(uintptr_t adr, char *value) {
    *(UA_Byte *)adr = (UA_Byte)atoi(value);
}

static void
setInt16(uintptr_t adr, char *value) {
    *(UA_Int16 *)adr = (UA_Int16)atoi(value);
}

static void
setUInt16(uintptr_t adr, char *value) {
    *(UA_UInt16 *)adr = (UA_UInt16)atoi(value);
}

static void
setInt32(uintptr_t adr, char *value) {
    *(UA_Int32 *)adr = atoi(value);
}

static void
setUInt32(uintptr_t adr, char *value) {
    *(UA_UInt32 *)adr = (UA_UInt32)atoi(value);
}

static void
setInt64(uintptr_t adr, char *value) {
    *(UA_Int64 *)adr = atoi(value);
}

static void
setUInt64(uintptr_t adr, char *value) {
    *(UA_UInt64 *)adr = (UA_UInt64)atoi(value);
}

static void
setFloat(uintptr_t adr, char *value) {
    *(UA_Float *)adr = (UA_Float)atof(value);
}

static void
setDouble(uintptr_t adr, char *value) {
    *(UA_Double *)adr = atof(value);
}

static void
setString(uintptr_t adr, char *value) {
    auto *s = (UA_String *)adr;
    s->length = strlen(value);
    s->data = (UA_Byte *)value;
}

static void
setDateTime(uintptr_t adr, char *value) {
    printf("DateTime: %s currently not handled\n", value);
}

static void
setNodeId(uintptr_t adr, char *value) {
    // todo: namespaceIndex should be converted?
    *(UA_NodeId *)adr = extractNodeId(value);  // getNodeIdFromChars(value);
}

static void
setNotImplemented(uintptr_t adr, char *value) {
    UA_assert(false && "not implemented");
}

static const ConversionFn conversionTable[UA_DATATYPEKINDS] = {
    setBoolean,        setSByte,          setByte,           setInt16,
    setUInt16,         setInt32,          setUInt32,         setInt64,
    setUInt64,         setFloat,          setDouble,         setString,
    setDateTime,       setNotImplemented,
    setString,  // handle bytestring like string
    setString,  // handle xmlElement like string
    setNodeId,         setNotImplemented, setNotImplemented, setNotImplemented,
    setNotImplemented,  // special handling needed
    setNotImplemented, setNotImplemented, setNotImplemented, setNotImplemented,
    setNotImplemented,
    setInt32,  // handle enum the same way like int32
    setNotImplemented, setNotImplemented, setNotImplemented, setNotImplemented};

static void
setScalarValueWithAddress(uintptr_t adr, UA_UInt32 kind, char *value) {
    if(value) {
        conversionTable[kind](adr, value);
    }
}

static void
setScalarValue(Value *val, const UA_DataType *type, char *value) {

    uintptr_t adr = (uintptr_t)val->value + val->offset;
    setScalarValueWithAddress(adr, type->typeKind, value);
    val->offset = val->offset + type->memSize;
}

static void
nextType(Value *val) {
    val->typestack = TypeList_pop(val->typestack);
    val->state = VALUE_STATE_DATA;

    if(!val->typestack) {
        val->state = VALUE_STATE_FINISHED;
        return;
    }

    if(val->typestack->memberIndex >= (size_t)val->typestack->type->membersSize - 1) {
        return;
    }

    val->typestack->memberIndex++;
    if(val->typestack->type->members) {
        val->name = val->typestack->type->members[val->typestack->memberIndex].memberName;
    } else {
        val->name = val->typestack->type->typeName;
    }
}

void
Value_end(Value *val, const char *localname, char *value) {
    switch(val->state) {
        case VALUE_STATE_BUILTIN:
            setScalarValue(val, val->typestack->type, value);
            nextType(val);
            break;
        case VALUE_STATE_NODEID:
            if(!strcmp(localname, "Identifier")) {
                setScalarValue(val, val->typestack->type, value);
                nextType(val);
            }
            break;
        case VALUE_STATE_LOCALIZEDTEXT:
            if(!strcmp(localname, "Locale")) {
                setScalarValueWithAddress(val->offset + (uintptr_t) &
                                              ((UA_LocalizedText *)val->value)->locale,
                                          UA_DATATYPEKIND_STRING, value);
            } else if(!strcmp(localname, "Text")) {
                setScalarValueWithAddress(val->offset + (uintptr_t) &
                                              ((UA_LocalizedText *)val->value)->text,
                                          UA_DATATYPEKIND_STRING, value);
            } else {
                val->offset = val->offset + sizeof(UA_LocalizedText);
                nextType(val);
            }
            break;
        case VALUE_STATE_QUALIFIEDNAME:
            if(!strcmp(localname, "NamespaceIndex")) {
                setScalarValueWithAddress(
                    val->offset + (uintptr_t) &
                        ((UA_QualifiedName *)val->value)->namespaceIndex,
                    UA_DATATYPEKIND_UINT16, value);
            } else if(!strcmp(localname, "Name")) {
                setScalarValueWithAddress(val->offset + (uintptr_t) &
                                              ((UA_QualifiedName *)val->value)->name,
                                          UA_DATATYPEKIND_STRING, value);
            } else {
                val->offset = val->offset + sizeof(UA_QualifiedName);
                nextType(val);
            }
            break;
        case VALUE_STATE_DATA:
            if(!strcmp(localname, val->typestack->type->typeName)) {
                nextType(val);
                break;
            }
            break;
        case VALUE_STATE_ERROR:
            break;
        case VALUE_STATE_FINISHED:
            break;
        default:
            break;
    }
}

void
Value_finish(Value *val) {
    if(VALUE_STATE_FINISHED != val->state) {
        printf("Warning: value finish called while value state != finished\n");
    }
}
