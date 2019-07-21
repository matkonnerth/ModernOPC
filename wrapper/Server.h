
#include <Method.hpp>
#include <open62541/server.h>

struct UA_Server;
namespace opc {

class Server {
public:
  Server();
  ~Server();
  void run();
  template <typename M>
  void addMethod(const std::string &name, const M &callback) {
    std::vector<UA_Argument> inputArgs = MethodTraits<M>::getInputArguments();

    UA_Argument *data = inputArgs.data();

    UA_MethodAttributes methAttr = UA_MethodAttributes_default;
    methAttr.executable = true;
    methAttr.userExecutable = true;

    UA_Server_addMethodNode(server, UA_NODEID_NULL,
                            UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
                            UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                            UA_QUALIFIEDNAME(1, "myMethod"), methAttr, nullptr, MethodTraits<M>::getNumArgs(), data, 0, nullptr, nullptr, nullptr);
  }

private:
  UA_Server *server;
  bool isRunning;
};
} // namespace opc