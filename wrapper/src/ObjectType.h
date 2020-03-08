#pragma once
#include <NodeId.h>
#include <map>
#include <string>

namespace opc {
class ObjectType {
  public:
    ObjectType(NodeId id) : id{id} {}
    void
    addMethodNode(const std::string &name) {
        methods.insert(std::make_pair(name, NodeId(0, 0)));
    }

  private:
    NodeId id;
    std::unordered_map<std::string, NodeId> methods;
};
};  // namespace opc
