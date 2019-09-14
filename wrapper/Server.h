#include <map>
#include <open62541/server.h>
#include <open62541/server_config.h>
#include <Method.hpp>
#include "DataSource.h"
#include "NodeId.h"
#include "MethodWrapper.h"

struct UA_Server;
namespace opc {

class Server {
  public:
    Server();
    ~Server();
    void
    run();

    void loadNodeset(const std::string& path);
    template <typename M>
    void
    addMethod(const std::string &name, const M &callback) {
        std::vector<UA_Argument> inputArgs = MethodTraits<M>::getInputArguments();

        UA_Argument *data = inputArgs.data();

        UA_MethodAttributes methAttr = UA_MethodAttributes_default;
        methAttr.executable = true;
        methAttr.userExecutable = true;

        UA_Server_addMethodNode(
            server, UA_NODEID_NULL, UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
            UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
            UA_QUALIFIEDNAME(1, const_cast<char *>(name.c_str())), methAttr, nullptr,
            MethodTraits<M>::getNumArgs(), data, 0, nullptr, nullptr, nullptr);
    }

    template <typename... ARGS>
    void bindMethodNode(const NodeId &id, std::function<void(ARGS...)> fn)
    {
        UA_Server_setMethodNode_callback(server, id.toUA_NodeId(), internalMethodCallback);
        callbacks.insert(std::pair<int, std::unique_ptr<ICallable>>(std::get<int>(id.getIdentifier()), std::make_unique<Functor<ARGS...>>(fn)));
        UA_Server_setNodeContext(server, id.toUA_NodeId(), this);
    }

    void call(NodeId id)
    {
        ICallable* c = callbacks.at(std::get<int>(id.getIdentifier())).get();
        if(c)
        {
            std::vector<std::variant<int,double>> vec;
            std::variant<int,double> val {10};
            vec.push_back(val);
            c->call(vec);
        }
    }

        template <typename T>
        void addVariableNode(const NodeId &parentId, const std::string &browseName,
                             T initialValue) {
        UA_VariableAttributes attr = TypeConverter::getVariableAttributes(initialValue);
        attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
        UA_Server_addVariableNode(server, UA_NODEID_NULL, parentId.toUA_NodeId(),
                                  UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                                  UA_QUALIFIEDNAME(1, (char *)browseName.c_str()),
                                  UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
                                  attr, nullptr, nullptr);
    }

    template <typename T>
    void
    addVariableNode(const NodeId &parentId, const NodeId &requestedId,
                    const std::string &browseName, T initialValue) {
        UA_VariableAttributes attr = TypeConverter::getVariableAttributes(initialValue);
        attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
        UA_Server_addVariableNode(
            server, requestedId.toUA_NodeId(), parentId.toUA_NodeId(),
            UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
            UA_QUALIFIEDNAME(1, (char *)browseName.c_str()),
            UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), attr, nullptr, nullptr);
    }

    void
    setDataSource(const NodeId &id, DataSource &src) {
        UA_Server_setNodeContext(server, id.toUA_NodeId(), &src);
        UA_Server_setVariableNode_dataSource(server, id.toUA_NodeId(),
                                             src.getRawSource());
    }

    

    void registerStructType()
    {
        struct DemoStruct {
            int x;
            int y;
        };        

        UA_ServerConfig *config = UA_Server_getConfig(server);
        //setup custom type array
        UA_DataTypeArray *typeArray = (UA_DataTypeArray*)calloc(1, sizeof(UA_DataTypeArray));
        typeArray->next = NULL;
        //typeArray->typesSize = 1;
        UA_DataType* type = (UA_DataType*)(calloc(1, sizeof(UA_DataType)));

        //the datatpye
        type->typeKind = UA_DATATYPEKIND_STRUCTURE;
        type->binaryEncodingId = 100;
        type->typeId = UA_NODEID_NUMERIC(1, 100);
        
        
        type->typeIndex = 0;
        
        // need some sort of reflection for this
        type->pointerFree = true;
        type->overlayable = false;
        type->memSize = sizeof(DemoStruct);
        type->membersSize = 2;
        type->members = (UA_DataTypeMember *)UA_calloc(2, sizeof(UA_DataTypeMember));

        //X
        type->members[0].isArray = false;
        type->members[0].namespaceZero = true;
        type->members[0].memberTypeIndex = UA_TYPES_INT32;
        type->members[0].padding = 0;

        //Y
        UA_DataTypeMember *memberY =
            (UA_DataTypeMember *)UA_calloc(1, sizeof(UA_DataTypeMember));
        type->members[0].isArray = false;
        type->members[0].namespaceZero = true;
        type->members[0].memberTypeIndex = UA_TYPES_INT32;
        type->members[0].padding =
            offsetof(DemoStruct, y) - offsetof(DemoStruct, x) - sizeof(int);

        typeArray->types = type;
        config->customDataTypes = typeArray;
    }

  private:
    UA_Server *server;
    bool isRunning;
    static UA_StatusCode internalMethodCallback(UA_Server *server, const UA_NodeId *sessionId,
                                       void *sessionContext, const UA_NodeId *methodId,
                                       void *methodContext, const UA_NodeId *objectId,
                                       void *objectContext, size_t inputSize,
                                       const UA_Variant *input, size_t outputSize,
                                       UA_Variant *output);
    std::map<int, std::unique_ptr<ICallable>> callbacks;
};
}  // namespace opc